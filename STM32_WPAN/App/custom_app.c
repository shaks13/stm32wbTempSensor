/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : App/custom_app.c
 * Description        : Custom Example Application (Server)
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
#include "main.h"
#include "app_common.h"
#include "dbg_trace.h"
#include "ble.h"
#include "custom_app.h"
#include "custom_stm.h"
#include "stm32_seq.h"
#include "iks01a3_env_sensors.h"
#include "uuid.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
	/* ambiantTemperature */
	uint8_t Temp_Notification_Status;
	uint8_t Temp_Indication_Status;
	/* BatteryLevel */
	uint8_t Batterylevel_Notification_Status;
	uint8_t Batterylevel_Indication_Status;
	/* USER CODE BEGIN CUSTOM_APP_Context_t */

	/* USER CODE END CUSTOM_APP_Context_t */

	uint16_t ConnectionHandle;
} Custom_App_Context_t;

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private defines ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DEFAULT_CUSTOMAPP_MEASUREMENT_INTERVAL   (10000000/CFG_TS_TICK_VAL)  /**< 10s */
#define DEFAULT_CUSTOMAPP_NOTIFICATION_INTERVAL   (1000000/CFG_TS_TICK_VAL)  /**< 1s */
#define CUSTIMAPP_UDPATEARRAY_SIZE				246
#define CUSTIMAPP_UDPATEARRAY_NBELMT			(CUSTIMAPP_UDPATEARRAY_SIZE/CUSTOM_TEMP_SIZE)
/* USER CODE END PD */

/* Private macros -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static uint8_t pui8AppTimerId;
static uint8_t pui8NtfTimerId;
static uint16_t ui16nthUpdateValue = 0; /* index of the last element of the UpdateCharData*/
//static uint16_t ui16nthNotifyRead = 0; /* index of the last element of the UpdateCharData*/
extern ADC_HandleTypeDef hadc1;
/**
 * START of Section BLE_APP_CONTEXT
 */

PLACE_IN_SECTION("BLE_APP_CONTEXT") static Custom_App_Context_t Custom_App_Context;

/**
 * END of Section BLE_APP_CONTEXT
 */

/* USER CODE BEGIN PV */
uint8_t UpdateCharData[CUSTIMAPP_UDPATEARRAY_SIZE];
uint8_t NotifyCharData[CUSTIMAPP_UDPATEARRAY_SIZE];
static uint16_t ui16nthNtfElemt = 0;
static uint8_t ui8state = CUSTOMAPP_STATE_IDLE;
static uint16_t ui16nbNtfSent = 0;
static uint16_t ui16nbTempAvailable = 0;
uint8_t SecureReadData;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* ambiantTemperature */
static void Custom_Temp_Update_Char(const uint32_t ui32uuid, const float fdata);
static void Custom_Temp_Send_Notification(const uint16_t ui16nthElemt);
static void Custom_Temp_Send_Indication(void);
/* BatteryLevel */
static void Custom_Batterylevel_Update_Char(const uint8_t ui8batteryLevel);
static void Custom_Batterylevel_Send_Notification(void);
static void Custom_Batterylevel_Send_Indication(void);

/* USER CODE BEGIN PFP */
static void ess_onTimeoutCb(void)
{
	/**
	 * The code shall be executed in the background as an aci command may be sent
	 * The background is the only place where the application can make sure a new aci command
	 * is not sent if there is a pending one
	 */
	UTIL_SEQ_SetTask(1 << CFG_TASK_ESS_MEAS_REQ_ID, CFG_SCH_PRIO_0);

	return;
}
static void ess_onNtfTimeoutCb(void)
{
	/**
	 * The code shall be executed in the background as an aci command may be sent
	 * The background is the only place where the application can make sure a new aci command
	 * is not sent if there is a pending one
	 */
	UTIL_SEQ_SetTask(1 << CFG_TASK_ESS_NTF_REQ_ID, CFG_SCH_PRIO_0);

	return;
}

/**
 * @brief  convert a float to sint16
 * @param[in] fdata
 * @param[in] pui8data
 */
