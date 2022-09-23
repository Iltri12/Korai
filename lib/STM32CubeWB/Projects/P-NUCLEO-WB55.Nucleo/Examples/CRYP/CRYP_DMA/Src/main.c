/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    CRYP/CRYP_DMA/Src/main.c
  * @author  MCD Application Team
  * @brief   This example provides a short description of how to use the CRYPTO
  *          peripheral to encrypt and decrypt data using CRYP Algorithm with
  *          AES ECB chaining mode.
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

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CRYP_HandleTypeDef hcryp1;
__ALIGN_BEGIN static const uint32_t pKeyAES1[4] __ALIGN_END = {
                            0x2B7E1516,0x28AED2A6,0xABF71588,0x09CF4F3C};
DMA_HandleTypeDef hdma_aes1_in;
DMA_HandleTypeDef hdma_aes1_out;

/* USER CODE BEGIN PV */
/* Private define ------------------------------------------------------------*/

#define KEY_SIZE          128 /* Key size in bits */

/* The size of the plaintext in Words */
#define PLAINTEXT_SIZE          ((uint32_t)16)
#define AES_TEXT_SIZE           ((uint32_t)16)

#define ECB               1
#define CBC               2
#define CTR               3

#if (USE_VCP_CONNECTION == 1)
/**
  * @brief Defines related to Timeout to uart tranmission
  */
#define UART_TIMEOUT_VALUE  1000 /* 1 Second */

/* UART handler declaration */
UART_HandleTypeDef     UartHandle;

/**
  * @brief  Retargets the C library printf function to the USARTx.
  * @param  ch: character to send
  * @param  f: pointer to file (not used)
  * @retval The character transmitted
  */
#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
int __io_putchar(int ch)
#else
int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the UART and Loop until the end of transmission */
  HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, UART_TIMEOUT_VALUE);

  return ch;
}

void BSP_COM_Init(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Peripheral clock enable */
  __HAL_RCC_USART1_CLK_ENABLE();
  
  __HAL_RCC_GPIOB_CLK_ENABLE();
  /**USART1 GPIO Configuration    
  PB6     ------> USART1_TX
  PB7     ------> USART1_RX 
  */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Configure the UART peripheral                                        */
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* UART configured as follows:
      - Word Length = 8 Bits
      - Stop Bit = One Stop bit
      - Parity = None
      - BaudRate = 115200 baud
      - Hardware flow control disabled (RTS and CTS signals) */
  UartHandle.Instance                    = USART1;
  UartHandle.Init.BaudRate               = 115200;
  UartHandle.Init.WordLength             = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits               = UART_STOPBITS_1;
  UartHandle.Init.Parity                 = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl              = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode                   = UART_MODE_TX_RX;
  UartHandle.Init.HwFlowCtl              = UART_HWCONTROL_NONE;
  UartHandle.Init.OverSampling           = UART_OVERSAMPLING_16;
  UartHandle.Init.OneBitSampling         = UART_ONE_BIT_SAMPLE_DISABLE;
  UartHandle.Init.ClockPrescaler         = UART_PRESCALER_DIV1;
  UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  
  if(HAL_UART_DeInit(&UartHandle) != HAL_OK)
  {
    Error_Handler();
  }
  
  if(HAL_UART_Init(&UartHandle) != HAL_OK)
  {
    Error_Handler();
  }  
}
#endif

/* Plaintext */
uint32_t aPlaintext[AES_TEXT_SIZE] =
                        { 0x6BC1BEE2 ,0x2E409F96 ,0xE93D7E11 ,0x7393172A ,
                          0xAE2D8A57 ,0x1E03AC9C ,0x9EB76FAC ,0x45AF8E51 ,
                          0x30C81C46 ,0xA35CE411 ,0xE5FBC119 ,0x1A0A52EF ,
                          0xF69F2445 ,0xDF4F9B17 ,0xAD2B417B ,0xE66C3710};


/* Ciphertext */
uint32_t aEncryptedtextExpected[AES_TEXT_SIZE] =
                          {0x3AD77BB4 ,0x0D7A3660 ,0xA89ECAF3 ,0x2466EF97 ,
                           0xF5D3D585 ,0x03B9699D ,0xE785895A ,0x96FDBAAF ,
                           0x43B1CD7F ,0x598ECE23 ,0x881B00E3 ,0xED030688 ,
                           0x7B0C785E ,0x27E8AD3F ,0x82232071 ,0x04725DD4};

/* Used for storing the encrypted text */
uint32_t aEncryptedText[AES_TEXT_SIZE] = {0};

