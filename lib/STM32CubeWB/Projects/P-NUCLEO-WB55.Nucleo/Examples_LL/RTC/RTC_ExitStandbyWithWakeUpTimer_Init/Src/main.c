/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Examples_LL/RTC/RTC_ExitStandbyWithWakeUpTimer_Init/Src/main.c
  * @author  MCD Application Team
  * @brief   This code example shows how to configure the RTC in order to work
  *          with the WUT through the STM32WBxx RTC LL API.
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* Value defined for WUT */
#define RTC_WUT_TIME               ((uint32_t)5)     /* 5 s */

/* Oscillator time-out values */
#define LSI_TIMEOUT_VALUE          ((uint32_t)100)   /* 100 ms */
#define LSE_TIMEOUT_VALUE          ((uint32_t)5000)  /* 5 s */
#define RTC_TIMEOUT_VALUE          ((uint32_t)1000)  /* 1 s */

/* Defines related to Clock configuration */
/* Uncomment to enable the adequate Clock Source */
#define RTC_CLOCK_SOURCE_LSE
/*#define RTC_CLOCK_SOURCE_LSI*/

#ifdef RTC_CLOCK_SOURCE_LSI
/* ck_apre=LSIFreq/(ASYNC prediv + 1) = 256Hz with LSIFreq=32 kHz RC */
#define RTC_ASYNCH_PREDIV          ((uint32_t)0x7F)
/* ck_spre=ck_apre/(SYNC prediv + 1) = 1 Hz */
#define RTC_SYNCH_PREDIV           ((uint32_t)0x00F9)
#endif

#ifdef RTC_CLOCK_SOURCE_LSE
/* ck_apre=LSEFreq/(ASYNC prediv + 1) = 256Hz with LSEFreq=32768Hz */
#define RTC_ASYNCH_PREDIV          ((uint32_t)0x7F)
/* ck_spre=ck_apre/(SYNC prediv + 1) = 1 Hz */
#define RTC_SYNCH_PREDIV           ((uint32_t)0x00FF)
#endif

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
__IO uint8_t ubButtonPress = 0;

#if (USE_TIMEOUT == 1)
uint32_t Timeout = 0; /* Variable used for Timeout management */
#endif /* USE_TIMEOUT */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
void     Configure_RTC(void);
void     EnterStandbyMode(void);
void     LED_Blinking(uint32_t Period);
void     WaitForUserButtonPress(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* System interrupt init*/

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

/* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */

  /* Configure RTC to use WUT */
  Configure_RTC();
  /* Note: On STM32WB, both CPU1 and CPU2 must be in standby mode to set the entire system in standby mode */
  if ((LL_PWR_IsActiveFlag_C1SB() != 0)
      && (LL_PWR_IsActiveFlag_C2SB() != 0)
     )
  {
    /* ##### Run after standby mode ##### */
    /* Clear Standby flag */
    LL_PWR_ClearFlag_C1STOP_C1STB();
    LL_PWR_ClearFlag_C2STOP_C2STB();

    /* Reset RTC Internal Wake up flag */
    LL_RTC_ClearFlag_WUT(RTC);

    /* Slow Toggle LED */
    LED_Blinking(LED_BLINK_SLOW);
  }
  else
  {
    /* ##### Run after normal reset ##### */
    /* Fast Toggle LED in waiting for user-button press */
    WaitForUserButtonPress();

    /* Enable wake-up timer and enter in standby mode */
    EnterStandbyMode();
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  /* HSI configuration and activation */
  LL_RCC_HSI_Enable();
  while(LL_RCC_HSI_IsReady() != 1)
  {
  }

  /* Sysclk activation on the HSI */
  /* Set CPU1 prescaler*/
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

  /* Set CPU2 prescaler*/
  LL_C2_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_2);

  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  {
  }

  /* Set AHB SHARED prescaler*/
  LL_RCC_SetAHB4Prescaler(LL_RCC_SYSCLK_DIV_1);

  /* Set APB1 prescaler*/
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);

  /* Set APB2 prescaler*/
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

  LL_Init1msTick(16000000);

  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(16000000);
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  /* USER CODE BEGIN Smps */

  /* USER CODE END Smps */
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_EXTI_InitTypeDef EXTI_InitStruct = {0};
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);

  /**/
  LL_GPIO_ResetOutputPin(LED2_GPIO_Port, LED2_Pin);

  /**/
  LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTC, LL_SYSCFG_EXTI_LINE4);

  /**/
  EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_4;
  EXTI_InitStruct.Line_32_63 = LL_EXTI_LINE_NONE;
  EXTI_InitStruct.LineCommand = ENABLE;
  EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
  EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;
  LL_EXTI_Init(&EXTI_InitStruct);

  /**/
  LL_GPIO_SetPinPull(USER_BUTTON_GPIO_Port, USER_BUTTON_Pin, LL_GPIO_PULL_UP);

  /**/
  LL_GPIO_SetPinMode(USER_BUTTON_GPIO_Port, USER_BUTTON_Pin, LL_GPIO_MODE_INPUT);

  /**/
  GPIO_InitStruct.Pin = LED2_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LED2_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  NVIC_SetPriority(EXTI4_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),3, 0));
  NVIC_EnableIRQ(EXTI4_IRQn);

}

