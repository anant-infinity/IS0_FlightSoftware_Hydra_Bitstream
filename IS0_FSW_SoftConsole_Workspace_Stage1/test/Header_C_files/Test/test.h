/*
 * test.h
 *
 *  Created on: Jan 25, 2020
 *      Author: Anant
 */
#ifndef _TEST_H
	#define _TEST_H

#include "CortexM3/GNU/cpu_types.h"
#include "../mss_spi/mss_spi.h"

//Added for Testing EPS I2C
#include "drivers/corei2c/core_i2c.h"
//#include "drivers/mss_uart/mss_uart.h"
#include "drivers_config/sys_config/sys_config.h"
#include "t1_hw_platform.h"

void test_IS0_Read_Sensor_Board_Data();
void test_ISO_test_beacon_debug_UART();
void test_ISO_GPIO_Subsystem_Enable_GPIOs();
void test_ISO_PeakPower();


// OLD IS1 Testing Code Relevent for IS0
//Removed the IS1 Testing Code not relevant for IS0
void test_main_seq();
void test_SD_Card();
void test_SD_Write_Packets();
void test_save_parameter_character();
void test_RTC();
void spi_flash_test2();
int test_SD_All_Sectors(mss_spi_slave_t slot, uint16_t sd_size);
int test_eps_subsystem_power_nonblocking(void);


#endif
