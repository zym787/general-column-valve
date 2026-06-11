#include "app_main.h"

#include "main.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"

/* User Code Begin 1 */
/* User Code End 1 */

/* External HAL Declarations */

/* DMA Resources */
static uint16_t adc1_buf[32];
static uint8_t usart1_tx_buf[128];
static uint8_t usart1_rx_buf[128];
static uint8_t usart2_tx_buf[128];
static uint8_t usart2_rx_buf[128];

void app_main(void)
{
        /* 初始化 */
        
        /* User Code Begin 2 */

        /* User Code End 2 */

        /* GPIO Configuration */

        /* Terminal Configuration */

        /* User Code Begin 3 */

        /* 实例化 */

        /* 初始化 */

#define V25 1.430

        for (;;) {
                //   volatile float VoteTempSensor = adc1_adc_channel_tempsensor.Read();
                //   volatile uint16_t VoteTempSensormV = VoteTempSensor * 1000;
                //   volatile float VoteVrefint = adc1_adc_channel_vrefint.Read();
                //   volatile uint16_t VoteVrefintmV = VoteVrefint * 1000;
                //   float Temp = (V25 - VoteTempSensor) / 0.0043 + 25;
                HAL_GPIO_TogglePin(LED1_G_GPIO_Port, LED1_G_Pin);
                HAL_Delay(1000);
        }
        /* User Code End 3 */
}