
/**
 * @file      : cmd_parser.c
 * @brief     : 命令解析器
 *
 * @version   : 1.0
 * @author    : Drinkto
 * @date      : Feb 10, 2026
 *
 * @changelog:
 * | Date | version | Author | Description |
 * | --- | --- | --- | --- |
 * | Feb 10, 2026 | 1.0 | Drinkto | xxx |
 */

#include "cmd_parser.h"
#include "bsp.h"
#include "stepper_lib.h"

// 缓冲区定义
static char cmd_buffer[MAX_CMD_LENGTH];
static uint8_t cmd_pos = 0;
static uint8_t cmd_ready = 0;
static uint32_t parser_time[8] = { 0 };

// 命令表（可根据需要扩展）
static command_def_t commands[] = {
    {"SPD", "Set/Get motor speed (e.g., SPD=20 or SPD)", NULL}, // 将在初始化时设置
    {"POS", "Set/Get motor position (e.g., POS=1000 or POS)", NULL},
    {"MOVE", "Move motor (e.g., MOVE=0,500,10)", NULL}, // 支持多参数
    {"MUL", "Multiply two numbers (e.g., MUL=5,3)", NULL},
    {"PWM", "Set PWM parameters (e.g., PWM=CH1,1000,50)", NULL},
    {"STOP", "Stop motor", NULL},
    {"HELP", "Show all commands", NULL},
    {"", "", NULL} // 结束标记
};

// 内部命令处理函数
static cmd_status_t cmd_spd_handler(cmd_args_t *args, char *response);
static cmd_status_t cmd_pos_handler(cmd_args_t *args, char *response);
static cmd_status_t cmd_move_handler(cmd_args_t *args, char *response);
static cmd_status_t cmd_mul_handler(cmd_args_t *args, char *response);
static cmd_status_t cmd_pwm_handler(cmd_args_t *args, char *response);
static cmd_status_t cmd_stop_handler(cmd_args_t *args, char *response);
static cmd_status_t cmd_help_handler(cmd_args_t *args, char *response);

/**
 * @brief     : 注册自定义命令
 * @param     : name 命令名称
 * @param     : desc 命令描述
 * @param     : handler 命令处理函数
 */
void CmdParser_RegisterCommand(const char *name, const char *desc, cmd_handler_func_t handler)
{
        BSP_Printf(" %8s  -  %s\r\n", name, desc);
        // 查找空槽位
        for (int i = 0; i < sizeof(commands) / sizeof(command_def_t) - 1; i++) {
                if (commands[i].name[0] == '\0' || strcmp(commands[i].name, name) == 0) {
                        commands[i].name = name;
                        commands[i].description = desc;
                        commands[i].handler = handler;
                        return;
                }
        }
}
/**
 * @brief     : 初始化命令解析器
 */
void CmdParser_Init(void)
{
        BSP_Printf(
            "------------------------------------------------------------\r\n");
        // 注册内置命令
        CmdParser_RegisterCommand("SPD", "Set/Get motor speed (e.g., SPD=20 or SPD)", cmd_spd_handler);
        CmdParser_RegisterCommand("POS", "Set/Get motor position (e.g., POS=1000 or POS)", cmd_pos_handler);
        CmdParser_RegisterCommand("MOVE", "Move motor (e.g., MOVE=1000,50 or MOVE=1000,50,1)", cmd_move_handler);
        CmdParser_RegisterCommand("MUL", "Multiply two numbers (e.g., MUL=5,3)", cmd_mul_handler);
        CmdParser_RegisterCommand("PWM", "Set PWM parameters (e.g., PWM=CH1,1000,50)", cmd_pwm_handler);
        CmdParser_RegisterCommand("STOP", "Stop motor", cmd_stop_handler);
        CmdParser_RegisterCommand("HELP", "Show all commands", cmd_help_handler);
        BSP_Printf(
            "------------------------------------------------------------\r\n");
        
        // 清空缓冲区
        memset(cmd_buffer, 0, sizeof(cmd_buffer));
        cmd_pos = 0;
        cmd_ready = 0;
}

