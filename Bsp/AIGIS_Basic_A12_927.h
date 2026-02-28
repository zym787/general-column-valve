/**
 * @file      : AIGIS_Basic_A12_929.c
 * @brief     : A12-929Ó²¼þ¶¨ÒåÎÄ¼þ
 *
 * @version   : 1.0
 * @author    : Drinkto
 * @date      : Jan 15, 2025
 *
 * @changelog:
 * | Date | version | Author | Description |
 * | --- | --- | --- | --- |
 * | Jan 15, 2025 | 1.0 | Drinkto | 901µçÂ·°å |
 */
#ifndef __AIGIS_BASIC_A12_929_H
#define __AIGIS_BASIC_A12_929_H

#ifdef __cplusplus
extern "C" {
#endif

/* PCB°æ±¾ */
#define AIGIS_BASIC_BOARD_VR                "A12-927-A&A12-928-A"
/* PCBÃèÊö */
#define AIGIS_BASIC_BOARD_DESCRIPTION       "Control and Display Panel"

/********** A **********/
//ADC
#define ADC_POWER_U_GROUP							(0)		//ADC1
#define ADC_POWER_U_CHANNEL							(0)		//ç¬¬1ç»„
#define ADC_POWER_I_GROUP							(0)		//ADC1
#define ADC_POWER_I_CHANNEL							(1)		//ç¬¬2ç»„
/********** B **********/
/********** BUTTON **********/
/********** BUTTON **********/
/********** BUTTON **********/
//(GPIO)
#define BUTTON_DOWN_GPIO_CLK_ENABLE()				__HAL_RCC_GPIOB_CLK_ENABLE();		//PB13
#define BUTTON_DOWN_GPIO_Port						(GPIOB)
#define BUTTON_DOWN_Pin 							(GPIO_PIN_9)

/********** C **********/
/********** LED **********/
/********** LED **********/
/********** LED **********/
//(GPIO)
#define LED_GPIO_CLK_ENABLE()				__HAL_RCC_GPIOB_CLK_ENABLE();		//PB12
#define LED_GPIO_Port						(GPIOB)
#define LED_GPIO_Pin 						(GPIO_PIN_12)


/********** D **********/
/********** DI-OPTO **********/
/********** DI-OPTO **********/
/********** DI-OPTO **********/
//(GPIO)
#define DI_OPTO_GPIO_CLK_ENABLE()				__HAL_RCC_GPIOB_CLK_ENABLE();		//PB8
#define DI_OPTO_GPIO_Port						(GPIOB)
#define DI_OPTO_Pin 							(GPIO_PIN_8)
#define DI_OPTO_TIM_CLK_ENABLE()				__HAL_RCC_TIM4_CLK_ENABLE()		//TIM4
#define	DI_OPTO_Get_TIM							(TIM4)
#define	DI_OPTO_Get_HTIM						(htim4)
#define DI_OPTO_Get_IRQn						(TIM4_IRQn)		//TIM4ä¸­æ–­

/********** E **********/

/********** F **********/

/********** G **********/

/********** H **********/
/********** HW_ELEC **********/
/********** HW_ELEC **********/
/********** HW_ELEC **********/
//(GPIO)
#define HW_ELEC_AP_GPIO_CLK_ENABLE()				__HAL_RCC_GPIOA_CLK_ENABLE()		//PA7
#define HW_ELEC_AP_GPIO_Port						(GPIOA)
#define HW_ELEC_AP_Pin								(GPIO_PIN_7)
#define HW_ELEC_AM_GPIO_CLK_ENABLE()				__HAL_RCC_GPIOA_CLK_ENABLE()		//PA6
#define HW_ELEC_AM_GPIO_Port						(GPIOA)
#define HW_ELEC_AM_Pin								(GPIO_PIN_6)
#define HW_ELEC_BP_GPIO_CLK_ENABLE()				__HAL_RCC_GPIOA_CLK_ENABLE()		//PA5
#define HW_ELEC_BP_GPIO_Port						(GPIOA)
#define HW_ELEC_BP_Pin								(GPIO_PIN_5)
#define HW_ELEC_BM_GPIO_CLK_ENABLE()				__HAL_RCC_GPIOA_CLK_ENABLE()		//PA4
#define HW_ELEC_BM_GPIO_Port						(GPIOA)
#define HW_ELEC_BM_Pin								(GPIO_PIN_4)
//(AFIO & TIM)
#define HW_ELEC_APWM_GPIO_CLK_ENABLE()		  	  __HAL_RCC_GPIOB_CLK_ENABLE()			//PB11
#define HW_ELEC_APWM_GPIO_Port				    	(GPIOB)
#define HW_ELEC_APWM_Pin					    	(GPIO_PIN_11)
#define HW_ELEC_APWM_CHANNEL				    	(TIM_CHANNEL_4)
#define HW_ELEC_BPWM_GPIO_CLK_ENABLE()      		__HAL_RCC_GPIOB_CLK_ENABLE()		//PB10
#define HW_ELEC_BPWM_GPIO_Port				    	(GPIOB)
#define HW_ELEC_BPWM_Pin					    	(GPIO_PIN_10)
#define HW_ELEC_BPWM_CHANNEL				   	 	(TIM_CHANNEL_3)
#define HW_ELEC_PWM_TIM_CLK_ENABLE()		   	 	__HAL_RCC_TIM2_CLK_ENABLE()			//TIM2
#define	HW_ELEC_PWM_Get_TIM					    	(TIM2)
#define	HW_ELEC_PWM_Get_HTIM				    	(htim2)
#define	HwElec_TIM_AFIO_REMAP				    	__HAL_AFIO_REMAP_TIM2_PARTIAL_2();	//TIM_AFIOç«¯å�£
/********** I **********/

/********** J **********/

/********** K **********/

/********** L **********/

/********** M **********/
/********** MT6816 **********/
/********** MT6816 **********/
/********** MT6816 **********/
//MT6816_SPIé‡‡é›†(GPIO)
#define MT6816_SPI_CS_GPIO_CLK_ENABLE()				__HAL_RCC_GPIOA_CLK_ENABLE()	  //PA15
#define MT6816_SPI_CS_GPIO_Port						(GPIOA)
#define MT6816_SPI_CS_Pin 							(GPIO_PIN_15)
//MT6816_SPIé‡‡é›†(AFIO & SPI)
#define MT6816_SPI_CLK_GPIO_CLK_ENABLE()			__HAL_RCC_GPIOB_CLK_ENABLE()	  //PB3
#define MT6816_SPI_CLK_GPIO_Port					(GPIOB)
#define MT6816_SPI_CLK_Pin 							(GPIO_PIN_3)
#define MT6816_SPI_MISO_GPIO_CLK_ENABLE()			__HAL_RCC_GPIOB_CLK_ENABLE()	//PB4
#define MT6816_SPI_MISO_GPIO_Port					(GPIOB)
#define MT6816_SPI_MISO_Pin 						(GPIO_PIN_4)
#define MT6816_SPI_MOSI_GPIO_CLK_ENABLE()			__HAL_RCC_GPIOB_CLK_ENABLE()	//PB5
#define MT6816_SPI_MOSI_GPIO_Port					(GPIOB)
#define MT6816_SPI_MOSI_Pin 						(GPIO_PIN_5)
#define MT6816_SPI_SPI_CLK_ENABLE()					__HAL_RCC_SPI1_CLK_ENABLE();	  //SPI1
#define MT6816_SPI_Get_SPI							(SPI1)
#define MT6816_SPI_Get_HSPI							(hspi1)
#define	MT6816_SPI_AFIO_REMAP						__HAL_AFIO_REMAP_SPI1_ENABLE();	  //SPI1_AFIOç«¯å�£é‡�æ–°æ˜ å°„
#define	MT6816_SPI_Prescaler						(SPI_BAUDRATEPRESCALER_8)		  //SPI1_8åˆ†é¢‘_9M

/********** Modbus **********/
/********** Modbus **********/
/********** Modbus **********/
//Modbusç«¯å�£(GPIO)
#define Modbus_USART_CLK_ENABLE()                	__HAL_RCC_USART1_CLK_ENABLE()
//#define Modbus_485_USART                             USART1
//#define Modbus_485_USART_CLK_ENABLE()                __HAL_RCC_USART1_CLK_ENABLE();
//#define Modbus_485_USART_BAUDRATE                    115200

//#define RCC_PERIPHCLK_Modbus_USART                	 RCC_PERIPHCLK_USART1
//#define RCC_Modbus_USARTCLKSOURCE_SYSCLK          	 RCC_USART1CLKSOURCE_SYSCLK

#define Modbus_RS485DIR_GPIO_CLK_ENABLE()			__HAL_RCC_GPIOC_CLK_ENABLE()	//PC14
#define Modbus_RS485DIR_GPIO_Port			     	(GPIOC)
#define Modbus_RS485DIR_GPIO_Pin 			      	(GPIO_PIN_14)
#define Modbus_RS485DIR_Set_RX()					(Modbus_RS485DIR_GPIO_Port -> BSRR = Modbus_RS485DIR_GPIO_Pin)
#define Modbus_RS485DIR_Set_TX()			      	(Modbus_RS485DIR_GPIO_Port ->  BRR = Modbus_RS485DIR_GPIO_Pin)
//Modbusç«¯å�£(AFIO & UART)
#define Modbus_UART_TX_GPIO_CLK_ENABLE()			__HAL_RCC_GPIOB_CLK_ENABLE()	//PB6
#define Modbus_UART_TX_GPIO_Port					(GPIOB)
#define Modbus_UART_TX_GPIO_Pin 					(GPIO_PIN_6)
#define Modbus_UART_RX_GPIO_CLK_ENABLE()			__HAL_RCC_GPIOB_CLK_ENABLE()	//PB7
#define Modbus_UART_RX_GPIO_Port					(GPIOB)
#define Modbus_UART_RX_GPIO_Pin 					(GPIO_PIN_7)
#define Modbus_UART_Get_UART						(USART1)
#define Modbus_UART_Get_HUART						(huart1)
#define Modbus_UART_AFIO_REMAP						__HAL_AFIO_REMAP_USART1_ENABLE();	//UART1_AFIOç«¯å�£é‡�æ–°æ˜ å°„
#define Modbus_UART_IRQn							(USART1_IRQn)						//ä¸²å�£1å…¨å±€ä¸­æ–­
//Modbusç«¯å�£
//#define Enabled_MUart1								//ä½¿èƒ½æ··å�ˆä¸²å�£åŠŸèƒ½ç»„
//#define Enabled_Cus_Slave1							//ä½¿èƒ½Cus_Modbusä»ŽæœºåŠŸèƒ½ç»„
//#define Enabled_Cus_Master1							//ä½¿èƒ½Cus_Modbusä¸»æœºåŠŸèƒ½ç»„
//#define Modbus_UART_Get_MUART						(muart1)		//Aigisåº“å®žä¾‹
//#define Modbus_UART_Get_Slave						(cus_slave1)	//Cusåº“å®žä¾‹
//#define Modbus_UART_Get_Master 						(cus_master1)	//Cusåº“å®žä¾‹
////Modbusç«¯å�£(DMA)
//#define Modbus_UART_TX_DMA_CH						(DMA1_Channel4)
//#define Modbus_UART_TX_HDMA							(hdma_usart1_tx)
//#define Modbus_UART_TX_DMA_IRQn						(DMA1_Channel4_IRQn)		//DMA1_CH4ä¸­æ–­å�‘é‡�
//#define Modbus_UART_RX_DMA_CH						(DMA1_Channel5)
//#define Modbus_UART_RX_HDMA							(hdma_usart1_rx)
//#define Modbus_UART_RX_DMA_IRQn						(DMA1_Channel5_IRQn)		//DMA1_CH5ä¸­æ–­å�‘é‡�


/********** P **********/
/********** USART2 **********/
/********** USART2 **********/
/********** USART2 **********/
//USART2ç«¯å�£(AFIO & UART)
#define USART2_TX_GPIO_CLK_ENABLE()			__HAL_RCC_GPIOA_CLK_ENABLE()	//PA2
#define USART2_TX_GPIO_Port					(GPIOA)
#define USART2_TX_GPIO_Pin 					(GPIO_PIN_2)
#define USART2_RX_GPIO_CLK_ENABLE()			__HAL_RCC_GPIOA_CLK_ENABLE()	//PA3
#define USART2_RX_GPIO_Port					(GPIOA)
#define USART2_RX_GPIO_Pin 					(GPIO_PIN_3)
#define USART2_Get_UART						(USART2)
#define USART2_Get_HUART					(huart2)
#define USART2_IRQn							(USART2_IRQn)					//ä¸²å�£2å…¨å±€ä¸­æ–­
#define USART2_BAUDRATE						(115200ul)
//USART2ç«¯å�£(AFIO & UART)
//´®¿Ú²¨ÌØÂÊ
#define DEBUG_USART_BAUDRATE                    115200
//´®¿ÚÒý½Å
#define DEBUG_USART                             USART2
#define DEBUG_USART_CLK_ENABLE()                __HAL_RCC_USART2_CLK_ENABLE();

#define DEBUG_USART_RX_GPIO_PORT                GPIOA
#define DEBUG_USART_RX_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define DEBUG_USART_RX_PIN                      GPIO_PIN_3

#define DEBUG_USART_TX_GPIO_PORT                GPIOA
#define DEBUG_USART_TX_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()
#define DEBUG_USART_TX_PIN                      GPIO_PIN_2

#define DEBUG_USART_IRQHandler                  USART2_IRQHandler
#define DEBUG_USART_IRQ                 		USART2_IRQn

/********** Q **********/

/********** R **********/

/********** S **********/
/********** SIGNAL_PORT **********/
/********** SIGNAL_PORT **********/
/********** SIGNAL_PORT **********/
//SIGNAL_ALERT
#define SIGNAL_CTRL_ALERT_GPIO_CLK_ENABLE()				__HAL_RCC_GPIOB_CLK_ENABLE()	//PB8
#define SIGNAL_CTRL_ALERT_GPIO_Port						(GPIOB)
#define SIGNAL_CTRL_ALERT_GPIO_Pin							(GPIO_PIN_0)
#define Out_ALERT_OFF()		(SIGNAL_CTRL_ALERT_GPIO_Port -> BSRR = SIGNAL_CTRL_ALERT_GPIO_Pin)
#define Out_ALERT_ON()		(SIGNAL_CTRL_ALERT_GPIO_Port -> BRR  = SIGNAL_CTRL_ALERT_GPIO_Pin)
//SIGNAL_ENA
#define SIGNAL_CTRL_ENA_CLK_ENABLE()					__HAL_RCC_GPIOA_CLK_ENABLE()	//PB1
#define SIGNAL_CTRL_ENA_GPIO_Port						(GPIOB)
#define SIGNAL_CTRL_ENA_GPIO_Pin						(GPIO_PIN_1)
//SIGNAL_AB(AFIO)
#define SIGNAL_CTRL_PUL_CLK_ENABLE()					__HAL_RCC_GPIOB_CLK_ENABLE()	//PB2
#define SIGNAL_CTRL_PUL_GPIO_Port						(GPIOB)
#define SIGNAL_CTRL_PUL_GPIO_Pin						(GPIO_PIN_2)

//SIGNAL_COUNT(GPIO)
#define SIGNAL_COUNT_ENA_CLK_ENABLE()				__HAL_RCC_GPIOA_CLK_ENABLE()	//PB0
#define SIGNAL_COUNT_ENA_GPIO_Port					(GPIOB)
#define SIGNAL_COUNT_ENA_Pin						(GPIO_PIN_0)
#define SIGNAL_COUNT_DIR_CLK_ENABLE()				__HAL_RCC_GPIOB_CLK_ENABLE()	//PB2
#define SIGNAL_COUNT_DIR_GPIO_Port					(GPIOB)
#define SIGNAL_COUNT_DIR_Pin						(GPIO_PIN_2)
#define SIGNAL_COUNT_DIR_Get_IRQn					(EXTI9_5_IRQn)	//EXTI7ä¸­æ–­
//SIGNAL_COUNT(AFIO & TIM)
#define SIGNAL_COUNT_PUL_CLK_ENABLE()				__HAL_RCC_GPIOB_CLK_ENABLE()	//PB1
#define SIGNAL_COUNT_PUL_GPIO_Port					(GPIOB)
#define SIGNAL_COUNT_PUL_Pin						(GPIO_PIN_1)
#define SIGNAL_COUNT_TIM_CLK_ENABLE()				__HAL_RCC_TIM3_CLK_ENABLE()		//TIM3
#define	SIGNAL_COUNT_Get_TIM						(TIM3)
#define	SIGNAL_COUNT_Get_HTIM						(htim3)
//SIGNAL_PWM(GPIO)
#define SIGNAL_PWM_ENA_CLK_ENABLE()					__HAL_RCC_GPIOA_CLK_ENABLE()	//PB0
#define SIGNAL_PWM_ENA_GPIO_Port					(GPIOB)
#define SIGNAL_PWM_ENA_Pin							(GPIO_PIN_0)
#define SIGNAL_PWM_DIR_CLK_ENABLE()					__HAL_RCC_GPIOB_CLK_ENABLE()	//PB2
#define SIGNAL_PWM_DIR_GPIO_Port					(GPIOB)
#define SIGNAL_PWM_DIR_Pin							(GPIO_PIN_2)
//SIGNAL_PWM(AFIO & TIM)
#define SIGNAL_PWM_PUL_CLK_ENABLE()					__HAL_RCC_GPIOB_CLK_ENABLE()	//PB1
#define SIGNAL_PWM_PUL_GPIO_Port					(GPIOB)
#define SIGNAL_PWM_PUL_Pin							(GPIO_PIN_1)
#define SIGNAL_PWM_TIM_CLK_ENABLE()					__HAL_RCC_TIM3_CLK_ENABLE()		//TIM3
#define	SIGNAL_PWM_Get_TIM							(TIM3)
#define	SIGNAL_PWM_Get_HTIM							(htim3)
#define SIGNAL_PWM_Get_IRQn							(TIM3_IRQn)		//TIM3ä¸­æ–­
/********** T **********/
//SIGNAL_OPTO(AFIO & TIM)
#define SIGNAL_OPTO_CLK_ENABLE()					__HAL_RCC_GPIOB_CLK_ENABLE()	//PB8
#define SIGNAL_OPTO_GPIO_Port						(GPIOB)
#define SIGNAL_OPTO_Pin								(GPIO_PIN_8)
#define SIGNAL_OPTO_TIM_CLK_ENABLE()				__HAL_RCC_TIM4_CLK_ENABLE()		//TIM3
#define	SIGNAL_OPTO_Get_TIM							(TIM4)
#define	SIGNAL_OPTO_Get_HTIM						(htim4)
#define SIGNAL_OPTO_Get_IRQn						(TIM4_IRQn)		//TIM3ä¸­æ–­
/********** U **********/

/********** V **********/

/********** W **********/

/********** X **********/

/********** Y **********/

/********** Z **********/

#ifdef __cplusplus
}
#endif

#endif
