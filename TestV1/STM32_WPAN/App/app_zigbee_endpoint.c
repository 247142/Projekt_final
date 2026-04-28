/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_zigbee_endpoint.c
  * Description        : Zigbee Application to manage endpoints and these clusters.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include <assert.h>
#include <stdint.h>

#include "app_common.h"
#include "app_conf.h"
#include "log_module.h"
#include "app_entry.h"
#include "app_zigbee.h"
#include "dbg_trace.h"
#include "ieee802154_enums.h"
#include "mcp_enums.h"

#include "stm32_lpm.h"
#include "stm32_rtos.h"
#include "stm32_timer.h"
#include "stm32_lpm_if.h"

#include "zigbee.h"
#include "zigbee.nwk.h"
#include "zigbee.security.h"

/* Private includes -----------------------------------------------------------*/
#include "zcl/zcl.h"
#include "zcl/general/zcl.basic.h"
#include "zcl/general/zcl.onoff.h"
#include "zcl/general/zcl.temp.meas.h"
#include "zcl/security/zcl.ias_zone.h"

/* USER CODE BEGIN PI */

/* USER CODE END PI */

/* Private defines -----------------------------------------------------------*/
#define APP_ZIGBEE_CHANNEL                13u
#define APP_ZIGBEE_CHANNEL_MASK           ( 1u << APP_ZIGBEE_CHANNEL )
#define APP_ZIGBEE_TX_POWER               ((int8_t) 10)    /* TX-Power is at +10 dBm. */

#define APP_ZIGBEE_ENDPOINT_1             1u
#define APP_ZIGBEE_PROFILE_ID_1           ZCL_PROFILE_HOME_AUTOMATION
#define APP_ZIGBEE_DEVICE_ID_1            ZCL_DEVICE_ONOFF_SWITCH

#define APP_ZIGBEE_ENDPOINT_2             2u
#define APP_ZIGBEE_PROFILE_ID_2           ZCL_PROFILE_HOME_AUTOMATION
#define APP_ZIGBEE_DEVICE_ID_2            ZCL_DEVICE_TEMPERATURE_SENSOR

#define APP_ZIGBEE_ENDPOINT_3             3u
#define APP_ZIGBEE_PROFILE_ID_3           ZCL_PROFILE_HOME_AUTOMATION
#define APP_ZIGBEE_DEVICE_ID_3            ZCL_DEVICE_IAS_ZONE

#define APP_ZIGBEE_CLUSTER1_1_ID          ZCL_CLUSTER_BASIC
#define APP_ZIGBEE_CLUSTER1_1_NAME        "Basic Client"

#define APP_ZIGBEE_CLUSTER2_1_ID          ZCL_CLUSTER_ONOFF
#define APP_ZIGBEE_CLUSTER2_1_NAME        "OnOff Client"

#define APP_ZIGBEE_CLUSTER3_2_ID          ZCL_CLUSTER_MEAS_TEMPERATURE
#define APP_ZIGBEE_CLUSTER3_2_NAME        "TempMeas Server"

#define APP_ZIGBEE_CLUSTER4_3_ID          ZCL_CLUSTER_IAS_ZONE
#define APP_ZIGBEE_CLUSTER4_3_NAME        "IasZone Server"

/* MeasTemperature for Endpoint 2 specific defines -----------------------------------*/
#define APP_ZIGBEE_TEMP_MIN_2             -27315
#define APP_ZIGBEE_TEMP_MAX_2             32767
#define APP_ZIGBEE_TEMP_TOLERANCE_2       2048
/* USER CODE BEGIN MeasTemperature2 defines */
/* USER CODE END MeasTemperature2 defines */

/* USER CODE BEGIN PD */

/* USER CODE END PD */

// -- Redefine Clusters to better code read --
#define BasicClient_1                     pstZbCluster[0]
#define OnOffClient_1                     pstZbCluster[1]
#define TempMeasServer_2                  pstZbCluster[2]
#define IasZoneServer_3                   pstZbCluster[3]

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private constants ---------------------------------------------------------*/
/* USER CODE BEGIN PC */

/* USER CODE END PC */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* IasZone Server3 Callbacks */
static enum ZclStatusCodeT  APP_ZIGBEE_IasZoneServerModeChangeCallback_3    ( struct ZbZclClusterT * pstCluster, void * arg, enum ZbZclIasZoneServerModeT eMode, struct ZbZclIasZoneClientTestModeReqT * pstRequest );

static struct ZbZclIasZoneServerCallbacksT stIasZoneServerCallbacks_3 =
{
  .mode_change = APP_ZIGBEE_IasZoneServerModeChangeCallback_3,
};

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/

