#include "bsp.h"

#include "tim.h"
#include "cmd_parser.h"

/* 1使用硬件定时器产生PWM 2软件PWM */
#define MOTOR_LIB 2

#if MOTOR_LIB == 1
#include "stepper_motor.h"
#elif MOTOR_LIB == 2
#include "stepper_lib.h"
#endif

/**
 * @brief     : 处理电机任务
 */
void bsp_MotorTask(void)
{
        CmdParser_ExecuteIfReady();
}

/**
 * @brief     : 处理电机命令解析任务
 */
void bsp_MotorCmdTask(void)
{
        uint8_t cmd;
        /* 从串口读入一个字符(非阻塞方式) */
        if (comGetChar(COM1, &cmd)) {
                CmdParser_ProcessChar(cmd);
        }
}

#if MOTOR_LIB == 1

// 定义步进电机实例
StepperMotor_t myStepper;

void bsp_MotorMenu(void);

// 位置到达回调函数
void PositionReachedCallback(void)
{
        bsp_println("目标位置已到达!");
}

// 错误回调函数
void ErrorCallback(uint8_t error_code)
{
        bsp_println("电机错误：%d", error_code);
}

void bsp_MotorInit(void)
{
        // 初始化步进电机
        Stepper_Init(&myStepper, &htim4, TIM_CHANNEL_4, DIR1_GPIO_Port, DIR1_Pin, // 方向引脚
                     ENA1_GPIO_Port, ENA1_Pin);                                   // 使能引脚

        // 配置电机参数
        StepperConfig_t config = {.steps_per_rev = 200,
                                  .max_speed = 800,    // 步/秒
                                  .acceleration = 300, // 步/秒²
                                  .deceleration = 300,
                                  .jerk = 5000,
                                  .min_pulse_width = 5,
                                  .max_pulse_width = 1000,
                                  .enable_active_low = true,
                                  .dir_cw_level = GPIO_PIN_SET};
        Stepper_Config(&myStepper, &config);

        // 设置回调函数
        myStepper.position_reached_cb = PositionReachedCallback;
        myStepper.error_cb = ErrorCallback;
        // 使能电机
        Stepper_Enable(&myStepper);

        // bsp_MotorMenu();
        CmdParser_Init();
}

void bsp_MotorMenu(void)
{
        BSP_Printf("\r\n------------------------------------------------\r\n");
        BSP_Printf("请选择操作命令:\r\n");
        BSP_Printf("1 - 移动到位置1000步\r\n");
        BSP_Printf("2 - 反向移动500步\r\n");
        BSP_Printf("3 - 速度模式运行\r\n");
        BSP_Printf("4 - S曲线加减速移动到位置2000\r\n");
}

void bsp_MotorControl(void)
{
        uint8_t cmd;
        /* 从串口读入一个字符(非阻塞方式) */
        if (comGetChar(COM1, &cmd)) {
                CmdParser_ProcessChar(cmd);
                // bsp_println("COM1: %c", cmd);
                // switch (cmd) {
                // case '1':
                //         BSP_Printf("\r\n【1 - 移动到位置1000步】\r\n");
                //         // 示例1：位置控制
                //         bsp_println("1 移动到位置1000步\r\n");
                //         // Stepper_MoveTo(&myStepper, 1000);
                //         // while (Stepper_IsMoving(&myStepper)) {
                //         //         // 等待移动完成
                //         // }

                //         break;

                // case '2':
                //         BSP_Printf("\r\n【2 - 反向移动500步】\r\n");
                //         // 示例2：相对移动
                //         bsp_println("2 反向移动500步\r\n");
                //         // Stepper_Move(&myStepper, -500);
                //         // while (Stepper_IsMoving(&myStepper)) {
                //         //         // 显示当前位置
                //         //         bsp_println("当前位置：%ld\r\n", Stepper_GetPosition(&myStepper));
                //         //         HAL_Delay(100);
                //         // }
                //         break;

                // case '3':
                //         BSP_Printf("\r\n【3 - 速度模式运行】\r\n");
                //         // 示例3：速度控制
                //         bsp_println("3 速度模式运行\r\n");
                //         // Stepper_SetSpeed(&myStepper, 300); // 300步/秒
                //         // Stepper_Run(&myStepper, true);     // 顺时针运行

                //         // // 运行5秒后停止
                //         // HAL_Delay(5000);
                //         // Stepper_Stop(&myStepper);
                //         break;

                // case '4':
                //         BSP_Printf("\r\n【4 - S曲线加减速移动到位置2000】\r\n");
                //         // 示例4：设置不同加减速曲线
                //         Stepper_SetAccelProfile(&myStepper, S_CURVE);
                //         Stepper_SetAcceleration(&myStepper, 200);
                //         Stepper_SetDeceleration(&myStepper, 200);

                //         bsp_println("4 S曲线加减速移动到位置2000\r\n");
                //         // Stepper_MoveTo(&myStepper, 2000);
                //         // while (Stepper_IsMoving(&myStepper)) {
                //         //         bsp_println("速度：%.1f 步/秒\r\n", Stepper_GetSpeed(&myStepper));
                //         //         HAL_Delay(300);
                //         // }
                //         break;

                // default:
                //         bsp_MotorMenu();
                //         break;
                // }
        } else {
                // CmdParser_ExecuteIfReady();
        }
}

