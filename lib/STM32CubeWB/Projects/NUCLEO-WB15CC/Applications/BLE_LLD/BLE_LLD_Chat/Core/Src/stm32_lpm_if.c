/**
  ******************************************************************************
  * @file    stm32_lpm_if.c
  * @author  MCD Application Team
  * @brief   Low layer function to enter/exit low power modes (stop, sleep)
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
#include "stm32_lpm_if.h"
#include "stm32_lpm.h"
#include "app_conf.h"
/* USER CODE BEGIN include */

/* USER CODE END include */

/* Exported variables --------------------------------------------------------*/
const struct UTIL_LPM_Driver_s UTIL_PowerDriver = 
{
  PWR_EnterSleepMode,
  PWR_ExitSleepMode,
  
  PWR_EnterStopMode,
  PWR_ExitStopMode, 
  
  PWR_EnterOffMode,
  PWR_ExitOffMode,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN Private_Function_Prototypes */
static void Switch_On_HSI( void );

/* USER CODE END Private_Function_Prototypes */
/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN Private_Typedef */

/* USER CODE END Private_Typedef */
/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Private_Define */

/* USER CODE END Private_Define */
/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Private_Macro */

/* USER CODE END Private_Macro */
/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Private_Variables */

/* USER CODE END Private_Variables */

/**
  * @brief Enters Low Power Off Mode
  * @param none
  * @retval none
  */
void PWR_EnterOffMode( void )
{
/* USER CODE BEGIN PWR_EnterOffMode */

  /**
   * The systick should be disabled for the same reason than when the device enters stop mode because
   * at this time, the device may enter either OffMode or StopMode.
   */
  HAL_SuspendTick();

  /************************************************************************************
   * ENTER OFF MODE
   ***********************************************************************************/
  /*
   * There is no risk to clear all the WUF here because in the current implementation, this API is called
   * in critical section. If an interrupt occurs while in that critical section before that point,
   * the flag is set and will be cleared here but the system will not enter Off Mode
   * because an interrupt is pending in the NVIC. The ISR will be executed when moving out
   * of this critical section
   */
  LL_PWR_ClearFlag_WU( );

  LL_PWR_SetPowerMode( LL_PWR_MODE_STANDBY );

  LL_LPM_EnableDeepSleep( ); /**< Set SLEEPDEEP bit of Cortex System Control Register */

  /**
   * This option is used to ensure that store operations are completed
   */
#if defined ( __CC_ARM)
  __force_stores( );
#endif

  __WFI( );
/* USER CODE END PWR_EnterOffMode */
}

/**
  * @brief Exits Low Power Off Mode
  * @param none
  * @retval none
  */
void PWR_ExitOffMode( void )
{
/* USER CODE BEGIN PWR_ExitOffMode */

  HAL_ResumeTick();

/* USER CODE END PWR_ExitOffMode */
}

/**
  * @brief Enters Low Power Stop Mode
  * @note ARM exists the function when waking up
  * @param none
  * @retval none
  */
void PWR_EnterStopMode( void )
{
/* USER CODE BEGIN PWR_EnterStopMode */
  /**
   * When HAL_DBGMCU_EnableDBGStopMode() is called to keep the debugger active in Stop Mode,
   * the systick shall be disabled otherwise the cpu may crash when moving out from stop mode
   *
   * When in production, the HAL_DBGMCU_EnableDBGStopMode() is not called so that the device can reach best power consumption
   * However, the systick should be disabled anyway to avoid the case when it is about to expire at the same time the device enters
   * stop mode ( this will abort the Stop Mode entry ).
   */
  HAL_SuspendTick();

  /**
   * This function is called from CRITICAL SECTION
   */
  while( LL_HSEM_1StepLock( HSEM, CFG_HW_RCC_SEMID ) );

  if ( ! LL_HSEM_1StepLock( HSEM, CFG_HW_ENTRY_STOP_MODE_SEMID ) )
  {
    if( LL_PWR_IsActiveFlag_C2DS( ) )
    {
      /* Release ENTRY_STOP_MODE semaphore */
      LL_HSEM_ReleaseLock( HSEM, CFG_HW_ENTRY_STOP_MODE_SEMID, 0 );

      Switch_On_HSI( );
    }
  }
  else
  {
    Switch_On_HSI( );
  }

  /* Release RCC semaphore */
  LL_HSEM_ReleaseLock( HSEM, CFG_HW_RCC_SEMID, 0 );

  /************************************************************************************
   * ENTER STOP MODE
   ***********************************************************************************/
  LL_PWR_SetPowerMode(CFG_PWR_MODE_STOP);

  LL_LPM_EnableDeepSleep( ); /**< Set SLEEPDEEP bit of Cortex System Control Register */

  /**
   * This option is used to ensure that store operations are completed
   */
#if defined ( __CC_ARM)
  __force_stores( );
#endif

  __WFI();
/* USER CODE END PWR_EnterStopMode */
}

/**
  * @brief Exits Low Power Stop Mode
  * @param none
  * @retval none
  */
void PWR_ExitStopMode( void )
{
/* USER CODE BEGIN PWR_ExitStopMode */
  /**
   * This function is called from CRITICAL SECTION
   */

  /* Release ENTRY_STOP_MODE semaphore */
  LL_HSEM_ReleaseLock( HSEM, CFG_HW_ENTRY_STOP_MODE_SEMID, 0 );

  while( LL_HSEM_1StepLock( HSEM, CFG_HW_RCC_SEMID ) );
// CCO : Taken from MAC project already validated in low-power
//  if(LL_RCC_GetSysClkSource( ) == LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  if(LL_RCC_GetSysClkSource( ) != LL_RCC_SYS_CLKSOURCE_STATUS_HSE)
  {
    LL_RCC_HSE_Enable( );
    while(!LL_RCC_HSE_IsReady( ));
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSE);
    while (LL_RCC_GetSysClkSource( ) != LL_RCC_SYS_CLKSOURCE_STATUS_HSE);
  }
  else
  {
    /**
     * As long as the current application is fine with HSE as system clock source,
     * there is nothing to do here
     */
  }

  /* Release RCC semaphore */
  LL_HSEM_ReleaseLock( HSEM, CFG_HW_RCC_SEMID, 0 );

  HAL_ResumeTick();

/* USER CODE END PWR_ExitStopMode */
}

