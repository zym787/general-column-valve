/*
 * stepper_lib.h
 * 步进电机驱动库头文件
 */

#ifndef STEPPER_LIB_H
#define STEPPER_LIB_H

#include "main.h"
#include "bsp.h"
#include <stdint.h>
#include <stdlib.h>

#define STEPPER_DEBUG 1

#if STEPPER_DEBUG == 1
#define sl_println(format, ...)                                                \
        printf("[%20s:%4d]STEP(%6d) " format "\r\n", FILE_NAME2, __LINE__,     \
               HAL_GetTick(), ##__VA_ARGS__)
#define sl_printf(format, ...)                                                 \
        printf("[%20s:%4d]STEP(%6d) " format, FILE_NAME2, __LINE__,            \
               HAL_GetTick(), ##__VA_ARGS__)
#else
#define sl_println(format, ...)
#define sl_printf(format, ...)
#endif

typedef enum {
        STEPPER_DISABLE = 0, /* 电机禁用 */
        STEPPER_ENABLE = 1,  /* 电机使能 */
} STEPPER_ENABLE_E;

typedef enum {
        STEPPER_DIR_CW = 0,  /* 顺时针 */
        STEPPER_DIR_CCW = 1, /* 逆时针 */
} STEPPER_DIR_E;

typedef enum {
        STEPPER_ENA_ACT_LOW = 0,  /* 使能信号低电平有效 */
        STEPPER_ENA_ACT_HIGH = 1, /* 使能信号高电平有效 */
} STEPPER_ENA_ACT_E;

typedef enum {
        STEPPER_NOPOSITION = 0, /* 电机未到达目标位置 */
        STEPPER_INPOSITION = 1, /* 电机已到达目标位置 */
} STEPPER_RUN_E;

typedef enum {
        SOFT_PLUSE_OFF = 0, /* 脉冲低电平 */
        SOFT_PLUSE_ON = 1,  /* 脉冲高电平 */
} STEPPER_PULSE_E;

// 最大电机数量
#define MAX_MOTORS 4
/**
 * @brief     : 电机索引枚举
 */
typedef enum {
        STEPPER_1 = 0,
        STEPPER_2 = 1,
        STEPPER_3 = 2,
        STEPPER_4 = 3,
        STEPPER_MAX
} STEPPER_INDEX_E;

#define MINIMUM_TIME_INTERVAL 2500 /* 最小时间间隔,单位微秒 */
#define DEFAULT_STEP_PER_S    10   /* 默认速度 10step/s */
#define MIN_STEP_PER_S        1    /* 最小速度 1step/s */
#define MAX_STEP_PER_S        4000 /* 最大速度 4000step/s */

// 电机控制结构体
typedef struct {
        GPIO_TypeDef *step_port;   /* 步进脉冲GPIO端口 */
        uint16_t step_pin;         /* 步进脉冲GPIO引脚 */
        GPIO_TypeDef *dir_port;    /* 方向控制GPIO端口 */
        uint16_t dir_pin;          /* 方向控制GPIO引脚 */
        GPIO_TypeDef *enable_port; /* 使能控制GPIO端口 */
        uint16_t enable_pin;       /* 使能控制GPIO引脚 */
        int32_t target_pos;        /* 目标位置 */
        int32_t current_pos;       /* 当前位置 */
        int32_t remaining_steps;   /* 剩余步数 */
        STEPPER_DIR_E direction;   /* 方向: 0=正转, 1=反转 */
        uint32_t step_per_s;       /* 当前脉冲间隔(微秒) */
        uint32_t max_step_per_s;   /* 最小脉冲间隔(对应最高速度) */
        uint32_t min_step_per_s;   /* 最大脉冲间隔(对应最低速度) */
        bool is_added;             /* 是否添加到电机列表: 1=已添加, 0=未添加 */
        STEPPER_ENA_ACT_E en_active_high; /* 使能信号有效电平: 1=高电平有效, 0=低电平有效 */
} stepper_motor_t;

// 最大脉冲通道数量
#define MAX_PULSE_CHANNEL 4
/**
 * @brief     : 脉冲通道枚举
 */
typedef enum {
        PULSE_CH1 = 0,
        PULSE_CH2 = 1,
        PULSE_CH3 = 2,
        PULSE_CH4 = 3,
        PULSE_MAX,
} PULSE_CHANNEL_E;

/**
 * @brief     : 物理输出端口定义
 * @details   : 定义了电机的物理输出端口，包括步进脉冲、方向控制和使能控制引脚。
 */
typedef struct {
        GPIO_TypeDef *step_port;   /* 步进脉冲GPIO端口 */
        uint16_t step_pin;         /* 步进脉冲GPIO引脚 */
        GPIO_TypeDef *dir_port;    /* 方向控制GPIO端口 */
        uint16_t dir_pin;          /* 方向控制GPIO引脚 */
        GPIO_TypeDef *enable_port; /* 使能控制GPIO端口 */
        uint16_t enable_pin;       /* 使能控制GPIO引脚 */
        STEPPER_ENABLE_E en_active_level; /* 使能信号有效电平: 1=高电平有效, 0=低电平有效 */
} BIND_GPIO_T;

/**
 * @brief     : 软脉冲生成器定义
 * @details   : 定义了软脉冲生成器的结构体，包括脉冲数、脉冲结果等。
 */
typedef struct {
        volatile uint32_t PulseCount;      /* 脉冲数 */
        volatile STEPPER_PULSE_E PulseRet; /* 脉冲生成的结果 */
} SOFT_PULSE_GENERATOR_T;

/**
 * @brief     : 控制器定义
 * @details   : 定义了控制器的结构体，包括目标位置、当前位置、剩余步数和方向。
 */
typedef struct {
        volatile bool IsRunning; /* 是否正在运行: 1=正在运行, 0=未运行 */
        volatile int32_t TargetPos;       /* 目标位置 */
        volatile int32_t CurrentPos;      /* 当前位置 */
        volatile int32_t RemainSteps;     /* 剩余步数 */
        volatile STEPPER_DIR_E Direction; /* 方向: 0=正转, 1=反转 */
        volatile uint32_t StepPerS;      /* 每秒脉冲数量 1-2000 */
} CONTROLLER_T;

/**
 * @brief     : 软脉冲控制联合体
 * @details   : 定义了软脉冲控制联合体的结构体，包括绑定的实际GPIO端口、是否添加到脉冲通道列表、脉冲生成器和控制器。
 */
typedef struct {
        BIND_GPIO_T BindPort; /* 绑定的实际GPIO端口 */
        bool IsAdded;         /* 是否添加到脉冲通道列表: 1=已添加, 0=未添加 */
        SOFT_PULSE_GENERATOR_T PulseGen;   /* 脉冲生成器 */
        CONTROLLER_T Controller;           /* 控制器 */

        // bool IsRunning; /* 是否正在运行: 1=正在运行, 0=未运行 */
        // volatile uint32_t PulseCount;      /* 脉冲数 */
        // volatile STEPPER_PULSE_E PulseRet; /* 脉冲生成的结果 */
} SOFT_PULSE_CTRL_T;

// 函数声明
void stepper_init(void);
int8_t stepper_add_motor(STEPPER_INDEX_E _index, GPIO_TypeDef *_step_port,
                         uint16_t _step_pin, GPIO_TypeDef *_dir_port,
                         uint16_t _dir_pin, GPIO_TypeDef *_enable_port,
                         uint16_t _enable_pin,
                         STEPPER_ENA_ACT_E _en_active_high);
void stepper_enable_motor(STEPPER_INDEX_E _index, STEPPER_ENABLE_E _enable);
void stepper_set_direction(STEPPER_INDEX_E _index, STEPPER_DIR_E _direction);
void stepper_set_speed_range(STEPPER_INDEX_E _index, uint32_t _min_speed,
                             uint32_t _max_speed);
void stepper_set_speed(STEPPER_INDEX_E _index, uint32_t _speed);
void stepper_set_target_position(STEPPER_INDEX_E _index, int32_t _position,
                                 uint32_t _speed);
int32_t stepper_get_position(STEPPER_INDEX_E _index);
STEPPER_RUN_E stepper_is_finished(STEPPER_INDEX_E _index);
void stepper_stop(STEPPER_INDEX_E _index);
void stepper_stop_all(void);
extern void stepper_timer_callback(void);

extern bool sl_PulseAdd(PULSE_CHANNEL_E _ch, GPIO_TypeDef *_step_port,
                        uint16_t _step_pin, GPIO_TypeDef *_dir_port,
                        uint16_t _dir_pin, GPIO_TypeDef *_enable_port,
                        uint16_t _enable_pin,
                        STEPPER_ENA_ACT_E _en_active_high);
extern void sl_PulseInit(void);
extern void sl_TimerSetSpeed(uint32_t _speed);
extern void sl_PulseStart(PULSE_CHANNEL_E _ch, uint32_t _pulseCount);

#endif /* STEPPER_LIB_H */