static void custom_float2Sin16(const float fdata, uint8_t *pui8data)
{
	pui8data[0] = (int8_t) (fdata);
	pui8data[1] = ((int8_t) (fdata * 100) % 100);
}

void essApp_measure(void)
{
	int32_t i32value;
	uint32_t ui32adc;
#if USE_ONCHIPSENSOR == 0
	float fvalue;
	IKS01A3_ENV_SENSOR_GetValue(IKS01A3_HTS221_0, ENV_TEMPERATURE, &fvalue);
	APP_DBG_MSG("temp %i\n", (int8_t) fvalue);
	Custom_Temp_Update_Char(TEMPERATURE_MEASUREMENT_CHAR_UUID, fvalue);
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

	HAL_ADC_Start(&hadc1);
	if (HAL_OK == HAL_ADC_PollForConversion(&hadc1, 100))
	{
		ui32adc = HAL_ADC_GetValue(&hadc1);
//		i32value = __HAL_ADC_CALC_DATA_TO_VOLTAGE( (uint32_t) *VREFINT_CAL_ADDR, ui32adc,
//				ADC_RESOLUTION_12B)*3;/* ADC_CHANNEL_VBA /*!< ADC internal channel connected to Vbat/3:*/
		i32value = (3 * ui32adc * 3300) / 4098;
		APP_DBG_MSG("adc %lu & battery %lu\n", ui32adc, i32value);
		Custom_Batterylevel_Update_Char((uint8_t) (i32value >> 4));
	}
	HAL_ADC_Stop(&hadc1);
	/* USER CODE END HTSAPP_Measurement */
	return;
}

/**
 * @brief  convert a float to sint16
 * @param[in] fdata
 * @param[in] pui8data
 */
static void essApp_sendNotification(void)
{

	if (0 != (ui8state & CUSTOMAPP_STATE_NOTIFICATION))
	{
		Custom_Temp_Send_Notification(ui16nthNtfElemt);
	}
}

