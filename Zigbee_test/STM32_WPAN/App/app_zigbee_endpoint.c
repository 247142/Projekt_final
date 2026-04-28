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
#include "zcl/general/zcl.temp.meas.h"
#include "zcl/general/zcl.onoff.h"
#include "zcl/security/zcl.ias_zone.h"

/* USER CODE BEGIN PI */
#include "main.h"
#include "epd_custom.h"
#include "zigbee.h"
#include "zigbee.nwk.h" // Tohle je důležité pro ZbNwkGetIeeeAddress
#include "zcl/security/zcl.ias_zone.h"
/* USER CODE END PI */

/* Private defines -----------------------------------------------------------*/
#define APP_ZIGBEE_CHANNEL                13u
#define APP_ZIGBEE_CHANNEL_MASK           ( 1u << APP_ZIGBEE_CHANNEL )
#define APP_ZIGBEE_TX_POWER               ((int8_t) 10)    /* TX-Power is at +10 dBm. */

#define APP_ZIGBEE_ENDPOINT               1u
#define APP_ZIGBEE_PROFILE_ID             ZCL_PROFILE_HOME_AUTOMATION
#define APP_ZIGBEE_DEVICE_ID              ZCL_DEVICE_ONOFF_SWITCH

#define APP_ZIGBEE_CLUSTER1_ID            ZCL_CLUSTER_MEAS_TEMPERATURE
#define APP_ZIGBEE_CLUSTER1_NAME          "TempMeas Client"

#define APP_ZIGBEE_CLUSTER2_ID            ZCL_CLUSTER_ONOFF
#define APP_ZIGBEE_CLUSTER2_NAME          "OnOff Server"

#define APP_ZIGBEE_CLUSTER3_ID            ZCL_CLUSTER_IAS_ZONE
#define APP_ZIGBEE_CLUSTER3_NAME          "IasZone Client"

/* USER CODE BEGIN PD */

/* USER CODE END PD */

// -- Redefine Clusters to better code read --
#define TempMeasClient                    pstZbCluster[0]
#define OnOffServer                       pstZbCluster[1]
#define IasZoneClient                     pstZbCluster[3]

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private constants ---------------------------------------------------------*/
/* USER CODE BEGIN PC */

/* USER CODE END PC */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
uint16_t kolega_adresa = 0xFFFF; // Definice (vytvoření) proměnné

extern volatile int16_t nova_teplota;
extern volatile uint8_t ma_se_prekreslit;
extern volatile uint8_t stav_dveri;

// Prototyp funkce pro reporting, aby to neházelo warning
void APP_ZIGBEE_ConfigReporting(uint16_t nwkAddr);

// Definice map pro číslice (převzato z tvého souboru)
extern const unsigned char DSPNUM_W0[];
extern const unsigned char DSPNUM_W1[];
extern const unsigned char DSPNUM_W2[];
extern const unsigned char DSPNUM_W3[];
extern const unsigned char DSPNUM_W4[];
extern const unsigned char DSPNUM_W5[];
extern const unsigned char DSPNUM_W6[];
extern const unsigned char DSPNUM_W7[];
extern const unsigned char DSPNUM_W8[];
extern const unsigned char DSPNUM_W9[];
extern const unsigned char DSPNUM_off[];
// Symbol "ZAVŘENO" (vypadá jako obdélník/zavřené dveře - využívá segmenty číslice 0)
const unsigned char SYMBOL_DVERE_ZAVRENO[] = {0x00, 0xbf, 0x1f, 0xbf, 0x1f, 0xbf, 0x1f, 0xbf, 0x1f, 0xbf, 0x1f, 0xbf, 0x1f, 0x00, 0x00};

// Symbol "OTEVŘENO" (vynecháme pravou stranu "dveří", takže to vypadá jako otevřené křídlo - tvar [ )
const unsigned char SYMBOL_DVERE_OTEVRENO[] = {0x00, 0xbf, 0x00, 0xbf, 0x00, 0xbf, 0x00, 0xbf, 0x00, 0xbf, 0x00, 0xbf, 0x00, 0x00, 0x00};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* OnOff Server Callbacks */
static enum ZclStatusCodeT  APP_ZIGBEE_OnOffServerOffCallback               ( struct ZbZclClusterT * pstCluster, struct ZbZclAddrInfoT * pstSrcInfo, void * arg );
static enum ZclStatusCodeT  APP_ZIGBEE_OnOffServerOnCallback                ( struct ZbZclClusterT * pstCluster, struct ZbZclAddrInfoT * pstSrcInfo, void * arg );
static enum ZclStatusCodeT  APP_ZIGBEE_OnOffServerToggleCallback            ( struct ZbZclClusterT * pstCluster, struct ZbZclAddrInfoT * pstSrcInfo, void * arg );

