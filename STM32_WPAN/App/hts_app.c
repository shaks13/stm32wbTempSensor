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
#include "iks01a3_env_sensors.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef struct {
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
	uint16_t MeasurementIntervalChar;
	uint8_t TimerMeasInt_Id;
	uint8_t Indication_Status;
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
PLACE_IN_SECTION("BLE_APP_CONTEXT") static int8_t HTS_CurrentIndex,
		HTS_OldIndex;
extern ADC_HandleTypeDef hadc1;
extern RTC_HandleTypeDef hrtc;
static uint8_t pui8AppTimerId;
/**
 * END of Section BLE_APP_CONTEXT
 */

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void HTSAPP_Update_TimeStamp(void);
static uint32_t HTSAPP_Read_RTC_SSR_SS(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
static uint32_t HTSAPP_Read_RTC_SSR_SS(void) {
	return ((uint32_t) (READ_BIT(RTC->SSR, RTC_SSR_SS)));
}

static void HTSAPP_UpdateMeasurement(void) {
	/* USER CODE BEGIN HTSAPP_UpdateMeasurement */

	/* USER CODE END HTSAPP_UpdateMeasurement */
	return;
}

#if(BLE_CFG_HTS_INTERMEDIATE_TEMPERATURE == 1)
static void HTSAPP_UpdateIntermediateTemperature(void) {
	/* USER CODE BEGIN HTSAPP_UpdateIntermediateTemperature */

	/* USER CODE END HTSAPP_UpdateIntermediateTemperature */
	return;
}
#endif

#if(BLE_CFG_HTS_MEASUREMENT_INTERVAL == 1)
static void HTSAPP_UpdateMeasurementInterval(void) {
	/* USER CODE BEGIN HTSAPP_UpdateMeasurementInterval */

	/* USER CODE END HTSAPP_UpdateMeasurementInterval */
	return;
}
#endif

#if(BLE_CFG_HTS_TIME_STAMP_FLAG != 0)
static void HTSAPP_Update_TimeStamp(void) {
	/* USER CODE BEGIN HTSAPP_Update_TimeStamp */

	/* USER CODE END HTSAPP_Update_TimeStamp */
}
#endif

#if(BLE_CFG_HTS_TIME_STAMP_FLAG != 0)
static void HTSAPP_Store(void) {
	/* USER CODE BEGIN HTSAPP_Store */

	/* USER CODE END HTSAPP_Store */
}

static void HTSAPP_Suppress(void) {
	/* USER CODE BEGIN HTSAPP_Suppress */

	/* USER CODE END HTSAPP_Suppress */
}
#endif

static void hts_onTimeoutCb(void) {
	/**
	 * The code shall be executed in the background as an aci command may be sent
	 * The background is the only place where the application can make sure a new aci command
	 * is not sent if there is a pending one
	 */
	UTIL_SEQ_SetTask(1 << CFG_TASK_HTS_MEAS_REQ_ID, CFG_SCH_PRIO_0);

	return;
}

static void hts_performTask(void) {
	APP_DBG_MSG("temperature measurement \r\n");
}

static void hts_configTempChannel(void) {
	ADC_ChannelConfTypeDef sConfig = { 0 };

	/** Configure Regular Channel	 */
	sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_12CYCLES_5;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
}

static void hts_getTimeStamp(HTS_TimeStamp_t *stimestamp) {
	RTC_DateTypeDef sDate;
	RTC_TimeTypeDef sTime;
	HAL_StatusTypeDef status;
	status = HAL_RTC_GetDate(&hrtc, &sDate,	RTC_FORMAT_BCD);
	status = HAL_RTC_GetTime(&hrtc, &sTime,	RTC_FORMAT_BCD);
	stimestamp->Day=sDate.Date;
	stimestamp->Month=sDate.Month;
	stimestamp->Year=sDate.Year;
	stimestamp->Hours=sTime.Hours;
	stimestamp->Minutes=sTime.Minutes;
	stimestamp->Seconds=sTime.Seconds;
}

/* Public functions ----------------------------------------------------------*/

void HTS_App_Notification(HTS_App_Notification_evt_t *pNotification) {
	/* USER CODE BEGIN HTS_App_Notification */

	/* USER CODE END HTS_App_Notification */
	return;
}

void HTSAPP_Init(void) {
	uint8_t id = 0;
	/* USER CODE BEGIN HTSAPP_Init */
	/*Register the measurement task*/
	UTIL_SEQ_RegTask(1 << CFG_TASK_HTS_MEAS_REQ_ID, UTIL_SEQ_RFU,
			HTSAPP_Measurement);
	/** Create timer to handle the temperature measurement   */
	HW_TS_Create(CFG_TIM_APP_ID_ISR, &pui8AppTimerId, hw_ts_Repeated,
			hts_onTimeoutCb);
	HW_TS_Start(pui8AppTimerId, DEFAULT_HTS_MEASUREMENT_INTERVAL);

	APP_DBG_MSG(" health thermometer initialized \n");
#if USE_ONCHIPSENSOR == 0
	IKS01A3_ENV_SENSOR_ReadID(IKS01A3_HTS221_0, &id);
	APP_DBG_MSG("id 0x%x\n", id);
#else
	hts_configTempChannel();
#endif
	/* USER CODE END HTSAPP_Init */
	return;
}

void HTSAPP_Measurement(void) {
#if USE_ONCHIPSENSOR == 0
	float fvalue;
	IKS01A3_ENV_SENSOR_GetValue(IKS01A3_HTS221_0, ENV_TEMPERATURE, &fvalue);
	APP_DBG_MSG("temp %i\n", (int8_t) fvalue);
	HTSAPP_Profile_UpdateChar(TEMPERATURE_MEASUREMENT_CHAR_UUID,(int16_t) fvalue);
#else
	int32_t i32value;
	uint32_t ui32adc;

	/* USER CODE BEGIN HTSAPP_Measurement */
//	APP_DBG_MSG("temperature measurement \r\n");
	HAL_ADC_Start(&hadc1);
	if (HAL_OK == HAL_ADC_PollForConversion(&hadc1, 100)) {
		ui32adc = HAL_ADC_GetValue(&hadc1);
		i32value = __HAL_ADC_CALC_TEMPERATURE(3300, ui32adc,
				ADC_RESOLUTION_12B);
	}
	HAL_ADC_Stop(&hadc1);
	APP_DBG_MSG("adc %lu & temp %li\n", ui32adc, i32value);
#endif
	/* USER CODE END HTSAPP_Measurement */
	return;
}

void HTSAPP_IntermediateTemperature(void) {
	/* USER CODE BEGIN HTSAPP_IntermediateTemperature */

	/* USER CODE END HTSAPP_IntermediateTemperature */
	return;
}

#if(BLE_CFG_HTS_MEASUREMENT_INTERVAL == 1)
void HTSAPP_MeasurementInterval(void) {
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
void HTSAPP_Profile_UpdateChar(uint8_t ui8characId, int16_t i16temp) {
	HTS_TemperatureValue_t sTemperatureValue;
	HTS_TimeStamp_t stimestamp;

	switch(ui8characId)
	{
		case TEMPERATURE_MEASUREMENT_CHAR_UUID:
			hts_getTimeStamp (&stimestamp);
			sTemperatureValue.TimeStamp = stimestamp;
			sTemperatureValue.MeasurementValue = i16temp;
			sTemperatureValue.TemperatureType = TT_Body;
			sTemperatureValue.Flags = 0;
			/* USER CODE BEGIN HTSAPP_Profile_UpdateChar */
			HTS_Update_Char(TEMPERATURE_MEASUREMENT_CHAR_UUID,
					(uint8_t*) &sTemperatureValue);
		break;
		default:
		break;
	}
	/* USER CODE END HTSAPP_Profile_UpdateChar */
}

/* USER CODE BEGIN FD */

/* USER CODE END FD */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