#elif MOTOR_LIB == 2

void bsp_MotorInit(void)
{
        CmdParser_Init();

        bsp_println("Init Cmd Parser!");

        // 初始化步进电机库
        stepper_init();

        bsp_println("Init Stepper Motor Driver!");

        // stepper_add_motor(0, GPIOA, GPIO_PIN_7, GPIOA, GPIO_PIN_6, GPIOA,
        //                   GPIO_PIN_4, 1);
        // stepper_add_motor(0, GPIOB, GPIO_PIN_13, GPIOB, GPIO_PIN_14, GPIOA,
        //                   GPIO_PIN_8, 1);
        // 添加四个步进电机 (包含使能引脚)
        stepper_add_motor(0,                          /* 驱动序号 */
                          STEP1_GPIO_Port, STEP1_Pin, /* 脉冲引脚 */
                          DIR1_GPIO_Port, DIR1_Pin,   /* 方向引脚 */
                          ENA1_GPIO_Port, ENA1_Pin,   /* 使能引脚 */
                          1);                         /* 使能信号电平 */
        stepper_add_motor(1,                          /* 驱动序号 */
                          STEP2_GPIO_Port, STEP2_Pin, /* 脉冲引脚 */
                          DIR2_GPIO_Port, DIR2_Pin,   /* 方向引脚 */
                          ENA2_GPIO_Port, ENA2_Pin,   /* 使能引脚 */
                          0);                         /* 使能信号电平 */
        stepper_add_motor(2,                          /* 驱动序号 */
                          STEP3_GPIO_Port, STEP3_Pin, /* 脉冲引脚 */
                          DIR3_GPIO_Port, DIR3_Pin,   /* 方向引脚 */
                          ENA3_GPIO_Port, ENA3_Pin,   /* 使能引脚 */
                          0);                         /* 使能信号电平 */
        stepper_add_motor(3,                          /* 驱动序号 */
                          STEP4_GPIO_Port, STEP4_Pin, /* 脉冲引脚 */
                          DIR4_GPIO_Port, DIR4_Pin,   /* 方向引脚 */
                          ENA4_GPIO_Port, ENA4_Pin,   /* 使能引脚 */
                          1);                         /* 使能信号电平 */
}

void bsp_MotorControlTask(void)
{
        // 启动电机运动
        // stepper_set_target_position(0, 5000, 500);  // 第1个电机走1000步，速度500步/秒
        // stepper_set_target_position(1, -500, 250.0f);  // 第2个电机反向走500步，速度250步/秒
        // stepper_set_target_position(2, 2000, 800.0f);  // 第3个电机走2000步，速度800步/秒
        // stepper_set_target_position(3, -1500, 600.0f); // 第4个电机反向走1500步，速度600步/秒
}

#endif