static struct ZbZclOnOffServerCallbacksT stOnOffServerCallbacks =
{
  .off = APP_ZIGBEE_OnOffServerOffCallback,
  .on = APP_ZIGBEE_OnOffServerOnCallback,
  .toggle = APP_ZIGBEE_OnOffServerToggleCallback,
};

/* IasZone Client Callbacks */
static void                 APP_ZIGBEE_IasZoneClientZoneStatusChangeCallback( struct ZbZclClusterT * pstCluster, void * arg, struct ZbZclIasZoneServerStatusChangeNotifyT * pstNotify, const struct ZbApsAddrT * pstSource );
static enum ZclStatusCodeT  APP_ZIGBEE_IasZoneClientZoneEnrollReqCallback   ( struct ZbZclClusterT * pstCluster, void * arg, struct ZbZclIasZoneServerEnrollRequestT * pstRequest, uint64_t lExtendedSrcAddress, enum ZbZclIasZoneClientResponseCodeT * pstRspCode, uint8_t * pcZoneId );

static struct ZbZclIasZoneClientCallbacksT stIasZoneClientCallbacks =
{
  .zone_status_change = APP_ZIGBEE_IasZoneClientZoneStatusChangeCallback,
  .zone_enroll_req = APP_ZIGBEE_IasZoneClientZoneEnrollReqCallback,
};

/* USER CODE BEGIN PFP */
void ZIGBEE_Precti_Teplotu_Od_Kolegy(void);
static void APP_ZIGBEE_TeplotaOdpovedCallback(const struct ZbZclReadRspT *readRsp, void *cb_arg);
static void APP_ZIGBEE_TempMeasServerReport(struct ZbZclClusterT *pstCluster, struct ZbZclHeaderT *pstZclHeader, struct ZbApsdeDataIndT *pstDataInd,
                                             uint16_t iAttributeId, enum ZclDataTypeT eDataType, const uint8_t *pDataInputPayload, uint16_t iDataInputLength,
                                             bool *bDiscard);

