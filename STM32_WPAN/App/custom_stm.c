/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : App/custom_stm.c
 * Description        : Custom Example Service.
 *
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
#include "common_blesvc.h"
#include "custom_stm.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
	uint16_t CustomEnvsensingHdle; /**< ambiantTemperature handle */
	uint16_t CustomTempHdle; /**< temperature handle */
	uint16_t CustomTempLogHdle; /**< temperature log handle */
	uint16_t CustomBatteryHdle; /**< BatteryLevel handle */
	uint16_t CustomBatterylevelHdle; /**< batLevel handle */
} CustomContext_t;

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
/* USER CODE BEGIN PM */
#define COPY_CAPP_WRITE_CHAR_UUID(uuid_struct)       COPY_UUID_128(uuid_struct,0x5F, 0x3B, 0x17, 0x85, 0x32, 0xA0, 0x43, 0x1A, 0x98, 0x49, 0x95, 0x44, 0x44, 0xA8, 0xA6, 0x4D)
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
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
	evt_blue_aci *blue_evt;
	/* USER CODE BEGIN Custom_STM_Event_Handler_1 */
	aci_gatt_attribute_modified_event_rp0 *attribute_modified;
	Custom_STM_App_Notification_evt_t Notification;
	/* USER CODE END Custom_STM_Event_Handler_1 */

	return_value = SVCCTL_EvtNotAck;
	event_pckt = (hci_event_pckt*) (((hci_uart_pckt*) Event)->data);

	switch (event_pckt->evt)
	{
	case EVT_VENDOR:
		blue_evt = (evt_blue_aci*) event_pckt->data;
		switch (blue_evt->ecode)
		{

		case EVT_BLUE_GATT_ATTRIBUTE_MODIFIED:
			/* USER CODE BEGIN EVT_BLUE_GATT_ATTRIBUTE_MODIFIED */
			attribute_modified =
					(aci_gatt_attribute_modified_event_rp0*) blue_evt->data;
			if (attribute_modified->Attr_Handle
					== (CustomContext.CustomTempHdle + 2))
			{
				return_value = SVCCTL_EvtAckFlowEnable;
				/**
				 * Notify to application to start measurement
				 */
				if (attribute_modified->Attr_Data[0] & COMSVC_Indication)
				{
					Notification.Custom_Evt_Opcode =
							CUSTOM_STM_TEMP_NOTIFY_ENABLED_EVT;
					Custom_STM_App_Notification(&Notification);

				}
				else
				{
					Notification.Custom_Evt_Opcode =
							CUSTOM_STM_TEMP_NOTIFY_DISABLED_EVT;
					Custom_STM_App_Notification(&Notification);

				}
			}
			else if (attribute_modified->Attr_Handle
					== (CustomContext.CustomTempLogHdle + 2))
			{
				return_value = SVCCTL_EvtAckFlowEnable;
				if (attribute_modified->Attr_Data[0] & COMSVC_Indication)
				{
					Notification.Custom_Evt_Opcode =
							CUSTOM_STM_TEMPLOG_NOTIFY_ENABLED_EVT;
					Custom_STM_App_Notification(&Notification);

				}
				else
				{
					Notification.Custom_Evt_Opcode =
							CUSTOM_STM_TEMPLOG_NOTIFY_DISABLED_EVT;
					Custom_STM_App_Notification(&Notification);

				}
			}
			/* USER CODE END EVT_BLUE_GATT_ATTRIBUTE_MODIFIED */
			break;
		case EVT_BLUE_GATT_READ_PERMIT_REQ:
			/* USER CODE BEGIN EVT_BLUE_GATT_READ_PERMIT_REQ */

			/* USER CODE END EVT_BLUE_GATT_READ_PERMIT_REQ */
			break;
		case EVT_BLUE_GATT_WRITE_PERMIT_REQ:
			/* USER CODE BEGIN EVT_BLUE_GATT_WRITE_PERMIT_REQ */

			/* USER CODE END EVT_BLUE_GATT_WRITE_PERMIT_REQ */
			break;

		default:
			/* USER CODE BEGIN EVT_DEFAULT */

			/* USER CODE END EVT_DEFAULT */
			break;
		}
		/* USER CODE BEGIN EVT_VENDOR*/

		/* USER CODE END EVT_VENDOR*/
		break; /* EVT_VENDOR */

		/* USER CODE BEGIN EVENT_PCKT_CASES*/

		/* USER CODE END EVENT_PCKT_CASES*/

	default:
		break;
	}

	/* USER CODE BEGIN Custom_STM_Event_Handler_2 */

	/* USER CODE END Custom_STM_Event_Handler_2 */

	return (return_value);
}/* end Custom_STM_Event_Handler */

/* Public functions ----------------------------------------------------------*/

