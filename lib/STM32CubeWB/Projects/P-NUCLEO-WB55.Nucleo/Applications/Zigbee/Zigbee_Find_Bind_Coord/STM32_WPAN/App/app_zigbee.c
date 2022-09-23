/* Copyright [2020 - 2020] Exegin Technologies Limited. All rights reserved. */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : App/app_zigbee.c
  * Description        : Zigbee Application.
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
#include "app_common.h"
#include "app_entry.h"
#include "dbg_trace.h"
#include "app_zigbee.h"
#include "zigbee_interface.h"
#include "shci.h"
#include "stm_logging.h"
#include "app_conf.h"
#include "stm32wbxx_core_interface_def.h"
#include "zigbee_types.h"
#include "stm32_seq.h"

/* Private includes -----------------------------------------------------------*/
#include <assert.h>
#include "zcl/zcl.h"
#include "zcl/general/zcl.onoff.h"
#include "zcl/general/zcl.identify.h"
#include "zcl/general/zcl.scenes.h"
#include "zcl/security/zcl.ias_wd.h"
#include "zcl/se/zcl.message.h"
#include "zcl/general/zcl.time.h"

/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private defines -----------------------------------------------------------*/
#define APP_ZIGBEE_STARTUP_FAIL_DELAY               500U
#define CHANNEL                                     13

#define SW1_ENDPOINT                                10
#define SERVER_SCENES_MAX                           10
#define IDENTIFY_MODE_TIME                          30

/* USER CODE BEGIN PD */
#define SW1_GROUP_ADDR          0x0001
/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* External definition -------------------------------------------------------*/
enum ZbStatusCodeT ZbStartupWait(struct ZigBeeT *zb, struct ZbStartupT *config);

/* USER CODE BEGIN ED */
/* USER CODE END ED */

/* Private function prototypes -----------------------------------------------*/
static void APP_ZIGBEE_StackLayersInit(void);
static void APP_ZIGBEE_ConfigEndpoints(void);
static void APP_ZIGBEE_NwkForm(void);

static void APP_ZIGBEE_TraceError(const char *pMess, uint32_t ErrCode);
static void APP_ZIGBEE_CheckWirelessFirmwareInfo(void);

static void Wait_Getting_Ack_From_M0(void);
static void Receive_Ack_From_M0(void);
static void Receive_Notification_From_M0(void);

/* USER CODE BEGIN PFP */
static void APP_ZIGBEE_SW1_Process(void);
static void APP_ZIGBEE_SW2_Process(void);
static void APP_ZIGBEE_ConfigGroupAddr(void);
/* USER CODE END PFP */

static enum ZclStatusCodeT zcl_attr_cb(struct ZbZclClusterT *clusterPtr, struct ZbZclAttrCbInfoT *cb);
static enum ZclStatusCodeT zcl_attr_write_cb(struct ZbZclClusterT *clusterPtr, const struct ZbApsAddrT *src, uint16_t attributeId,
  const uint8_t *inputData, unsigned int inputMaxLen, void *attrData, ZclWriteModeT mode, void *app_cb_arg);

static uint8_t APP_ZIGBEE_IAS_WD_Server_StartWarning_cb(struct ZbZclClusterT *clusterPtr, void *arg, struct ZbZclIasWdClientStartWarningReqT *warn_req);
static uint8_t APP_ZIGBEE_IAS_WD_Server_Squawk_cb(struct ZbZclClusterT *clusterPtr, void *arg, struct ZbZclIasWdClientSquawkReqT *squawk_req);
static void APP_ZIGBEE_IAS_WD_Server_Squawk_Processing(void);
static void APP_ZIGBEE_IAS_WD_Server_Alarm_Processing(void);

static enum ZclStatusCodeT APP_ZIGBEE_Msg_get_last_cb(struct ZbZclClusterT *cluster, void *arg, struct ZbZclAddrInfoT *srcInfo);
static enum ZclStatusCodeT APP_ZIGBEE_Msg_confirm_cb(struct ZbZclClusterT *cluster, void *arg,  struct ZbZclMsgMessageConfT *conf, struct ZbZclAddrInfoT *srcInfo);


/* Private variables ---------------------------------------------------------*/
static TL_CmdPacket_t *p_ZIGBEE_otcmdbuffer;
static TL_EvtPacket_t *p_ZIGBEE_notif_M0_to_M4;
static TL_EvtPacket_t *p_ZIGBEE_request_M0_to_M4;
static __IO uint32_t CptReceiveNotifyFromM0 = 0;
static __IO uint32_t CptReceiveRequestFromM0 = 0;

