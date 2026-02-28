/**
 * @file      : stepper_lib.c
 * @brief     : 步进电机驱动库源文件
 *
 * @version   : 1.0
 * @author    : Drinkto
 * @date      : Feb 24, 2026
 *
 * @changelog:
 * | Date | version | Author | Description |
 * | --- | --- | --- | --- |
 * | Feb 24, 2026 | 1.0 | Drinkto | First version |
 */

#include "stepper_lib.h"
#include <limits.h>
#include "tim.h"

/* 定义用于脉冲生成硬件定时器的TIM 可使用: TIM2 - TIM4 */
#define USE_HTIM2
// #define USE_HTIM3
// #define USE_HTIM4

#ifdef USE_HTIM2
#define HTIM                       htim2
#define HTIM_HARD                  TIM2
#define HTIM_HARD_C                STR(TIM2)
#define RCC_HTIM_HARD_CLK_ENABLE() __HAL_RCC_TIM2_CLK_ENABLE()
#define HTIM_IRQn                  TIM2_IRQn
#define HTIM_IRQHandler            TIM2_IRQHandler
#endif

#ifdef USE_HTIM3
#define HTIM                       htim3
#define HTIM_HARD                  TIM3
#define HTIM_HARD_C                STR(TIM3)
#define RCC_HTIM_HARD_CLK_ENABLE() __HAL_RCC_TIM3_CLK_ENABLE()
#define HTIM_IRQn                  TIM3_IRQn
#define HTIM_IRQHandler            TIM3_IRQHandler
#endif

#ifdef USE_HTIM4
#define HTIM                       htim4
#define HTIM_HARD                  TIM4
#define HTIM_HARD_C                STR(TIM4)
#define RCC_HTIM_HARD_CLK_ENABLE() __HAL_RCC_TIM4_CLK_ENABLE()
#define HTIM_IRQn                  TIM4_IRQn
#define HTIM_IRQHandler            TIM4_IRQHandler
#endif

// 电机实例数组
static stepper_motor_t motors[STEPPER_MAX];

/* 软件脉冲生成器 */
volatile SOFT_PULSE_CTRL_T g_Pulse[PULSE_MAX];

// // 内部函数声明
// static void motor_step_handler(stepper_motor_t *motor);

/**
 * @brief     : 初始化硬件定时器
 * @details   : 配置 TIMx，用于us级别硬件定时。TIMx将自由运行，永不停止.
 *             TIMx可以用TIM2 - TIM4 之间的TIM, 这些TIM有4个通道, 挂在 APB1
 *             上，输入时钟=SystemCoreClock / 2
 */
void sl_TimerInit(void)
{
        TIM_HandleTypeDef TIM_InitStruct = {0};
        uint16_t usPeriod;
        uint16_t usPrescaler;
        TIM_TypeDef *TIMx = HTIM_HARD;

        RCC_HTIM_HARD_CLK_ENABLE(); /* 使能TIM时钟 */

        /* 2KHz */
        usPrescaler = 36000 - 1; /* 分频比 36000 72MHz / 36000 = 2KHz */
        usPeriod = 2 - 1;

        /*
         设置分频为usPrescaler后，那么定时器计数器计1次就是1us
         而参数usPeriod的值是决定了最大计数：
         usPeriod = 0xFFFF 表示最大0xFFFF微秒。
         usPeriod = 0xFFFFFFFF 表示最大0xFFFFFFFF微秒。
         */
        TIM_InitStruct.Instance = TIMx;
        TIM_InitStruct.Init.Prescaler = usPrescaler;
        TIM_InitStruct.Init.Period = usPeriod;
        TIM_InitStruct.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        TIM_InitStruct.Init.CounterMode = TIM_COUNTERMODE_UP;
        TIM_InitStruct.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

        if (HAL_TIM_Base_Init(&TIM_InitStruct) != HAL_OK) {
                bsp_ErrorHandler(__FILE__, __LINE__);
        }

        /* 配置定时器中断，给CC捕获比较中断使用 */
        HAL_NVIC_SetPriority(HTIM_IRQn, 0, 3);
        HAL_NVIC_EnableIRQ(HTIM_IRQn);

        /* 启动定时器 */
        // HAL_TIM_Base_Start_IT(&TIM_InitStruct);
        // sl_println(" Init %s  Prescaler: %d, Period: %d  Generate %dHz Pulse",
        //            HTIM_HARD_C, usPrescaler, usPeriod,
        //            72000000 / (usPrescaler + 1) / (usPeriod + 1));
}