/**
 * @brief     : 处理输入字符
 * @param     : ch 输入字符
 */
void CmdParser_ProcessChar(uint8_t ch)
{
        static uint8_t last_was_cr = 0; /* 标记上一个字符是否为CR */

        // LF 处理换行符（命令结束）\n是换行符
        if (ch == '\n') {
                // 如果上一个字符是CR，说明是CRLF序列，忽略LF
                if (last_was_cr) {
                        last_was_cr = 0;
                        return;
                }
                // 否则是单独的LF，处理命令
                if (cmd_pos > 0) {
                        cmd_buffer[cmd_pos] = '\0';
                        cmd_ready = 1;
                        parser_time[0] = HAL_GetTick();
                }
                cmd_pos = 0;
                bsp_println("LF %d", last_was_cr);
                return;
        }

        // CR 处理回车符（命令结束） \r是回车符
        if (ch == '\r') {
                // 如果是CR，检查下一个字符是否是LF
                // 这里我们先处理命令，然后等待可能的LF
                if (cmd_pos > 0) {
                        cmd_buffer[cmd_pos] = '\0';
                        cmd_ready = 1;
                        parser_time[1] = HAL_GetTick();
                }
                cmd_pos = 0;
                last_was_cr = 1; // 标记刚收到CR
                bsp_println("CR %d", last_was_cr);
                return;
        }

        // 如果上一个字符是CR而当前不是LF，则处理之前的命令
        if (last_was_cr) {
                last_was_cr = 0;
                if (cmd_pos > 0) {
                        cmd_buffer[cmd_pos] = '\0';
                        cmd_ready = 1;
                        cmd_pos = 0;
                        parser_time[2] = HAL_GetTick();
                        bsp_println("ready %d", cmd_ready);
                }
        }

        // 忽略退格符以外的控制字符
        if (ch < 32) {
                return;
        }

        // 检查缓冲区溢出
        if (cmd_pos >= MAX_CMD_LENGTH - 1) {
                char overflow_msg[] = "Error: Command too long\r\n";
                CmdParser_SendResponse(overflow_msg);
                cmd_pos = 0;
                last_was_cr = 0;
                return;
        }

        // 添加字符到缓冲区
        cmd_buffer[cmd_pos++] = ch;
}

/**
 * @brief     : 解析命令参数
 * @param     : input 输入字符串
 * @param     : args  解析后的参数结构体指针
 * @return    : uint8_t 错误码
 */
uint8_t CmdParse_Arguments(const char *input, cmd_args_t *args)
{
        uint8_t arg_start = 0;
        uint8_t input_len = strlen(input);
        uint8_t arg_count = 0;

        for (uint8_t i = 0; i <= input_len; i++) {
                if (input[i] == ',' || input[i] == '\0') {
                        if (i > arg_start) {
                                // 提取参数
                                uint8_t len = i - arg_start;
                                if (len >= MAX_ARG_LENGTH)
                                        len = MAX_ARG_LENGTH - 1;

                                strncpy(args->values[arg_count], input + arg_start, len);
                                args->values[arg_count][len] = '\0';

                                arg_count++;
                                if (arg_count >= MAX_ARGS) {
                                        return 1; // 错误：参数过多
                                }
                        }
                        arg_start = i + 1;
                }
        }

        args->count = arg_count;
        return 0; // 成功
}

/**
 * @brief     : 解析并执行命令
 * @param     : cmd_str 输入命令字符串
 * @param     : response 响应缓冲区
 * @param     : resp_size 响应缓冲区大小
 * @return    : cmd_status_t 命令执行状态
 */