PLACE_IN_SECTION("MB_MEM1") ALIGN(4) static TL_ZIGBEE_Config_t ZigbeeConfigBuffer;
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static TL_CmdPacket_t ZigbeeOtCmdBuffer;
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static uint8_t ZigbeeNotifRspEvtBuffer[sizeof(TL_PacketHeader_t) + TL_EVT_HDR_SIZE + 255U];
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static uint8_t ZigbeeNotifRequestBuffer[sizeof(TL_PacketHeader_t) + TL_EVT_HDR_SIZE + 255U];

struct zigbee_app_info {
  bool has_init;
  struct ZigBeeT *zb;
  enum ZbStartType startupControl;
  enum ZbStatusCodeT join_status;
  uint32_t join_delay;
  bool init_after_join;

  struct ZbZclClusterT *onoff_server;
  struct ZbZclClusterT *identify_server;
  struct ZbZclClusterT *scenes_server;
  struct ZbZclClusterT *ias_wd_server;
  struct ZbZclClusterT *messaging_server;
};
static struct zigbee_app_info zigbee_app_info;
const char *msg_buf = "ZCL Test Message";

/* Callbacks for IAS WD Server */
static struct ZbZclIasWdServerCallbacksT ZbZclIasWdServerCallbacks = {
  .start_warning = APP_ZIGBEE_IAS_WD_Server_StartWarning_cb,
  .squawk = APP_ZIGBEE_IAS_WD_Server_Squawk_cb,
};

static struct ZbZclIasWdClientStartWarningReqT warningReq;
static bool is_Squawk_strobe = false;

/* Callbacks for Messaging cluster */
static struct ZbZclMsgServerCallbacksT ZbZclMsgServerCallbacks = {
  .get_last_message = APP_ZIGBEE_Msg_get_last_cb,
  .message_confirmation = APP_ZIGBEE_Msg_confirm_cb,
  .get_message_cancellation = NULL,
};

/* Overwrite attribute from cluster with application specific callbacks */
static const struct ZbZclAttrT zcl_onoff_attr = {
  ZCL_ONOFF_ATTR_ONOFF, ZCL_DATATYPE_BOOLEAN,
  ZCL_ATTR_FLAG_REPORTABLE | ZCL_ATTR_FLAG_CB_WRITE, 0, zcl_attr_cb, {0, 0}, {0, 0}
};

/**
 * @brief  ZCL Attribute write callback handler
 * @param  clusterPtr  : ZCL Cluster pointer
 * @param  src         : Sender info
 * @param  attributeId : Attribute ID
 * @param  inputData   : Data to be written to attribute
 * @param  inputMaxLen : Max Length of input data
 * @param  attrData    : Attribute data information
 * @param  mode        : ZCL Write mode
 * @param  app_cb_arg  : Passed argument
 * @retval ZCL Status code
 */
static enum ZclStatusCodeT
zcl_attr_write_cb(struct ZbZclClusterT *clusterPtr, const struct ZbApsAddrT *src, uint16_t attributeId,
  const uint8_t *inputData, unsigned int inputMaxLen, void *attrData, ZclWriteModeT mode, void *app_cb_arg)
{
  unsigned int len = 0;

  switch (attributeId) {
    case ZCL_ONOFF_ATTR_ONOFF:
      len = 1;
      (void)memcpy(attrData, inputData, len);
      if (inputData[0] == 1) {
        APP_DBG("WRITE LED_GREEN ON");
        BSP_LED_On(LED_GREEN);
      }
      else {
        APP_DBG("WRITE LED_GREEN OFF");
        BSP_LED_Off(LED_GREEN);
      }
      return ZCL_STATUS_SUCCESS;

    default:
        return ZCL_STATUS_UNSUPP_ATTRIBUTE;
  }
}

/**
 * @brief  ZCL Attribute read/write callback handler
 * @param  clusterPtr : ZCL Cluster pointer
 * @param  cb         : Attribute callback info
 * @retval ZCL Status code
 */
static enum ZclStatusCodeT
zcl_attr_cb(struct ZbZclClusterT *clusterPtr, struct ZbZclAttrCbInfoT *cb)
{
  if (cb->type == ZCL_ATTR_CB_TYPE_WRITE) {
        return zcl_attr_write_cb(clusterPtr, cb->src, cb->info->attributeId, cb->zcl_data, cb->zcl_len,
            cb->attr_data, cb->write_mode, cb->app_cb_arg);
  }
  else {
      return ZCL_STATUS_FAILURE;
  }
}

/**
 * @brief  IAS WD StartWarning server command callback
 * @param  clusterPtr: cluster pointer
 * @param  arg: passed argument
 * @param  warn_req: StartWarning request
 * @retval ZCL status
 */