static enum ZclStatusCodeT APP_ZIGBEE_TempMeasServerCommand(struct ZbZclClusterT *pstCluster, struct ZbZclHeaderT *pstZclHeader, struct ZbApsdeDataIndT *pstDataInd);
void ZIGBEE_Cluster_Dashboard(void);
uint16_t Moje_Zobraz_Binding_Tabulku(struct ZigBeeT *pstZigbee);
static void My_IAS_Write_CB(const struct ZbZclWriteRspT *pstWriteRsp, void *arg);
uint64_t ZbExtendedAddress(struct ZigBeeT *zb);
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
  printf("[1/4] STACK: Zigbee stack inicializace.\r\n");
  /* Initialization of the Zigbee stack */
  APP_ZIGBEE_Init();

  /* Configure Application Form/Join parameters : Startup, Persistence and Start with/without Form/Join */
  stZigbeeAppInfo.eStartupControl = ZbStartTypeForm;
  stZigbeeAppInfo.bPersistNotification = false;
  stZigbeeAppInfo.bNwkStartup = true;

  /* USER CODE BEGIN APP_ZIGBEE_ApplicationInit */
  /* USER CODE BEGIN APP_ZIGBEE_Init_2 */

    // Třikrát rychle blikneme modrou LED (PB4) jako signál startu

    // Necháme ji svítit, abychom věděli, že jsme v Zigbee smyčce

    printf("[2/4] STACK: Zigbee stack inicializovan.\r\n");
    /* USER CODE END APP_ZIGBEE_Init_2 */
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

	printf("[4/4] RADIO: Startuji Zigbee sit na kanale %d...\r\n", APP_ZIGBEE_CHANNEL);
	LOG_INFO_APP("TEST: Sit startuje, zapinam LED natvrdo!");
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

  /* Add EndPoint */
  memset( &stRequest, 0, sizeof( stRequest ) );
  memset( &stConfig, 0, sizeof( stConfig ) );

  stRequest.profileId = APP_ZIGBEE_PROFILE_ID;
  stRequest.deviceId = APP_ZIGBEE_DEVICE_ID;
  stRequest.endpoint = APP_ZIGBEE_ENDPOINT;
  ZbZclAddEndpoint( stZigbeeAppInfo.pstZigbee, &stRequest, &stConfig );
  assert( stConfig.status == ZB_STATUS_SUCCESS );

  /* Add TempMeas Client Cluster */
  stZigbeeAppInfo.TempMeasClient = ZbZclTempMeasClientAlloc( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT );
  assert( stZigbeeAppInfo.TempMeasClient != NULL );
  if ( ZbZclClusterEndpointRegister( stZigbeeAppInfo.TempMeasClient ) == false )
  {
    LOG_ERROR_APP( "Error during TempMeas Client Endpoint Register." );
  }

  /* Add OnOff Server Cluster */
  stZigbeeAppInfo.OnOffServer = ZbZclOnOffServerAlloc( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT, &stOnOffServerCallbacks, NULL );
  assert( stZigbeeAppInfo.OnOffServer != NULL );
  if ( ZbZclClusterEndpointRegister( stZigbeeAppInfo.OnOffServer ) == false )
  {
    LOG_ERROR_APP( "Error during OnOff Server Endpoint Register." );
  }

  /* Add IasZone Client Cluster */
  stZigbeeAppInfo.IasZoneClient = ZbZclIasZoneClientAlloc( stZigbeeAppInfo.pstZigbee, APP_ZIGBEE_ENDPOINT, &stIasZoneClientCallbacks, NULL );
  assert( stZigbeeAppInfo.IasZoneClient != NULL );
  if ( ZbZclClusterEndpointRegister( stZigbeeAppInfo.IasZoneClient ) == false )
  {
    LOG_ERROR_APP( "Error during IasZone Client Endpoint Register." );
  }

  /* USER CODE BEGIN APP_ZIGBEE_ConfigEndpoints2 */

  else
    {
      printf("[OK] Cluster OnOff Server uspesne pridan.\r\n");
    }
  if (stZigbeeAppInfo.TempMeasClient != NULL) {
        stZigbeeAppInfo.TempMeasClient->report = APP_ZIGBEE_TempMeasServerReport;
        stZigbeeAppInfo.TempMeasClient->command = APP_ZIGBEE_TempMeasServerCommand;
        printf("[OK] Callbacky pro reporty zaregistrovany.\r\n");
    }
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
  /* Pomocný callback, abychom viděli, co senzor na ten zápis říká */

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
  /* USER CODE BEGIN APP_ZIGBEE_SetNewDevice */
  kolega_adresa = iShortAddress;

  // ZÍSKÁNÍ TVOJÍ ADRESY: Tato funkce je v STM32WBA prokazatelně funkční!
  uint64_t mojeIeee = ZbExtendedAddress(stZigbeeAppInfo.pstZigbee);

  struct ZbZclWriteReqT writeReq;
  memset(&writeReq, 0, sizeof(writeReq));

  writeReq.dst.mode = ZB_APSDE_ADDRMODE_SHORT;
  writeReq.dst.nwkAddr = iShortAddress;
  writeReq.dst.endpoint = 3; // Senzor IAS

  writeReq.attr[0].attrId = ZCL_IAS_ZONE_SVR_ATTR_CIE_ADDR;
  writeReq.attr[0].type = (enum ZclDataTypeT)0xf0;
  writeReq.attr[0].value = (uint8_t *)&mojeIeee;
  writeReq.attr[0].length = sizeof(mojeIeee);
  writeReq.count = 1;

  printf("[ZIGBEE] Zapisuji moji adresu (0x%llx) kolegovi na EP 3...\r\n", mojeIeee);
  ZbZclWriteReq(stZigbeeAppInfo.IasZoneClient, &writeReq, My_IAS_Write_CB, NULL);

  APP_ZIGBEE_ConfigReporting(iShortAddress);
  ZIGBEE_Cluster_Dashboard();
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
  LOG_INFO_APP( "Network config : CENTRALIZED COORDINATOR" );

  /* USER CODE BEGIN APP_ZIGBEE_PrintApplicationInfo1 */

  /* USER CODE END APP_ZIGBEE_PrintApplicationInfo1 */
  LOG_INFO_APP( "Channel used: %d.", APP_ZIGBEE_CHANNEL );

  APP_ZIGBEE_PrintGenericInfo();

  LOG_INFO_APP( "Clusters allocated are:" );
  LOG_INFO_APP( "  %s on Endpoint %d.", APP_ZIGBEE_CLUSTER1_NAME, APP_ZIGBEE_ENDPOINT );
  LOG_INFO_APP( "  %s on Endpoint %d.", APP_ZIGBEE_CLUSTER2_NAME, APP_ZIGBEE_ENDPOINT );
  LOG_INFO_APP( "  %s on Endpoint %d.", APP_ZIGBEE_CLUSTER3_NAME, APP_ZIGBEE_ENDPOINT );

  /* USER CODE BEGIN APP_ZIGBEE_PrintApplicationInfo2 */

  /* USER CODE END APP_ZIGBEE_PrintApplicationInfo2 */

  LOG_INFO_APP( "**********************************************************" );
}

