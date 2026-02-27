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
 * | Jan 20, 2026 | 1.2 | Drinkto | 新增打印等级及调试打印输出debug_println |
 */

#ifndef __BSP_LOG_H
#define __BSP_LOG_H

// #include "config.h"
#include "bsp_user_lib.h"

// 用于批量注释掉调试输出语句
//"..."代表一个可以变化的参数表。使用保留名"__VA_ARGS__"把参数传递给宏。
// 当宏的调用展开时，实际的参数就传递给 printf()了。
#define FSW_DBG_PRINT  1  // 输出调试信息开关
#define FSW_WORK_PRINT 0  // 输出运行信息开关

#if FSW_DBG_PRINT
#define LV_NA            0
#define LV_INFO          1
#define LV_DBG           2
#define printd(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define prInfo(lv, fmt, ...)                        \
        do {                                        \
                if (lv >= LV_INFO)                  \
                        printf(fmt, ##__VA_ARGS__); \
        } while (0)
#define prDbg(lv, fmt, ...)                         \
        do {                                        \
                if (lv >= LV_DBG)                   \
                        printf(fmt, ##__VA_ARGS__); \
        } while (0)
#else
#define printd(...)
#endif

#if FSW_WORK_PRINT
#define printw(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define printw(...)
#endif

#if DEBUG_BSP == 1
#define bsp_println(format, ...)        printf("[%20s:%4d] BSP(%6d)  " format "\r\n", FILE_NAME2, __LINE__, HAL_GetTick(), ##__VA_ARGS__)
#define bsp_printf(format, ...)         printf("[%20s:%4d] BSP(%6d)  " format, FILE_NAME2, __LINE__, HAL_GetTick(), ##__VA_ARGS__)
#else
#define bsp_println(format, ...)
#define bsp_printf(format, ...)
#endif

extern void bsp_LogPrintfInfo(void);

#endif /* __BSP_LOG_H */
