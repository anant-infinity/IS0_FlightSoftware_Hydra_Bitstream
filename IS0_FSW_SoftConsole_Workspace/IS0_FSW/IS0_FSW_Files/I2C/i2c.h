/*
 * i2c.h
 *
 *  Created on: Jan 25, 2020
 *      Author: Anant
 */

#ifndef _I2C_H
	#define _I2C_H

#undef  GLOBAL
#ifdef _I2C_C
#define GLOBAL
#else
#define GLOBAL  extern
#endif

#include "hal/hal.h"
#include "drivers/corei2c/core_i2c.h"
#include "../../t1_hw_platform.h"

	/*------------------------------------------------------------------------------
	 * Register base address info for three EPS CoreI2C peripherals
	 */
	#define COREI2C_0                     I_EPS_I2C_C_0			//I2C Critical Channel 0
	#define COREI2C_1                     J_EPS_I2C_C_1			//I2C Critical Channel 1
	#define COREI2C_2					  K_EPS_I2C_NC			//I2C Non-Critical Channel

	/*------------------------------------------------------------------------------
	 * Register base address info for Sensor Board CoreI2C peripheral
	 */
	#define COREI2C_3					  L_SENSOR_BOARD_I2C_0	//I2C for Sensor Board

	/* Defining Common TX and RX lengths for I2C Protocol Communication */
	#define TX_LENGTH 1u
	#define TX_LENGTH_2 2u
	#define TX_LENGTH_3 3u

	#define RX_LENGTH 1u
	#define RX_LENGTH_2 2u

	/*-----------------------------------------------------------------------------
	 * I2C operation timeout value in mS. Define as I2C_NO_TIMEOUT to disable the
	 * timeout functionality.
	 */
	#define DEMO_I2C_TIMEOUT 3000u

i2c_status_t do_write_transaction(i2c_instance_t * this_core_i2c,uint8_t ,uint8_t * ,uint8_t );

i2c_status_t do_read_transaction(i2c_instance_t * this_core_i2c, uint8_t, uint8_t * , uint8_t);

i2c_status_t do_write_read_transaction( i2c_instance_t * this_core_i2c, uint8_t , uint8_t , uint8_t , uint8_t * , uint8_t);




#endif