/* USER CODE BEGIN 4 */

/**
  * Brief   This function configures RTC.
  * Param   None
  * Retval  None
  */
void Configure_RTC(void)
{
  /*##-1- Enables the PWR Clock and Enables access to the backup domain #######*/
  /* To change the source clock of the RTC feature (LSE, LSI), you have to:
     - Enable the power clock
     - Enable write access to configure the RTC clock source (to be done once after reset).
     - Reset the Back up Domain
     - Configure the needed RTC clock source */
  LL_PWR_EnableBkUpAccess();

  /*##-2- Configure LSE/LSI as RTC clock source ###############################*/
#ifdef RTC_CLOCK_SOURCE_LSE
  /* Enable LSE only if disabled.*/
  if (LL_RCC_LSE_IsReady() == 0)
  {
    LL_RCC_ForceBackupDomainReset();
    LL_RCC_ReleaseBackupDomainReset();
    LL_RCC_LSE_Enable();
#if (USE_TIMEOUT == 1)
    Timeout = LSE_TIMEOUT_VALUE;
#endif /* USE_TIMEOUT */
    while (LL_RCC_LSE_IsReady() != 1)
    {
#if (USE_TIMEOUT == 1)
      if (LL_SYSTICK_IsActiveCounterFlag())
      {
        Timeout --;
      }
      if (Timeout == 0)
      {
        /* LSE activation error */
        LED_Blinking(LED_BLINK_ERROR);
      }
#endif /* USE_TIMEOUT */
    }
    LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);

    /*##-3- Enable RTC peripheral Clocks #######################################*/
    /* Enable RTC Clock */
    LL_RCC_EnableRTC();
    LL_APB2_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_RTCAPB);
  }
#elif defined(RTC_CLOCK_SOURCE_LSI)
  if (LL_RCC_LSI1_IsReady() == 0)
  {
    LL_RCC_ForceBackupDomainReset();
    LL_RCC_ReleaseBackupDomainReset();
    LL_RCC_LSI1_Enable();
#if (USE_TIMEOUT == 1)
    Timeout = LSI_TIMEOUT_VALUE;
#endif /* USE_TIMEOUT */
    while (LL_RCC_LSI1_IsReady() != 1)
    {
#if (USE_TIMEOUT == 1)
      if (LL_SYSTICK_IsActiveCounterFlag())
      {
        Timeout --;
      }
      if (Timeout == 0)
      {
        /* LSI1 activation error */
        LED_Blinking(LED_BLINK_ERROR);
      }
#endif /* USE_TIMEOUT */
    }
    LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSI);

    /*##-3- Enable RTC peripheral Clocks #######################################*/
    /* Enable RTC Clock */
    LL_RCC_EnableRTC();
    LL_APB2_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_RTCAPB);
  }

#else
#error "configure clock for RTC"
#endif

  /*##-4- Configure RTC ######################################################*/
  /* Disable RTC registers write protection */
  LL_RTC_DisableWriteProtection(RTC);

  /* Set prescaler according to source clock */
  LL_RTC_SetAsynchPrescaler(RTC, RTC_ASYNCH_PREDIV);
  LL_RTC_SetSynchPrescaler(RTC, RTC_SYNCH_PREDIV);

  /* Disable wake up timer to modify it */
  LL_RTC_WAKEUP_Disable(RTC);

  /* Wait until it is allow to modify wake up reload value */