/**
 * @brief  Zigbee application initialization
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_ApplicationInit(void)
{
  LOG_INFO_APP( "ZIGBEE Application Init" );

  /* Initialization of the Zigbee stack */
  APP_ZIGBEE_Init();

  /* Configure Application Form/Join parameters : Startup, Persistence and Start with/without Form/Join */
  stZigbeeAppInfo.eStartupControl = ZbStartTypeJoin;
  stZigbeeAppInfo.bPersistNotification = false;
  stZigbeeAppInfo.bNwkStartup = true;

  /* USER CODE BEGIN APP_ZIGBEE_ApplicationInit */

  /* USER CODE END APP_ZIGBEE_ApplicationInit */

  /* Initialize Zigbee stack layers */
  APP_ZIGBEE_StackLayersInit();
}

/**
 * @brief  Zigbee application start
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_ApplicationStart( void )
{
  /* USER CODE BEGIN APP_ZIGBEE_ApplicationStart */

  /* USER CODE END APP_ZIGBEE_ApplicationStart */

#if ( CFG_LPM_LEVEL != 0)
  /* Authorize LowPower now */
  UTIL_LPM_SetMaxMode( 1 << CFG_LPM_APP, UTIL_LPM_MAX_MODE );
#endif /* CFG_LPM_LEVEL */
}

/**
 * @brief  Configure Zigbee application endpoints
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_ConfigEndpoints(void)
{
  struct ZbApsmeAddEndpointReqT   stRequest;
  struct ZbApsmeAddEndpointConfT  stConfig;
  /* USER CODE BEGIN APP_ZIGBEE_ConfigEndpoints1 */

  /* USER CODE END APP_ZIGBEE_ConfigEndpoints1 */

  /* Add EndPoint 1 */
  memset( &stRequest, 0, sizeof( stRequest ) );
  memset( &stConfig, 0, sizeof( stConfig ) );

  stRequest.profileId = APP_ZIGBEE_PROFILE_ID_1;
  stRequest.deviceId = APP_ZIGBEE_DEVICE_ID_1;
  stRequest.endpoint = APP_ZIGBEE_ENDPOINT_1;
  ZbZclAddEndpoint( stZigbeeAppInfo.pstZigbee, &stRequest, &stConfig );
  assert( stConfig.status == ZB_STATUS_SUCCESS );

  /* Add Basic Client 1 Cluster */
  stZigbeeAppInfo.BasicClient_1 = ZbZclBasicClientAlloc( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT_1 );
  assert( stZigbeeAppInfo.BasicClient_1 != NULL );
  if ( ZbZclClusterEndpointRegister( stZigbeeAppInfo.BasicClient_1 ) == false )
  {
    LOG_ERROR_APP( "Error during Basic Client Endpoint Register." );
  }

  /* Add OnOff Client 1 Cluster */
  stZigbeeAppInfo.OnOffClient_1 = ZbZclOnOffClientAlloc( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT_1 );
  assert( stZigbeeAppInfo.OnOffClient_1 != NULL );
  if ( ZbZclClusterEndpointRegister( stZigbeeAppInfo.OnOffClient_1 ) == false )
  {
    LOG_ERROR_APP( "Error during OnOff Client Endpoint Register." );
  }

  /* Add EndPoint 2 */
  memset( &stRequest, 0, sizeof( stRequest ) );
  memset( &stConfig, 0, sizeof( stConfig ) );

  stRequest.profileId = APP_ZIGBEE_PROFILE_ID_2;
  stRequest.deviceId = APP_ZIGBEE_DEVICE_ID_2;
  stRequest.endpoint = APP_ZIGBEE_ENDPOINT_2;
  ZbZclAddEndpoint( stZigbeeAppInfo.pstZigbee, &stRequest, &stConfig );
  assert( stConfig.status == ZB_STATUS_SUCCESS );

  /* Add TempMeas Server 2 Cluster */
  stZigbeeAppInfo.TempMeasServer_2 = ZbZclTempMeasServerAlloc( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT_2, APP_ZIGBEE_TEMP_MIN_2, APP_ZIGBEE_TEMP_MAX_2, APP_ZIGBEE_TEMP_TOLERANCE_2 );
  assert( stZigbeeAppInfo.TempMeasServer_2 != NULL );
  if ( ZbZclClusterEndpointRegister( stZigbeeAppInfo.TempMeasServer_2 ) == false )
  {
    LOG_ERROR_APP( "Error during TempMeas Server Endpoint Register." );
  }

  /* Add EndPoint 3 */
  memset( &stRequest, 0, sizeof( stRequest ) );
  memset( &stConfig, 0, sizeof( stConfig ) );

  stRequest.profileId = APP_ZIGBEE_PROFILE_ID_3;
  stRequest.deviceId = APP_ZIGBEE_DEVICE_ID_3;
  stRequest.endpoint = APP_ZIGBEE_ENDPOINT_3;
  ZbZclAddEndpoint( stZigbeeAppInfo.pstZigbee, &stRequest, &stConfig );
  assert( stConfig.status == ZB_STATUS_SUCCESS );

  /* Add IasZone Server 3 Cluster */
  stZigbeeAppInfo.IasZoneServer_3 = ZbZclIasZoneServerAlloc( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT_3, ZCL_IAS_ZONE_SVR_ZONE_TYPE_DOOR_WINDOW, 0x00, false, &stIasZoneServerCallbacks_3, NULL);
  assert( stZigbeeAppInfo.IasZoneServer_3 != NULL );
  if ( ZbZclClusterEndpointRegister( stZigbeeAppInfo.IasZoneServer_3 ) == false )
  {
    LOG_ERROR_APP( "Error during IasZone Server Endpoint Register." );
  }

  /* USER CODE BEGIN APP_ZIGBEE_ConfigEndpoints2 */
  // Změna ZoneType na "Contact Switch" (Dveřní / Okenní senzor)
  	ZbZclAttrIntegerWrite(stZigbeeAppInfo.IasZoneServer_3,
  			ZCL_IAS_ZONE_SVR_ATTR_ZONE_TYPE,
  			ZCL_IAS_ZONE_SVR_ZONE_TYPE_DOOR_WINDOW);
  /* USER CODE END APP_ZIGBEE_ConfigEndpoints2 */
}

