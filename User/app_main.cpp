#include "app_main.h"
#include <cstdint>

#include "adc.hpp"
#include "cdc_uart.hpp"
#include "libxr.hpp"
#include "libxr_def.hpp"
#include "libxr_rw.hpp"
#include "main.h"
#include "ramfs.hpp"
#include "stm32_adc.hpp"
#include "stm32_can.hpp"
#include "stm32_canfd.hpp"
#include "stm32_dac.hpp"
#include "stm32_flash.hpp"
#include "stm32_gpio.hpp"
#include "stm32_i2c.hpp"
#include "stm32_power.hpp"
#include "stm32_pwm.hpp"
#include "stm32_spi.hpp"
#include "stm32_timebase.hpp"
#include "stm32_uart.hpp"
#include "stm32_usb_dev.hpp"
#include "stm32_watchdog.hpp"
#include "flash_map.hpp"

using namespace LibXR;

/* User Code Begin 1 */
/* User Code End 1 */
// NOLINTBEGIN
// clang-format off
/* External HAL Declarations */
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

/* DMA Resources */
static uint16_t adc1_buf[32];
static uint8_t usart1_tx_buf[128];
static uint8_t usart1_rx_buf[128];
static uint8_t usart2_tx_buf[128];
static uint8_t usart2_rx_buf[128];