#if (USE_TIMEOUT == 1)
  Timeout = RTC_TIMEOUT_VALUE;
#endif /* USE_TIMEOUT */

  while (LL_RTC_IsActiveFlag_WUTW(RTC) != 1)
  {
#if (USE_TIMEOUT == 1)
    if (LL_SYSTICK_IsActiveCounterFlag())
    {
      Timeout --;
    }
    if (Timeout == 0)
    {
      /* LSI activation error */
      LED_Blinking(LED_BLINK_ERROR);
    }
#endif /* USE_TIMEOUT */
  }

  /* Setting the Wakeup time to RTC_WUT_TIME s
       If LL_RTC_WAKEUPCLOCK_CKSPRE is selected, the frequency is 1Hz,
       this allows to get a wakeup time equal to RTC_WUT_TIME s
       if the counter is RTC_WUT_TIME */
  LL_RTC_WAKEUP_SetAutoReload(RTC, RTC_WUT_TIME);
  LL_RTC_WAKEUP_SetClock(RTC, LL_RTC_WAKEUPCLOCK_CKSPRE);

  /* Enable RTC registers write protection */
  LL_RTC_EnableWriteProtection(RTC);

}
/**
  * @brief  Function to configure and enter in STANDBY Mode.
  * @param  None
  * @retval None
  */
void EnterStandbyMode(void)
{
  /* ######## ENABLE WUT #################################################*/
  /* Disable RTC registers write protection */
  LL_RTC_DisableWriteProtection(RTC);

  /* Enable wake up counter and wake up interrupt */
  /* Note: Periodic wakeup interrupt should be enabled to exit the device
     from low-power modes.*/
  LL_RTC_EnableIT_WUT(RTC);
  LL_RTC_WAKEUP_Enable(RTC);

  /* Enable RTC registers write protection */
  LL_RTC_EnableWriteProtection(RTC);

  /* ######## ENTER IN STANDBY MODE ######################################*/
  /** Request to enter STANDBY mode
    * Following procedure describe in STM32WBxx Reference Manual
    * See PWR part, section Low-power modes, Standby mode
    */
  /* Reset Internal Wake up flag */
  LL_RTC_ClearFlag_WUT(RTC);

  /* Check that PWR Internal Wake-up is enabled */
  if (LL_PWR_IsEnabledInternWU() == 0)
  {
    /* Need to enable the Internal Wake-up line */
    LL_PWR_EnableInternWU();
  }

  /* Set Standby mode */
  LL_PWR_SetPowerMode(LL_PWR_MODE_STANDBY);

  /* Set Standby mode of CPU2 */
  /* Note: On STM32WB, both CPU1 and CPU2 must be in Standby mode to set the entire system in Standby mode */
  LL_C2_PWR_SetPowerMode(LL_PWR_MODE_STANDBY);

  /* Set SLEEPDEEP bit of Cortex System Control Register */
  LL_LPM_EnableDeepSleep();

  /* This option is used to ensure that store operations are completed */
#if defined ( __CC_ARM)
  __force_stores();
#endif

  /* Request Wait For Interrupt */
  __WFI();
}

/**
  * @brief  Set LED2 to Blinking mode for an infinite loop (toggle period based on value provided as input parameter).
  * @param  Period : Period of time (in ms) between each toggling of LED
  *   This parameter can be user defined values. Pre-defined values used in that example are :
  *     @arg LED_BLINK_FAST : Fast Blinking
  *     @arg LED_BLINK_SLOW : Slow Blinking
  *     @arg LED_BLINK_ERROR : Error specific Blinking
  * @retval None
  */
void LED_Blinking(uint32_t Period)
{
  /* Toggle IO in an infinite loop */
  while (1)
  {
    LL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
    LL_mDelay(Period);
  }
}

/**
  * @brief  Wait for User push-button
  * @param  None
  * @retval None
  */
/*  */
void WaitForUserButtonPress(void)
{
  while (ubButtonPress == 0)
  {
    LL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
    LL_mDelay(LED_BLINK_FAST);
  }
}



/******************************************************************************/
/*   USER IRQ HANDLER TREATMENT                                               */
/******************************************************************************/
/**
  * @brief  Function to manage User button
  * @param  None
  * @retval None
  */
void UserButton_Callback(void)
{
  /* Update User push-button (SW1) variable : to be checked in waiting loop in main function */
  ubButtonPress = 1;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