/* Used for storing the decrypted text */
uint32_t aDecryptedText[PLAINTEXT_SIZE] = {0};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_DMA_Init(void);
static void MX_AES1_Init(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
static void Display_PlainData(uint32_t datalength);
static void Display_EncryptedData(uint8_t mode, uint16_t keysize, uint32_t datalength);
static void Display_DecryptedData(uint8_t mode, uint16_t keysize, uint32_t datalength);
#if defined(__GNUC__)
extern void initialise_monitor_handles(void);
#endif

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
#if defined(__GNUC__)
  initialise_monitor_handles(); 
#endif

  /* STM32WBxx HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user 
         can eventually implement his proper time base source (a general purpose 
         timer for example or other time source), keeping in mind that Time base 
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_DMA_Init();
  MX_AES1_Init();
  /* USER CODE BEGIN 2 */

  /* Configure LEDs */
  BSP_LED_Init(LED2);
  BSP_LED_Init(LED3);

#if (USE_VCP_CONNECTION == 1)
	/* Configure the virtual com port */
  BSP_COM_Init(&UartHandle);
#endif

   /*#######################################################################*/
   /*                                                                       */
   /*##-         DMA-based AES 128 ECB encryption              #############*/
   /*                                                                       */   
   /*#######################################################################*/
  /*##- Configure the CRYP peripheral ######################################*/
  if (HAL_CRYP_DeInit(&hcryp1) != HAL_OK)
  {
    Error_Handler();
  }
  
  hcryp1.Instance = AES1;
  hcryp1.Init.DataType      = CRYP_DATATYPE_32B;
  hcryp1.Init.KeySize       = CRYP_KEYSIZE_128B;
  hcryp1.Init.Algorithm     = CRYP_AES_ECB;
  hcryp1.Init.pKey          = (uint32_t *)pKeyAES1;
  hcryp1.Init.DataWidthUnit = CRYP_DATAWIDTHUNIT_WORD;

  if (HAL_CRYP_Init(&hcryp1) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  
  /* Display Plain Data*/
  Display_PlainData(PLAINTEXT_SIZE);

  HAL_Delay(1);
  
  if (HAL_CRYP_Encrypt_DMA(&hcryp1, aPlaintext, PLAINTEXT_SIZE, aEncryptedText) != HAL_OK)
  {
    /* Processing Error */
    Error_Handler(); 
  }

  /*  Before starting a new process, the current state of the peripheral is checked; 
      as long as the state is not set back to READY, no new ciphering processing
      can be started.
      For simplicity's sake, this example is just waiting till the end of the 
      process, but application may perform other tasks while cihering operation
      is ongoing. */ 
  while (HAL_CRYP_GetState(&hcryp1) != HAL_CRYP_STATE_READY)
  {
  }
  
  /* Display encrypted Data */
  Display_EncryptedData(ECB, 128, AES_TEXT_SIZE);
  
  /*##- Compare the encrypted text with the expected one #####################*/
  if(memcmp(aEncryptedText, aEncryptedtextExpected, PLAINTEXT_SIZE*4) != 0)
  {
    Error_Handler();
  }
  else
  {
    /* Correct encryption */
  }
  
   /*#######################################################################*/
   /*                                                                       */
   /*##-         DMA-based AES 128 ECB decryption              #############*/
   /*                                                                       */   
   /*#######################################################################*/
  /* Deinitialize Crypto peripheral */
  HAL_CRYP_DeInit(&hcryp1);
  
  
  if(HAL_CRYP_Init(&hcryp1) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler(); 
  }
  
  if(HAL_CRYP_Decrypt_DMA(&hcryp1, aEncryptedtextExpected, PLAINTEXT_SIZE, aDecryptedText) != HAL_OK)
  {
    /* Processing Error */
    Error_Handler(); 
  }
  
  /*  Before starting a new process, the current state of the peripheral is checked; 
      as long as the state is not set back to READY, no new ciphering processing
      can be started.
      For simplicity's sake, this example is just waiting till the end of the 
      process, but application may perform other tasks while cihering operation
      is ongoing. */ 
  while (HAL_CRYP_GetState(&hcryp1) != HAL_CRYP_STATE_READY)
  {
  }
          
  /* Display decrypted Data */
  Display_DecryptedData(ECB, 128, PLAINTEXT_SIZE);

  /*##- Compare the decrypted text with the expected one #####################*/
  if(memcmp(aDecryptedText, aPlaintext, PLAINTEXT_SIZE*4) != 0)
  {
    Error_Handler();
  }
  else
  {
    /* Correct decryption */
  }
  
  printf("======================================================\n");
  printf("\n\r DMA-based AES 128 ECB encryption/decryption done.\n");
  printf("\n\r No issue detected.\n");

  /* Turn LED2 on */
  BSP_LED_On(LED2);
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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 32;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV5;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the SYSCLKSource, HCLK, PCLK1 and PCLK2 clocks dividers
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK4|RCC_CLOCKTYPE_HCLK2
                              |RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK2Divider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.AHBCLK4Divider = RCC_SYSCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief AES1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_AES1_Init(void)
{

  /* USER CODE BEGIN AES1_Init 0 */

  /* USER CODE END AES1_Init 0 */

  /* USER CODE BEGIN AES1_Init 1 */

  /* USER CODE END AES1_Init 1 */
  hcryp1.Instance = AES1;
  hcryp1.Init.DataType = CRYP_DATATYPE_32B;
  hcryp1.Init.KeySize = CRYP_KEYSIZE_128B;
  hcryp1.Init.pKey = (uint32_t *)pKeyAES1;
  hcryp1.Init.Algorithm = CRYP_AES_ECB;
  hcryp1.Init.DataWidthUnit = CRYP_DATAWIDTHUNIT_WORD;
  hcryp1.Init.KeyIVConfigSkip = CRYP_KEYIVCONFIG_ALWAYS;
  if (HAL_CRYP_Init(&hcryp1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN AES1_Init 2 */

  /* USER CODE END AES1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMAMUX1_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMA1_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);

}

/* USER CODE BEGIN 4 */

/**
  * @brief  Display Plain Data
  * @param  datalength: length of the data to display
  * @retval None
  */
static void Display_PlainData(uint32_t datalength)
{
  uint32_t BufferCounter = 0;
  uint32_t count = 0;
  uint8_t * ptr = (uint8_t *)aPlaintext;

  printf("\n\r =============================================================\n\r");
  printf(" ================== Crypt Using HW Cryp  =====================\n\r");
  printf(" =============================================================\n\r");
  printf(" ---------------------------------------\n\r");
  printf(" Plain Data :\n\r");
  printf(" ---------------------------------------\n\r");

  for (BufferCounter = 0; BufferCounter < datalength*4; BufferCounter++)
  {
    printf("[0x%02X]", *ptr++);
    count++;

    if (count == 16)
    {
      count = 0;
      printf("  Block %lu \n\r", BufferCounter / 16);
    }
  }
}

/**
  * @brief  Display Encrypted Data
  * @param  mode: chaining mode
  * @param  keysize: AES key size used
  * @param  datalength: length of the data to display
  * @retval None
  */
static void Display_EncryptedData(uint8_t mode, uint16_t keysize, uint32_t datalength)
{
  uint32_t BufferCounter = 0;
  uint32_t count = 0;
  uint8_t * ptr = (uint8_t *)aEncryptedText;

  printf("\n\r =======================================\n\r");
  printf(" Encrypted Data with AES %d  Mode  ", keysize);

  if (mode == ECB)
  {
    printf("ECB\n\r");
  }
  else if (mode == CBC)
  {
    printf("CBC\n\r");
  }
  else /* if(mode == CTR)*/
  {
    printf("CTR\n\r");
  }

  printf(" ---------------------------------------\n\r");

  for (BufferCounter = 0; BufferCounter < datalength*4; BufferCounter++)
  {
    printf("[0x%02X]", *ptr++);

    count++;
    if (count == 16)
    {
      count = 0;
      printf(" Block %lu \n\r", BufferCounter / 16);
    }
  }
}

/**
  * @brief  Display Decrypted Data
  * @param  mode: chaining mode
  * @param  keysize: AES key size used
  * @param  datalength: length of the data to display
  * @retval None
  */
static void Display_DecryptedData(uint8_t mode, uint16_t keysize, uint32_t datalength)
{
  uint32_t BufferCounter = 0;
  uint32_t count = 0;
  uint8_t * ptr = (uint8_t *)aDecryptedText;

  printf("\n\r =======================================\n\r");
  printf(" Decrypted Data with AES %d  Mode  ", keysize);

  if (mode == ECB)
  {
    printf("ECB\n\r");
  }
  else if (mode == CBC)
  {
    printf("CBC\n\r");
  }
  else /* if(mode == CTR)*/
  {
    printf("CTR\n\r");
  }

  printf(" ---------------------------------------\n\r");

  for (BufferCounter = 0; BufferCounter < datalength*4; BufferCounter++)
  {
    printf("[0x%02X]", *ptr++);
    count++;

    if (count == 16)
    {
      count = 0;
      printf(" Block %lu \n\r", BufferCounter / 16);
    }
  }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  printf("\n\r Error Detected...\n ");  

  while(1)
  {
    /* Toggle LED3 */
    BSP_LED_Toggle(LED3);
    HAL_Delay(200);
  }
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

