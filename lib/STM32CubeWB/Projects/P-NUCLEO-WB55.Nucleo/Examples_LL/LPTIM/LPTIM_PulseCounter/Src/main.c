/**
  ******************************************************************************
  * @file    Examples_LL/LPTIM/LPTIM_PulseCounter/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to use the LPTIM in counter mode
  *          using the STM32WBxx LPTIM LL API.
  *          Peripheral initialization done using LL unitary services functions.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32WBxx_LL_Examples
  * @{
  */

/** @addtogroup LPTIM_PulseCounter
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void     SystemClock_Config(void);
void     Enable_LSI(void);
void     Configure_LPTIMCounter(void);
void     EnterStop1Mode(void);
void     LED_Init(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* Configure the system clock to 64 MHz */
  SystemClock_Config();

  /* Initialize LED2 */
  LED_Init();

  /* Enable the LSI Clock */
  Enable_LSI();
  
  /* Configures LPTIM1 in counter mode */
  Configure_LPTIMCounter();
  
  /* Enter STOP 1 mode */
  EnterStop1Mode();
  
  /* Infinite loop */
  while (1)
  {
  }
}

/**
  * @brief  Enable Internal Low Speed Clock (LSI)
  * @param  None
  * @retval Status
  */
void Enable_LSI(void)
{
  /* Enable LSI Oscillator */
  LL_RCC_LSI1_Enable();
  
  while(LL_RCC_LSI1_IsReady() != 1) 
  {
  }
}

/**
  * @brief  Configures the LPTIM1 instance in counter mode. 
  * @note   Peripheral configuration is minimal configuration from reset values.
  *         Thus, some useless LL unitary functions calls below are provided as
  *         commented examples - setting is default configuration from reset.
  * @param  None
  * @retval None
  */
void Configure_LPTIMCounter(void)
{
  /***************************************/
  /* Select LSI as LPTIM1 clock source */
  /***************************************/
  LL_RCC_SetLPTIMClockSource(LL_RCC_LPTIM1_CLKSOURCE_LSI);
  
  /*************************/
  /* GPIO AF configuration */
  /*************************/
  /* Enable the peripheral clock of GPIOs */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
  
  /* GPIO LPTIM1_IN1 configuration */
  LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_0, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetPinPull(GPIOC, LL_GPIO_PIN_0, LL_GPIO_PULL_DOWN);
  LL_GPIO_SetPinSpeed(GPIOC, LL_GPIO_PIN_0, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetAFPin_0_7(GPIOC, LL_GPIO_PIN_0, LL_GPIO_AF_1);

  /*************************************************/
  /* Configure the NVIC to handle LPTIM1 interrupt */
  /*************************************************/
  NVIC_SetPriority(LPTIM1_IRQn, 0);
  NVIC_EnableIRQ(LPTIM1_IRQn);

  /******************************/
  /* Peripheral clocks enabling */
  /******************************/
  /* Enable the timer peripheral clock */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_LPTIM1); 

  /****************************/
  /* LPTIM1 interrupts set-up */
  /****************************/
  /* Enable the Autoreload match Interrupt */
  LL_LPTIM_EnableIT_ARRM(LPTIM1);
  
  /*****************************/
  /* LPTIM1 configuration      */
  /*****************************/
  /* Set the source of the clock used by the LPTIM instance */
  //LL_LPTIM_SetClockSource(LPTIM1, LL_LPTIM_CLK_SOURCE_INTERNAL);
  
  /* Set actual prescaler division ratio */
  //LL_LPTIM_SetPrescaler(LPTIM1, LL_LPTIM_PRESCALER_DIV1);
  
  /* Configure the LPTIM instance output */
  //LL_LPTIM_ConfigOutput(LPTIM1, LL_LPTIM_OUTPUT_WAVEFORM_PWM, LL_LPTIM_OUTPUT_POLARITY_REGULAR);
  
  /* The counter is incremented following each valid clock pulse on the LPTIM external Input1 */
  LL_LPTIM_SetCounterMode(LPTIM1, LL_LPTIM_COUNTER_MODE_EXTERNAL);
  
  /*****************************/
  /* Enable the LPTIM1 counter */
  /*****************************/
  LL_LPTIM_Enable(LPTIM1);
  
  /****************************/
  /* Set the Autoreload value */
  /****************************/
  LL_LPTIM_SetAutoReload(LPTIM1, 1000);
  
  /************************/
  /* Start LPTIM1 counter */
  /************************/
  /* Start the LPTIM counter in continuous mode */
  LL_LPTIM_StartCounter(LPTIM1, LL_LPTIM_OPERATING_MODE_CONTINUOUS);
}