// 频率范围
#define FREQ_MIN 1
#define FREQ_MAX 2000

/**
 * @brief     计算最优的预分频和重装载值
 * @details   用于步进电机驱动库，计算最优的预分频和重装载值，以达到目标频率
 *            优化目标：让ARR尽量大以提高精度，同时保证PSC在16位范围内(0-65535)
 * @param     _freq 目标频率 (1-2000Hz)
 * @param     _psc 输出预分频值
 * @param     _arr 输出重装载值
 * @return    uint32_t 实际能达到的频率（误差分析）
 */
uint32_t sl_TimerCalcParams(uint32_t _freq, uint16_t *_psc, uint16_t *_arr)
{
        if (_freq < FREQ_MIN)
                _freq = FREQ_MIN;
        if (_freq > FREQ_MAX)
                _freq = FREQ_MAX;

        // 目标：ARR尽量大（提高占空比精度），PSC尽量小
        // 公式：freq = SYSCLK / (PSC+1) / (ARR+1)
        // 即：(PSC+1)*(ARR+1) = SYSCLK / freq

        uint32_t total_div = SystemCoreClock / _freq; // 总分频系数

        // 策略：让ARR尽量接近65535以获得最高精度，但不超过
        // 如果total_div <= 65536，可以PSC=0, ARR=total_div-1
        // 否则需要分配PSC和ARR

        if (total_div <= 65536) {
                *_psc = 0;
                *_arr = (uint16_t)(total_div - 1);
        } else {
                // 需要PSC > 0，寻找最优分配
                // PSC+1 = ceil(total_div / 65536)，但尽量让ARR大
                uint32_t psc_temp = (total_div + 65535) / 65536; // 向上取整
                if (psc_temp > 65536)
                        psc_temp = 65536; // 限制最大值

                *_psc = (uint16_t)(psc_temp - 1);
                *_arr = (uint16_t)(total_div / psc_temp - 1);
        }

        // 计算实际频率（用于误差分析）
        uint32_t actual_freq = SystemCoreClock / ((*_psc + 1) * (*_arr + 1));
        return actual_freq;
}

/**
 * @brief     设置硬件定时器速度
 * @details   用于步进电机驱动库，设置硬件定时器速度
 * @param     _speed 目标速度 (1-1000step/s)
 */
void sl_TimerSetSpeed(uint32_t _speed)
{
        /* 速度 0-1000step/s */

        if (_speed < FREQ_MIN)
                _speed = FREQ_MIN;
        if (_speed > FREQ_MAX)
                _speed = FREQ_MAX;

        _speed *= 2;    /* 一个脉冲以 高电平+低电平 */

        /* 计算最优的预分频和重装载值 */
        uint16_t psc, arr;
        uint32_t actual_speed = sl_TimerCalcParams(_speed, &psc, &arr);

        HAL_TIM_Base_Stop(&HTIM);
        __HAL_TIM_SET_PRESCALER(&HTIM, psc);
        __HAL_TIM_SET_AUTORELOAD(&HTIM, arr);

        sl_println(" %s  Prescaler %d  AutoReload %d  Generate %dHz Pulse  "
                   "Actual %dHz",
                   HTIM_HARD_C, psc, arr, _speed, actual_speed);
}

/**
 * @brief     : 初始化GPIO引脚
 * @details   : 初始化GPIO引脚为输出模式，默认电平为_levle
 * @param     : _port GPIO端口
 * @param     : _pin GPIO引脚
 * @param     : _levle GPIO引脚电平 1=高电平, 0=低电平
 */
