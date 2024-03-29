/*
 * sensor_board.h
 *
 *  Created on: Jan 25, 2020
 *      Author: Anant
 */

#ifndef IS0_FSW_FILES_SENSOR_BOARD_SENSOR_BOARD_H_
#define IS0_FSW_FILES_SENSOR_BOARD_SENSOR_BOARD_H_

#include "CortexM3/GNU/cpu_types.h"
#include "../mss_spi/mss_spi.h"
#include "drivers/corei2c/core_i2c.h"
#include "drivers_config/sys_config/sys_config.h"
#include "t1_hw_platform.h"

/*-----------------------------------------------------------------------------
	 *  Slave Addresses of sensor board - on coreI2C 3
	 */

	#define	SENSOR_MASTER 				((uint8_t)0x49)	// Master sensor in the triad
	#define SENSOR_VML		 			((uint8_t)0x10)	// UV spectral sensor


	/*-----------------------------------------------------------------------------
	 * Defining the Register Map of the VEML6075
	 */

	#define	SENSOR_VEML_UV_CONF			((uint8_t)0x00)		// 16 bits UV Configuration register; OPs -> R/W - Not used currently
	#define	SENSOR_VEML_UVA_Data		((uint8_t)0x07)		// 16 bits UVA Data; OPs -> R
	#define	SENSOR_VEML_UVB_Data		((uint8_t)0x09)		// 16 bits UVB Data;	Ops -> R
	#define	SENSOR_VEML_UVCOMP1_Data	((uint8_t)0x0A)		// 16 bits UVCOMP1_Data;	Ops -> R
	#define	SENSOR_VEML_UVCOMP2_Data	((uint8_t)0x0B)		// 16 bits UVCOMP2_Data;	Ops -> R
	#define	SENSOR_DEVICE_ID			((uint8_t)0x0C)		// 16 bits Device ID;	Ops -> R

	/*-----------------------------------------------------------------------------
	 * Defining the Register Map of the AS72651
	 */

	#define	SENSOR_AS72651_STATUS_REG	((uint8_t)0x00)		// 8  bit Status Register; OPs ->
	#define	SENSOR_AS72651_WRITE_REG	((uint8_t)0x01)		// 8  bit Status Register; OPs ->
	#define	SENSOR_AS72651_READ_REG		((uint8_t)0x02)		// 8  bit Status Register; OPs ->

	#define I2C_AS72651_SLAVE_TX_VALID 	((uint8_t)0x02)
	#define I2C_AS72651_SLAVE_RX_VALID 	((uint8_t)0x01)

	/*-----------------------------------------------------------------------------
	 * Defining the virtual Register Map of the AS72651
	 */

	//Hardware and Software Version
	#define	SENSOR_AS72651_HWVERSION_H	((uint8_t)0x00)		// 8  bit Hardware Version H Register; OPs ->R
	#define	SENSOR_AS72651_HWVERSION_L	((uint8_t)0x01)		// 8  bit Hardware Version L Register; OPs ->R
	#define	SENSOR_AS72651_SWVERSION_H	((uint8_t)0x02)		// 8  bit Software Version H Register; OPs ->R
	#define	SENSOR_AS72651_SWVERSION_L	((uint8_t)0x03)		// 8  bit Software Version L Register; OPs ->R
	#define	SENSOR_CONFIG_REGISTER		((uint8_t)0x04)		// 8  bit Configuration register; OPs ->R

	//Raw Data registers
	#define	SENSOR_AS72651_RAW_RGA_H	((uint8_t)0x08)		// 8  bit RAW Value H Register; OPs ->R
	#define	SENSOR_AS72651_RAW_RGA_L	((uint8_t)0x09)		// 8  bit RAW Value L Register; OPs ->R
	#define	SENSOR_AS72651_RAW_SHB_H	((uint8_t)0x0A)		// 8  bit RAW Value H Register; OPs ->R
	#define	SENSOR_AS72651_RAW_SHB_L	((uint8_t)0x0B)		// 8  bit RAW Value L Register; OPs ->R
	#define	SENSOR_AS72651_RAW_TIC_H	((uint8_t)0x0C)		// 8  bit RAW Value H Register; OPs ->R
	#define	SENSOR_AS72651_RAW_TIC_L	((uint8_t)0x0D)		// 8  bit RAW Value L Register; OPs ->R
	#define	SENSOR_AS72651_RAW_UJD_H	((uint8_t)0x0E)		// 8  bit RAW Value H Register; OPs ->R
	#define	SENSOR_AS72651_RAW_UJD_L	((uint8_t)0x0F)		// 8  bit RAW Value L Register; OPs ->R
	#define	SENSOR_AS72651_RAW_VKE_H	((uint8_t)0x10)		// 8  bit RAW Value H Register; OPs ->R
	#define	SENSOR_AS72651_RAW_VKE_L	((uint8_t)0x11)		// 8  bit RAW Value L Register; OPs ->R
	#define	SENSOR_AS72651_RAW_WLF_H	((uint8_t)0x12)		// 8  bit RAW Value H Register; OPs ->R
	#define	SENSOR_AS72651_RAW_WLF_L	((uint8_t)0x13)		// 8  bit RAW Value L Register; OPs ->R

	// DEVSELECT register Address
	#define	SENSOR_AS72651_DEVSEL_ADDR	((uint8_t)0x4F)		// 8  bit Device Select Address; OPs ->R/W

	//Values to write to the devselect register
	#define	SENSOR_AS72651_DEVSEL_MASTER	((uint8_t)0x00)		// 8 bit , write this to dev select register to select master
	#define	SENSOR_AS72651_DEVSEL_FIRSTSLAVE	((uint8_t)0x01)		// 8 bit , write this to dev select register to select first sensor slave
	#define	SENSOR_AS72651_DEVSEL_SECONDSLAVE	((uint8_t)0x02)		// 8 bit , write this to dev select register to select second sensor slave

void Initialize_Sensor_Board();
void Get_VEML6075_Data();
void Get_AS7265x_Data();
uint8_t read_AS7265x_reg(uint8_t);
void write_AS7265x_reg(uint8_t);
void AS7265xAssignInit();
void VEML6075AssignInit();

#endif /* IS0_FSW_FILES_SENSOR_BOARD_SENSOR_BOARD_H_ */