/**
 * @brief  Set Group Addressing mode (if used)
 * @param  None
 * @retval 'true' if Group Address used else 'false'.
 */
bool APP_ZIGBEE_ConfigGroupAddr( void )
{
  /* Not used */

  return false;
}

/**
 * @brief  Return the Startup Configuration
 * @param  pstConfig  Configuration structure to fill
 * @retval None
 */
void APP_ZIGBEE_GetStartupConfig( struct ZbStartupT * pstConfig )
{
  /* Attempt to join a zigbee network */
  ZbStartupConfigGetProDefaults( pstConfig );

  /* Using the default HA preconfigured Link Key */
  memcpy( pstConfig->security.preconfiguredLinkKey, sec_key_ha, ZB_SEC_KEYSIZE );

  /* Setting up additional startup configuration parameters */
  pstConfig->startupControl = stZigbeeAppInfo.eStartupControl;
  pstConfig->channelList.count = 1;
  pstConfig->channelList.list[0].page = 0;
  pstConfig->channelList.list[0].channelMask = APP_ZIGBEE_CHANNEL_MASK;

  /* Set the TX-Power */
  if ( APP_ZIGBEE_SetTxPower( APP_ZIGBEE_TX_POWER ) == false )
  {
    LOG_ERROR_APP( "Switching to %d dB failed.", APP_ZIGBEE_TX_POWER );
    return;
  }

  /* USER CODE BEGIN APP_ZIGBEE_GetStartupConfig */

  /* USER CODE END APP_ZIGBEE_GetStartupConfig */
}

/**
 * @brief  Manage a New Device on Network (called only if Coord or Router).
 * @param  iShortAddress      Short Address of new Device
 * @param  dlExtendedAddress  Extended Address of new Device
 * @param  cCapability        Capability of new Device
 * @retval Group Address
 */
void APP_ZIGBEE_SetNewDevice( uint16_t iShortAddress, uint64_t dlExtendedAddress, uint8_t cCapability )
{
  LOG_INFO_APP( "New Device (%d) on Network : with Extended ( " LOG_DISPLAY64() " ) and Short ( 0x%04X ) Address.", cCapability, LOG_NUMBER64( dlExtendedAddress ), iShortAddress );

  /* USER CODE BEGIN APP_ZIGBEE_SetNewDevice */

  /* USER CODE END APP_ZIGBEE_SetNewDevice */
}