/* USER CODE END PFP */
/* Functions Definition ------------------------------------------------------*/
void Custom_STM_App_Notification(
		Custom_STM_App_Notification_evt_t *pNotification)
{
	/* USER CODE BEGIN CUSTOM_STM_App_Notification_1 */

	/* USER CODE END CUSTOM_STM_App_Notification_1 */
	switch (pNotification->Custom_Evt_Opcode)
	{
	/* USER CODE BEGIN CUSTOM_STM_App_Notification_Custom_Evt_Opcode */

	/* USER CODE END CUSTOM_STM_App_Notification_Custom_Evt_Opcode */

	/* ambiantTemperature */
	case CUSTOM_STM_TEMP_READ_EVT:
		/* USER CODE BEGIN CUSTOM_STM_TEMP_READ_EVT */

		/* USER CODE END CUSTOM_STM_TEMP_READ_EVT */
		break;

	case CUSTOM_STM_TEMP_NOTIFY_ENABLED_EVT:
		/* USER CODE BEGIN CUSTOM_STM_TEMP_NOTIFY_ENABLED_EVT */
		APP_DBG_MSG("[ntf] on\n");
		ui8state |= CUSTOMAPP_STATE_NOTIFICATION;
		/* USER CODE END CUSTOM_STM_TEMP_NOTIFY_ENABLED_EVT */
		break;

	case CUSTOM_STM_TEMP_NOTIFY_DISABLED_EVT:
		/* USER CODE BEGIN CUSTOM_STM_TEMP_NOTIFY_DISABLED_EVT */
		APP_DBG_MSG("[ntf] off\n");
		ui8state &= ~CUSTOMAPP_STATE_NOTIFICATION;
//		HW_TS_Stop(pui8NtfTimerId);
		/* USER CODE END CUSTOM_STM_TEMP_NOTIFY_DISABLED_EVT */
		break;

	case CUSTOM_STM_TEMP_INDICATE_ENABLED_EVT:
		/* USER CODE BEGIN CUSTOM_STM_TEMP_INDICATE_ENABLED_EVT */
		APP_DBG_MSG("[ind] on\n");
		ui8state |= CUSTOMAPP_STATE_INDICATION;
		/* USER CODE END CUSTOM_STM_TEMP_INDICATE_ENABLED_EVT */
		break;

	case CUSTOM_STM_TEMP_INDICATE_DISABLED_EVT:
		/* USER CODE BEGIN CUSTOM_STM_TEMP_INDICATE_DISABLED_EVT */
		APP_DBG_MSG("[ind] off\n");
		ui8state &= ~CUSTOMAPP_STATE_INDICATION;
		/* USER CODE END CUSTOM_STM_TEMP_INDICATE_DISABLED_EVT */
		break;

	case CUSTOM_STM_TEMPLOG_NOTIFY_ENABLED_EVT:
		/* USER CODE BEGIN CUSTOM_STM_TEMP_NOTIFY_ENABLED_EVT */
		APP_DBG_MSG("[ntf] log on\n");
		ui8state |= CUSTOMAPP_STATE_NOTIFICATION;
		HW_TS_Start(pui8NtfTimerId, DEFAULT_CUSTOMAPP_NOTIFICATION_INTERVAL);
		ui16nthNtfElemt = ui16nthUpdateValue;
		ui16nbNtfSent = 0;
		Custom_Temp_Send_Notification(ui16nthNtfElemt);
		/* USER CODE END CUSTOM_STM_TEMP_NOTIFY_ENABLED_EVT */
		break;

	case CUSTOM_STM_TEMPLOG_NOTIFY_DISABLED_EVT:
		/* USER CODE BEGIN CUSTOM_STM_TEMP_NOTIFY_ENABLED_EVT */
		APP_DBG_MSG("[ntf] log off\n");
		ui8state &= ~CUSTOMAPP_STATE_NOTIFICATION;
		HW_TS_Stop(pui8NtfTimerId);
		/* USER CODE END CUSTOM_STM_TEMP_NOTIFY_ENABLED_EVT */
		break;

		/* BatteryLevel */
	case CUSTOM_STM_BATTERYLEVEL_READ_EVT:
		/* USER CODE BEGIN CUSTOM_STM_BATTERYLEVEL_READ_EVT */

		/* USER CODE END CUSTOM_STM_BATTERYLEVEL_READ_EVT */
		break;

	case CUSTOM_STM_BATTERYLEVEL_NOTIFY_ENABLED_EVT:
		/* USER CODE BEGIN CUSTOM_STM_BATTERYLEVEL_NOTIFY_ENABLED_EVT */

		/* USER CODE END CUSTOM_STM_BATTERYLEVEL_NOTIFY_ENABLED_EVT */
		break;

	case CUSTOM_STM_BATTERYLEVEL_NOTIFY_DISABLED_EVT:
		/* USER CODE BEGIN CUSTOM_STM_BATTERYLEVEL_NOTIFY_DISABLED_EVT */

		/* USER CODE END CUSTOM_STM_BATTERYLEVEL_NOTIFY_DISABLED_EVT */
		break;

	case CUSTOM_STM_BATTERYLEVEL_INDICATE_ENABLED_EVT:
		/* USER CODE BEGIN CUSTOM_STM_BATTERYLEVEL_INDICATE_ENABLED_EVT */

		/* USER CODE END CUSTOM_STM_BATTERYLEVEL_INDICATE_ENABLED_EVT */
		break;

	case CUSTOM_STM_BATTERYLEVEL_INDICATE_DISABLED_EVT:
		/* USER CODE BEGIN CUSTOM_STM_BATTERYLEVEL_INDICATE_DISABLED_EVT */

		/* USER CODE END CUSTOM_STM_BATTERYLEVEL_INDICATE_DISABLED_EVT */
		break;

	default:
		/* USER CODE BEGIN CUSTOM_STM_App_Notification_default */

		/* USER CODE END CUSTOM_STM_App_Notification_default */
		break;
	}
	/* USER CODE BEGIN CUSTOM_STM_App_Notification_2 */

	/* USER CODE END CUSTOM_STM_App_Notification_2 */
	return;
}

