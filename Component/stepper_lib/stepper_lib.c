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

/* 定义用于硬件定时器的TIM 可使用: TIM2 - TIM4 */
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

// 内部函数声明
static void motor_step_handler(stepper_motor_t *motor);

/**
 * @brief     : 初始化硬件定时器
 * @details   : 配置 TIMx，用于us级别硬件定时。TIMx将自由运行，永不停止.
 *             TIMx可以用TIM2 - TIM5 之间的TIM, 这些TIM有4个通道, 挂在 APB1
 *             上，输入时钟=SystemCoreClock / 2
 */
void sl_InitHardTimer(void)
{
        TIM_HandleTypeDef TimHandle = {0};
        uint32_t usPeriod;
        uint16_t usPrescaler;
        TIM_TypeDef *TIMx = HTIM_HARD;

        RCC_HTIM_HARD_CLK_ENABLE(); /* 使能TIM时钟 */

        usPrescaler = 72 - 1;             /* 分频比 = 1 */
        usPeriod = MINIMUM_TIME_INTERVAL; /// F0定时器均为16位

        /*
         设置分频为usPrescaler后，那么定时器计数器计1次就是1us
         而参数usPeriod的值是决定了最大计数：
         usPeriod = 0xFFFF 表示最大0xFFFF微秒。
         usPeriod = 0xFFFFFFFF 表示最大0xFFFFFFFF微秒。
         */
        TimHandle.Instance = TIMx;
        TimHandle.Init.Prescaler = usPrescaler;
        TimHandle.Init.Period = usPeriod;
        TimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
        TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

        if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK) {
                bsp_ErrorHandler(__FILE__, __LINE__);
        }

        /* 配置定时器中断，给CC捕获比较中断使用 */
        {
                HAL_NVIC_SetPriority(HTIM_IRQn, 0, 3);
                HAL_NVIC_EnableIRQ(HTIM_IRQn);
        }

        /* 启动定时器 */
        HAL_TIM_Base_Start_IT(&TimHandle);
        sl_println(" Init and Start %s  Prescaler: %d, Period: %d", HTIM_HARD_C,
                   usPrescaler, usPeriod);
}

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
                motors[i].step_per_s = DEFAULT_STEP_PER_S; /* 默认速度 */
                motors[i].max_step_per_s = MAX_STEP_PER_S; /* 最高速度 */
                motors[i].min_step_per_s = MIN_STEP_PER_S; /* 最低速度 */
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
        HAL_GPIO_WritePin(_port, _pin,
                          _level ? GPIO_PIN_RESET : GPIO_PIN_SET);
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
    int8_t
    stepper_add_motor(STEPPER_INDEX_E _index, GPIO_TypeDef *_step_port,
                      uint16_t _step_pin, GPIO_TypeDef *_dir_port,
                      uint16_t _dir_pin, GPIO_TypeDef *_enable_port,
                      uint16_t _enable_pin, STEPPER_ENA_ACT_E _en_active_high)
{
        if (_index >= STEPPER_MAX) {
                sl_println(" Motor Over");
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

        sl_println(" Add Motor %d: Step=%d, Dir=%d, En=%d Init completed", _index, _step_pin,
                   _dir_pin, _enable_pin);

        /* 初始化STEP/DIR/ENA状态 */
        HAL_GPIO_WritePin(motors[_index].step_port, motors[_index].step_pin,
                          GPIO_PIN_RESET);
        HAL_GPIO_WritePin(motors[_index].dir_port, motors[_index].dir_pin,
                          GPIO_PIN_RESET);
        // stepper_enable_motor(_index, STEPPER_DISABLE); /* 默认禁用电机 */
        stepper_enable_motor(_index, STEPPER_ENABLE); // 使能电机

        sl_println(" Add Motor %d: EnActiveHigh=%d", _index,
                   motors[_index].en_active_high);
        return 0;
}

/**
 * @brief 使能或禁用电机
 * @param _index 电机索引
 * @param _enable 1=使能，0=禁用
 */
void stepper_enable_motor(STEPPER_INDEX_E _index, STEPPER_ENABLE_E _enable)
{
        if (_index >= STEPPER_MAX || motors[_index].is_added == false) {
                return;
        }

        if (motors[_index].enable_port != NULL &&
            motors[_index].enable_pin != 0) {
                if (motors[_index].en_active_high == STEPPER_ENA_ACT_HIGH) {
                        HAL_GPIO_WritePin(motors[_index].enable_port,
                                          motors[_index].enable_pin,
                                          _enable ? GPIO_PIN_SET
                                                  : GPIO_PIN_RESET);
                } else {
                        HAL_GPIO_WritePin(motors[_index].enable_port,
                                          motors[_index].enable_pin,
                                          _enable ? GPIO_PIN_RESET
                                                  : GPIO_PIN_SET);
                }
        }
        // sl_println(" Motor[%d] En: %d %s", _index, _enable,
        //            _enable ? "Enabled" : "Disabled");
}

/**
 * @brief 设置电机方向
 * @param _index 电机索引
 * @param _direction 电机方向 0=顺时针，1=逆时针
 */
void stepper_set_direction(STEPPER_INDEX_E _index, STEPPER_DIR_E _direction)
{
        if (_index >= STEPPER_MAX || motors[_index].is_added == false) {
                return;
        }

        motors[_index].direction = _direction;
        HAL_GPIO_WritePin(motors[_index].dir_port, motors[_index].dir_pin,
                          _direction ? GPIO_PIN_SET : GPIO_PIN_RESET);
        // sl_println(" Motor[%d] Dir: %d %s", _index, _direction,
        //            _direction ? "CCW" : "CW");
}

/**
 * @brief 设置电机速度范围
 * @param _index 电机索引
 * @param _min_speed 最小速度(步/秒)
 * @param _max_speed 最大速度(步/秒)
 */
void stepper_set_speed_range(STEPPER_INDEX_E _index, uint32_t _min_speed,
                             uint32_t _max_speed)
{
        if (_index >= STEPPER_MAX || motors[_index].is_added == false) {
                return;
        }

        if (_min_speed > 0) {
                motors[_index].min_step_per_s = _min_speed;
        }

        if (_max_speed > 0) {
                motors[_index].max_step_per_s = _max_speed;
        }
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
                return;
        }

        /* 根据速度计算脉冲间隔(单位: 10微秒) */
        if (_speed > 0) {
                uint32_t new_delay = (uint32_t)(_speed * 1000000); // 转换为微秒

                /* 限制在最小最大延迟范围内 */
                if (new_delay < motors[_index].max_step_per_s) {
                        new_delay = motors[_index].max_step_per_s;
                } else if (new_delay > motors[_index].min_step_per_s) {
                        new_delay = motors[_index].min_step_per_s;
                }

                motors[_index].step_per_s = _speed;
                sl_println(" Motor[%d] Speed: %d (%d-%d)", _index, _speed,
                           motors[_index].min_step_per_s,
                           motors[_index].max_step_per_s);
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
                sl_println(" Motor Error!");
                return;
        }

        /* 更新目标位置和剩余步数 */
        motors[_index].target_pos = _position;
        motors[_index].remaining_steps = _position - motors[_index].current_pos;

        /* 设置速度 */
        stepper_set_speed(_index, _speed);

        /* 设置方向 */
        if (motors[_index].remaining_steps >= 0) {
                stepper_set_direction(_index, 0); // 正转
                // motors[_index].remaining_steps = motors[_index].remaining_steps;
        } else {
                stepper_set_direction(_index, 1); // 反转
                motors[_index].remaining_steps *= -1;
        }

        // 启动定时器
        if (motors[_index].remaining_steps > 0) {
                sl_println(" State %d", HAL_TIM_Base_GetState(&HTIM));
                if (HAL_TIM_Base_GetState(&HTIM) == HAL_TIM_STATE_READY) {
                        HAL_TIM_Base_Start_IT(&HTIM);
                        sl_println(" Start Timer");
                }
                stepper_enable_motor(_index, STEPPER_ENABLE); // 使能电机
        }
        sl_println(
            " %s(): index=%d, position=%d, speed=%d, remain=%d",
            __FUNCTION__, _index, _position, _speed, motors[_index].remaining_steps);
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
 */
static void motor_step_handler(stepper_motor_t *_motor)
{
        static STEPPER_PULSE_E pulse_state = PLUSE_LOW;
        static uint8_t first_flag = 1;
        static uint32_t handler_count = 0;
        if (_motor->remaining_steps > 0) {
                handler_count++;

                /* 产生步进脉冲 */
                if (pulse_state == PLUSE_LOW) {
                        HAL_GPIO_WritePin(_motor->step_port, _motor->step_pin,
                                          GPIO_PIN_SET);
                        pulse_state = PLUSE_HIGH;
                } else if (pulse_state == PLUSE_HIGH) {
                        HAL_GPIO_WritePin(_motor->step_port, _motor->step_pin,
                                          GPIO_PIN_RESET);
                        pulse_state = PLUSE_LOW;
                }

                /* 根据方向更新位置 */
                if (_motor->direction == STEPPER_DIR_CW) {
                        _motor->current_pos++;
                } else {
                        _motor->current_pos--;
                }

                _motor->remaining_steps--;

                // sl_println(" (%d)c:%d r:%d", handler_count, _motor->current_pos,
                //            _motor->remaining_steps);
        } else {
                // sl_println("Pluse Completed.  Position:%d",
                //            _motor->current_pos);
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

        // bsp_LedToggle(LED_GREEN);

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
                // sl_println(" Stop Timer");
        }
}

/**
 * @brief     : 定时器中断处理函数
 */
void HTIM_IRQHandler(void)
{
        stepper_timer_callback();

        HAL_TIM_IRQHandler(&HTIM);
}