cmd_status_t CmdParser_ParseAndExecute(char *cmd_str, char *response, uint8_t resp_size)
{
        // 查找等号分隔符
        char *eq_pos = strchr(cmd_str, '=');
        char cmd_name[MAX_ARG_LENGTH];
        char arg_string[MAX_CMD_LENGTH];

        if (eq_pos != NULL) {
                // 分离命令名和参数字符串
                uint8_t cmd_len = eq_pos - cmd_str;
                if (cmd_len >= MAX_ARG_LENGTH)
                        return CMD_ERROR_INVALID;

                strncpy(cmd_name, cmd_str, cmd_len);
                cmd_name[cmd_len] = '\0';

                // 获取参数字符串
                strcpy(arg_string, eq_pos + 1);
        } else {
                // 没有参数的命令
                strncpy(cmd_name, cmd_str, sizeof(cmd_name) - 1);
                cmd_name[sizeof(cmd_name) - 1] = '\0';
                arg_string[0] = '\0';
        }

        // 转换为大写以便匹配
        for (int i = 0; cmd_name[i]; i++) {
                if (cmd_name[i] >= 'a' && cmd_name[i] <= 'z') {
                        cmd_name[i] = cmd_name[i] - 'a' + 'A';
                }
        }

        // 解析参数
        cmd_args_t args;
        args.count = 0;
        if (strlen(arg_string) > 0) {
                if (CmdParse_Arguments(arg_string, &args) != 0) {
                        return CMD_ERROR_TOO_MANY_ARGS;
                }
        }

        // 查找匹配的命令
        for (int i = 0; commands[i].name[0] != '\0'; i++) {
                if (strcmp(commands[i].name, cmd_name) == 0) {
                        if (commands[i].handler != NULL) {
                                bsp_println("Function %s", commands[i].name);
                                return commands[i].handler(&args, response);
                        }
                }
        }

        return CMD_ERROR_INVALID;
}

// 内置命令处理器实现
static cmd_status_t cmd_spd_handler(cmd_args_t *args, char *response)
{
        static int motor_speed = 0; // 模拟电机速度变量

        if (args->count == 0) {
                // 查询模式
                sprintf(response, "Current speed: %d RPM", motor_speed);
        } else if (args->count == 1) {
                // 设置模式
                int new_speed = atoi(args->values[0]);
                if (new_speed < 0 || new_speed > 1000) {
                        return CMD_ERROR_INVALID_ARG;
                }
                motor_speed = new_speed;
                sprintf(response, "Speed set to: %d RPM", motor_speed);
        } else {
                return CMD_ERROR_INVALID_ARG;
        }
        return CMD_SUCCESS;
}

static cmd_status_t cmd_pos_handler(cmd_args_t *args, char *response)
{
        static int motor_position = 0; // 模拟电机位置变量

        if (args->count == 0) {
                // 查询模式
                sprintf(response, "Current position: %d steps", motor_position);
        } else if (args->count == 1) {
                // 设置模式
                int new_pos = atoi(args->values[0]);
                if (new_pos < -10000 || new_pos > 10000) {
                        return CMD_ERROR_INVALID_ARG;
                }
                motor_position = new_pos;
                sprintf(response, "Position set to: %d steps", motor_position);
        } else {
                return CMD_ERROR_INVALID_ARG;
        }
        return CMD_SUCCESS;
}

static cmd_status_t cmd_move_handler(cmd_args_t *args, char *response)
{
        if (args->count == 0) {
                return CMD_ERROR_MISSING_ARG;
        } else if (args->count == 1) {
                // 单参数：步数
                int steps = atoi(args->values[0]);
                sprintf(response, "1 Moving motor %d steps at default speed", steps);
                // 这里可以调用步进电机驱动函数
                // Stepper_Move(&motor1, steps);
        } else if (args->count == 2) {
                // 两参数：步数，速度
                int channel = atoi(args->values[0]);
                int steps = atoi(args->values[1]);
                sprintf(response, "2 Moving motor[%d] %d steps", channel, steps);
                // 这里可以调用步进电机驱动函数
                // Stepper_SetSpeed(&motor1, speed);
                // Stepper_Move(&motor1, steps);
                // sl_PulseStart(channel, steps);
        } else if (args->count == 3) {
                // 三参数：通道,步数，速度
                uint8_t channel = atoi(args->values[0]);
                int steps = atoi(args->values[1]);
                int speed = atoi(args->values[2]);
                // stepper_set_target_position(channel, steps, speed);
                sl_TimerSetSpeed(speed);
                sl_PulseStart(channel, steps);
                sprintf(response, "3 Moving motor%d  %d steps at %d step/s", channel, steps, speed);
                // 这里可以调用步进电机驱动函数
                // Stepper_MoveAdvanced(&motor1, steps, speed, accel);
        } else {
                return CMD_ERROR_INVALID_ARG;
        }
        return CMD_SUCCESS;
}

