/**
  ******************************************************************************
  * @file    stm32wb5mm_dk_conf.h
  * @author  MCD Application Team
  * @brief   configuration file.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32WB5MM_DK_CONF_H
#define STM32WB5MM_DK_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32wbxx_hal.h"

/* Environmental Sensors usage */
#define USE_ENV_SENSOR_STTS22H_0             1U
#define USE_ENV_SENSOR_LPS22HH_0             0U

/* Motion Sensors usage */
#define USE_MOTION_SENSOR_ISM330DLC_0        0U
#define USE_MOTION_SENSOR_IIS2MDC_0          0U

/* COM  port usage */
#define USE_BSP_COM_FEATURE                  1U
#define USE_COM_LOG                          0U

/* IRQ priorities */
#define BSP_BUTTON_USERx_IT_PRIORITY         0x0FUL
#define PWM_LED_CLOCK_IT_PRIORITY            0x00UL

/* I2C3 Frequeny in Hz  */
#define BUS_I2C3_FREQUENCY                   100000UL /* Frequency of I2C3 = 100 KHz*/

/* Indicates whether or not TCXO is supported by the board
 * 0: TCXO not supported
 * 1: TCXO supported
 */
#define IS_TCXO_SUPPORTED                   0U

/* Indicates whether or not DCDC is supported by the board
 * 0: DCDC not supported
 * 1: DCDC supported
 */
#define IS_DCDC_SUPPORTED                   1U

#define STM32WB5MM_DK_I2C_Init BSP_I2C3_Init
#define STM32WB5MM_DK_I2C_DeInit BSP_I2C3_DeInit
#define STM32WB5MM_DK_I2C_ReadReg BSP_I2C3_ReadReg
#define STM32WB5MM_DK_I2C_WriteReg BSP_I2C3_WriteReg
  
#define STM32WB5MM_DK_GetTick BSP_GetTick

/*Number of millisecond of audio at each DMA interrupt*/
#define N_MS_PER_INTERRUPT               (20U)
 
#define AUDIO_IN_CHANNELS               1
#define AUDIO_IN_SAMPLING_FREQUENCY     16000

#define AUDIO_CHANNELS_OUT              1
#define AUDIO_OUT_SAMPLING_FREQUENCY    16000

#define AUDIO_IN_BUFFER_SIZE            DEFAULT_AUDIO_IN_BUFFER_SIZE  
  
#if (AUDIO_IN_SAMPLING_FREQUENCY == 8000)
#define MAX_DECIMATION_FACTOR 160
#else
#define MAX_DECIMATION_FACTOR 128
#endif

#define MAX_MIC_FREQ                      1280  /*kHz - Maximum PDM clock */
#define MAX_AUDIO_IN_CHANNEL_NBR_PER_IF   1 /* Maximum number of microhpones channels for peripheral interface */
#define MAX_AUDIO_IN_CHANNEL_NBR_TOTAL    1

#ifdef __cplusplus
}
#endif

#endif /* STM32WB5MM_DK_CONF_H */