static uint8_t APP_ZIGBEE_IAS_WD_Server_StartWarning_cb(struct ZbZclClusterT *clusterPtr, void *arg, struct ZbZclIasWdClientStartWarningReqT *warn_req){
  APP_DBG("[IAS WD] IAS WD StartWarning command received.");

  /* Dumping the request parameters */
  APP_DBG("[IAS WD] Warning mode: 0x%02x.",  warn_req->warning_mode);
  APP_DBG("[IAS WD] Warning duration: 0x%04x.",  warn_req->warning_duration);
  APP_DBG("[IAS WD] Siren level: 0x%02x.",  warn_req->siren_level);
  APP_DBG("[IAS WD] Strobe enabled?  %s.",  (warn_req->strobe) ? "Yes" : "No");
  APP_DBG("[IAS WD] Strobe duty cycle: 0x%02x.\n",  warn_req->strobe_dutycycle);

  /* activate the warning state */
  memcpy(&warningReq, warn_req, sizeof(warningReq));
  UTIL_SEQ_SetTask(1U << CFG_TASK_ZIGBEE_APP_IAS_WD_START_WARNING, CFG_SCH_PRIO_0);

  return ZCL_STATUS_SUCCESS;
}

/**
 * @brief  IAS WD Squawk server command callback
 * @param  clusterPtr: cluster pointer
 * @param  arg: passed argument
 * @param  warn_req: Squawk request
 * @retval ZCL status
 */
static uint8_t APP_ZIGBEE_IAS_WD_Server_Squawk_cb(struct ZbZclClusterT *clusterPtr, void *arg, struct ZbZclIasWdClientSquawkReqT *squawk_req){
  APP_DBG("[IAS WD] IAS WD Squawk command received.");

  /* Dumping the request parameters */
  APP_DBG("[IAS WD] Squawk mode: 0x%02x.",  squawk_req->squawk_mode);
  APP_DBG("[IAS WD] Squawk level: 0x%02x.",  squawk_req->squawk_level);
  APP_DBG("[IAS WD] Strobe enabled?  %s.\n",  (squawk_req->strobe) ? "Yes" : "No");

  APP_DBG("[IAS WD] Activating Squawk signal.");
  is_Squawk_strobe = (bool)squawk_req->strobe;
  UTIL_SEQ_SetTask(1U << CFG_TASK_ZIGBEE_APP_IAS_WD_SQUAWK, CFG_SCH_PRIO_0);

  return ZCL_STATUS_SUCCESS;
}

/**
 * @brief  Zigbee application IAS WD StartWarning command proccessing
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_IAS_WD_Server_Alarm_Processing(void){
  enum ZclStatusCodeT status;
  enum ZclDataTypeT type;
  uint16_t warning_duration = warningReq.warning_duration;
  uint16_t max_duration = 0;

  /* reading local MaxDuration attribute */
  type = ZCL_DATATYPE_UNSIGNED_16BIT;
  max_duration = (uint16_t)ZbZclAttrIntegerRead(zigbee_app_info.ias_wd_server, ZCL_IAS_WD_SVR_ATTR_MAX_DURATION, &type, &status);
  if (status != ZCL_STATUS_SUCCESS) {
    APP_DBG("[IAS WD] Error, ZbZclAttrIntegerRead failed");
    assert(0);
  }

  if (max_duration<warning_duration) {
     /* the alarm cannot last ;ore that MaxDuration attribute value */
    warning_duration = max_duration;
  }

  for (unsigned int cpt=0 ; cpt<warning_duration ; cpt++) {
    APP_DBG("[IAS WD] Alarm!");
    if (warningReq.strobe) {
      /* strobe enabled */
      BSP_LED_On(LED_RED);
      HAL_Delay(warningReq.strobe_dutycycle*10); /* time in ms corresponding to strobe duty cycle */
      BSP_LED_Off(LED_RED);
      HAL_Delay(1000 - warningReq.strobe_dutycycle*10); /* remaining time in 1s */
    } else {
      HAL_Delay(1000);
    }
  }

  APP_DBG("[IAS WD] Alarm ended.\n");
}

/**
 * @brief  Zigbee application IAS WD Squawk command proccessing
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_IAS_WD_Server_Squawk_Processing(void){
  for (unsigned int cpt=0 ; cpt<3 ; cpt++) { /* 3 sec loop */
    APP_DBG("[IAS WD] Squawk!");
    if (is_Squawk_strobe) {
      BSP_LED_On(LED_RED);
    }
    HAL_Delay(100);

    if (is_Squawk_strobe) {
      BSP_LED_Off(LED_RED);
    }
    HAL_Delay(100);
  }
  APP_DBG("[IAS WD] Squawk signal ended.\n");
}

/**
 * @brief  Messaging get last message server command callback
 * @param  clusterPtr: cluster pointer
 * @param  arg: passed argument
 * @param  conf: message confirmation
 * @param  srcInfo: sender address information
 * @retval ZCL status
 */
