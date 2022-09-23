/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    custom_stm.c
  * @author  MCD Application Team
  * @brief   ustom Example Service.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020-2021 STMicroelectronics.
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
#include "common_blesvc.h"
#include "custom_stm.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef struct{
  uint16_t  CustomMy_P2PsHdle;                    /**< My_P2P_Server handle */
  uint16_t  CustomLed_CHdle;                  /**< My_LED_Char handle */
  uint16_t  CustomSwitch_CHdle;                  /**< My_Switch_Char handle */
  uint16_t  CustomMy_HrsHdle;                    /**< My_Heart_Rate handle */
  uint16_t  CustomHrs_MHdle;                  /**< My_HRS_Meas handle */
  uint16_t  CustomHrs_SlHdle;                  /**< My_Sensor_Loc handle */
  uint16_t  CustomHrs_CtrlpHdle;                  /**< My_HRS_CTRL_Point handle */
}CustomContext_t;

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private defines -----------------------------------------------------------*/
#define UUID_128_SUPPORTED  1

#if (UUID_128_SUPPORTED == 1)
#define BM_UUID_LENGTH  UUID_TYPE_128
#else
#define BM_UUID_LENGTH  UUID_TYPE_16
#endif

#define BM_REQ_CHAR_SIZE    (3)

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
#define CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET         2
#define CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET              1
/* USER CODE BEGIN PM */
#define CUSTOM_STM_HRS_CNTL_POINT_RESET_ENERGY_EXPENDED      (0x01)
#define CUSTOM_STM_HRS_CNTL_POINT_VALUE_IS_SUPPORTED         (0x00)
#define CUSTOM_STM_HRS_CNTL_POINT_VALUE_NOT_SUPPORTED        (0x80)
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static const uint8_t SizeLed_C=2;
static const uint8_t SizeSwitch_C=2;
static const uint8_t SizeHrs_M=5;
static const uint8_t SizeHrs_Sl=1;
static const uint8_t SizeHrs_Ctrlp=1;
/**
 * START of Section BLE_DRIVER_CONTEXT
 */
PLACE_IN_SECTION("BLE_DRIVER_CONTEXT") static CustomContext_t CustomContext;

/**
 * END of Section BLE_DRIVER_CONTEXT
 */

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static SVCCTL_EvtAckStatus_t Custom_STM_Event_Handler(void *pckt);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
/* USER CODE BEGIN PFD */

/* USER CODE END PFD */

/* Private functions ----------------------------------------------------------*/

#define COPY_UUID_128(uuid_struct, uuid_15, uuid_14, uuid_13, uuid_12, uuid_11, uuid_10, uuid_9, uuid_8, uuid_7, uuid_6, uuid_5, uuid_4, uuid_3, uuid_2, uuid_1, uuid_0) \
do {\
    uuid_struct[0] = uuid_0; uuid_struct[1] = uuid_1; uuid_struct[2] = uuid_2; uuid_struct[3] = uuid_3; \
        uuid_struct[4] = uuid_4; uuid_struct[5] = uuid_5; uuid_struct[6] = uuid_6; uuid_struct[7] = uuid_7; \
            uuid_struct[8] = uuid_8; uuid_struct[9] = uuid_9; uuid_struct[10] = uuid_10; uuid_struct[11] = uuid_11; \
                uuid_struct[12] = uuid_12; uuid_struct[13] = uuid_13; uuid_struct[14] = uuid_14; uuid_struct[15] = uuid_15; \
}while(0)

/* Hardware Characteristics Service */
/*
 The following 128bits UUIDs have been generated from the random UUID
 generator:
 D973F2E0-B19E-11E2-9E96-0800200C9A66: Service 128bits UUID
 D973F2E1-B19E-11E2-9E96-0800200C9A66: Characteristic_1 128bits UUID
 D973F2E2-B19E-11E2-9E96-0800200C9A66: Characteristic_2 128bits UUID
 */
