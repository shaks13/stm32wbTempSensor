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
  uint8_t               Temp_Notification_Status;
  uint8_t               Temp_Indication_Status;
  /* BatteryLevel */
  uint8_t               Batterylevel_Notification_Status;
  uint8_t               Batterylevel_Indication_Status;
/* USER CODE BEGIN CUSTOM_APP_Context_t */

/* USER CODE END CUSTOM_APP_Context_t */

  uint16_t              ConnectionHandle;
} Custom_App_Context_t;

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private defines ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DEFAULT_CUSTOMAPP_MEASUREMENT_INTERVAL   (1000000/CFG_TS_TICK_VAL)  /**< 1s */
/* USER CODE END PD */

/* Private macros -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static uint8_t pui8AppTimerId;
/**
 * START of Section BLE_APP_CONTEXT
 */

PLACE_IN_SECTION("BLE_APP_CONTEXT") static Custom_App_Context_t Custom_App_Context;

/**
 * END of Section BLE_APP_CONTEXT
 */

/* USER CODE BEGIN PV */
uint8_t UpdateCharData[247];
uint8_t NotifyCharData[247];

uint8_t SecureReadData;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
  /* ambiantTemperature */
static void Custom_Temp_Update_Char(uint32_t ui32uuid, void *pdata);
static void Custom_Temp_Send_Notification(void);
static void Custom_Temp_Send_Indication(void);
  /* BatteryLevel */
static void Custom_Batterylevel_Update_Char(void);
static void Custom_Batterylevel_Send_Notification(void);
static void Custom_Batterylevel_Send_Indication(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

static void ess_onTimeoutCb(void) {
	/**
	 * The code shall be executed in the background as an aci command may be sent
	 * The background is the only place where the application can make sure a new aci command
	 * is not sent if there is a pending one
	 */
	UTIL_SEQ_SetTask(1 << CFG_TASK_ESS_MEAS_REQ_ID, CFG_SCH_PRIO_0);

	return;
}

void essApp_measure(void) {
#if USE_ONCHIPSENSOR == 0
	float fvalue;
	IKS01A3_ENV_SENSOR_GetValue(IKS01A3_HTS221_0, ENV_TEMPERATURE, &fvalue);
	APP_DBG_MSG("temp %i\n", (int8_t) fvalue);
	Custom_Temp_Update_Char(TEMPERATURE_MEASUREMENT_CHAR_UUID,(int16_t) fvalue);
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
/* Functions Definition ------------------------------------------------------*/
void Custom_STM_App_Notification(Custom_STM_App_Notification_evt_t *pNotification)
{
/* USER CODE BEGIN CUSTOM_STM_App_Notification_1 */

/* USER CODE END CUSTOM_STM_App_Notification_1 */
  switch(pNotification->Custom_Evt_Opcode)
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

/* USER CODE END CUSTOM_STM_TEMP_NOTIFY_ENABLED_EVT */
      break;

    case CUSTOM_STM_TEMP_NOTIFY_DISABLED_EVT:
/* USER CODE BEGIN CUSTOM_STM_TEMP_NOTIFY_DISABLED_EVT */

/* USER CODE END CUSTOM_STM_TEMP_NOTIFY_DISABLED_EVT */
      break;

    case CUSTOM_STM_TEMP_INDICATE_ENABLED_EVT:
/* USER CODE BEGIN CUSTOM_STM_TEMP_INDICATE_ENABLED_EVT */

/* USER CODE END CUSTOM_STM_TEMP_INDICATE_ENABLED_EVT */
      break;

    case CUSTOM_STM_TEMP_INDICATE_DISABLED_EVT:
/* USER CODE BEGIN CUSTOM_STM_TEMP_INDICATE_DISABLED_EVT */

/* USER CODE END CUSTOM_STM_TEMP_INDICATE_DISABLED_EVT */
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

  switch(pNotification->Custom_Evt_Opcode)
  {
/* USER CODE BEGIN CUSTOM_APP_Notification_Custom_Evt_Opcode */

/* USER CODE END P2PS_CUSTOM_Notification_Custom_Evt_Opcode */
  case CUSTOM_CONN_HANDLE_EVT :
/* USER CODE BEGIN CUSTOM_CONN_HANDLE_EVT */

/* USER CODE END CUSTOM_CONN_HANDLE_EVT */
    break;

    case CUSTOM_DISCON_HANDLE_EVT :
/* USER CODE BEGIN CUSTOM_DISCON_HANDLE_EVT */

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
	/** Create timer to handle the temperature measurement   */
	HW_TS_Create(CFG_TIM_ESS_ID_ISR, &pui8AppTimerId, hw_ts_Repeated,
			ess_onTimeoutCb);
	HW_TS_Start(pui8AppTimerId, DEFAULT_CUSTOMAPP_MEASUREMENT_INTERVAL);
	APP_DBG_MSG("environement sensing initialized \n");
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
void Custom_Temp_Update_Char(uint32_t ui32uuid, void *pdata) /* Property Read */
{
  Custom_STM_App_Update_Char(CUSTOM_STM_TEMP, (uint8_t *)UpdateCharData);
  /* USER CODE BEGIN Temp_UC*/

  /* USER CODE END Temp_UC*/
  return;
}

/**
 * @note a notification is acknowledged message
 */
void Custom_Temp_Send_Notification(void) /* Property Notification */
 {
  if(Custom_App_Context.Temp_Notification_Status)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_TEMP, (uint8_t *)NotifyCharData);
    /* USER CODE BEGIN Temp_NS*/

    /* USER CODE END Temp_NS*/
  }
  else
  {
    APP_DBG_MSG("-- CUSTOM APPLICATION : CAN'T INFORM CLIENT -  NOTIFICATION DISABLED\n ");
  }
  return;
}

void Custom_Temp_Send_Indication(void) /* Property Indication */
{
  if(Custom_App_Context.Temp_Indication_Status)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_TEMP, (uint8_t *)NotifyCharData);
    /* USER CODE BEGIN Temp_IS*/

    /* USER CODE END Temp_IS*/
  }
  else
  {
    APP_DBG_MSG("-- CUSTOM APPLICATION : CAN'T INFORM CLIENT -  NOTIFICATION DISABLED\n ");
  }
  return;
}

  /* BatteryLevel */
void Custom_Batterylevel_Update_Char(void) /* Property Read */
{
  Custom_STM_App_Update_Char(CUSTOM_STM_BATTERYLEVEL, (uint8_t *)UpdateCharData);
  /* USER CODE BEGIN Batterylevel_UC*/

  /* USER CODE END Batterylevel_UC*/
  return;
}

void Custom_Batterylevel_Send_Notification(void) /* Property Notification */
 {
  if(Custom_App_Context.Batterylevel_Notification_Status)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_BATTERYLEVEL, (uint8_t *)NotifyCharData);
    /* USER CODE BEGIN Batterylevel_NS*/

    /* USER CODE END Batterylevel_NS*/
  }
  else
  {
    APP_DBG_MSG("-- CUSTOM APPLICATION : CAN'T INFORM CLIENT -  NOTIFICATION DISABLED\n ");
  }
  return;
}

void Custom_Batterylevel_Send_Indication(void) /* Property Indication */
{
  if(Custom_App_Context.Batterylevel_Indication_Status)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_BATTERYLEVEL, (uint8_t *)NotifyCharData);
    /* USER CODE BEGIN Batterylevel_IS*/

    /* USER CODE END Batterylevel_IS*/
  }
  else
  {
    APP_DBG_MSG("-- CUSTOM APPLICATION : CAN'T INFORM CLIENT -  NOTIFICATION DISABLED\n ");
  }
  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/

/* USER CODE END FD_LOCAL_FUNCTIONS*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