static enum ZclStatusCodeT APP_ZIGBEE_Msg_get_last_cb(struct ZbZclClusterT *cluster, void *arg, struct ZbZclAddrInfoT *srcInfo)
{
  APP_DBG("[MESSAGE] Get Last Message");
  APP_DBG(msg_buf);
  return ZCL_STATUS_SUCCESS;
}

/**
 * @brief  Messaging get last message server command callback
 * @param  clusterPtr: cluster pointer
 * @param  arg: passed argument
 * @param  conf: message confirmation
 * @param  srcInfo: sender address information
 * @retval ZCL status
 */
static enum ZclStatusCodeT APP_ZIGBEE_Msg_confirm_cb(struct ZbZclClusterT *cluster, void *arg,
struct ZbZclMsgMessageConfT *conf, struct ZbZclAddrInfoT *srcInfo)
{
  APP_DBG("[MESSAGE] MSG CONFIRM");
  return ZCL_STATUS_SUCCESS;
}

/**
 * @brief  Zigbee application initialization
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_Init(void)
{
  SHCI_CmdStatus_t ZigbeeInitStatus;

  APP_DBG("APP_ZIGBEE_Init");

  /* Check the compatibility with the Coprocessor Wireless Firmware loaded */
  APP_ZIGBEE_CheckWirelessFirmwareInfo();

  /* Register cmdbuffer */
  APP_ZIGBEE_RegisterCmdBuffer(&ZigbeeOtCmdBuffer);

  /* Init config buffer and call TL_ZIGBEE_Init */
  APP_ZIGBEE_TL_INIT();

  /* Register task */
  /* Create the different tasks */

  UTIL_SEQ_RegTask(1U << (uint32_t)CFG_TASK_NOTIFY_FROM_M0_TO_M4, UTIL_SEQ_RFU, APP_ZIGBEE_ProcessNotifyM0ToM4);
  UTIL_SEQ_RegTask(1U << (uint32_t)CFG_TASK_REQUEST_FROM_M0_TO_M4, UTIL_SEQ_RFU, APP_ZIGBEE_ProcessRequestM0ToM4);

  /* Task associated with network creation process */
  UTIL_SEQ_RegTask(1U << CFG_TASK_ZIGBEE_NETWORK_FORM, UTIL_SEQ_RFU, APP_ZIGBEE_NwkForm);

  /* Register tasks to handle button 1 and 2 press */
  UTIL_SEQ_RegTask(1U << CFG_TASK_BUTTON_SW1, UTIL_SEQ_RFU, APP_ZIGBEE_SW1_Process);
  UTIL_SEQ_RegTask(1U << CFG_TASK_BUTTON_SW2, UTIL_SEQ_RFU, APP_ZIGBEE_SW2_Process);

  /* Register tasks associated with IAS WD */
  UTIL_SEQ_RegTask(1U << (uint32_t)CFG_TASK_ZIGBEE_APP_IAS_WD_START_WARNING, UTIL_SEQ_RFU, APP_ZIGBEE_IAS_WD_Server_Alarm_Processing);
  UTIL_SEQ_RegTask(1U << (uint32_t)CFG_TASK_ZIGBEE_APP_IAS_WD_SQUAWK, UTIL_SEQ_RFU, APP_ZIGBEE_IAS_WD_Server_Squawk_Processing);

  /* USER CODE BEGIN APP_ZIGBEE_INIT */
  /* USER CODE END APP_ZIGBEE_INIT */

  /* Start the Zigbee on the CPU2 side */
  ZigbeeInitStatus = SHCI_C2_ZIGBEE_Init();
  /* Prevent unused argument(s) compilation warning */
  UNUSED(ZigbeeInitStatus);

  /* Initialize Zigbee stack layers */
  APP_ZIGBEE_StackLayersInit();

}

/**
 * @brief  Initialize Zigbee stack layers
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_StackLayersInit(void)
{
  APP_DBG("APP_ZIGBEE_StackLayersInit");

  zigbee_app_info.zb = ZbInit(0U, NULL, NULL);
  assert(zigbee_app_info.zb != NULL);

  /* Create the endpoint and cluster(s) */
  APP_ZIGBEE_ConfigEndpoints();

  /* USER CODE BEGIN APP_ZIGBEE_StackLayersInit */
  BSP_LED_Off(LED_RED);
  BSP_LED_Off(LED_GREEN);
  BSP_LED_Off(LED_BLUE);
  /* USER CODE END APP_ZIGBEE_StackLayersInit */

  /* Configure the joining parameters */
  zigbee_app_info.join_status = (enum ZbStatusCodeT) 0x01; /* init to error status */
  zigbee_app_info.join_delay = HAL_GetTick(); /* now */
  zigbee_app_info.startupControl = ZbStartTypeForm;

  /* Initialization Complete */
  zigbee_app_info.has_init = true;

  /* run the task */
  UTIL_SEQ_SetTask(1U << CFG_TASK_ZIGBEE_NETWORK_FORM, CFG_SCH_PRIO_0);
}