#define COPY_MY_P2P_SERVER_UUID(uuid_struct)          COPY_UUID_128(uuid_struct,0x00,0x00,0xfe,0x40,0xcc,0x7a,0x48,0x2a,0x98,0x4a,0x7f,0x2e,0xd5,0xb3,0xe5,0x8f)
#define COPY_MY_LED_CHAR_UUID(uuid_struct)    COPY_UUID_128(uuid_struct,0x00,0x00,0xfe,0x41,0x8e,0x22,0x45,0x41,0x9d,0x4c,0x21,0xed,0xae,0x82,0xed,0x19)
#define COPY_MY_SWITCH_CHAR_UUID(uuid_struct)    COPY_UUID_128(uuid_struct,0x00,0x00,0xfe,0x42,0x8e,0x22,0x45,0x41,0x9d,0x4c,0x21,0xed,0xae,0x82,0xed,0x19)

/* USER CODE BEGIN PF */

/* USER CODE END PF */

/**
 * @brief  Event handler
 * @param  Event: Address of the buffer holding the Event
 * @retval Ack: Return whether the Event has been managed or not
 */
static SVCCTL_EvtAckStatus_t Custom_STM_Event_Handler(void *Event)
{
  SVCCTL_EvtAckStatus_t return_value;
  hci_event_pckt *event_pckt;
  evt_blecore_aci *blecore_evt;
  aci_gatt_attribute_modified_event_rp0 *attribute_modified;
  aci_gatt_write_permit_req_event_rp0   *write_perm_req;
  Custom_STM_App_Notification_evt_t     Notification;
  /* USER CODE BEGIN Custom_STM_Event_Handler_1 */
  /* read_req is useful if Characteristic property = CHAR_PROP_READ 
                           Gatt Event Mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP are defined, so:
                           BLE core event ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE must be considered*/
  /* aci_gatt_read_permit_req_event_rp0    *read_req; */ 
  /* USER CODE END Custom_STM_Event_Handler_1 */

  return_value = SVCCTL_EvtNotAck;
  event_pckt = (hci_event_pckt *)(((hci_uart_pckt*)Event)->data);

  switch(event_pckt->evt)
  {
    case HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE:
      blecore_evt = (evt_blecore_aci*)event_pckt->data;
      switch(blecore_evt->ecode)
      {
        case ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE:
          /* USER CODE BEGIN EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_BEGIN */

          /* USER CODE END EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_BEGIN */
          attribute_modified = (aci_gatt_attribute_modified_event_rp0*)blecore_evt->data;
          if(attribute_modified->Attr_Handle == (CustomContext.CustomSwitch_CHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /* USER CODE BEGIN CUSTOM_STM_Service_1_Char_2 */
            /**
            *  Manage My_Switch_Char Characteristic, Notify descriptor
            */
            /* USER CODE END CUSTOM_STM_Service_1_Char_2 */
            switch(attribute_modified->Attr_Data[0])
            {
              /* USER CODE BEGIN CUSTOM_STM_Service_1_Char_2_attribute_modified */

              /* USER CODE END CUSTOM_STM_Service_1_Char_2_attribute_modified */

              /* Disabled Notification management */
              case (!(COMSVC_Notification)):
                /* USER CODE BEGIN CUSTOM_STM_Service_1_Char_2_Disabled_BEGIN */

                /* USER CODE END CUSTOM_STM_Service_1_Char_2_Disabled_BEGIN */
                Notification.Custom_Evt_Opcode = CUSTOM_STM_SWITCH_C_NOTIFY_DISABLED_EVT;
                Custom_STM_App_Notification(&Notification);
                /* USER CODE BEGIN CUSTOM_STM_Service_1_Char_2_Disabled_END */

                /* USER CODE END CUSTOM_STM_Service_1_Char_2_Disabled_END */
                break;

              /* Enabled Notification management */
              case COMSVC_Notification:
                /* USER CODE BEGIN CUSTOM_STM_Service_1_Char_2_COMSVC_Notification_BEGIN */

                /* USER CODE END CUSTOM_STM_Service_1_Char_2_COMSVC_Notification_BEGIN */
                Notification.Custom_Evt_Opcode = CUSTOM_STM_SWITCH_C_NOTIFY_ENABLED_EVT;
                Custom_STM_App_Notification(&Notification);
                /* USER CODE BEGIN CUSTOM_STM_Service_1_Char_2_COMSVC_Notification_END */

                /* USER CODE END CUSTOM_STM_Service_1_Char_2_COMSVC_Notification_END */
                break;

              default:
                /* USER CODE BEGIN CUSTOM_STM_Service_1_Char_2_default */

                /* USER CODE END CUSTOM_STM_Service_1_Char_2_default */
              break;
            }
          }  /* if(attribute_modified->Attr_Handle == (CustomContext.CustomSwitch_CHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))*/

          else if(attribute_modified->Attr_Handle == (CustomContext.CustomHrs_MHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /* USER CODE BEGIN CUSTOM_STM_Service_2_Char_1 */
            /**
             *  Manage My_HRS_Meas Characteristic, Notify descriptor
            */
            /* USER CODE END CUSTOM_STM_Service_2_Char_1 */
            switch(attribute_modified->Attr_Data[0])
            {
              /* USER CODE BEGIN CUSTOM_STM_Service_2_Char_1_attribute_modified */

              /* USER CODE END CUSTOM_STM_Service_2_Char_1_attribute_modified */

              /* Disabled Notification management */
              case (!(COMSVC_Notification)):
                /* USER CODE BEGIN CUSTOM_STM_Service_2_Char_1_Disabled_BEGIN */

                /* USER CODE END CUSTOM_STM_Service_2_Char_1_Disabled_BEGIN */
                Notification.Custom_Evt_Opcode = CUSTOM_STM_HRS_M_NOTIFY_DISABLED_EVT;
                Custom_STM_App_Notification(&Notification);
                /* USER CODE BEGIN CUSTOM_STM_Service_2_Char_1_Disabled_END */

                /* USER CODE END CUSTOM_STM_Service_2_Char_1_Disabled_END */
                break;

              /* Enabled Notification management */
              case COMSVC_Notification:
                /* USER CODE BEGIN CUSTOM_STM_Service_2_Char_1_COMSVC_Notification_BEGIN */

                /* USER CODE END CUSTOM_STM_Service_2_Char_1_COMSVC_Notification_BEGIN */
                Notification.Custom_Evt_Opcode = CUSTOM_STM_HRS_M_NOTIFY_ENABLED_EVT;
                Custom_STM_App_Notification(&Notification);
                /* USER CODE BEGIN CUSTOM_STM_Service_2_Char_1_COMSVC_Notification_END */

                /* USER CODE END CUSTOM_STM_Service_2_Char_1_COMSVC_Notification_END */
                break;

              default:
                /* USER CODE BEGIN CUSTOM_STM_Service_2_Char_1_default */

                /* USER CODE END CUSTOM_STM_Service_2_Char_1_default */
              break;
            }
          }  /* if(attribute_modified->Attr_Handle == (CustomContext.CustomHrs_MHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))*/

          else if(attribute_modified->Attr_Handle == (CustomContext.CustomLed_CHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /* USER CODE BEGIN CUSTOM_STM_Service_1_Char_1_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
            /**
            *  Manage My_LED_Char Characteristic Write
            */
            Notification.Custom_Evt_Opcode = CUSTOM_STM_LED_C_WRITE_NO_RESP_EVT;
            Notification.DataTransfered.Length=attribute_modified->Attr_Data_Length;
            Notification.DataTransfered.pPayload=attribute_modified->Attr_Data;
            Custom_STM_App_Notification(&Notification);  
            /* USER CODE END CUSTOM_STM_Service_1_Char_1_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
          } /* if(attribute_modified->Attr_Handle == (CustomContext.CustomLed_CHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/
          /* USER CODE BEGIN EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */

          /* USER CODE END EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */
          break;

        case ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE :
          /* USER CODE BEGIN EVT_BLUE_GATT_READ_PERMIT_REQ_BEGIN */

          /* USER CODE END EVT_BLUE_GATT_READ_PERMIT_REQ_BEGIN */
          /* USER CODE BEGIN EVT_BLUE_GATT_READ_PERMIT_REQ_END */

          /* USER CODE END EVT_BLUE_GATT_READ_PERMIT_REQ_END */
          break;

        case ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE:
          /* USER CODE BEGIN EVT_BLUE_GATT_WRITE_PERMIT_REQ_BEGIN */

          /* USER CODE END EVT_BLUE_GATT_WRITE_PERMIT_REQ_BEGIN */
          write_perm_req = (aci_gatt_write_permit_req_event_rp0*)blecore_evt->data;
          if(write_perm_req->Attribute_Handle == (CustomContext.CustomHrs_CtrlpHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /* Allow or reject a write request from a client using aci_gatt_write_resp(...) function */
            /*USER CODE BEGIN CUSTOM_STM_Service_2_Char_3_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE */
            /**
            *  Manage My_HRS_CTRL_Point Characteristic Write
            *  Configure Heart Rate CtrlPoint Characteristic to enable a Client to write control points to a Server to control behavior.
            *  Support for this characteristic is mandatory if the Server supports the Energy Expended feature. 
            */

            if (write_perm_req->Data[0] == CUSTOM_STM_HRS_CNTL_POINT_RESET_ENERGY_EXPENDED)
            {
              /* received a correct value for My_HRS_CTRL_Point char */
              aci_gatt_write_resp(write_perm_req->Connection_Handle,
                                  write_perm_req->Attribute_Handle,
                                  0x00,                                                     /* write_status = 0 (no error))*/
                                  (uint8_t)CUSTOM_STM_HRS_CNTL_POINT_VALUE_IS_SUPPORTED,    /* err_code */
                                  write_perm_req->Data_Length,
                                  (uint8_t *)&write_perm_req->Data[0]);

              /**
               * Notify the application to Reset The Energy Expended Value
               */
              Notification.Custom_Evt_Opcode = CUSTOM_STM_HRS_CTRLP_WRITE_EVT;
              Custom_STM_App_Notification(&Notification);
            }
            else
            {
              /* received value of HRM control point char is incorrect */
              aci_gatt_write_resp(write_perm_req->Connection_Handle,
                                  write_perm_req->Attribute_Handle,
                                  0x1,                                                      /* write_status = 1 (error))*/
                                  (uint8_t)CUSTOM_STM_HRS_CNTL_POINT_VALUE_NOT_SUPPORTED,   /* err_code */
                                  write_perm_req->Data_Length,
                                  (uint8_t *)&write_perm_req->Data[0]);
            }
            
            /*USER CODE END CUSTOM_STM_Service_2_Char_3_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE*/
          } /*if(write_perm_req->Attribute_Handle == (CustomContext.CustomHrs_CtrlpHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

          /* USER CODE BEGIN EVT_BLUE_GATT_WRITE_PERMIT_REQ_END */

          /* USER CODE END EVT_BLUE_GATT_WRITE_PERMIT_REQ_END */
          break;
        /* USER CODE BEGIN BLECORE_EVT */

        /* USER CODE END BLECORE_EVT */
        default:
          /* USER CODE BEGIN EVT_DEFAULT */

          /* USER CODE END EVT_DEFAULT */
          break;
      }
      /* USER CODE BEGIN EVT_VENDOR*/

      /* USER CODE END EVT_VENDOR*/
      break; /* HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE */

      /* USER CODE BEGIN EVENT_PCKT_CASES*/

      /* USER CODE END EVENT_PCKT_CASES*/

    default:
      /* USER CODE BEGIN EVENT_PCKT*/

      /* USER CODE END EVENT_PCKT*/
      break;
  }

  /* USER CODE BEGIN Custom_STM_Event_Handler_2 */

  /* USER CODE END Custom_STM_Event_Handler_2 */

  return(return_value);
}/* end Custom_STM_Event_Handler */

/* Public functions ----------------------------------------------------------*/

/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void SVCCTL_InitCustomSvc(void)
{

  Char_UUID_t  uuid;
  /* USER CODE BEGIN SVCCTL_InitCustomSvc_1 */

  /* USER CODE END SVCCTL_InitCustomSvc_1 */

  /**
   *  Register the event handler to the BLE controller
   */
  SVCCTL_RegisterSvcHandler(Custom_STM_Event_Handler);

  /*
   *          My_P2P_Server
   *
   * Max_Attribute_Records = 1 + 2*2 + 1*no_of_char_with_notify_or_indicate_property + 1*no_of_char_with_broadcast_property
   * service_max_attribute_record = 1 for My_P2P_Server +
   *                                2 for My_LED_Char +
   *                                2 for My_Switch_Char +
   *                                1 for My_Switch_Char configuration descriptor +
   *                              = 6
   */

  COPY_MY_P2P_SERVER_UUID(uuid.Char_UUID_128);
  aci_gatt_add_service(UUID_TYPE_128,
                       (Service_UUID_t *) &uuid,
                       PRIMARY_SERVICE,
                       6,
                       &(CustomContext.CustomMy_P2PsHdle));

  /**
   *  My_LED_Char
   */
  COPY_MY_LED_CHAR_UUID(uuid.Char_UUID_128);
  aci_gatt_add_char(CustomContext.CustomMy_P2PsHdle,
                    UUID_TYPE_128, &uuid,
                    SizeLed_C,
                    CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RESP,
                    ATTR_PERMISSION_NONE,
                    GATT_NOTIFY_ATTRIBUTE_WRITE,
                    0x10,
                    CHAR_VALUE_LEN_VARIABLE,
                    &(CustomContext.CustomLed_CHdle));
  /**
   *  My_Switch_Char
   */
  COPY_MY_SWITCH_CHAR_UUID(uuid.Char_UUID_128);
  aci_gatt_add_char(CustomContext.CustomMy_P2PsHdle,
                    UUID_TYPE_128, &uuid,
                    SizeSwitch_C,
                    CHAR_PROP_NOTIFY,
                    ATTR_PERMISSION_NONE,
                    GATT_NOTIFY_ATTRIBUTE_WRITE,
                    0x10,
                    CHAR_VALUE_LEN_VARIABLE,
                    &(CustomContext.CustomSwitch_CHdle));

  /*
   *          My_Heart_Rate
   *
   * Max_Attribute_Records = 1 + 2*3 + 1*no_of_char_with_notify_or_indicate_property + 1*no_of_char_with_broadcast_property
   * service_max_attribute_record = 1 for My_Heart_Rate +
   *                                2 for My_HRS_Meas +
   *                                2 for My_Sensor_Loc +
   *                                2 for My_HRS_CTRL_Point +
   *                                1 for My_HRS_Meas configuration descriptor +
   *                              = 8
   */

  uuid.Char_UUID_16 = 0x180d;
  aci_gatt_add_service(UUID_TYPE_16,
                       (Service_UUID_t *) &uuid,
                       PRIMARY_SERVICE,
                       8,
                       &(CustomContext.CustomMy_HrsHdle));

  /**
   *  My_HRS_Meas
   */
  uuid.Char_UUID_16 = 0x2a37;
  aci_gatt_add_char(CustomContext.CustomMy_HrsHdle,
                    UUID_TYPE_16, &uuid,
                    SizeHrs_M,
                    CHAR_PROP_NOTIFY,
                    ATTR_PERMISSION_NONE,
                    GATT_DONT_NOTIFY_EVENTS,
                    0x10,
                    CHAR_VALUE_LEN_VARIABLE,
                    &(CustomContext.CustomHrs_MHdle));
  /**
   *  My_Sensor_Loc
   */
  uuid.Char_UUID_16 = 0x2a38;
  aci_gatt_add_char(CustomContext.CustomMy_HrsHdle,
                    UUID_TYPE_16, &uuid,
                    SizeHrs_Sl,
                    CHAR_PROP_READ,
                    ATTR_PERMISSION_NONE,
                    GATT_DONT_NOTIFY_EVENTS,
                    0x10,
                    CHAR_VALUE_LEN_CONSTANT,
                    &(CustomContext.CustomHrs_SlHdle));
  /**
   *  My_HRS_CTRL_Point
   */
  uuid.Char_UUID_16 = 0x2a39;
  aci_gatt_add_char(CustomContext.CustomMy_HrsHdle,
                    UUID_TYPE_16, &uuid,
                    SizeHrs_Ctrlp,
                    CHAR_PROP_WRITE,
                    ATTR_PERMISSION_NONE,
                    GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
                    0x10,
                    CHAR_VALUE_LEN_CONSTANT,
                    &(CustomContext.CustomHrs_CtrlpHdle));

  /* USER CODE BEGIN SVCCTL_InitCustomSvc_2 */

  /* USER CODE END SVCCTL_InitCustomSvc_2 */

  return;
}

/**
 * @brief  Characteristic update
 * @param  CharOpcode: Characteristic identifier
 * @param  Service_Instance: Instance of the service to which the characteristic belongs
 *
 */
tBleStatus Custom_STM_App_Update_Char(Custom_STM_Char_Opcode_t CharOpcode, uint8_t *pPayload)
{
  tBleStatus result = BLE_STATUS_INVALID_PARAMS;
  /* USER CODE BEGIN Custom_STM_App_Update_Char_1 */

  /* USER CODE END Custom_STM_App_Update_Char_1 */

  switch(CharOpcode)
  {

    case CUSTOM_STM_LED_C:
      result = aci_gatt_update_char_value(CustomContext.CustomMy_P2PsHdle,
                                          CustomContext.CustomLed_CHdle,
                                          0, /* charValOffset */
                                          SizeLed_C, /* charValueLen */
                                          (uint8_t *)  pPayload);
      /* USER CODE BEGIN CUSTOM_STM_Service_1_Char_1*/

      /* USER CODE END CUSTOM_STM_Service_1_Char_1*/
      break;

    case CUSTOM_STM_SWITCH_C:
      result = aci_gatt_update_char_value(CustomContext.CustomMy_P2PsHdle,
                                          CustomContext.CustomSwitch_CHdle,
                                          0, /* charValOffset */
                                          SizeSwitch_C, /* charValueLen */
                                          (uint8_t *)  pPayload);
      /* USER CODE BEGIN CUSTOM_STM_Service_1_Char_2*/
            /**
            *  Manage My_Switch_Char Characteristic, Notify descriptor
            */
      /* USER CODE END CUSTOM_STM_Service_1_Char_2*/
      break;

    case CUSTOM_STM_HRS_M:
      result = aci_gatt_update_char_value(CustomContext.CustomMy_HrsHdle,
                                          CustomContext.CustomHrs_MHdle,
                                          0, /* charValOffset */
                                          SizeHrs_M, /* charValueLen */
                                          (uint8_t *)  pPayload);
      /* USER CODE BEGIN CUSTOM_STM_Service_2_Char_1*/
            /**
             *  Manage My_HRS_Meas Characteristic, Notify descriptor
            */
      /* USER CODE END CUSTOM_STM_Service_2_Char_1*/
      break;

    case CUSTOM_STM_HRS_SL:
      result = aci_gatt_update_char_value(CustomContext.CustomMy_HrsHdle,
                                          CustomContext.CustomHrs_SlHdle,
                                          0, /* charValOffset */
                                          SizeHrs_Sl, /* charValueLen */
                                          (uint8_t *)  pPayload);
      /* USER CODE BEGIN CUSTOM_STM_Service_2_Char_2*/

      /* USER CODE END CUSTOM_STM_Service_2_Char_2*/
      break;

    case CUSTOM_STM_HRS_CTRLP:
      result = aci_gatt_update_char_value(CustomContext.CustomMy_HrsHdle,
                                          CustomContext.CustomHrs_CtrlpHdle,
                                          0, /* charValOffset */
                                          SizeHrs_Ctrlp, /* charValueLen */
                                          (uint8_t *)  pPayload);
      /* USER CODE BEGIN CUSTOM_STM_Service_2_Char_3*/

      /* USER CODE END CUSTOM_STM_Service_2_Char_3*/
      break;

    default:
      break;
  }

  /* USER CODE BEGIN Custom_STM_App_Update_Char_2 */

  /* USER CODE END Custom_STM_App_Update_Char_2 */

  return result;
}