static cmd_status_t cmd_mul_handler(cmd_args_t *args, char *response)
{
        if (args->count != 2) {
                return CMD_ERROR_INVALID_ARG;
        }

        int a = atoi(args->values[0]);
        int b = atoi(args->values[1]);
        int result = a * b;

        sprintf(response, "%d * %d = %d", a, b, result);
        return CMD_SUCCESS;
}

static cmd_status_t cmd_pwm_handler(cmd_args_t *args, char *response)
{
        if (args->count < 2) {
                return CMD_ERROR_MISSING_ARG;
        }

        char *channel = args->values[0];
        int freq = atoi(args->values[1]);
        int duty = args->count > 2 ? atoi(args->values[2]) : 50; // 默认50%占空比

        sprintf(response, "Setting PWM on channel %s: %d Hz, %d%% duty cycle", channel, freq, duty);
        // 这里可以调用PWM设置函数
        // SetPWM(channel, freq, duty);
        return CMD_SUCCESS;
}

static cmd_status_t cmd_stop_handler(cmd_args_t *args, char *response)
{
        strcpy(response, "Motor stopped");
        // 这里可以调用步进电机停止函数
        // Stepper_Stop(&motor1);
        return CMD_SUCCESS;
}

static cmd_status_t cmd_help_handler(cmd_args_t *args, char *response)
{
        strcpy(response, "Available commands:\r\n");

        for (int i = 0; commands[i].name[0] != '\0'; i++) {
                char temp[128];
                sprintf(temp, "  %s - %s\r\n", commands[i].name, commands[i].description);
                strcat(response, temp);
        }

        return CMD_SUCCESS;
}
// 发送响应的模拟函数（需根据实际硬件实现）
void CmdParser_SendResponse(const char *response)
{
        // 这里应替换为实际的串口发送函数
        // 例如: HAL_UART_Transmit(&huart1, (uint8_t*)response, strlen(response), HAL_MAX_DELAY);
        BSP_Printf("[%20s:%4d]STEP(%6d) %s\r\n", FILE_NAME2, __LINE__,
                   HAL_GetTick(), response);
}

void CmdParser_ExecuteIfReady(void)
{
        if (!cmd_ready)
                return;
        parser_time[3] = HAL_GetTick();

        char response[128];
        cmd_status_t result = CmdParser_ParseAndExecute(cmd_buffer, response, sizeof(response));
        parser_time[4] = HAL_GetTick();

        switch (result) {
        case CMD_SUCCESS:
                strcat(response, "\r\n**OK**\r\n");
                break;
        case CMD_ERROR_INVALID:
                sprintf(response, "Error: Unknown command '%s'\r\n", cmd_buffer);
                break;
        case CMD_ERROR_MISSING_ARG:
                sprintf(response, "Error: Missing argument for command '%s'\r\n", cmd_buffer);
                break;
        case CMD_ERROR_INVALID_ARG:
                sprintf(response, "Error: Invalid argument for command '%s'\r\n", cmd_buffer);
                break;
        case CMD_ERROR_TOO_MANY_ARGS:
                sprintf(response, "Error: Too many arguments for command '%s'\r\n", cmd_buffer);
                break;
        default:
                sprintf(response, "Error: Command execution failed\r\n");
                break;
        }

        parser_time[5] = HAL_GetTick();
        CmdParser_SendResponse(response);
        parser_time[6] = HAL_GetTick();
        cmd_ready = 0;
        cmd_pos = 0;
        // for (uint8_t i = 0; i < 8; i++)
        // {
        //         bsp_println("time %d  %d", i, parser_time[i]);
        // }
}
