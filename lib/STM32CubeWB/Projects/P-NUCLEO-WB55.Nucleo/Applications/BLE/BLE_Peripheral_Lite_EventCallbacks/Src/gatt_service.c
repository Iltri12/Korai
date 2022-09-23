/**
  ******************************************************************************
* @file    gatt_service.c
* @author  MCD Application Team
* @brief   My Very Own Service (Custom STM)
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
#include "ble_common.h"
#include "ble.h"
#include "gatt_service.h"
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* My Very Own Service and Characteristics UUIDs */
/*
The following 128bits UUIDs have been generated from the random UUID
generator:
D973F2E0-B19E-11E2-9E96-0800200C9A66: Service 128bits UUID
D973F2E1-B19E-11E2-9E96-0800200C9A66: Characteristic_1 128bits UUID
D973F2E2-B19E-11E2-9E96-0800200C9A66: Characteristic_2 128bits UUID
*/
#define COPY_MY_VERY_OWN_SERVICE_UUID(uuid_struct)                 COPY_UUID_128(uuid_struct,0x00,0x00,0xfe,0x40,0xcc,0x7a,0x48,0x2a,0x98,0x4a,0x7f,0x2e,0xd5,0xb3,0xe5,0x8f)
#define COPY_MY_VERY_OWN_WRITE_CHARACTERISTIC_UUID(uuid_struct)    COPY_UUID_128(uuid_struct,0x00,0x00,0xfe,0x41,0x8e,0x22,0x45,0x41,0x9d,0x4c,0x21,0xed,0xae,0x82,0xed,0x19)
#define COPY_MY_VERY_OWN_NOTIFY_CHARACTERISTIC_UUID(uuid_struct)   COPY_UUID_128(uuid_struct,0x00,0x00,0xfe,0x42,0x8e,0x22,0x45,0x41,0x9d,0x4c,0x21,0xed,0xae,0x82,0xed,0x19)

/** Max_Attribute_Records = 2*no_of_char + 1
  * service_max_attribute_record = 1 for My Very Own service +
  *                                2 for My Very Own Write characteristic +
  *                                2 for My Very Own Notify characteristic +
  *                                1 for client char configuration descriptor +
  *                                
  */
#define MY_VERY_OWN_SERVICE_MAX_ATT_RECORDS                8

/* Private macros ------------------------------------------------------------*/
#define COPY_UUID_128(uuid_struct, uuid_15, uuid_14, uuid_13, uuid_12, uuid_11, uuid_10, uuid_9, uuid_8, uuid_7, uuid_6, uuid_5, uuid_4, uuid_3, uuid_2, uuid_1, uuid_0) \
do {\
  uuid_struct[0] = uuid_0; uuid_struct[1] = uuid_1; uuid_struct[2] = uuid_2; uuid_struct[3] = uuid_3; \
    uuid_struct[4] = uuid_4; uuid_struct[5] = uuid_5; uuid_struct[6] = uuid_6; uuid_struct[7] = uuid_7; \
      uuid_struct[8] = uuid_8; uuid_struct[9] = uuid_9; uuid_struct[10] = uuid_10; uuid_struct[11] = uuid_11; \
        uuid_struct[12] = uuid_12; uuid_struct[13] = uuid_13; uuid_struct[14] = uuid_14; uuid_struct[15] = uuid_15; \
}while(0)

/* Private variables ---------------------------------------------------------*/

/**
* START of Section BLE_DRIVER_CONTEXT
*/
PLACE_IN_SECTION("BLE_DRIVER_CONTEXT") MyVeryOwnServiceContext_t myVeryOwnServiceContext;

/**
* END of Section BLE_DRIVER_CONTEXT
*/
/* Private function prototypes -----------------------------------------------*/
/* Functions Definition ------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/

/**
* @brief  Service initialization
* @param  None
* @retval None
*/
void MyVeryOwnService_Init(void)
{
  tBleStatus ret = BLE_STATUS_SUCCESS;
  Char_UUID_t  uuid16;
  
  /**
  *  Add My Very Own Service
  */
  COPY_MY_VERY_OWN_SERVICE_UUID(uuid16.Char_UUID_128);
  ret = aci_gatt_add_service(UUID_TYPE_128,
                       (Service_UUID_t *) &uuid16,
                       PRIMARY_SERVICE,
                       MY_VERY_OWN_SERVICE_MAX_ATT_RECORDS,
                       &(myVeryOwnServiceContext.MyVeryOwnServiceHandle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    Error_Handler(); /* UNEXPECTED */
  }
  
  /**
  *  Add My Very Own Write Characteristic
  */
  COPY_MY_VERY_OWN_WRITE_CHARACTERISTIC_UUID(uuid16.Char_UUID_128);
  ret = aci_gatt_add_char(myVeryOwnServiceContext.MyVeryOwnServiceHandle,
                    UUID_TYPE_128, &uuid16,
                    MY_VERY_OWN_WRITE_CHARACTERISTIC_VALUE_LENGTH,                                   
                    CHAR_PROP_WRITE_WITHOUT_RESP|CHAR_PROP_READ,
                    ATTR_PERMISSION_NONE,
                    GATT_NOTIFY_ATTRIBUTE_WRITE, /* gattEvtMask */
                    10, /* encryKeySize */
                    1, /* isVariable */
                    &(myVeryOwnServiceContext.MyVeryOwnWriteCharacteristicHandle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    Error_Handler(); /* UNEXPECTED */
  }
  
  /**
  *   Add My Very Own Notify Characteristic
  */
  COPY_MY_VERY_OWN_NOTIFY_CHARACTERISTIC_UUID(uuid16.Char_UUID_128);
  ret = aci_gatt_add_char(myVeryOwnServiceContext.MyVeryOwnServiceHandle,
                    UUID_TYPE_128, &uuid16,
                    MY_VERY_OWN_NOTIFY_CHARACTERISTIC_VALUE_LENGTH,
                    CHAR_PROP_NOTIFY,
                    ATTR_PERMISSION_NONE,
                    GATT_NOTIFY_ATTRIBUTE_WRITE, /* gattEvtMask */
                    10, /* encryKeySize */
                    1, /* isVariable: 1 */
                    &(myVeryOwnServiceContext.MyVeryOwnNotifyCharacteristicHandle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    Error_Handler(); /* UNEXPECTED */
  }
  
  return;
} /* MyVeryOwnService_Init() */

/**
* @brief Characteristic update
* @param UUID: UUID of the characteristic
* @param newValueLength: Length of the new value data to be written
* @param pNewValue: Pointer to the new value data 
*/
tBleStatus MyVeryOwnWriteCharacteristic_Update(uint16_t UUID, uint16_t newValueLength, uint8_t *pNewValue)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  
  switch(UUID)
  {
  case MY_VERY_OWN_NOTIFY_CHARACTERISTIC_UUID:
    if (newValueLength <= MY_VERY_OWN_NOTIFY_CHARACTERISTIC_VALUE_LENGTH)
    {
      ret = aci_gatt_update_char_value(myVeryOwnServiceContext.MyVeryOwnServiceHandle,
                                          myVeryOwnServiceContext.MyVeryOwnNotifyCharacteristicHandle,
                                          0, /* charValOffset */
                                          newValueLength, /* charValueLen */
                                          (uint8_t *) pNewValue);
    }
    break;
  default:
    break;
  }
  
  return ret;
}/* end MyVeryOwnWriteCharacteristic_Update() */