/**
  * @brief  Function to configure and enter in STOP 1 Mode.
  * @param  None
  * @retval None
  */
void EnterStop1Mode(void)
{
  /* Set low-power mode "Stop" */
  LL_PWR_SetPowerMode(LL_PWR_MODE_STOP1);
  
  /* Set low-power mode "Stop" of CPU2 */
  /* Note: On STM32WB, both CPU1 and CPU2 must be in stop mode to set the entire system in stop mode */
  LL_C2_PWR_SetPowerMode(LL_PWR_MODE_STOP1);
  
  /* Set SLEEPDEEP bit of Cortex System Control Register */
  LL_LPM_EnableDeepSleep();

  /* Request Wait For Interrupt */
  __WFI();
}

/**
  * @brief  Initialize LED2.
  * @param  None
  * @retval None
  */
void LED_Init(void)
{
  /* Enable the LED2 Clock */
  LED2_GPIO_CLK_ENABLE();

  /* Configure IO in output push-pull mode to drive external LED2 */
  LL_GPIO_SetPinMode(LED2_GPIO_PORT, LED2_PIN, LL_GPIO_MODE_OUTPUT);
  /* Reset value is LL_GPIO_OUTPUT_PUSHPULL */
  //LL_GPIO_SetPinOutputType(LED2_GPIO_PORT, LED2_PIN, LL_GPIO_OUTPUT_PUSHPULL);
  /* Reset value is LL_GPIO_SPEED_FREQ_LOW */
  //LL_GPIO_SetPinSpeed(LED2_GPIO_PORT, LED2_PIN, LL_GPIO_SPEED_FREQ_LOW);
  /* Reset value is LL_GPIO_PULL_NO */
  //LL_GPIO_SetPinPull(LED2_GPIO_PORT, LED2_PIN, LL_GPIO_PULL_NO);
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = MSI
  *            SYSCLK(Hz)                     = 64000000
  *            HCLK(Hz)                       = 64000000
  *            HCLK1 Prescaler                = 1
  *            HCKL2 Prescaler                = 2
  *            HCKLS Prescaler                = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 4000000
  *            PLL_M                          = 1
  *            PLL_N                          = 32
  *            PLL_R                          = 2
  *            Flash Latency(WS)              = 4
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_3);
  /* HSI configuration and activation */
  LL_RCC_MSI_Enable();
  while(LL_RCC_MSI_IsReady() != 1) 
  {
  }

  /* Main PLL configuration and activation */
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_MSI, LL_RCC_PLLM_DIV_1, 32, LL_RCC_PLLR_DIV_2);
  LL_RCC_PLL_Enable();
  LL_RCC_PLL_EnableDomain_SYS();
  while(LL_RCC_PLL_IsReady() != 1) 
  {
  }

  /* Sysclk activation on the main PLL */
  /* Set CPU1 prescaler*/
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

  /* Set CPU2 prescaler*/
  LL_C2_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_2);

  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) 
  {
  }

  /* Set AHB SHARED prescaler*/
  LL_RCC_SetAHB4Prescaler(LL_RCC_SYSCLK_DIV_1);

  /* Set APB1 prescaler*/
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);

  /* Set APB2 prescaler*/
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

  /* Set systick to 1ms in using frequency set to 64MHz */
  /* This frequency can be calculated through LL RCC macro */
  /* ex: __LL_RCC_CALC_PLLCLK_FREQ(__LL_RCC_CALC_MSI_FREQ(LL_RCC_MSIRANGESEL_RUN, LL_RCC_MSIRANGE_6), 
                                  LL_RCC_PLLM_DIV_1, 32, LL_RCC_PLLR_DIV_2)*/
  LL_Init1msTick(64000000);

  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(64000000);
}

/******************************************************************************/
/*   USER IRQ HANDLER TREATMENT                                               */
/******************************************************************************/
/**
  * @brief  LPTimer Autoreload match interrupt processing
  * @param  None
  * @retval None
  */
void LPTimerAutoreloadMatch_Callback(void)
{
  LL_GPIO_TogglePin(LED2_GPIO_PORT, LED2_PIN);  
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */
