/*
 * gpios.h
 *
 *  Created on: Jan 27, 2020
 *      Author: Anant
 */
#ifndef _GPIOS_H
	#define _GPIOS_H
#include "drivers/mss_gpio/mss_gpio.h"


// Only Defining the GPIOs Used for IS0, others are left out

//Subsystem Enable Switches
#define GPIO_DSW0_DAXSS		MSS_GPIO_1
#define GPIO_DSW1_CIP		MSS_GPIO_2
#define GPIO_DSW2_ADCS		MSS_GPIO_3
#define GPIO_DSW3_Sband		MSS_GPIO_4
#define GPIO_DSW4_Bat_heat	MSS_GPIO_5

//ADC Conversion Start Signal - NOT USED in current mode
#define GPIO_CONVEST		MSS_GPIO_6

//Sensor Board GPIOs
#define GPIO_EN_SENSOR_BOARD	MSS_GPIO_22
#define GPIO_RESET_SENSER_BOARD	MSS_GPIO_25
#define GPIO_SENSER_BOARD_WRITE_PROTECT	MSS_GPIO_26
#define GPIO_SENSER_BOARD_CHIP_SELECT	MSS_GPIO_30


//CDH Internal GPIOs
#define GPIO_PWR_CYCLE_SC	MSS_GPIO_8
#define GPIO_RESET_IC		MSS_GPIO_9
#define GPIO_EN_BUS_TRAN	MSS_GPIO_10

#define GPIO_EN_SD0			MSS_GPIO_16
#define GPIO_EN_SD1			MSS_GPIO_17
#define GPIO_CTRL_MOSI_SD	MSS_GPIO_20


//Defining the GPIO Masks
#define GPIO_DSW0_DAXSS_Mask	MSS_GPIO_1_MASK
#define GPIO_DSW1_CIP_Mask		MSS_GPIO_2_MASK
#define GPIO_DSW2_ADCS_Mask		MSS_GPIO_3_MASK
#define GPIO_DSW3_Sband_Mask	MSS_GPIO_4_MASK
#define GPIO_DSW4_Bat_heat_Mask	MSS_GPIO_5_MASK

#define GPIO_CONVEST_Mask		MSS_GPIO_6_MASK

#define GPIO_EN_SENSOR_BOARD_Mask				MSS_GPIO_22_MASK
#define GPIO_RESET_SENSER_BOARD_Mask			MSS_GPIO_25_MASK
#define GPIO_SENSER_BOARD_WRITE_PROTECT_Mask	MSS_GPIO_26_MASK

#define GPIO_PWR_CYCLE_SC_Mask	MSS_GPIO_8_MASK
#define GPIO_RESET_IC_Mask		MSS_GPIO_9_MASK
#define GPIO_EN_BUS_TRAN_Mask	MSS_GPIO_10_MASK

#define GPIO_EN_SD0_Mask		MSS_GPIO_16_MASK
#define GPIO_EN_SD1_Mask		MSS_GPIO_17_MASK
#define GPIO_SD0_CS				MSS_GPIO_28_MASK
#define GPIO_SD1_CS				MSS_GPIO_29_MASK

#define GPIO_CTRL_MOSI_SD_Mask	MSS_GPIO_20_MASK

void Global_Init_GPIOs();





#endif