void Custom_APP_Notification(Custom_App_ConnHandle_Not_evt_t *pNotification)
{
	/* USER CODE BEGIN CUSTOM_APP_Notification_1 */

	/* USER CODE END CUSTOM_APP_Notification_1 */

	switch (pNotification->Custom_Evt_Opcode)
	{
	/* USER CODE BEGIN CUSTOM_APP_Notification_Custom_Evt_Opcode */

	/* USER CODE END P2PS_CUSTOM_Notification_Custom_Evt_Opcode */
	case CUSTOM_CONN_HANDLE_EVT:
		/* USER CODE BEGIN CUSTOM_CONN_HANDLE_EVT */
		APP_DBG_MSG("peripheral connected");
		ui8state |= CUSTOMAPP_STATE_CONNECTED;
		/* USER CODE END CUSTOM_CONN_HANDLE_EVT */
		break;

	case CUSTOM_DISCON_HANDLE_EVT:
		/* USER CODE BEGIN CUSTOM_DISCON_HANDLE_EVT */
		APP_DBG_MSG("peripheral disconnected");
		ui8state &= ~(CUSTOMAPP_STATE_CONNECTED | CUSTOMAPP_STATE_NOTIFICATION
				| CUSTOMAPP_STATE_INDICATION);
		/* USER CODE END CUSTOM_DISCON_HANDLE_EVT */
		break;
	default:
		/* USER CODE BEGIN CUSTOM_APP_Notification_default */

		/* USER CODE END CUSTOM_APP_Notification_default */
		break;
	}

	/* USER CODE BEGIN CUSTOM_APP_Notification_2 */

	/* USER CODE END CUSTOM_APP_Notification_2 */

	return;
}

void Custom_APP_Init(void)
{
	/* USER CODE BEGIN CUSTOM_APP_Init */

	UTIL_SEQ_RegTask(1 << CFG_TASK_ESS_MEAS_REQ_ID, UTIL_SEQ_RFU,
			essApp_measure);
	UTIL_SEQ_RegTask(1 << CFG_TASK_ESS_NTF_REQ_ID, UTIL_SEQ_RFU,
			essApp_sendNotification);
	/** Create timer to handle the temperature measurement   */
	HW_TS_Create(CFG_TIM_ESS_ID_ISR, &pui8AppTimerId, hw_ts_Repeated,
			ess_onTimeoutCb);
	HW_TS_Start(pui8AppTimerId, DEFAULT_CUSTOMAPP_MEASUREMENT_INTERVAL);
	/** Create timer to handle the notification period transmission   */
	HW_TS_Create(CFG_TIM_NTF_ID_ISR, &pui8NtfTimerId, hw_ts_Repeated,
			ess_onNtfTimeoutCb);
	HW_TS_Start(pui8AppTimerId, DEFAULT_CUSTOMAPP_MEASUREMENT_INTERVAL);
	APP_DBG_MSG("Environment sensing initialized \n");

	/* USER CODE END CUSTOM_APP_Init */
	return;
}

/* USER CODE BEGIN FD */

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/

/* ambiantTemperature */
void Custom_Temp_Update_Char(const uint32_t ui32uuid, const float fdata) /* Property Read */
{

	/* USER CODE BEGIN Temp_UC*/
	custom_float2Sin16(fdata, &NotifyCharData[ui16nthUpdateValue]);
	if (0 == (ui8state & CUSTOMAPP_STATE_INDICATION))
	{/* don't update the characteristic when the indication is enabled. the log transmission is on going*/
		Custom_STM_App_Update_Char(CUSTOM_STM_TEMP,
				(uint8_t*) (&NotifyCharData[ui16nthUpdateValue]));
		ui16nthUpdateValue = (ui16nthUpdateValue + 2)
				% CUSTIMAPP_UDPATEARRAY_SIZE;
		if (ui16nbTempAvailable < CUSTIMAPP_UDPATEARRAY_NBELMT)
		{
			ui16nbTempAvailable++;
			/* update the number of temperature available */
			Custom_STM_App_Update_Char(CUSTOM_STM_TEMPLOG,
					(uint8_t*) &ui16nbTempAvailable);
		}

	}
	/* USER CODE END Temp_UC*/
	return;
}

