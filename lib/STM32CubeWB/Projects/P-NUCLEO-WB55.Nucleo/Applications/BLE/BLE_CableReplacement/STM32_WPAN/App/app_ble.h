
/**
  ******************************************************************************
  * @file    app_ble.h
  * @author  MCD Application Team
  * @brief   Header for ble application
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
#ifndef __APP_BLE_H
#define __APP_BLE_H

#ifdef __cplusplus
extern "C" {
#endif

  /* Includes ------------------------------------------------------------------*/
#include "hci_tl.h"
  
  /* Exported types ------------------------------------------------------------*/
  typedef enum
  {
    APP_BLE_IDLE,
    APP_BLE_FAST_ADV,
    APP_BLE_LP_ADV,
    APP_BLE_LP_CONNECTING,
    APP_BLE_CONNECTED_SERVER,
    APP_BLE_CONNECTED_CLIENT
  } APP_BLE_ConnStatus_t;

  /* Exported constants --------------------------------------------------------*/
  /* External variables --------------------------------------------------------*/
  /* Exported macros -----------------------------------------------------------*/
  /* Exported functions ------------------------------------------------------- */
  void APP_BLE_Init( void );
  
  
  APP_BLE_ConnStatus_t APP_BLE_Get_Client_Connection_Status(uint16_t Connection_Handle);
  APP_BLE_ConnStatus_t APP_BLE_Get_Server_Connection_Status(void);
  void APP_BLE_Key_Button1_Action(void);
  void APP_BLE_Key_Button2_Action(void);
  void APP_BLE_Key_Button3_Action(void);

#ifdef __cplusplus
}
#endif

#endif /*__APP_BLE_H */