/**
 * @brief  Configure Zigbee application endpoints
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_ConfigEndpoints(void)
{
  struct ZbApsmeAddEndpointReqT req;
  struct ZbApsmeAddEndpointConfT conf;

  memset(&req, 0, sizeof(req));

  /* Endpoint: SW1_ENDPOINT */
  req.profileId = ZCL_PROFILE_HOME_AUTOMATION;
  req.deviceId = ZCL_DEVICE_ONOFF_SWITCH;
  req.endpoint = SW1_ENDPOINT;
  ZbZclAddEndpoint(zigbee_app_info.zb, &req, &conf);
  assert(conf.status == ZB_STATUS_SUCCESS);

  /* OnOff server */
  zigbee_app_info.onoff_server = ZbZclOnOffServerAlloc(zigbee_app_info.zb, SW1_ENDPOINT, NULL, NULL);

  assert(zigbee_app_info.onoff_server != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.onoff_server);
  if (ZbZclAttrAppendList(zigbee_app_info.onoff_server, &zcl_onoff_attr, 1)) {
    APP_DBG("Config Clusters: Failed to append OnOff attribute list");
    assert(0);
  }

  /* Idenfity server */
  zigbee_app_info.identify_server = ZbZclIdentifyServerAlloc(zigbee_app_info.zb, SW1_ENDPOINT, NULL);
  assert(zigbee_app_info.identify_server != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.identify_server);

  /* Scenes server */
  zigbee_app_info.scenes_server = ZbZclScenesServerAlloc(zigbee_app_info.zb, SW1_ENDPOINT, SERVER_SCENES_MAX);
  assert(zigbee_app_info.scenes_server != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.scenes_server);

  /* IAS WD Server */
  zigbee_app_info.ias_wd_server = ZbZclIasWdServerAlloc(zigbee_app_info.zb, SW1_ENDPOINT, &ZbZclIasWdServerCallbacks, NULL);
  assert(zigbee_app_info.ias_wd_server != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.ias_wd_server);

  zigbee_app_info.messaging_server = ZbZclMsgServerAlloc(zigbee_app_info.zb, SW1_ENDPOINT, &ZbZclMsgServerCallbacks, NULL);
  assert(zigbee_app_info.messaging_server != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.messaging_server);
  ZbZclClusterSetProfileId(zigbee_app_info.messaging_server, ZCL_PROFILE_HOME_AUTOMATION);

}

/**
 * @brief  Handle Zigbee network forming and joining
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_NwkForm(void)
{
  if ((zigbee_app_info.join_status != ZB_STATUS_SUCCESS) && (HAL_GetTick() >= zigbee_app_info.join_delay))
  {
    struct ZbStartupT config;
    enum ZbStatusCodeT status;

    /* Configure Zigbee Logging */
    ZbSetLogging(zigbee_app_info.zb, ZB_LOG_MASK_LEVEL_6, NULL);

    /* Attempt to join a zigbee network */
    ZbStartupConfigGetProDefaults(&config);

    /* Set the centralized network */
    APP_DBG("Network config : APP_STARTUP_CENTRALIZED_COORDINATOR");
    config.startupControl = zigbee_app_info.startupControl;

    /* Using the default HA preconfigured Link Key */
    memcpy(config.security.preconfiguredLinkKey, sec_key_ha, ZB_SEC_KEYSIZE);

    config.channelList.count = 1;
    config.channelList.list[0].page = 0;
    config.channelList.list[0].channelMask = 1 << CHANNEL; /*Channel in use */

    config.bdbCommissioningMode |= BDB_COMMISSION_MODE_FIND_BIND;

    /* Using ZbStartupWait (blocking) */
    status = ZbStartupWait(zigbee_app_info.zb, &config);

    APP_DBG("ZbStartup Callback (status = 0x%02x)", status);
    zigbee_app_info.join_status = status;

    if (status == ZB_STATUS_SUCCESS) {
      /* USER CODE BEGIN 3 */
      zigbee_app_info.join_delay = 0U;
      zigbee_app_info.init_after_join = true;
      BSP_LED_On(LED_BLUE);
    }
    else
    {
      /* USER CODE END 3 */
      APP_DBG("Startup failed, attempting again after a short delay (%d ms)", APP_ZIGBEE_STARTUP_FAIL_DELAY);
      zigbee_app_info.join_delay = HAL_GetTick() + APP_ZIGBEE_STARTUP_FAIL_DELAY;
    }
  }

  /* If Network forming/joining was not successful reschedule the current task to retry the process */
  if (zigbee_app_info.join_status != ZB_STATUS_SUCCESS)
  {
    UTIL_SEQ_SetTask(1U << CFG_TASK_ZIGBEE_NETWORK_FORM, CFG_SCH_PRIO_0);
  }

  /* USER CODE BEGIN NW_FORM */
  else
  {
    zigbee_app_info.init_after_join = false;

    /* Assign ourselves to the group addresses */
    APP_ZIGBEE_ConfigGroupAddr();

    /* Since we're using group addressing (broadcast), shorten the broadcast timeout */
    uint32_t bcast_timeout = 3;
    ZbNwkSet(zigbee_app_info.zb, ZB_NWK_NIB_ID_NetworkBroadcastDeliveryTime, &bcast_timeout, sizeof(bcast_timeout));
  }
  /* USER CODE END NW_FORM */
}

