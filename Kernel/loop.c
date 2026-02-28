/**
 * @file      : loop.c
 * @brief     :
 *
 * @version   : 1.0
 * @author    : Drinkto
 * @date      : Jan 27, 2026
 *
 * @changelog:
 * | Date | version | Author | Description |
 * | --- | --- | --- | --- |
 * | Jan 27, 2026 | 1.0 | Drinkto | xxx |
 */

#include <stdbool.h>

#include "loop.h"
#include "main.h"
#include "stm32f1xx_hal.h"
#include "bsp.h"
#include "tim.h"

/********************************************** 任务优先级,数值越小优先级越高 */

/* 任务抢占阈值,高于此值的任务才允许抢占 如果该值等于任务优先级,则禁用抢占阈值 */

/******************************************************** 任务栈大小,单位字节 */

/*********************************************** 静态变量: 任务启动线程控制块 */

/******************************************************************* 函数声明 */

/******************************************************************* 变量声明 */
// static TX_MUTEX AppPrintfSemp; /* 用于printf互斥 */

/**
 * @brief     : 启动任务
 *              启动软定时器，开始简单时间片轮转调度
 * @note      : 时间片轮转调度1ms，多个周期可公因的任务，会在那个公倍时间点会集中触发
 *              可能导致后续任务执行时间延迟（执行时间不确定性），CPU负载分布在时间线上并不均匀
 *              建议增加基于任务执行周期的可配置的固定时差偏移功能，如10ms周期任务，固定时差3ms
 *              从0ms起始的时间线上看，分别在13，23，33，(x*10+3,x>0)ms的时候执行，确保和同周期的其它任务错峰使用CPU
 */
void kernel_TaskStart(void)
{
        /// 任务1  保活进程 1001ms 0.999Hz
        bsp_StartAutoTimer(TASK_INDEX_1, 1001);

        /// 任务2  电机命令进程 51ms 19.99Hz
        bsp_StartAutoTimer(TASK_INDEX_2, 51);

        /// 任务3  电机命令执行进程 43ms
        bsp_StartAutoTimer(TASK_INDEX_3, 43);

        /// 任务4  Modbus解析进程 30ms
        // bsp_StartAutoTimer(TASK_INDEX_4, 121);

        /// 任务5  保活进程 1000ms 1Hz
        // bsp_StartAutoTimer(TASK_INDEX_5, 1001);

        /// 任务6  指示灯闪烁进程 300ms 3.3Hz
        // bsp_StartAutoTimer(TASK_INDEX_6, 301);
}

/**
 * @brief     : 主循环 用于替代main函数,防止CubeMX重新生成时覆盖
 *              c程序入口
 */
void loop(void)
{
        // /* 内核开启前关闭HAL的时间基准 */
        // HAL_SuspendTick();

        /// 进行关键外设初始化前等待电源稳定
        HAL_Delay(500);
        /// bsp_DelayMS(500); (x禁止使用!)

        /* ↑---------------------------- 注意 ! ----------------------------↑ */
        /// ↑箭头上方禁止使用bsp_timer.c和bsp_tim_pwm.c内函数
        /// 未完成bsp初始化禁止使用bsp层函数,特别时和时基有关的函数
        bsp_Init(); // bsp及外设硬件初始化

        /* 转用bsp提供的延时函数,进行应用初始化前等待系统稳定 */
        bsp_DelayMS(500); /* 延时以确保bsp初始化完成 */

        /* 启动任务 */
        kernel_TaskStart();

        bsp_println("Start Task Scheduling");

        // bsp_MotorControlTask();

        /* Infinite FOR Circulation */
        for (;;) {
                /// 实时进程
                bsp_Idle(); /* 空闲时执行的函数 */

                /* 任务1 */
                if (bsp_CheckTimer(TASK_INDEX_1)) {
                        // bsp_println("Sec: %d", HAL_GetTick() / 1000);
                        // stepper_task();
                        // HAL_GPIO_TogglePin(STEP1_GPIO_Port, STEP1_Pin);
                        // HAL_GPIO_TogglePin(STEP2_GPIO_Port, STEP2_Pin);
                        // HAL_GPIO_TogglePin(STEP3_GPIO_Port, STEP3_Pin);
                        // HAL_GPIO_TogglePin(STEP4_GPIO_Port, STEP4_Pin);

                        bsp_LedToggle(LED_RED);
                }

                /* 任务2 */
                if (bsp_CheckTimer(TASK_INDEX_2)) {
                        bsp_MotorCmdTask();
                        // bsp_MotorControl();
                        // HAL_GPIO_TogglePin(DIR1_GPIO_Port, DIR1_Pin);
                        // HAL_GPIO_TogglePin(DIR2_GPIO_Port, DIR2_Pin);
                        // HAL_GPIO_TogglePin(DIR3_GPIO_Port, DIR3_Pin);
                        // HAL_GPIO_TogglePin(DIR4_GPIO_Port, DIR4_Pin);
                }

                /* 任务3 */
                if (bsp_CheckTimer(TASK_INDEX_3)) {
                        bsp_MotorTask();
                        // HAL_GPIO_TogglePin(ENA1_GPIO_Port, ENA1_Pin);
                        // HAL_GPIO_TogglePin(ENA2_GPIO_Port, ENA2_Pin);
                        // HAL_GPIO_TogglePin(ENA3_GPIO_Port, ENA3_Pin);
                        // HAL_GPIO_TogglePin(ENA4_GPIO_Port, ENA4_Pin);
                }

                /* 任务4 */
                if (bsp_CheckTimer(TASK_INDEX_4)) {
                }

                /* 任务5 */
                if (bsp_CheckTimer(TASK_INDEX_5)) {
                }

                /* 任务6 */
                if (bsp_CheckTimer(TASK_INDEX_6)) {
                }
        }
}
