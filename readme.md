#BOM
*p nucelo stm32wb
*IKS0113

# howto

1- use cubemx to generate a hts profile 
2- add the next updates
add the irq management (uart and I2C)
add the board IKS0113 (software pack)
add the management of the hts 

## add in stm32wbxx_hal_msp.c 
#include "otp.h"

and  in the function HAL_MspInit

#warning
“Following code is valid only for P NUCLEO WB55 boards and should be re
implemented depending on the target HW and HSE capacitor tuning value storage location.“
OTP_ID0_t
* p_
/**
* Read HSE_Tuning from OTP
*/
p_otp = (
OTP_ID0_t *) OTP_Read(
if
(p_
{
LL_RCC_HSE_SetCapacitorTuning(p_otp
hse_tuning
}


STM32WB80 howto.pdf 
*Add the HSE tuning slide 129
*Add STM32_WPAN ISRs
/**
  * @brief This function handles RTC wake-up interrupt through EXTI line 19.
  */
void RTC_WKUP_IRQHandler(void)
{
  HW_TS_RTC_Wakeup_Handler();
}
/**
  * @brief This function handles IPCC RX occupied interrupt.
  */
void IPCC_C1_RX_IRQHandler(void)
{
	HW_IPCC_Rx_Handler();
}
/**
  * @brief This function handles IPCC TX free interrupt.
  */
void IPCC_C1_TX_IRQHandler(void)
{
  HW_IPCC_Tx_Handler();
}

* add UTIL_SEQ_Run(~0); in the main endless loop

## add the management of the sensor (IKS0113)
* add the I2C bus
* 
# add a custom service environement sensing ESS
* add a custom template
* 