/**
 * @brief  Print application information to the console
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_PrintApplicationInfo(void)
{
  LOG_INFO_APP( "**********************************************************" );
  LOG_INFO_APP( "Network config : CENTRALIZED ROUTER" );

  /* USER CODE BEGIN APP_ZIGBEE_PrintApplicationInfo1 */

  /* USER CODE END APP_ZIGBEE_PrintApplicationInfo1 */
  LOG_INFO_APP( "Channel used: %d.", APP_ZIGBEE_CHANNEL );

  APP_ZIGBEE_PrintGenericInfo();

  LOG_INFO_APP( "Clusters allocated are:" );
  LOG_INFO_APP( "  %s on Endpoint %d.", APP_ZIGBEE_CLUSTER1_1_NAME, APP_ZIGBEE_ENDPOINT_1 );
  LOG_INFO_APP( "  %s on Endpoint %d.", APP_ZIGBEE_CLUSTER2_1_NAME, APP_ZIGBEE_ENDPOINT_1 );
  LOG_INFO_APP( "  %s on Endpoint %d.", APP_ZIGBEE_CLUSTER3_2_NAME, APP_ZIGBEE_ENDPOINT_2 );
  LOG_INFO_APP( "  %s on Endpoint %d.", APP_ZIGBEE_CLUSTER4_3_NAME, APP_ZIGBEE_ENDPOINT_3 );

  /* USER CODE BEGIN APP_ZIGBEE_PrintApplicationInfo2 */

  /* USER CODE END APP_ZIGBEE_PrintApplicationInfo2 */

  LOG_INFO_APP( "**********************************************************" );
}

/**
 * @brief  IasZone Server 'ModeChange' 3 command Callback
 */
static enum ZclStatusCodeT APP_ZIGBEE_IasZoneServerModeChangeCallback_3( struct ZbZclClusterT * pstCluster, void * arg, enum ZbZclIasZoneServerModeT eMode, struct ZbZclIasZoneClientTestModeReqT * pstRequest )
{
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;
  /* USER CODE BEGIN APP_ZIGBEE_IasZoneServerModeChangeCallback_3 */

  /* USER CODE END APP_ZIGBEE_IasZoneServerModeChangeCallback_3 */
  return eStatus;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

void APP_ZIGBEE_UpdateDoorState(bool is_open) {
	uint16_t current_status = 0;
	enum ZclStatusCodeT attr_status;

	// 1. Přečtení aktuálního stavu (abychom přepsali jen Alarm bit)
	current_status = (uint16_t)ZbZclAttrIntegerRead(
	        stZigbeeAppInfo.IasZoneServer_3,
	        ZCL_IAS_ZONE_SVR_ATTR_ZONE_STATUS,
	        NULL,
	        &attr_status);

	// 2. Úprava nultého bitu (Alarm 1)
	if (is_open) {
		current_status |= 0x0001;  // Nastavení bitu (otevřeno)
	} else {
		current_status &= ~0x0001; // Vymazání bitu (zavřeno)
	}

	// 3. Zápis nového stavu zpět do paměti
	enum ZclStatusCodeT write_status = ZbZclAttrIntegerWrite(
	        stZigbeeAppInfo.IasZoneServer_3,
	        ZCL_IAS_ZONE_SVR_ATTR_ZONE_STATUS,
	        current_status);

	if (write_status == ZCL_STATUS_SUCCESS) {
		LOG_INFO_APP("[IAS ZONE] Stav dveri zapsan: %s",
				is_open ? "OTEVRENO" : "ZAVRENO");
	} else {
		LOG_ERROR_APP("Chyba zapisu IAS Zone atributu: 0x%02X", write_status);
	}

}

void APP_ZIGBEE_UpdateTemperature(float temperature_celsius)
{
    int16_t zigbee_temp = (int16_t)(temperature_celsius * 100.0f);

    enum ZclStatusCodeT status = ZbZclAttrIntegerWrite(
        stZigbeeAppInfo.TempMeasServer_2, // Náš Teplotní Server
        ZCL_TEMP_MEAS_ATTR_MEAS_VAL,
        zigbee_temp
    );

    if (status == ZCL_STATUS_SUCCESS) {
        LOG_INFO_APP("Teplota %d ulozena do pameti (ZCL format).", zigbee_temp);
    } else {
        LOG_ERROR_APP("Chyba zapisu teploty: 0x%02X", status);
    }
}

void APP_BSP_Button1Action(void)
{
  struct ZbApsAddrT     stDest;
  enum ZclStatusCodeT   eStatus;

  // First, verify if Appli has already Join a Network
  if ( APP_ZIGBEE_IsAppliJoinNetwork() != false )
  {
    //Prepare destination
    memset( &stDest, 0, sizeof( stDest) );
    stDest.endpoint = APP_ZIGBEE_ENDPOINT_1;
    stDest.mode = ZB_APSDE_ADDRMODE_SHORT;
    stDest.nwkAddr = 0;

    LOG_INFO_APP( "\r[ONOFF] SW1 pushed, sending 'TOGGLE'" );
    eStatus = ZbZclOnOffClientToggleReq( stZigbeeAppInfo.OnOffClient_1, &stDest, NULL, NULL );
    if ( eStatus != ZCL_STATUS_SUCCESS )
    {
      LOG_ERROR_APP( "[ONOFF] Error, OnOff Client Request failed (0x%02X).", eStatus );
    }
  }
}

/* USER CODE END FD_LOCAL_FUNCTIONS */
