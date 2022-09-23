/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Examples_LL/I2C/I2C_TwoBoards_WakeUpFromStop2_IT_Init/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to send/receive bytes over I2C IP using
  *          the STM32WBxx I2C LL API.
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
/**
  * @brief I2C devices settings
  */
/* Timing register value is computed with the STM32CubeMX Tool,
  * Standard Mode @100kHz with I2CCLK = 16 MHz,
  * rise time = 50ns, fall time = 10ns
  * Timing Value = (uint32_t)0x0020098E
  */
#define I2C_TIMING                     0x0020098E

/**
  * @brief Define related to SlaveTransmit process
  */
#define SLAVE_BYTE_TO_SEND       0xA5

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
#ifndef SLAVE_BOARD
/**
  * @brief Variables related to MasterReceive process
  */
uint8_t      aReceiveBuffer[0xF] = {0};
__IO uint8_t ubReceiveIndex      = 0;
__IO uint8_t ubButtonPress       = 0;
#endif /* SLAVE_BOARD */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C3_Init(void);
/* USER CODE BEGIN PFP */
void     LED_On(void);
void     LED_Off(void);
void     LED_Blinking(uint32_t Period);
#ifdef SLAVE_BOARD
void     Configure_PWR(void);
void     EnterSTOP2Mode(void);
#else /* MASTER_BOARD */
void     WaitForUserButtonPress(void);
void     Handle_I2C_Master(void);
#endif /* SLAVE_BOARD */
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
  MX_I2C3_Init();
  /* USER CODE BEGIN 2 */

  /* Set LED2 Off */
  LED_Off();

#ifdef SLAVE_BOARD
  /* Configure Power IP */
  Configure_PWR();

  /* Enter Stop 2 mode */
  EnterSTOP2Mode();
#else /* MASTER_BOARD */

  /* Wait for User push-button (SW1) press to start transfer */
  WaitForUserButtonPress();

  /* Handle I2C3 events (Master) */
  Handle_I2C_Master();
#endif /* SLAVE_BOARD */
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
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  LL_I2C_InitTypeDef I2C_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
  /**I2C3 GPIO Configuration
  PC0   ------> I2C3_SCL
  PC1   ------> I2C3_SDA
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_0|LL_GPIO_PIN_1;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C3);

  /* USER CODE BEGIN I2C3_Init 1 */
  /* (2) Enable the I2C3 peripheral clock and I2C3 clock source ***************/

  /* Set I2C3 clock source as SYSCLK */
  LL_RCC_SetI2CClockSource(LL_RCC_I2C3_CLKSOURCE_HSI);

  /* (3) Configure NVIC for I2C3 **********************************************/

  /* Configure Event IT:
   *  - Set priority for I2C3_EV_IRQn
   *  - Enable I2C3_EV_IRQn
   */
  NVIC_SetPriority(I2C3_EV_IRQn, 0);  
  NVIC_EnableIRQ(I2C3_EV_IRQn);

  /* Configure Error IT:
   *  - Set priority for I2C3_ER_IRQn
   *  - Enable I2C3_ER_IRQn
   */
  NVIC_SetPriority(I2C3_ER_IRQn, 0);  
  NVIC_EnableIRQ(I2C3_ER_IRQn);

  /* USER CODE END I2C3_Init 1 */
  /** I2C Initialization
  */
  I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
  I2C_InitStruct.Timing = 0x00403D5A;
  I2C_InitStruct.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
  I2C_InitStruct.DigitalFilter = 0;
  I2C_InitStruct.OwnAddress1 = 0x5a;
  I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
  I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
  LL_I2C_Init(I2C3, &I2C_InitStruct);
  LL_I2C_EnableAutoEndMode(I2C3);
  LL_I2C_SetOwnAddress2(I2C3, 0, LL_I2C_OWNADDRESS2_NOMASK);
  LL_I2C_DisableOwnAddress2(I2C3);
  LL_I2C_DisableGeneralCall(I2C3);
  LL_I2C_EnableClockStretching(I2C3);
  /* USER CODE BEGIN I2C3_Init 2 */