/*************************************************************
 * ZbStartupWait Blocking Call
 *************************************************************/
struct ZbStartupWaitInfo {
  bool active;
  enum ZbStatusCodeT status;
};

static void ZbStartupWaitCb(enum ZbStatusCodeT status, void *cb_arg)
{
  struct ZbStartupWaitInfo *info = cb_arg;

  info->status = status;
  info->active = false;
  UTIL_SEQ_SetEvt(EVENT_ZIGBEE_STARTUP_ENDED);
}

enum ZbStatusCodeT ZbStartupWait(struct ZigBeeT *zb, struct ZbStartupT *config)
{
  struct ZbStartupWaitInfo *info;
  enum ZbStatusCodeT status;

  info = malloc(sizeof(struct ZbStartupWaitInfo));
  if (info == NULL) {
    return ZB_STATUS_ALLOC_FAIL;
  }
  memset(info, 0, sizeof(struct ZbStartupWaitInfo));

  info->active = true;
  status = ZbStartup(zb, config, ZbStartupWaitCb, info);
  if (status != ZB_STATUS_SUCCESS) {
    info->active = false;
    return status;
  }
  UTIL_SEQ_WaitEvt(EVENT_ZIGBEE_STARTUP_ENDED);
  status = info->status;
  free(info);
  return status;
}

/**
 * @brief  Trace the error or the warning reported.
 * @param  ErrId :
 * @param  ErrCode
 * @retval None
 */
void APP_ZIGBEE_Error(uint32_t ErrId, uint32_t ErrCode)
{
  switch (ErrId) {
  default:
    APP_ZIGBEE_TraceError("ERROR Unknown ", 0);
    break;
  }
}

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/

/**
 * @brief  Warn the user that an error has occurred.
 *
 * @param  pMess  : Message associated to the error.
 * @param  ErrCode: Error code associated to the module (Zigbee or other module if any)
 * @retval None
 */
static void APP_ZIGBEE_TraceError(const char *pMess, uint32_t ErrCode)
{
  APP_DBG("**** Fatal error = %s (Err = %d)", pMess, ErrCode);
  /* USER CODE BEGIN TRACE_ERROR */
  while (1U == 1U) {
    BSP_LED_Toggle(LED1);
    HAL_Delay(500U);
    BSP_LED_Toggle(LED2);
    HAL_Delay(500U);
    BSP_LED_Toggle(LED3);
    HAL_Delay(500U);
  }
  /* USER CODE END TRACE_ERROR */
}

/**
 * @brief Check if the Coprocessor Wireless Firmware loaded supports Zigbee
 *        and display associated information
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_CheckWirelessFirmwareInfo(void)
{
  WirelessFwInfo_t wireless_info_instance;
  WirelessFwInfo_t *p_wireless_info = &wireless_info_instance;

  if (SHCI_GetWirelessFwInfo(p_wireless_info) != SHCI_Success) {
    APP_ZIGBEE_Error((uint32_t)ERR_ZIGBEE_CHECK_WIRELESS, (uint32_t)ERR_INTERFACE_FATAL);
  }
  else {
    APP_DBG("**********************************************************");
    APP_DBG("WIRELESS COPROCESSOR FW:");
    /* Print version */
    APP_DBG("VERSION ID = %d.%d.%d", p_wireless_info->VersionMajor, p_wireless_info->VersionMinor, p_wireless_info->VersionSub);

    switch (p_wireless_info->StackType) {
    case INFO_STACK_TYPE_ZIGBEE_FFD:
      APP_DBG("FW Type : FFD Zigbee stack");
      break;
   case INFO_STACK_TYPE_ZIGBEE_RFD:
      APP_DBG("FW Type : RFD Zigbee stack");
      break;
    default:
      /* No Zigbee device supported ! */
      APP_ZIGBEE_Error((uint32_t)ERR_ZIGBEE_CHECK_WIRELESS, (uint32_t)ERR_INTERFACE_FATAL);
      break;
    }
    APP_DBG("**********************************************************");
  }
}

/*************************************************************
 *
 * WRAP FUNCTIONS
 *
 *************************************************************/

