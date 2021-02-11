/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : App/hts_app.c
  * Description        : Health Thermometer Service Application
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_common.h"
#include "dbg_trace.h"
#include "app_ble.h"
#include "ble.h"
#include "hts_app.h"
#include <time.h>
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  HTS_TemperatureValue_t TemperatureMeasurementChar;
#if(BLE_CFG_HTS_TEMPERATURE_TYPE_VALUE_STATIC == 1)
  HTS_Temperature_Type_t TemperatureTypeChar;
#endif
#if(BLE_CFG_HTS_INTERMEDIATE_TEMPERATURE == 1)
  HTS_TemperatureValue_t IntermediateTemperatureChar;
  uint8_t TimerIntTemp_Id;
  uint8_t IntTempEnabled;
#endif
#if(BLE_CFG_HTS_MEASUREMENT_INTERVAL == 1)
  uint16_t  MeasurementIntervalChar;
  uint8_t   TimerMeasInt_Id;
  uint8_t   Indication_Status;
#endif
  uint8_t TimerMeasurement_Id;
  uint8_t TimerMeasurementStarted;
} HTSAPP_Context_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private defines ------------------------------------------------------------*/
#define DEFAULT_HTS_MEASUREMENT_INTERVAL   (1000000/CFG_TS_TICK_VAL)  /**< 1s */
#define DEFAULT_TEMPERATURE_TYPE          TT_Armpit
#define NB_SAVED_MEASURES                                                     10
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macros -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/**
 * START of Section BLE_APP_CONTEXT
 */

PLACE_IN_SECTION("BLE_APP_CONTEXT") static HTSAPP_Context_t HTSAPP_Context;
PLACE_IN_SECTION("BLE_APP_CONTEXT") static HTS_TemperatureValue_t HTSMeasurement[NB_SAVED_MEASURES];
PLACE_IN_SECTION("BLE_APP_CONTEXT") static int8_t HTS_CurrentIndex, HTS_OldIndex;

/**
 * END of Section BLE_APP_CONTEXT
 */

 /* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void HTSAPP_Update_TimeStamp(void);
static uint32_t HTSAPP_Read_RTC_SSR_SS ( void );
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
static uint32_t HTSAPP_Read_RTC_SSR_SS ( void )
{
  return ((uint32_t)(READ_BIT(RTC->SSR, RTC_SSR_SS)));
}

static void HTSAPP_UpdateMeasurement( void )
{
/* USER CODE BEGIN HTSAPP_UpdateMeasurement */

/* USER CODE END HTSAPP_UpdateMeasurement */
  return;
}

#if(BLE_CFG_HTS_INTERMEDIATE_TEMPERATURE == 1)
static void HTSAPP_UpdateIntermediateTemperature( void )
{
/* USER CODE BEGIN HTSAPP_UpdateIntermediateTemperature */

/* USER CODE END HTSAPP_UpdateIntermediateTemperature */
  return;
}
#endif

#if(BLE_CFG_HTS_MEASUREMENT_INTERVAL == 1)
static void HTSAPP_UpdateMeasurementInterval( void )
{
/* USER CODE BEGIN HTSAPP_UpdateMeasurementInterval */

/* USER CODE END HTSAPP_UpdateMeasurementInterval */
  return;
}
#endif

#if(BLE_CFG_HTS_TIME_STAMP_FLAG != 0)
static void HTSAPP_Update_TimeStamp(void)
{
/* USER CODE BEGIN HTSAPP_Update_TimeStamp */

/* USER CODE END HTSAPP_Update_TimeStamp */
}
#endif

#if(BLE_CFG_HTS_TIME_STAMP_FLAG != 0)
static void HTSAPP_Store(void)
{
/* USER CODE BEGIN HTSAPP_Store */

/* USER CODE END HTSAPP_Store */
}

static void HTSAPP_Suppress(void)
{
/* USER CODE BEGIN HTSAPP_Suppress */

/* USER CODE END HTSAPP_Suppress */
}
#endif

/* Public functions ----------------------------------------------------------*/

void HTS_App_Notification(HTS_App_Notification_evt_t *pNotification)
{
/* USER CODE BEGIN HTS_App_Notification */

/* USER CODE END HTS_App_Notification */
  return;
}

void HTSAPP_Init(void)
{
/* USER CODE BEGIN HTSAPP_Init */
	/** Create timer to handle the temperature measurement   */
	HW_TS_Create(CFG_TIM_APP_ID_ISR, &pui8AppTimerId, hw_ts_Repeated, hts_onTimeoutCb);
	HW_TS_Start (CFG_TIM_APP_ID_ISR, DEFAULT_HTS_MEASUREMENT_INTERVAL);
	/*Register the measurement task*/
	UTIL_SEQ_RegTask( 1<<CFG_TASK_HTS_MEAS_REQ_ID, UTIL_SEQ_RFU, HTSAPP_Measurement);
	APP_DBG_MSG(" health thermometer initialized \n");
/* USER CODE END HTSAPP_Init */
return;
}

void HTSAPP_Measurement(void)
{
/* USER CODE BEGIN HTSAPP_Measurement */
	APP_DBG_MSG("temperature measurement \r\n");
/* USER CODE END HTSAPP_Measurement */
  return;
}

void HTSAPP_IntermediateTemperature(void)
{
/* USER CODE BEGIN HTSAPP_IntermediateTemperature */

/* USER CODE END HTSAPP_IntermediateTemperature */
  return;
}

#if(BLE_CFG_HTS_MEASUREMENT_INTERVAL == 1)
void HTSAPP_MeasurementInterval(void)
{
/* USER CODE BEGIN HTSAPP_MeasurementInterval */

/* USER CODE END HTSAPP_MeasurementInterval */
  return;
}
#endif

/**
 * @brief  Application service update characteristic
 * @param  None
 * @retval None
 */
void HTSAPP_Profile_UpdateChar(void)
{
/* USER CODE BEGIN HTSAPP_Profile_UpdateChar */

/* USER CODE END HTSAPP_Profile_UpdateChar */
}

/* USER CODE BEGIN FD */

/* USER CODE END FD */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/