#ifdef SLAVE_BOARD
  /* Enable Wake Up From Stop */
  LL_I2C_EnableWakeUpFromStop(I2C3);

  /* (5) Enable I2C3 **********************************************************/
  LL_I2C_Enable(I2C3);

  /* (6) Enable I2C3 address match/error interrupts:
   *  - Enable Address Match Interrupt
   *  - Enable Not acknowledge received interrupt
   *  - Enable Error interrupts
   *  - Enable Stop interrupt
   */
  LL_I2C_EnableIT_ADDR(I2C3);
  LL_I2C_EnableIT_NACK(I2C3);
  LL_I2C_EnableIT_ERR(I2C3);
  LL_I2C_EnableIT_STOP(I2C3);
  #else /* MASTER_BOARD */
  LL_I2C_SetTiming(I2C3, I2C_TIMING);
  LL_I2C_EnableIT_RX(I2C3);
  LL_I2C_EnableIT_NACK(I2C3);
  LL_I2C_EnableIT_ERR(I2C3);
  LL_I2C_EnableIT_STOP(I2C3);
  #endif
  /* USER CODE END I2C3_Init 2 */

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
  NVIC_SetPriority(EXTI4_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(EXTI4_IRQn);

}

/* USER CODE BEGIN 4 */
#ifdef SLAVE_BOARD
/**
  * @brief  Function to configure and initialize PWR IP.
  * @param  None
  * @retval None
  */
void Configure_PWR(void)
{
  /* Ensure that HSI is wake-up system clock */ 
  LL_RCC_SetClkAfterWakeFromStop(LL_RCC_STOP_WAKEUPCLOCK_HSI);

  /* Optional: Keeping the HSI16 ON in Stop mode allows to avoid slowing down */
  /* the communication speed because of the HSI16 startup time.               */
  //LL_RCC_HSI_EnableInStopMode();
}

/**
  * @brief  Function to configure and enter in Stop 2 Mode.
  * @param  None
  * @retval None
  */
void EnterSTOP2Mode(void)
{
    /** Request to enter Stop 2 mode:
      *  - Following procedure describe in STM32WBxx Reference Manual
      *  - See PWR part, section Low-power modes, STOP2 mode
      */
    /* Set low-power mode "Stop 2" */
    LL_PWR_SetPowerMode(LL_PWR_MODE_STOP2);
    
    /* Set low-power mode "Stop 2" of CPU2 */
    /* Note: On STM32WB, both CPU1 and CPU2 must be in Stop 2 mode to set the entire system in stop mode */
    LL_C2_PWR_SetPowerMode(LL_PWR_MODE_STOP2);

    /* Set SLEEPDEEP bit of Cortex System Control Register */
    LL_LPM_EnableDeepSleep();  

    /* Request Wait For Interrupt */
    __WFI();
}
#endif /* SLAVE_BOARD */

/**
  * @brief  Turn-on LED2.
  * @param  None
  * @retval None
  */
void LED_On(void)
{
  /* Turn LED2 on */
  LL_GPIO_SetOutputPin(LED2_GPIO_Port, LED2_Pin);
}

/**
  * @brief  Turn-off LED2.
  * @param  None
  * @retval None
  */
void LED_Off(void)
{
  /* Turn LED2 off */
  LL_GPIO_ResetOutputPin(LED2_GPIO_Port, LED2_Pin);
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
  /* Turn LED2 on */
  LL_GPIO_SetOutputPin(LED2_GPIO_Port, LED2_Pin);

  /* Toggle IO in an infinite loop */
  while (1)
  {
    LL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
    LL_mDelay(Period);
  }
}

#ifndef SLAVE_BOARD

#endif /* SLAVE_BOARD */
/******************************************************************************/
/*   IRQ HANDLER TREATMENT Functions                                          */
/******************************************************************************/
#ifdef SLAVE_BOARD
/**
  * @brief  Function called from I2C IRQ Handler when TXIS flag is set
  *         Function is in charge of transmit a byte on I2C lines.
  * @param  None
  * @retval None
  */
