/**
 * @file      : bsp_log.c
 * @brief     : 打印信息
 *
 * @version   : 1.2
 * @author    : Drinkto
 * @date      : Jan 06, 2026
 *
 * @changelog:
 * | Date | version | Author | Description |
 * | --- | --- | --- | --- |
 * | Jul 10, 2025 | 1.0 | Drinkto | 首次发布 |
 * | Jan 10, 2026 | 1.1 | Drinkto | 使用bsp板级打印宏 |
 * | Jan 20, 2026 | 1.2 | Drinkto | 调整函数名,放弃使用bsp板级打印宏 |
 */

#include "bsp.h"

static void bsp_LogDisplayBanner(void);

/*
*********************************************************************************************************
*	函 数 名: bsp_LogPrintfInfo
*	功能说明: 打印BSP信息, 接上串口线后，打开PC机的超级终端软件可以观察结果
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_LogPrintfInfo(void)
{
        bsp_LogDisplayBanner();

        printf("********************************************************************************\r\n");
        printf("* \r\n"); /* 打印一行空格 */

        /* 检测CPU ID */
        uint32_t CPU_Sn0, CPU_Sn1, CPU_Sn2;

        CPU_Sn0 = HAL_GetUIDw0();
        CPU_Sn1 = HAL_GetUIDw1();
        CPU_Sn2 = HAL_GetUIDw2();

        printf("* CPU       : STM32F103C8T6  LQFP48   Max Freq.%dMHz\r\n", SystemCoreClock / 1000000);

        printf("* UID       : %08X %08X %08X\r\n", CPU_Sn2, CPU_Sn1, CPU_Sn0);

        printf("* PCB       : %s  %s\r\n", BOARD_VERSION_CHAR, BOARD_DESCRIPTION);

        /* 打印ST的HAL库版本 */
        printf("* HAL库版本  : %08X\r\n", HAL_GetHalVersion());
        printf("* \r\n"); /* 打印一行空格 */

        printf("* 软件 版 本 : %s_%d (%04X %04d)\r\n", VERSION_STRING, VERSION_DATA_NUMBER, SOFTWARE_NAME_NUMBER,
               VERSION_NUMBER);

        printf("* 编译 时 间 : %s %s\r\n", __DATE__, __TIME__);

        printf("* 软件 描 述 : %s %s  (%s)\r\n", SOFTWARE_DESCRIPTION, CUSTOMIZE_DESCRIPTION, CONTROL_DESCRIPTION);

        printf("* BSP  版 本 : %s %s\r\n", __STM32F1_BSP_VERSION, __STM32F1_BSP_REV_DATE);

        printf("* \r\n"); /* 打印一行空格 */

        printf("********************************************************************************\r\n");
}

void bsp_LogDisplayBanner(void)
{
        printf("\n");
        printf("\t\t%s\n", "███████ ███████        █████   ██████  ███████ ");
        printf("\t\t%s\n", "   ███  ██            ██   ██ ██       ██      ");
        printf("\t\t%s\n", "  ███   █████   █████ ███████ ██   ███ ███████ ");
        printf("\t\t%s\n", " ███    ██            ██   ██ ██    ██      ██ ");
        printf("\t\t%s\n", "███████ ██            ██   ██  ██████  ███████ ");
        printf("\n");
}