void sl_GpioOutputInit(GPIO_TypeDef *_port, uint16_t _pin, bool _level)
{
        GPIO_InitTypeDef GPIO_InitStruct = {0};

        GPIO_InitStruct.Pin = _pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(_port, &GPIO_InitStruct);
        HAL_GPIO_WritePin(_port, _pin, _level ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

/**
 * @brief     : 添加脉冲通道
 * @details   : 添加一个脉冲通道到脉冲生成器中
 * @param     : _ch 脉冲通道序号
 * @param     : _step_port 脉冲引脚端口
 * @param     : _step_pin 脉冲引脚
 * @param     : _dir_port 方向引脚端口
 * @param     : _dir_pin 方向引脚
 * @param     : _enable_port 使能引脚端口
 * @param     : _enable_pin 使能引脚
 * @param     : _en_active_high 使能信号电平 1=高电平, 0=低电平
 * @return    : true  添加成功
 * @return    : false 添加失败
 */
bool sl_PulseAdd(PULSE_CHANNEL_E _ch, GPIO_TypeDef *_step_port,
                 uint16_t _step_pin, GPIO_TypeDef *_dir_port, uint16_t _dir_pin,
                 GPIO_TypeDef *_enable_port, uint16_t _enable_pin,
                 STEPPER_ENA_ACT_E _ena_active_level)
{
        if (_ch >= PULSE_MAX || _step_port == NULL || _dir_port == NULL ||
            _enable_port == NULL) {
                return false;
        }

        /* 物理输出端口 */
        g_Pulse[_ch].BindPort.step_port = _step_port;
        g_Pulse[_ch].BindPort.step_pin = _step_pin;
        g_Pulse[_ch].BindPort.dir_port = _dir_port;
        g_Pulse[_ch].BindPort.dir_pin = _dir_pin;
        g_Pulse[_ch].BindPort.enable_port = _enable_port;
        g_Pulse[_ch].BindPort.enable_pin = _enable_pin;

        g_Pulse[_ch].IsAdded = true;

        /* 控制参数 */
        g_Pulse[_ch].Controller.IsRunning = false;
        g_Pulse[_ch].Controller.TargetPos = 0;
        g_Pulse[_ch].Controller.CurrentPos = 0;
        g_Pulse[_ch].Controller.RemainSteps = 0;
        g_Pulse[_ch].Controller.Direction = STEPPER_DIR_CW;
        g_Pulse[_ch].Controller.StepPerS = 0;

        sl_GpioOutputInit(_step_port, _step_pin, 1);
        sl_GpioOutputInit(_dir_port, _dir_pin, 1);
        sl_GpioOutputInit(_enable_port, _enable_pin, _ena_active_level);
        sl_GpioOutputInit(LED_GREEN_PORT, LED_GREEN_PIN, 0);

        return true;
}

/**
 * @brief     : 初始化脉冲通道
 * @details   : 初始化所有添加的脉冲通道
 */
void sl_PulseInit(void)
{
        sl_PulseAdd(PULSE_CH1,            /* 驱动序号 */
                    GPIOA, GPIO_PIN_7,    /* 脉冲引脚 */
                    GPIOA, GPIO_PIN_6,    /* 方向引脚 */
                    GPIOA, GPIO_PIN_4,    /* 使能引脚 */
                    STEPPER_ENA_ACT_LOW); /* 使能信号电平 */
        // sl_PulseAdd(2,                                /* 驱动序号 */
        //             STEP3_GPIO_Port, STEP3_Pin,       /* 脉冲引脚 */
        //             DIR3_GPIO_Port, DIR3_Pin,         /* 方向引脚 */
        //             ENA3_GPIO_Port, ENA3_Pin,         /* 使能引脚 */
        //             STEPPER_ENA_ACT_LOW);             /* 使能信号电平 */
        // sl_PulseAdd(3,                                /* 驱动序号 */
        //             STEP4_GPIO_Port, STEP4_Pin,       /* 脉冲引脚 */
        //             DIR4_GPIO_Port, DIR4_Pin,         /* 方向引脚 */
        //             ENA4_GPIO_Port, ENA4_Pin,         /* 使能引脚 */
        //             STEPPER_ENA_ACT_LOW);             /* 使能信号电平 */

        for (uint8_t i = 0; i < PULSE_MAX; i++) {
                g_Pulse[i].PulseGen.PulseCount = 0;
                g_Pulse[i].PulseGen.PulseRet = SOFT_PLUSE_OFF;
        }

        sl_TimerInit();
}

/**
 * @brief     : 更新脉冲通道
 * @details   : 更新所有添加的脉冲通道的状态,必须定时调用
 */
void sl_PulseUpdate(void)
{
        static uint8_t CompleteCycle[PULSE_MAX] = {0};

        for (uint8_t i = 0; i < PULSE_MAX; i++) {
                /* 脉冲通道正在运行 */
                if (g_Pulse[i].Controller.IsRunning == true) {
                        // sl_println("Pulse Generator[%d] is Operating", i);
                        break;
                }
                /* 未添加到脉冲通道列表 */
                if (g_Pulse[i].IsAdded == false) {
                        // sl_println("Pulse Generator[%d] is Not Added", i);
                        continue;
                }

                /* 脉冲数不为0 */
                if (g_Pulse[i].PulseGen.PulseCount > 0) {
                        bsp_LedToggle(LED_GREEN);

                        /* 产生步进脉冲 */
                        if (g_Pulse[i].PulseGen.PulseRet == SOFT_PLUSE_ON) {
                                g_Pulse[i].PulseGen.PulseRet = SOFT_PLUSE_OFF;
                                CompleteCycle[i] = 2;
                        } else if (g_Pulse[i].PulseGen.PulseRet ==
                                   SOFT_PLUSE_OFF) {
                                g_Pulse[i].PulseGen.PulseRet = SOFT_PLUSE_ON;
                                CompleteCycle[i] = 1;
                        }

                        /* 周期计数器 */
                        if (CompleteCycle[i] == 2) {
                                CompleteCycle[i] = 0; /* 重置周期计数器 */
                                /*一个完整的周期再 根据方向更新位置 */
                                g_Pulse[i].PulseGen.PulseCount--;
                        }
                } else {
                        g_Pulse[i].PulseGen.PulseRet = SOFT_PLUSE_OFF;
                        HAL_TIM_Base_Stop_IT(&HTIM);
                        // sl_println(" Stop %s  State %d", HTIM_HARD_C,
                        //            HAL_TIM_Base_GetState(&HTIM));
                }

                HAL_GPIO_WritePin(
                    g_Pulse[i].BindPort.step_port, g_Pulse[i].BindPort.step_pin,
                    (GPIO_PinState)(g_Pulse[i].PulseGen.PulseRet));
        }
}

/**
 * @brief     : 启动脉冲通道
 * @details   : 启动指定脉冲通道的脉冲输出，输出_pulseCount个脉冲
 * @param     _ch 脉冲通道枚举值
 * @param     _pulseCount 脉冲数量
 */
void sl_PulseStart(PULSE_CHANNEL_E _ch, uint32_t _pulseCount)
{
        // sl_println("Start %d Pulse %d", _ch, _pulseCount);
        if (_ch >= PULSE_MAX || g_Pulse[_ch].IsAdded == false) {
                return;
        }

        HAL_TIM_Base_Stop_IT(&HTIM);
        g_Pulse[_ch].PulseGen.PulseCount = _pulseCount;

        if (HAL_TIM_Base_GetState(&HTIM) == HAL_TIM_STATE_READY) {
                HAL_TIM_Base_Start_IT(&HTIM);
                // sl_println(" Start %s  State %d", HTIM_HARD_C,
                //            HAL_TIM_Base_GetState(&HTIM));
        }
}

void sl_PulseStop(PULSE_CHANNEL_E _ch)
{
        if (_ch >= PULSE_MAX || g_Pulse[_ch].IsAdded == false) {
                return;
        }

        g_Pulse[_ch].PulseGen.PulseCount = 0;
        HAL_TIM_Base_Stop_IT(&HTIM);
}

/**
 * @brief     : 使能/禁用步进电机
 * @details   : 根据_ena参数设置指定步进电机的使能状态
 * @param     _index 步进电机索引
 * @param     _ena 使能状态(STEPPER_ENABLE或STEPPER_DISABLE)
 */
void sl_StepperSetEna(STEPPER_INDEX_E _index, STEPPER_ENABLE_E _ena)
{
        if (_index >= PULSE_MAX || g_Pulse[_index].IsAdded == false ||
            g_Pulse[_index].BindPort.enable_port == NULL) {
                return;
        }

        if (_ena == STEPPER_ENABLE) {
                g_Pulse[_index].Controller.IsRunning = true;
                HAL_GPIO_WritePin(g_Pulse[_index].BindPort.enable_port,
                                  g_Pulse[_index].BindPort.enable_pin,
                                  g_Pulse[_index].BindPort.en_active_level == STEPPER_ENA_ACT_HIGH ? GPIO_PIN_SET : GPIO_PIN_RESET);
        } else {
                g_Pulse[_index].Controller.IsRunning = false;
                HAL_GPIO_WritePin(g_Pulse[_index].BindPort.enable_port,
                                  g_Pulse[_index].BindPort.enable_pin,
                                  g_Pulse[_index].BindPort.en_active_level == STEPPER_ENA_ACT_HIGH ? GPIO_PIN_RESET : GPIO_PIN_SET);
        }
}

/**
 * @brief     : 设置步进电机方向
 * @details   : 根据_dir参数设置指定步进电机的方向
 * @param     _index 步进电机索引
 * @param     _dir 方向(STEPPER_DIR_CW或STEPPER_DIR_CCW)
 */
void sl_StepperSetDir(STEPPER_INDEX_E _index, STEPPER_DIR_E _dir)
{
        if (_index >= PULSE_MAX || g_Pulse[_index].IsAdded == false ||
            g_Pulse[_index].BindPort.dir_port == NULL) {
                return;
        }

        g_Pulse[_index].Controller.Direction = _dir;

        HAL_GPIO_WritePin(g_Pulse[_index].BindPort.dir_port,
                          g_Pulse[_index].BindPort.dir_pin,
                          _dir ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/**
 * @brief     : 设置步进电机速度
 * @details   : 根据_speed参数设置指定步进电机的速度
 * @param     _index 步进电机索引
 * @param     _speed 速度(微秒)
 */
void sl_StepperSetSpeed(STEPPER_INDEX_E _index, uint32_t _speed)
{
        if (_index >= PULSE_MAX || g_Pulse[_index].IsAdded == false ||
            g_Pulse[_index].BindPort.step_port == NULL) {
                return;
        }
        g_Pulse[_index].Controller.StepPerS = _speed;
}

void sl_StepperStop(STEPPER_INDEX_E _index)
{
        if (_index >= PULSE_MAX || g_Pulse[_index].IsAdded == false) {
                return;
        }

        g_Pulse[_index].Controller.RemainSteps = 0;

        sl_StepperSetEna(_index, STEPPER_DISABLE); // 禁用电机
}

/**
 * @brief 停止所有电机,并禁用所以电机驱动
 */
void sl_StepperStopAll(void)
{
        for (uint8_t i = 0; i < PULSE_MAX; i++) {
                sl_StepperStop(i); // 禁用电机
        }
}

/**
 * @brief     : 绝对位置移动
 * @details   : 将指定步进电机移动到绝对位置_targetPos
 * @param     _index 步进电机索引
 * @param     _targetPos 目标位置
 */
void sl_StepperMoveAbs(STEPPER_INDEX_E _index, int32_t _targetPos)
{
        if (_index >= PULSE_MAX || g_Pulse[_index].IsAdded == false ||
            g_Pulse[_index].BindPort.step_port == NULL) {
                return;
        }

        /* 更新目标位置和剩余步数 */
        g_Pulse[_index].Controller.TargetPos = _targetPos;
        g_Pulse[_index].Controller.RemainSteps =
            _targetPos - g_Pulse[_index].Controller.CurrentPos;

        /* 设置方向 */
        if (g_Pulse[_index].Controller.RemainSteps >= 0) {
                sl_StepperSetDir(_index, STEPPER_DIR_CW); // 正转
        } else {
                sl_StepperSetDir(_index, STEPPER_DIR_CCW); // 反转
                g_Pulse[_index].Controller.RemainSteps *= -1;
        }

        /* 启动脉冲生成器 */
        if (g_Pulse[_index].Controller.RemainSteps > 0) {
                sl_StepperSetEna(_index, STEPPER_ENABLE); /* 使能电机 */
                sl_PulseStart(_index, g_Pulse[_index].Controller.RemainSteps);
        }

        sl_println(" %s(): index=%d, position=%d, speed=%d, remain=%d",
                   __FUNCTION__, _index, g_Pulse[_index].Controller.CurrentPos,
                   g_Pulse[_index].Controller.StepPerS,
                   g_Pulse[_index].Controller.RemainSteps);
}

void sl_StepperMoveRel(STEPPER_INDEX_E _index, int32_t _offset)
{
        sl_StepperMoveAbs(_index, g_Pulse[_index].Controller.CurrentPos + _offset);
}

/**
 * @brief 获取电机当前位置
 * @param _index 电机索引
 * @return 当前位置(步数)
 */
int32_t sl_StepperGetPosition(STEPPER_INDEX_E _index)
{
        if (_index >= STEPPER_MAX || g_Pulse[_index].IsAdded == false) {
                return 0;
        }
        return g_Pulse[_index].Controller.CurrentPos;
}

/**
 * @brief 检查电机是否到达目标位置
 * @param _index 电机索引
 * @return 1表示到达，0表示未到达
 */
STEPPER_RUN_E sl_StepperIsFinished(STEPPER_INDEX_E _index)
{
        if (_index >= PULSE_MAX || g_Pulse[_index].IsAdded == false) {
                return STEPPER_NOPOSITION;
        }
        return (g_Pulse[_index].Controller.RemainSteps == 0
                    ? STEPPER_INPOSITION
                    : STEPPER_NOPOSITION);
}

// void sl_MotorStart(STEPPER_INDEX_E _index, uint32_t _pulseCount)
// {
//         if (_index >= STEPPER_MAX || motors[_index].is_added == false) {
//                 return;
//         }
//         sl_PulseStart(motors[_index].pulse_ch, _pulseCount);
// }


#if 0
/**
 * @brief 初始化步进电机驱动库
 */
void stepper_init(void)
{
        /* 初始化硬件定时器 4 脉冲产生 */
        sl_InitHardTimer();

        /* 初始化所有电机实例 */
        for (uint8_t i = 0; i < STEPPER_MAX; i++) {
                motors[i].step_port = NULL;
                motors[i].step_pin = 0;
                motors[i].dir_port = NULL;
                motors[i].dir_pin = 0;
                motors[i].enable_port = NULL;
                motors[i].enable_pin = 0;
                motors[i].target_pos = 0;
                motors[i].current_pos = 0;
                motors[i].remaining_steps = 0;
                motors[i].direction = STEPPER_DIR_CW;
                motors[i].step_per_s =
                    DEFAULT_STEP_PER_S; /* 默认脉冲间隔(微秒) */
                motors[i].is_added = false;
                motors[i].en_active_high =
                    STEPPER_ENA_ACT_HIGH; /* 默认使能高电平有效 */
        }
}

/**
 * @brief     : 初始化GPIO引脚
 * @details   : 初始化GPIO引脚为输出模式，默认电平为_levle
 * @param     : _port GPIO端口
 * @param     : _pin GPIO引脚
 * @param     : _levle GPIO引脚电平 1=高电平, 0=低电平
 */
void stepper_init_gpio(GPIO_TypeDef *_port, uint16_t _pin, bool _level)
{
        GPIO_InitTypeDef GPIO_InitStruct = {0};

        GPIO_InitStruct.Pin = _pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(_port, &GPIO_InitStruct);
        HAL_GPIO_WritePin(_port, _pin, _level ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

/**
 * @brief     : 添加步进电机实例,绑定物理端口
 * @details
 * @param     : _index 电机索引(0-3)
 * @param     : _step_port 步进信号GPIO端口
 * @param     : _step_pin 步进信号GPIO引脚
 * @param     : _dir_port 方向信号GPIO端口
 * @param     : _dir_pin 方向信号GPIO引脚
 * @param     : _enable_port 使能信号GPIO端口
 * @param     : _enable_pin 使能信号GPIO引脚
 * @param     : _en_active_high 使能信号有效电平: 1=高电平有效, 0=低电平有效
 * @return    : int8_t 0成功，-1失败
 */
int8_t stepper_add_motor(STEPPER_INDEX_E _index, GPIO_TypeDef *_step_port,
                         uint16_t _step_pin, GPIO_TypeDef *_dir_port,
                         uint16_t _dir_pin, GPIO_TypeDef *_enable_port,
                         uint16_t _enable_pin,
                         STEPPER_ENA_ACT_E _en_active_high)
{
        if (_index >= STEPPER_MAX || _step_port == NULL || _dir_port == NULL ||
            _enable_port == NULL) {
                sl_println(" Motor[%d] Add Error", _index);
                return -1;
        }

        motors[_index].step_port = _step_port;
        motors[_index].step_pin = _step_pin;
        motors[_index].dir_port = _dir_port;
        motors[_index].dir_pin = _dir_pin;
        motors[_index].enable_port = _enable_port;
        motors[_index].enable_pin = _enable_pin;
        motors[_index].en_active_high = _en_active_high;
        motors[_index].is_added = true;

        stepper_init_gpio(motors[_index].step_port, motors[_index].step_pin, 1);
        stepper_init_gpio(motors[_index].dir_port, motors[_index].dir_pin, 1);
        stepper_init_gpio(motors[_index].enable_port, motors[_index].enable_pin,
                          motors[_index].en_active_high);

        /* 初始化STEP/DIR/ENA状态 */
        HAL_GPIO_WritePin(motors[_index].step_port, motors[_index].step_pin,
                          GPIO_PIN_RESET);
        HAL_GPIO_WritePin(motors[_index].dir_port, motors[_index].dir_pin,
                          GPIO_PIN_RESET);
        // stepper_enable_motor(_index, STEPPER_DISABLE); /* 默认禁用电机 */
        stepper_enable_motor(_index, STEPPER_ENABLE); // 使能电机

        sl_println(" Motor[%d] EnActiveHigh=%d: Step=%d,Dir=%d,Ena=%d Added!",
                   _index, motors[_index].en_active_high, _step_pin, _dir_pin,
                   _enable_pin);
        return 0;
}

/**
 * @brief 使能或禁用电机
 * @param _index 电机索引
 * @param _enable 1=使能，0=禁用
 */
void stepper_enable_motor(STEPPER_INDEX_E _index, STEPPER_ENABLE_E _enable)
{
        if (_index >= STEPPER_MAX || motors[_index].is_added == false ||
            motors[_index].enable_port == NULL) {
                sl_println(" Motor[%d] Ena Error", _index);
                return;
        }

        if (motors[_index].en_active_high == STEPPER_ENA_ACT_HIGH) {
                HAL_GPIO_WritePin(motors[_index].enable_port,
                                  motors[_index].enable_pin,
                                  _enable ? GPIO_PIN_SET : GPIO_PIN_RESET);
        } else {
                HAL_GPIO_WritePin(motors[_index].enable_port,
                                  motors[_index].enable_pin,
                                  _enable ? GPIO_PIN_RESET : GPIO_PIN_SET);
        }
}

/**
 * @brief 设置电机方向
 * @param _index 电机索引
 * @param _direction 电机方向 0=顺时针，1=逆时针
 */
void stepper_set_direction(STEPPER_INDEX_E _index, STEPPER_DIR_E _direction)
{
        if (_index >= STEPPER_MAX || motors[_index].is_added == false ||
            motors[_index].dir_port == NULL) {
                sl_println(" Motor[%d] Dir Error", _index);
                return;
        }

        motors[_index].direction = _direction;
        HAL_GPIO_WritePin(motors[_index].dir_port, motors[_index].dir_pin,
                          _direction ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/**
 * @brief     : 设置电机速度
 * @details   : 设置电机的旋转速度(步/秒)
 * @param     : _index 电机序号
 * @param     : _speed 电机速度(步/秒)
 */
void stepper_set_speed(STEPPER_INDEX_E _index, uint32_t _speed)
{
        if (_index >= STEPPER_MAX || motors[_index].is_added == false) {
                sl_println(" Motor[%d] Update Speed Error", _index);
                return;
        }

        /* 速度 0-1000step/s */
        if (_speed >= 0) {
                uint32_t new_arr = 2 * (1000 - _speed);

                HAL_TIM_Base_Stop(&HTIM);
                __HAL_TIM_SET_AUTORELOAD(&HTIM, _speed);

                motors[_index].step_per_s = _speed;
                sl_println(" %s  Period: %d  Generate %dHz Pulse", HTIM_HARD_C,
                           _speed, 2000 / (_speed + 1));
        }
}

/**
 * @brief 设置电机目标位置
 * @param _index 电机索引
 * @param _position 目标位置(步数)
 * @param _speed 最大速度(步/秒)
 */
void stepper_set_target_position(STEPPER_INDEX_E _index, int32_t _position,
                                 uint32_t _speed)
{
        if (_index >= STEPPER_MAX || motors[_index].is_added == false) {
                sl_println(" Motor[%d] Set Position Error!", _index);
                return;
        }

        /* 更新目标位置和剩余步数 */
        motors[_index].target_pos = _position;
        motors[_index].remaining_steps = _position - motors[_index].current_pos;

        /* 设置速度 */
        stepper_set_speed(_index, _speed);

        /* 设置方向 */
        if (motors[_index].remaining_steps >= 0) {
                stepper_set_direction(_index, STEPPER_DIR_CW); // 正转
        } else {
                stepper_set_direction(_index, STEPPER_DIR_CCW); // 反转
                motors[_index].remaining_steps *= -1;
        }

        // 启动定时器
        if (motors[_index].remaining_steps > 0) {
                if (HAL_TIM_Base_GetState(&HTIM) == HAL_TIM_STATE_READY) {
                        HAL_TIM_Base_Start_IT(&HTIM);
                        sl_println(" Start %s  State %d", HTIM_HARD_C,
                                   HAL_TIM_Base_GetState(&HTIM));
                }
                stepper_enable_motor(_index, STEPPER_ENABLE); // 使能电机
        }

        sl_println(" %s(): index=%d, position=%d, speed=%d, remain=%d",
                   __FUNCTION__, _index, _position, _speed,
                   motors[_index].remaining_steps);
}

/**
 * @brief 获取电机当前位置
 * @param _index 电机索引
 * @return 当前位置(步数)
 */
int32_t stepper_get_position(STEPPER_INDEX_E _index)
{
        if (_index >= STEPPER_MAX) {
                return 0;
        }
        return motors[_index].current_pos;
}

/**
 * @brief 检查电机是否到达目标位置
 * @param _index 电机索引
 * @return 1表示到达，0表示未到达
 */
STEPPER_RUN_E stepper_is_finished(STEPPER_INDEX_E _index)
{
        if (_index >= STEPPER_MAX || motors[_index].is_added == false) {
                return STEPPER_NOPOSITION;
        }
        return (motors[_index].remaining_steps == 0 ? STEPPER_INPOSITION
                                                    : STEPPER_NOPOSITION);
}

void stepper_stop(STEPPER_INDEX_E _index)
{
        if (_index >= STEPPER_MAX || motors[_index].is_added == false) {
                return;
        }
        motors[_index].remaining_steps = 0;
        stepper_enable_motor(_index, STEPPER_DISABLE); // 禁用电机
}

/**
 * @brief 停止所有电机,并禁用定时器更新
 */
void stepper_stop_all(void)
{
        for (int i = 0; i < STEPPER_MAX; i++) {
                stepper_stop(i); // 禁用电机
        }

        HAL_TIM_Base_Stop_IT(&HTIM); /* 关闭定时器 */
}

/**
 * @brief 单个电机步进处理
 * @param _motor 电机指针
 * @note  每个电机需要独立维护脉冲状态，一个完整步进 = 高电平 + 低电平
 */
static void motor_step_handler(stepper_motor_t *_motor)
{
        static STEPPER_PULSE_E pulse_state = SOFT_PLUSE_OFF;
        static uint8_t complete_cycle = 0;
        static uint32_t handler_count = 0;

        if (_motor->remaining_steps > 0) {
                handler_count++;

                /* 产生步进脉冲 */
                if (pulse_state == SOFT_PLUSE_OFF) {
                        HAL_GPIO_WritePin(_motor->step_port, _motor->step_pin,
                                          GPIO_PIN_SET);
                        bsp_LedOn(LED_GREEN);
                        pulse_state = SOFT_PLUSE_ON;
                        complete_cycle = 1;
                } else if (pulse_state == SOFT_PLUSE_ON) {
                        HAL_GPIO_WritePin(_motor->step_port, _motor->step_pin,
                                          GPIO_PIN_RESET);
                        bsp_LedOff(LED_GREEN);
                        pulse_state = SOFT_PLUSE_OFF;
                        if (complete_cycle == 1) {
                                complete_cycle = 2; /* 一个完整的周期 */
                        }
                }

                if (complete_cycle == 2) {
                        complete_cycle = 0; /* 重置周期计数器 */
                        /*一个完整的周期再 根据方向更新位置 */
                        if (_motor->direction == STEPPER_DIR_CW) {
                                _motor->current_pos++;
                        } else {
                                _motor->current_pos--;
                        }
                        _motor->remaining_steps--;
                }
        } else {
                handler_count = 0;
        }
}

/**
 * @brief 定时器中断回调函数
 * 这个函数需要在stm32f1xx_it.c中调用
 */
void stepper_timer_callback(void)
{
        uint8_t any_active = 0;

        // 为每个活动电机执行一步
        for (uint8_t i = 0; i < STEPPER_MAX; i++) {
                if (motors[i].is_added == STEPPER_DISABLE ||
                    motors[i].remaining_steps == 0) {
                        continue;
                }
                motor_step_handler(&motors[i]);
                any_active = 1;
        }

        // 如果没有活动电机，停止定时器
        if (!any_active) {
                stepper_stop_all();
                sl_println(" Stop %s  State %d", HTIM_HARD_C,
                           HAL_TIM_Base_GetState(&HTIM));
        }
}

#endif

/**
 * @brief     : 定时器中断处理函数
 */
void HTIM_IRQHandler(void)
{
        HAL_TIM_IRQHandler(&HTIM);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
        /* Prevent unused argument(s) compilation warning */
        UNUSED(htim);

        if (htim == &HTIM) {
                // stepper_timer_callback();
                sl_PulseUpdate();
        }
}