void APP_ZIGBEE_RegisterCmdBuffer(TL_CmdPacket_t *p_buffer)
{
  p_ZIGBEE_otcmdbuffer = p_buffer;
}

Zigbee_Cmd_Request_t * ZIGBEE_Get_OTCmdPayloadBuffer(void)
{
  return (Zigbee_Cmd_Request_t *)p_ZIGBEE_otcmdbuffer->cmdserial.cmd.payload;
}

Zigbee_Cmd_Request_t * ZIGBEE_Get_OTCmdRspPayloadBuffer(void)
{
  return (Zigbee_Cmd_Request_t *)((TL_EvtPacket_t *)p_ZIGBEE_otcmdbuffer)->evtserial.evt.payload;
}

Zigbee_Cmd_Request_t * ZIGBEE_Get_NotificationPayloadBuffer(void)
{
  return (Zigbee_Cmd_Request_t *)(p_ZIGBEE_notif_M0_to_M4)->evtserial.evt.payload;
}

Zigbee_Cmd_Request_t * ZIGBEE_Get_M0RequestPayloadBuffer(void)
{
  return (Zigbee_Cmd_Request_t *)(p_ZIGBEE_request_M0_to_M4)->evtserial.evt.payload;
}

/**
 * @brief  This function is used to transfer the commands from the M4 to the M0.
 *
 * @param   None
 * @return  None
 */
void ZIGBEE_CmdTransfer(void)
{
  Zigbee_Cmd_Request_t *cmd_req = (Zigbee_Cmd_Request_t *)p_ZIGBEE_otcmdbuffer->cmdserial.cmd.payload;

  /* Zigbee OT command cmdcode range 0x280 .. 0x3DF = 352 */
  p_ZIGBEE_otcmdbuffer->cmdserial.cmd.cmdcode = 0x280U;
  /* Size = otCmdBuffer->Size (Number of OT cmd arguments : 1 arg = 32bits so multiply by 4 to get size in bytes)
   * + ID (4 bytes) + Size (4 bytes) */
  p_ZIGBEE_otcmdbuffer->cmdserial.cmd.plen = 8U + (cmd_req->Size * 4U);

  TL_ZIGBEE_SendM4RequestToM0();

  /* Wait completion of cmd */
  Wait_Getting_Ack_From_M0();
}

/**
 * @brief  This function is called when the M0+ acknowledge  the fact that it has received a Cmd
 *
 *
 * @param   Otbuffer : a pointer to TL_EvtPacket_t
 * @return  None
 */
void TL_ZIGBEE_CmdEvtReceived(TL_EvtPacket_t *Otbuffer)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Otbuffer);

  Receive_Ack_From_M0();
}

/**
 * @brief  This function is called when notification from M0+ is received.
 *
 * @param   Notbuffer : a pointer to TL_EvtPacket_t
 * @return  None
 */
void TL_ZIGBEE_NotReceived(TL_EvtPacket_t *Notbuffer)
{
  p_ZIGBEE_notif_M0_to_M4 = Notbuffer;

  Receive_Notification_From_M0();
}

/**
 * @brief  This function is called before sending any ot command to the M0
 *         core. The purpose of this function is to be able to check if
 *         there are no notifications coming from the M0 core which are
 *         pending before sending a new ot command.
 * @param  None
 * @retval None
 */
void Pre_ZigbeeCmdProcessing(void)
{
  UTIL_SEQ_WaitEvt(EVENT_SYNCHRO_BYPASS_IDLE);
}

/**
 * @brief  This function waits for getting an acknowledgment from the M0.
 *
 * @param  None
 * @retval None
 */
static void Wait_Getting_Ack_From_M0(void)
{
  UTIL_SEQ_WaitEvt(EVENT_ACK_FROM_M0_EVT);
}

/**
 * @brief  Receive an acknowledgment from the M0+ core.
 *         Each command send by the M4 to the M0 are acknowledged.
 *         This function is called under interrupt.
 * @param  None
 * @retval None
 */
static void Receive_Ack_From_M0(void)
{
  UTIL_SEQ_SetEvt(EVENT_ACK_FROM_M0_EVT);
}

/**
 * @brief  Receive a notification from the M0+ through the IPCC.
 *         This function is called under interrupt.
 * @param  None
 * @retval None
 */
static void Receive_Notification_From_M0(void)
{
    CptReceiveNotifyFromM0++;
    UTIL_SEQ_SetTask(1U << (uint32_t)CFG_TASK_NOTIFY_FROM_M0_TO_M4, CFG_SCH_PRIO_0);
}

/**
 * @brief  This function is called when a request from M0+ is received.
 *
 * @param   Notbuffer : a pointer to TL_EvtPacket_t
 * @return  None
 */