void Slave_Ready_To_Transmit_Callback(void)
{
  /* Send the Byte requested by the Master */
  LL_I2C_TransmitData8(I2C3, SLAVE_BYTE_TO_SEND);
}

/**
  * @brief  Function called from I2C IRQ Handler when STOP flag is set
  *         LED2 is On if data are correct.
  * @param  None
  * @retval None
  */
void Slave_Complete_Callback(void)
{
  /* Turn LED2 On:
   *  - Expected bytes have been sent
   *  - Slave Tx sequence completed successfully
   */
  LED_On();
}
#else /* MASTER_BOARD */
/**
  * @brief  Wait for User push-button (SW1) press to start transfer.
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
  /* Turn LED2 off */
  LL_GPIO_ResetOutputPin(LED2_GPIO_Port, LED2_Pin);
}

/**
  * @brief  Function to manage User push-button (SW1)
  * @param  None
  * @retval None
  */
void UserButton_Callback(void)
{
  /* Update User push-button (SW1) variable : to be checked in waiting loop in main program */
  ubButtonPress = 1;
}

/**
  * @brief  This Function handle Master events to perform a transmission process
  * @note  This function is composed in one step :
  *        -1- Initiate a Start condition to the Slave device
  * @param  None
  * @retval None
  */
void Handle_I2C_Master(void)
{
  /* (1) Initiate a Start condition to the Slave device ***********************/

  /* Master Generate Start condition for a read request:
   *  - to the Slave with a 7-Bit SLAVE_OWN_ADDRESS
   *  - with a auto stop condition generation when receive 1 byte
   */
  LL_I2C_HandleTransfer(I2C3, SLAVE_OWN_ADDRESS, LL_I2C_ADDRSLAVE_7BIT, 1, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);
}

/**
  * @brief  Function called from I2C IRQ Handler when RXNE flag is set
  *         Function is in charge of reading byte received on I2C lines.
  * @param  None
  * @retval None
  */
void Master_Reception_Callback(void)
{
  /* Read character in Receive Data register.
  RXNE flag is cleared by reading data in RXDR register */
  aReceiveBuffer[ubReceiveIndex++] = LL_I2C_ReceiveData8(I2C3);
}

/**
  * @brief  Function called from I2C IRQ Handler when STOP flag is set
  *         Function is in charge of checking data received,
  *         LED2 is On if data are correct.
  * @param  None
  * @retval None
  */
void Master_Complete_Callback(void)
{
  /* Read Received character. RXNE flag is cleared by reading of RXDR register */
  if(aReceiveBuffer[ubReceiveIndex-1] == SLAVE_BYTE_TO_SEND)
  {
    /* Turn LED2 On:
     *  - Expected byte has been received
     *  - Master Rx sequence completed successfully
     */
    LED_On();
  }
  else
  {
    /* Call Error function */
    Error_Callback();
  }
}
#endif /* SLAVE_BOARD */

/**
  * @brief  Function called in case of error detected in I2C IT Handler
  * @param  None
  * @retval None
  */
void Error_Callback(void)
{
#ifdef SLAVE_BOARD
  /* Disable I2C3_EV_IRQn */
  NVIC_DisableIRQ(I2C3_EV_IRQn);
#else
  /* Disable I2C3_EV_IRQn */
  NVIC_DisableIRQ(I2C3_EV_IRQn);
#endif /* SLAVE_BOARD */

  /* Disable I2C3_ER_IRQn */
#ifdef SLAVE_BOARD
  NVIC_DisableIRQ(I2C3_ER_IRQn);
#else
  /* Disable I2C3_ER_IRQn */
  NVIC_DisableIRQ(I2C3_ER_IRQn);
#endif /* SLAVE_BOARD */

  /* Unexpected event : Set LED2 to Blinking mode to indicate error occurs */
  LED_Blinking(LED_BLINK_ERROR);
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