/**
  * @brief Enters Low Power Sleep Mode
  * @note ARM exits the function when waking up
  * @param none
  * @retval none
  */
void PWR_EnterSleepMode( void )
{
/* USER CODE BEGIN PWR_EnterSleepMode */

  HAL_SuspendTick();

  /************************************************************************************
   * ENTER SLEEP MODE
   ***********************************************************************************/
  LL_LPM_EnableSleep( ); /**< Clear SLEEPDEEP bit of Cortex System Control Register */

  /**
   * This option is used to ensure that store operations are completed
   */
#if defined ( __CC_ARM)
  __force_stores();
#endif

  __WFI( );
/* USER CODE END PWR_EnterSleepMode */
}

/**
  * @brief Exits Low Power Sleep Mode
  * @note ARM exits the function when waking up
  * @param none
  * @retval none
  */
void PWR_ExitSleepMode( void )
{
/* USER CODE BEGIN PWR_ExitSleepMode */

  HAL_ResumeTick();

/* USER CODE END PWR_ExitSleepMode */
}

/* USER CODE BEGIN Private_Functions */
/**
  * @brief Switch the system clock on HSI
  * @param none
  * @retval none
  */
static void Switch_On_HSI( void )
{
  LL_RCC_HSI_Enable( );
  while(!LL_RCC_HSI_IsReady( ));
  LL_RCC_SetSysClkSource( LL_RCC_SYS_CLKSOURCE_HSI );
  LL_RCC_SetSMPSClockSource(LL_RCC_SMPS_CLKSOURCE_HSI);
  while (LL_RCC_GetSysClkSource( ) != LL_RCC_SYS_CLKSOURCE_STATUS_HSI);
}

/* USER CODE END Private_Functions */