/**
 * @brief  OnOff Server 'Off' command Callback
 */
static enum ZclStatusCodeT APP_ZIGBEE_OnOffServerOffCallback( struct ZbZclClusterT * pstCluster, struct ZbZclAddrInfoT * pstSrcInfo, void * arg )
{
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;
  /* USER CODE BEGIN APP_ZIGBEE_OnOffServerOffCallback */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET);
  printf("\r\n[ZIGBEE] PRIJAT PRIKAZ: OFF -> LED zhasnuta\r\n");
  LOG_INFO_APP("ZIGBEE: Prijat prikaz OFF -> LED zhasnuta");
  /* USER CODE END APP_ZIGBEE_OnOffServerOffCallback */
  return eStatus;
}

/**
 * @brief  OnOff Server 'On' command Callback
 */
static enum ZclStatusCodeT APP_ZIGBEE_OnOffServerOnCallback( struct ZbZclClusterT * pstCluster, struct ZbZclAddrInfoT * pstSrcInfo, void * arg )
{
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;
  /* USER CODE BEGIN APP_ZIGBEE_OnOffServerOnCallback */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);
  printf("\r\n[ZIGBEE] PRIJAT PRIKAZ: ON -> LED sviti\r\n");
  LOG_INFO_APP("ZIGBEE: Prijat prikaz ON -> LED sviti");
  /* USER CODE END APP_ZIGBEE_OnOffServerOnCallback */
  return eStatus;
}

/**
 * @brief  OnOff Server 'Toggle' command Callback
 */
static enum ZclStatusCodeT APP_ZIGBEE_OnOffServerToggleCallback( struct ZbZclClusterT * pstCluster, struct ZbZclAddrInfoT * pstSrcInfo, void * arg )
{
  enum ZclStatusCodeT   eStatus = ZCL_STATUS_SUCCESS;
  /* USER CODE BEGIN APP_ZIGBEE_OnOffServerToggleCallback */
  HAL_GPIO_TogglePin(GPIOB, LED_BLUE_Pin);
  printf("\r\n[ZIGBEE] PRIJAT PRIKAZ: TOGGLE\r\n");
  LOG_INFO_APP("ZIGBEE: Prijat prikaz TOGGLE");
  /* USER CODE END APP_ZIGBEE_OnOffServerToggleCallback */
  return eStatus;
}

/**
 * @brief  IasZone Client 'ZoneStatusChange' command Callback
 */
static void APP_ZIGBEE_IasZoneClientZoneStatusChangeCallback( struct ZbZclClusterT * pstCluster, void * arg, struct ZbZclIasZoneServerStatusChangeNotifyT * pstNotify, const struct ZbApsAddrT * pstSource )
{
  /* USER CODE BEGIN APP_ZIGBEE_IasZoneClientZoneStatusChangeCallback */
	printf("\r\n[ZIGBEE] ClusterIAS přijat");
  // Bit 0 (0x0001) v zone_status znamená Alarm 1 (u dveřního senzoru je to otevření)
  if (pstNotify->zone_status & 0x0001) {
      stav_dveri = 1; // Nastavíme naši globální proměnnou na OTEVŘENO
      printf("\r\n[ZIGBEE] POZOR: Dvere OTEVRENY! (Status: 0x%04X)\r\n", pstNotify->zone_status);
  } else {
      stav_dveri = 0; // Nastavíme naši globální proměnnou na ZAVŘENO
      printf("\r\n[ZIGBEE] KLID: Dvere ZAVRENY. (Status: 0x%04X)\r\n", pstNotify->zone_status);
  }

  // Zvedneme vlajku, aby main.c věděl, že má překreslit displej
  ma_se_prekreslit = 1;

  /* USER CODE END APP_ZIGBEE_IasZoneClientZoneStatusChangeCallback */
}