/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void SVCCTL_InitCustomSvc(void)
{

	Char_UUID_t uuid;
	/* USER CODE BEGIN SVCCTL_InitCustomSvc_1 */

	/* USER CODE END SVCCTL_InitCustomSvc_1 */

	/**
	 *	Register the event handler to the BLE controller
	 */
	SVCCTL_RegisterSvcHandler(Custom_STM_Event_Handler);

	/*
	 *          ambiantTemperature
	 *
	 * Max_Attribute_Records = 1 + 2*1 + 1*no_of_char_with_notify_or_indicate_property + 1*no_of_char_with_broadcast_property
	 * service_max_attribute_record = 1 for ambiantTemperature +
	 *                                2 for temperature +
	 *                                1 for temperature configuration descriptor +
	 *                                1 for temperature broadcast property +
	 *                              = 5
	 */

	uuid.Char_UUID_16 = ENVIRONMENTAL_SENSING_SERVICE_UUID;
	aci_gatt_add_service(UUID_TYPE_16, (Service_UUID_t*) &uuid,
	PRIMARY_SERVICE, 8, &(CustomContext.CustomEnvsensingHdle));

	/**
	 *  temperature
	 */
	uuid.Char_UUID_16 = TEMPERATURE_MEASUREMENT_CHAR_UUID; //0x2a1c;
	aci_gatt_add_char(CustomContext.CustomEnvsensingHdle,
	UUID_TYPE_16, &uuid,
	CUSTOM_TEMP_SIZE,
	CHAR_PROP_READ | CHAR_PROP_NOTIFY, //CHAR_PROP_BROADCAST | CHAR_PROP_READ | CHAR_PROP_NOTIFY | CHAR_PROP_INDICATE,
			ATTR_PERMISSION_NONE,
			GATT_DONT_NOTIFY_EVENTS, //GATT_NOTIFY_ATTRIBUTE_WRITE | GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP, GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
			0x10,
			CHAR_VALUE_LEN_CONSTANT, &(CustomContext.CustomTempHdle));

	COPY_CAPP_WRITE_CHAR_UUID(uuid.Char_UUID_128); /*5F3B1785-32A0-431A-9849-954444A8A64D */
	aci_gatt_add_char(CustomContext.CustomEnvsensingHdle,
	UUID_TYPE_128, &uuid,
	CUSTOM_TEMP_SIZE,
	CHAR_PROP_READ | CHAR_PROP_INDICATE, //CHAR_PROP_BROADCAST | CHAR_PROP_READ | CHAR_PROP_NOTIFY | CHAR_PROP_INDICATE,
			ATTR_PERMISSION_NONE,
			GATT_DONT_NOTIFY_EVENTS, //GATT_NOTIFY_ATTRIBUTE_WRITE | GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP, GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
			0x10,
			CHAR_VALUE_LEN_CONSTANT, &(CustomContext.CustomTempLogHdle));

	/*
	 *          BatteryLevel
	 *
	 * Max_Attribute_Records = 1 + 2*1 + 1*no_of_char_with_notify_or_indicate_property + 1*no_of_char_with_broadcast_property
	 * service_max_attribute_record = 1 for BatteryLevel +
	 *                                2 for batLevel +
	 *                                1 for batLevel configuration descriptor +
	 *                              = 4
	 */

	uuid.Char_UUID_16 = 0x180f;
	aci_gatt_add_service(UUID_TYPE_16, (Service_UUID_t*) &uuid,
	PRIMARY_SERVICE, 4, &(CustomContext.CustomBatteryHdle));

	/**
	 *  batLevel
	 */
	uuid.Char_UUID_16 = 0x2a19;
	aci_gatt_add_char(CustomContext.CustomBatteryHdle,
	UUID_TYPE_16, &uuid,
	CUSTOM_BATTERY_SIZE,
	CHAR_PROP_READ | CHAR_PROP_INDICATE,
	ATTR_PERMISSION_NONE,
	GATT_DONT_NOTIFY_EVENTS, //GATT_NOTIFY_ATTRIBUTE_WRITE | GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP | GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
			0x10,
			CHAR_VALUE_LEN_CONSTANT, &(CustomContext.CustomBatterylevelHdle));

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
tBleStatus Custom_STM_App_Update_Char(Custom_STM_Char_Opcode_t CharOpcode,
		uint8_t *pPayload)
{
	tBleStatus result = BLE_STATUS_INVALID_PARAMS;
	/* USER CODE BEGIN Custom_STM_App_Update_Char_1 */

	/* USER CODE END Custom_STM_App_Update_Char_1 */

	switch (CharOpcode)
	{

	case CUSTOM_STM_TEMP:
		result = aci_gatt_update_char_value(CustomContext.CustomEnvsensingHdle,
				CustomContext.CustomTempHdle, 0, /* charValOffset */
				CUSTOM_TEMP_SIZE, /* charValueLen */
				(uint8_t*) pPayload);
		/* USER CODE BEGIN CUSTOM_STM_TEMP*/

		/* USER CODE END CUSTOM_STM_TEMP*/
		break;

	case CUSTOM_STM_TEMPLOG:
		result = aci_gatt_update_char_value(CustomContext.CustomEnvsensingHdle,
				CustomContext.CustomTempLogHdle, 0, /* charValOffset */
				CUSTOM_TEMPLOG_SIZE, /* charValueLen */
				(uint8_t*) pPayload);
		break;

	case CUSTOM_STM_BATTERYLEVEL:
		result = aci_gatt_update_char_value(CustomContext.CustomBatteryHdle,
				CustomContext.CustomBatterylevelHdle, 0, /* charValOffset */
				CUSTOM_BATTERY_SIZE, /* charValueLen */
				(uint8_t*) pPayload);
		/* USER CODE BEGIN CUSTOM_STM_BATTERYLEVEL*/

		/* USER CODE END CUSTOM_STM_BATTERYLEVEL*/
		break;

	default:
		break;
	}

	/* USER CODE BEGIN Custom_STM_App_Update_Char_2 */

	/* USER CODE END Custom_STM_App_Update_Char_2 */

	return result;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