extern "C" void app_main(void) {
  // clang-format on
  // NOLINTEND
  /* User Code Begin 2 */
  
  /* User Code End 2 */
  // clang-format off
  // NOLINTBEGIN
  STM32Timebase timebase;
  PlatformInit();
  STM32PowerManager power_manager;

  /* GPIO Configuration */
  STM32GPIO OPTO_IN1(OPTO_IN1_GPIO_Port, OPTO_IN1_Pin);
  STM32GPIO OPTO_IN2(OPTO_IN2_GPIO_Port, OPTO_IN2_Pin);
  STM32GPIO OPTO_IN3(OPTO_IN3_GPIO_Port, OPTO_IN3_Pin);
  STM32GPIO OPTO_IN4(OPTO_IN4_GPIO_Port, OPTO_IN4_Pin);
  STM32GPIO RS485_CTR(RS485_CTR_GPIO_Port, RS485_CTR_Pin);
  STM32GPIO DIR2(DIR2_GPIO_Port, DIR2_Pin);
  STM32GPIO ENA2(ENA2_GPIO_Port, ENA2_Pin);
  STM32GPIO STEP2(STEP2_GPIO_Port, STEP2_Pin);
  STM32GPIO ENA3(ENA3_GPIO_Port, ENA3_Pin);
  STM32GPIO STEP4(STEP4_GPIO_Port, STEP4_Pin);
  STM32GPIO DIR3(DIR3_GPIO_Port, DIR3_Pin);
  STM32GPIO STEP3(STEP3_GPIO_Port, STEP3_Pin);
  STM32GPIO I2C_SDA(I2C_SDA_GPIO_Port, I2C_SDA_Pin);
  STM32GPIO I2C_SCK(I2C_SCK_GPIO_Port, I2C_SCK_Pin);
  STM32GPIO ENA4(ENA4_GPIO_Port, ENA4_Pin);
  STM32GPIO DIR4(DIR4_GPIO_Port, DIR4_Pin);
  STM32GPIO LED1_G(LED1_G_GPIO_Port, LED1_G_Pin);
  STM32GPIO DIPSW2(DIPSW2_GPIO_Port, DIPSW2_Pin);
  STM32GPIO DIPSW4(DIPSW4_GPIO_Port, DIPSW4_Pin);
  STM32GPIO DIPSW1(DIPSW1_GPIO_Port, DIPSW1_Pin);
  STM32GPIO DIPSW8(DIPSW8_GPIO_Port, DIPSW8_Pin);
  STM32GPIO ENA1(ENA1_GPIO_Port, ENA1_Pin);
  STM32GPIO DIR1(DIR1_GPIO_Port, DIR1_Pin);
  STM32GPIO LED2_R(LED2_R_GPIO_Port, LED2_R_Pin);

  STM32ADC adc1(&hadc1, adc1_buf, {ADC_CHANNEL_TEMPSENSOR, ADC_CHANNEL_VREFINT}, 3.3);
  auto adc1_adc_channel_tempsensor = adc1.GetChannel(0);
  UNUSED(adc1_adc_channel_tempsensor);
  auto adc1_adc_channel_vrefint = adc1.GetChannel(1);
  UNUSED(adc1_adc_channel_vrefint);

  STM32PWM pwm_tim4_ch4(&htim4, TIM_CHANNEL_4, false);

  STM32UART usart1(&huart1,
              usart1_rx_buf, usart1_tx_buf, 5);

  STM32UART usart2(&huart2,
              usart2_rx_buf, usart2_tx_buf, 5);

  /* Terminal Configuration */
  STDIO::read_ = usart1.read_port_;
  STDIO::write_ = usart1.write_port_;

  RamFS ramfs("XRobot");
  Terminal<32, 32, 5, 5> terminal(ramfs);
  auto terminal_task = Timer::CreateTask(terminal.TaskFun, &terminal, 10);
  Timer::Add(terminal_task);
  Timer::Start(terminal_task);

  // clang-format on
  // NOLINTEND
  /* User Code Begin 3 */

  /* 实例化 */

  /* 创建STM32Flash对象 */
  STM32Flash flash(FLASH_SECTORS, FLASH_SECTOR_NUMBER);

  /* 写入粒度 4 字节 */
  LibXR::DatabaseRaw<4> database(flash);

  /* 定义一个键用以记录开机次数 */
  LibXR::Database::Key boot_count(database, "boot_count", 0u);

  int counter = 0;
  /* 创建可执行文件 */
  auto exec_file = RamFS::CreateFile<int*>(
    "runme",
    [](int* arg, int argc, char** argv){
      UNUSED(argc);
      UNUSED(argv);
      (*arg)++;
      // LibXR::STDIO::Printf("arg:%d\r\n", *arg);
      return 0;
    },
    &counter
  );
  /* 创建读写文件 */
  auto data_file = RamFS::CreateFile("value", counter);

  /* 创建目录和设备 */
  auto dir = RamFS::CreateDir("mydir");
  auto dev = RamFS::Device("mydev");

  /* 构建文件系统结构 */
  ramfs.Add(data_file);
  ramfs.Add(dir);
  dir.Add(exec_file);
  dir.Add(dev);

  //   /* Bind STDio to USART1 */
//   STDIO::write_ = usart1.write_port_;
//   STDIO::read_ = usart1.read_port_;

    // volatile float v = LibXR::ADC::Read();

    /* 初始化 */
    boot_count.Load();  /* 从Flash中加载当前值,若不存在则使用默认值0 */
    uint32_t current = static_cast<unsigned long>(boot_count);
    boot_count.Set(current + 1);  /* 每次上电或复位是自增并写回Flash */
    // boot_count.
    LibXR::STDIO::Printf("开机次数:%d", static_cast<unsigned long>(boot_count));
    
    LibXR::STDIO::Printf("开机次数:%d  countr:%d", static_cast<unsigned long>(boot_count), counter);

    // 多次运行 exec 文件，修改计数值
    for (int i = 1; i <= 5; ++i) {
      exec_file->Run(0, nullptr);
      ASSERT(data_file->GetData<int>() == i);
    }

    

  #define V25   1.430

  while (true) {

        //   volatile float VoteTempSensor = adc1_adc_channel_tempsensor.Read();
        //   volatile uint16_t VoteTempSensormV = VoteTempSensor * 1000;
        //   volatile float VoteVrefint = adc1_adc_channel_vrefint.Read();
        //   volatile uint16_t VoteVrefintmV = VoteVrefint * 1000;
        //   float Temp = (V25 - VoteTempSensor) / 0.0043 + 25;

        //   LibXR::STDIO::Printf("(%8d)  TempSensor:%4dmV  Vrefint:%4dmV  Temp:%dCelsius\r\n",
        //       LibXR::Timebase::GetMilliseconds(), VoteTempSensormV, VoteVrefintmV, (int)Temp);
        //   LibXR::STDIO::Printf("(%d) TempSensor: %.4fV  Vrefint: %.4fV Temp:%.4fCelsius\r\n",
        //       LibXR::Timebase::GetMilliseconds(), VoteTempSensor, VoteVrefint, Temp);
              
          LED2_R.Write(false);
          Thread::Sleep(500);
          LED2_R.Write(true);
          Thread::Sleep(500);
          XR_LOG_DEBUG("Debug value: %d\r\n", counter);
          LibXR::STDIO::Printf("开机次数:%d  countr:%d", static_cast<unsigned long>(boot_count), counter);


  }
  /* User Code End 3 */
}