/**
 * @brief update the characteristics value when a notification is updated
 * @param[in] ui16nthElemt index of the next element of the array to be sent.
 * the size of the value is CUSTOM_TEMP_SIZE
 * @note a notification is an unacknowledged message
 */
void Custom_Temp_Send_Notification(const uint16_t ui16nthElemt) /* Property Notification */
{
	uint8_t aui8NotifyEndOfLogData[CUSTOM_TEMP_SIZE] =
	{ 0xFF, 0xFF };

	if (Custom_App_Context.Temp_Notification_Status)
	{

		/* USER CODE BEGIN Temp_NS*/
		if ((ui16nbNtfSent >= CUSTIMAPP_UDPATEARRAY_NBELMT)
				|| (ui16nbNtfSent >= ui16nbTempAvailable))
		{/* when all the available data have been sent */
			Custom_STM_App_Update_Char(CUSTOM_STM_TEMP, aui8NotifyEndOfLogData);
			ui16nbNtfSent++;
		}
		else
		{
			ui16nbNtfSent++;
			Custom_STM_App_Update_Char(CUSTOM_STM_TEMP,
					&NotifyCharData[ui16nthElemt]);
		}

		/* USER CODE END Temp_NS*/
	}
	else
	{
		APP_DBG_MSG(
				"-- CUSTOM APPLICATION : CAN'T INFORM CLIENT -  NOTIFICATION DISABLED\n ");
	}
	return;
}

/**
 * @note a indification is acknowledged message
 */
void Custom_Temp_Send_Indication(void) /* Property Indication */
{
	if (Custom_App_Context.Temp_Indication_Status)
	{
		Custom_STM_App_Update_Char(CUSTOM_STM_TEMP, (uint8_t*) NotifyCharData);
		/* USER CODE BEGIN Temp_IS*/

		/* USER CODE END Temp_IS*/
	}
	else
	{
		APP_DBG_MSG(
				"-- CUSTOM APPLICATION : CAN'T INFORM CLIENT -  NOTIFICATION DISABLED\n ");
	}
	return;
}

/* BatteryLevel */
void Custom_Batterylevel_Update_Char(const uint8_t ui8batteryLevel) /* Property Read */
{
	Custom_STM_App_Update_Char(CUSTOM_STM_BATTERYLEVEL,
			(uint8_t*) UpdateCharData);
	/* USER CODE BEGIN Batterylevel_UC*/

	/* USER CODE END Batterylevel_UC*/
	return;
}

void Custom_Batterylevel_Send_Notification(void) /* Property Notification */
{
	if (Custom_App_Context.Batterylevel_Notification_Status)
	{
		Custom_STM_App_Update_Char(CUSTOM_STM_BATTERYLEVEL,
				(uint8_t*) NotifyCharData);
		/* USER CODE BEGIN Batterylevel_NS*/

		/* USER CODE END Batterylevel_NS*/
	}
	else
	{
		APP_DBG_MSG(
				"-- CUSTOM APPLICATION : CAN'T INFORM CLIENT -  NOTIFICATION DISABLED\n ");
	}
	return;
}

void Custom_Batterylevel_Send_Indication(void) /* Property Indication */
{
	if (Custom_App_Context.Batterylevel_Indication_Status)
	{
		Custom_STM_App_Update_Char(CUSTOM_STM_BATTERYLEVEL,
				(uint8_t*) NotifyCharData);
		/* USER CODE BEGIN Batterylevel_IS*/

		/* USER CODE END Batterylevel_IS*/
	}
	else
	{
		APP_DBG_MSG(
				"-- CUSTOM APPLICATION : CAN'T INFORM CLIENT -  NOTIFICATION DISABLED\n ");
	}
	return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/

/* USER CODE END FD_LOCAL_FUNCTIONS*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
