/**
 * @file      : bsp_modbus.h
 * @brief     : modbus
 *
 * @version   : 1.1
 * @author    : Drinkto
 * @date      : Jan 4, 2026
 *
 * @changelog:
 * | Date | version | Author | Description |
 * | --- | --- | --- | --- |
 * | Jan 4, 2026 | 1.0 | Drinkto | 首次发布 |
 * | Jan 10, 2026 | 1.1 | Drinkto | 分离解析函数,新增支持读03写06保持寄存器,支持波特率可设,提供bsp层接口 |
 */

#ifndef __BSP_MODBUS_H
#define __BSP_MODBUS_H

#define DEBUG_MB    2   ///是否在串口1输出每次232/485串口接收到的数据
#define IGNORE_MB   0   ///是否屏蔽Modbus

/* 保持寄存器起始地址 */
#define REG_HOLDING_START 0x0000

/* 保持寄存器数量(0-199总共200) */
#define REG_HOLDING_NREGS 10

#define MB_BAUDRATE_SETTING_NUM 5 /* 波特率选项数量(0开始):0 1 2 3 4 */

/* 传递Modbus轮询函数 */
// #define bsp_ModbusPoll MODS_Poll

// 数据转换函数指针（处理16位寄存器与实际值的换算，可选，增强灵活性）
// 参数：data_in 输入数据（寄存器值/实际值），is_reg_to_app 方向（true：寄存器→实际值，false：实际值→寄存器）
typedef uint32_t (*Modbus_Reg_Conv_Fun)(uint32_t data_in, bool is_reg_to_app);

// Modbus寄存器映射项结构体（核心：关联所有相关信息）
typedef struct {
        uint16_t reg_addr;             // 寄存器地址（0x0000~0xFFFF）
        void *app_data_ptr;            // 实际值指针（关联app_data中的变量）
        uint16_t reg_count;            // 占用寄存器数量（16位寄存器：1=16位，2=32位；1位寄存器：始终1）
        Modbus_Reg_Conv_Fun conv_fun;  // 数据转换函数（NULL=无需转换）
        const char *desc;              // 寄存器描述（便于调试，如"环境温度(0.1℃)"）
} Modbus_Reg_Map_Item_T;

// Modbus寄存器表（外部声明，Modbus核心遍历使用）
extern const Modbus_Reg_Map_Item_T g_Modbus_Reg_Map[];
extern const uint16_t g_Modbus_Reg_Map_Count;
/* 保持寄存器内容 */
extern uint16_t usRegHoldingBuf[REG_HOLDING_NREGS];

/**
 * @brief     : modbus 波特率枚举
 */
typedef enum
{
    MB_BAUD_4800    = 0U,   /* 4800bps */
    MB_BAUD_9600    = 1U,   /* 9600bps */
    MB_BAUD_19200   = 2U,   /* 19200bps */
    MB_BAUD_38400   = 3U,   /* 38400bps */
    MB_BAUD_115200  = 4U,   /* 115200bps */
    MB_BAUD_NUM
}MB_BAUDRATE_E;

///波特率表
extern uint32_t g_MBBaudRateTable[MB_BAUD_NUM];


extern void bsp_ModbusInit(void);
uint8_t bsp_GetModbusPollFlag(void);
void bsp_SetModbusPollFlag(uint8_t _switch);
// extern void bsp_ModbusPoll(void);
extern void bsp_ModbusTask(void);

#endif // !__BSP_MODBUS_H