/**
 * @brief  IasZone Client 'ZoneEnrollReq' command Callback
 */

static enum ZclStatusCodeT APP_ZIGBEE_IasZoneClientZoneEnrollReqCallback( struct ZbZclClusterT * pstCluster, void * arg, struct ZbZclIasZoneServerEnrollRequestT * pstRequest, uint64_t lExtendedSrcAddress, enum ZbZclIasZoneClientResponseCodeT * pstRspCode, uint8_t * pcZoneId )
{
  /* USER CODE BEGIN APP_ZIGBEE_IasZoneClientZoneEnrollReqCallback */
  printf("\r\n[ZIGBEE] IAS Zone Enroll Request od 0x%llx. Schvaluji...\r\n", lExtendedSrcAddress);


  *pstRspCode = ZCL_IAS_ZONE_CLI_RESP_SUCCESS; // Říkáme senzoru: OK!
  *pcZoneId = 0x01; // Přiřadíme mu ID 1

  /* USER CODE END APP_ZIGBEE_IasZoneClientZoneEnrollReqCallback */
  return ZCL_STATUS_SUCCESS;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

/**
 * @brief  Tato funkce se zavolá AUTOMATICKY, když kolega pošle report
 */
static void APP_ZIGBEE_TempMeasServerReport(struct ZbZclClusterT *pstCluster, struct ZbZclHeaderT *pstZclHeader, struct ZbApsdeDataIndT *pstDataInd,
                                             uint16_t iAttributeId, enum ZclDataTypeT eDataType, const uint8_t *pDataInputPayload, uint16_t iDataInputLength,
                                             bool *bDiscard)
{
  if (iAttributeId == ZCL_TEMP_MEAS_ATTR_MEAS_VAL)
  {
      // Přečteme 16-bitovou hodnotu
      int16_t iAttrValue = (int16_t)pletoh16(pDataInputPayload);

      // Přepočet na desetiny (2543 -> 254)
      nova_teplota = (int16_t)(iAttrValue / 10);

      // Nastavíme vlajku
      ma_se_prekreslit = 1;

      printf("\r\n[REPORT] Nova teplota: %d.%d C\r\n", (nova_teplota / 10), (nova_teplota % 10));
  }
}

/**
 * @brief  Povinný callback pro commandy (v tomto případě stačí SUCCESS)
 */
static enum ZclStatusCodeT APP_ZIGBEE_TempMeasServerCommand(struct ZbZclClusterT *pstCluster, struct ZbZclHeaderT *pstZclHeader, struct ZbApsdeDataIndT *pstDataInd)
{
    return ZCL_STATUS_SUCCESS;
}

/**
 * @brief  Konfigurace reportování - pošle kolegovi příkaz, aby hlásil teplotu sám
 */
void APP_ZIGBEE_ConfigReporting(uint16_t nwkAddr)
{
    struct ZbZclAttrReportConfigT stReportConfig;
    memset(&stReportConfig, 0, sizeof(stReportConfig));

    stReportConfig.dst.mode = ZB_APSDE_ADDRMODE_SHORT;
    stReportConfig.dst.nwkAddr = nwkAddr;
    stReportConfig.dst.endpoint = 2; // Pozor: Pokud kolega jede example, dej tu 17!
    stReportConfig.num_records = 1;

    stReportConfig.record_list[0].direction = ZCL_REPORT_DIRECTION_NORMAL;
    stReportConfig.record_list[0].attr_id = ZCL_TEMP_MEAS_ATTR_MEAS_VAL;
    stReportConfig.record_list[0].attr_type = ZCL_DATATYPE_SIGNED_16BIT;
    stReportConfig.record_list[0].min = 5;   // Posílat nejčastěji po 5s
    stReportConfig.record_list[0].max = 30;  // Posílat nejméně každých 30s
    stReportConfig.record_list[0].change = 0; // Poslat při jakékoliv změně

    printf("[ZIGBEE] Konfiguruji reporting pro 0x%04X...\r\n", nwkAddr);
    ZbZclAttrReportConfigReq(stZigbeeAppInfo.TempMeasClient, &stReportConfig, NULL, NULL);
}

// Původní funkce pro manuální čtení (můžeš ji nechat jako zálohu)
void ZIGBEE_Precti_Teplotu(void)
{
    if (kolega_adresa == 0xFFFF) return;
    struct ZbZclReadReqT readReq;
    memset(&readReq, 0, sizeof(readReq));
    readReq.dst.mode = ZB_APSDE_ADDRMODE_SHORT;
    readReq.dst.nwkAddr = kolega_adresa;
    readReq.dst.endpoint = 2; // Zase: Pozor na 17 u examplu
    readReq.attr[0] = ZCL_TEMP_MEAS_ATTR_MEAS_VAL;
    readReq.count = 1;
    ZbZclReadReq(stZigbeeAppInfo.TempMeasClient, &readReq, APP_ZIGBEE_TeplotaOdpovedCallback, NULL);
}

static void APP_ZIGBEE_TeplotaOdpovedCallback(const struct ZbZclReadRspT *readRsp, void *cb_arg)
{
    if (readRsp->status == ZCL_STATUS_SUCCESS && readRsp->attr[0].status == ZCL_STATUS_SUCCESS) {
        int16_t rawTemp;
        memcpy(&rawTemp, readRsp->attr[0].value, sizeof(int16_t));

        nova_teplota = (int16_t)(rawTemp / 10);
        ma_se_prekreslit = 1;

        printf("\r\n[MANUAL] Teplota: %d.%d C\r\n", (nova_teplota / 10), (nova_teplota % 10));
    }
}
void ZIGBEE_Cluster_Dashboard(void)
{
    printf("\r\n========== ZIGBEE CLUSTER DASHBOARD ==========\r\n");

    // 1. STAV PŘIPOJENÍ
    if (kolega_adresa == 0xFFFF) {
        printf("[SIT] Stav: CEKANI NA KOLEGU...\r\n");
    } else {
        printf("[SIT] Kolega: 0x%04X (Online)\r\n", kolega_adresa);
    }

    // 2. IAS ZONE (Dveře) - KLIENT
    if (stZigbeeAppInfo.IasZoneClient != NULL) {
        // Tady zkusíme přečíst interní stav, pokud ho stack ukládá,
        // nebo aspoň vypíšeme, že je cluster alokován.
        printf("[IAS] Cluster: Alokovan (Client)\r\n");
        printf("[IAS] Posledni stav dveri: %s\r\n", (stav_dveri ? "OTEVRENO" : "ZAVRENO"));
        // Tip: Pokud se dostaneš k atributům, můžeš vypsat i Zone ID
    } else {
        printf("[IAS] Cluster: CHYBA - Nealokovan!\r\n");
    }

    // 3. TEPLOTA - KLIENT
    if (stZigbeeAppInfo.TempMeasClient != NULL) {
        printf("[TMP] Cluster: Alokovan (Client)\r\n");
        printf("[TMP] Posledni hodnota: %d.%d C\r\n", (nova_teplota / 10), (nova_teplota % 10));
    }

    // 4. ON/OFF - SERVER
    if (stZigbeeAppInfo.OnOffServer != NULL) {
        uint8_t onOffState;
        // Přečteme lokální stav LED z clusteru
        ZbZclAttrRead(stZigbeeAppInfo.OnOffServer, ZCL_ONOFF_ATTR_ONOFF, NULL, &onOffState, sizeof(onOffState), false);
        printf("[LED] Cluster: Alokovan (Server)\r\n");
        printf("[LED] Stav v databazi: %s\r\n", (onOffState ? "ZAPNUTO" : "VYPNUTO"));
    }

    // 5. BINDING TABLE (to nejdůležitější pro propojení)


    printf("==============================================\r\n");
}
static void My_IAS_Write_CB(const struct ZbZclWriteRspT *pstWriteRsp, void *arg)
  {
    if (pstWriteRsp->status == ZCL_STATUS_SUCCESS) {
      printf("\r\n[DEBUG] Senzor potvrdil prijeti CIE adresy! Ted by mel poslat Enroll Request...");
    } else {
      printf("\r\n[DEBUG] CHYBA: Senzor odmitl zapis (Status: 0x%02X). Zkontroluj Endpoint!", pstWriteRsp->status);
    }
  }
/* USER CODE END FD_LOCAL_FUNCTIONS */
/* USER CODE END FD_LOCAL_FUNCTIONS */