void TL_ZIGBEE_M0RequestReceived(TL_EvtPacket_t *Reqbuffer)
{
    p_ZIGBEE_request_M0_to_M4 = Reqbuffer;

    CptReceiveRequestFromM0++;
    UTIL_SEQ_SetTask(1U << (uint32_t)CFG_TASK_REQUEST_FROM_M0_TO_M4, CFG_SCH_PRIO_0);
}

/**
 * @brief Perform initialization of TL for Zigbee.
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_TL_INIT(void)
{
    ZigbeeConfigBuffer.p_ZigbeeOtCmdRspBuffer = (uint8_t *)&ZigbeeOtCmdBuffer;
    ZigbeeConfigBuffer.p_ZigbeeNotAckBuffer = (uint8_t *)ZigbeeNotifRspEvtBuffer;
    ZigbeeConfigBuffer.p_ZigbeeNotifRequestBuffer = (uint8_t *)ZigbeeNotifRequestBuffer;
    TL_ZIGBEE_Init(&ZigbeeConfigBuffer);
}

/**
 * @brief Process the messages coming from the M0.
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_ProcessNotifyM0ToM4(void)
{
    if (CptReceiveNotifyFromM0 != 0) {
        /* If CptReceiveNotifyFromM0 is > 1. it means that we did not serve all the events from the radio */
        if (CptReceiveNotifyFromM0 > 1U) {
            APP_ZIGBEE_Error(ERR_REC_MULTI_MSG_FROM_M0, 0);
        }
        else {
            Zigbee_CallBackProcessing();
        }
        /* Reset counter */
        CptReceiveNotifyFromM0 = 0;
    }
}

/**
 * @brief Process the requests coming from the M0.
 * @param
 * @return
 */
void APP_ZIGBEE_ProcessRequestM0ToM4(void)
{
    if (CptReceiveRequestFromM0 != 0) {
        Zigbee_M0RequestProcessing();
        CptReceiveRequestFromM0 = 0;
    }
}
/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

/**
 * @brief  Set group addressing mode
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_ConfigGroupAddr(void)
{
  struct ZbApsmeAddGroupReqT req;
  struct ZbApsmeAddGroupConfT conf;

  memset(&req, 0, sizeof(req));
  req.endpt = SW1_ENDPOINT;
  req.groupAddr = SW1_GROUP_ADDR;
  ZbApsmeAddGroupReq(zigbee_app_info.zb, &req, &conf);

}

/**
 * @brief  Button 1 handler. Put Identify cluster into identify mode.
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_SW1_Process()
{
  uint64_t epid = 0U;

  if (zigbee_app_info.zb == NULL) {
    APP_DBG("Error, zigbee stack not initialized");
    return;
  }

  /* Check if the router joined the network */
  if (ZbNwkGet(zigbee_app_info.zb, ZB_NWK_NIB_ID_ExtendedPanId, &epid, sizeof(epid)) != ZB_STATUS_SUCCESS) {
    APP_DBG("Error, failed to get network information");
    return;
  }
  if (epid == 0U) {
    APP_DBG("Error, device not on a network");
    return;
  }
  APP_DBG("SW1 PUSHED (Turn on Identify Mode)");

  if (zigbee_app_info.identify_server != NULL) {
    ZbZclIdentifyServerSetTime(zigbee_app_info.identify_server, IDENTIFY_MODE_TIME);
  }
}

/**
 * @brief  Button 2 handler. Send ZCL Display Message request via bindings.
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_SW2_Process()
{
  uint64_t epid = 0U;
  struct ZbZclMsgMessageT msg;
  enum ZclStatusCodeT status;


  if (zigbee_app_info.zb == NULL) {
    APP_DBG("Error, zigbee stack not initialized");
    return;
  }

  /* Check if the router joined the network */
  if (ZbNwkGet(zigbee_app_info.zb, ZB_NWK_NIB_ID_ExtendedPanId, &epid, sizeof(epid)) != ZB_STATUS_SUCCESS) {
    APP_DBG("Error, failed to get network information");
    return;
  }
  if (epid == 0U) {
    APP_DBG("Error, device not on a network");
    return;
  }

  APP_DBG("SW2 PUSHED (Send New Message)");
  memset(&msg, 0, sizeof(msg));
  if (zigbee_app_info.messaging_server != NULL) {
    /* Message ID doesn't change to keep the demo simple, and the message doesn't change anyways. */
    msg.message_id = 1;
    strcpy(msg.message_str, msg_buf);
    msg.duration = 1;
    status = ZbZclMsgServerDisplayMessageReq(zigbee_app_info.messaging_server, ZbApsAddrBinding, &msg, NULL, NULL);
    if (status != ZCL_STATUS_SUCCESS) {
      APP_DBG("Failed to display message: %d", status);
    }
  }
}

/* USER CODE END FD_LOCAL_FUNCTIONS */


