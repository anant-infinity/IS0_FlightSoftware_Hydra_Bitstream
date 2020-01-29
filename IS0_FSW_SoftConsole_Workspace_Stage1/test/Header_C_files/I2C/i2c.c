/*
 * i2c.c
 *
 *  Created on: Jan 25, 2020
 *      Author: Anant
 */
#ifndef _I2C_C
	#define _I2C_C

#include "seq_ops/main_seq.h"
#include <stdint.h>		// Standard library
#include "i2c.h"
#include "Utils/utils.h"
#include "CortexM3/GNU/cpu_types.h"




i2c_instance_t g_core_i2c0;				// Core I2C for EPS_C0;
i2c_instance_t g_core_i2c1; 			// Core I2C for EPS_C1;
i2c_instance_t g_core_i2c2; 			// Core I2C for EPS_NC;
i2c_instance_t g_core_i2c3; 			// Core I2C for SENSOR_0;



// ******************************** I2C Driver Functions ***************************************************************
/*------------------------------------------------------------------------------
	 * Perform read transaction with parameters gathered from the command line
	 * interface. This function is called as a result of the user's input in the
	 * command line interface.
	 */
	i2c_status_t do_read_transaction
	(
		i2c_instance_t * this_core_i2c,
	    uint8_t serial_addr,
	    uint8_t * rx_buffer,
	    uint8_t read_length
	)
	{
	    i2c_status_t status;

	    I2C_read(this_core_i2c, serial_addr, rx_buffer, read_length, I2C_RELEASE_BUS);

	    status = I2C_wait_complete(this_core_i2c, DEMO_I2C_TIMEOUT);

	    return status;
	}

	/*------------------------------------------------------------------------------
	 * Perform write transaction with parameters gathered from the command line
	 * interface. This function is called as a result of the user's input in the
	 * command line interface.
	 */
	i2c_status_t do_write_transaction
	(
		i2c_instance_t * this_core_i2c,
	    uint8_t serial_addr,
	    uint8_t * tx_buffer,
	    uint8_t write_length
	)
	{
	    i2c_status_t status;

	    I2C_write(this_core_i2c, serial_addr, tx_buffer, write_length, I2C_RELEASE_BUS);

	    status = I2C_wait_complete(this_core_i2c, DEMO_I2C_TIMEOUT);

	    return status;
	}

	/*------------------------------------------------------------------------------
	 * Perform write-read transaction with parameters gathered from the command
	 * line interface. This function is called as a result of the user's input in
	 * the command line interface.
	 */
	i2c_status_t do_write_read_transaction
	(
		i2c_instance_t * this_core_i2c,
	    uint8_t serial_addr,
	    uint8_t register_address,
	    uint8_t write_length,
	    uint8_t * rx_buffer,
	    uint8_t read_length
	)
	{
	    i2c_status_t status;

	    uint8_t tx_buffer[] = {register_address};

	    I2C_write_read(this_core_i2c,
	                       serial_addr,
	                       tx_buffer,
	                       write_length,
	                       rx_buffer,
	                       read_length,
	                       I2C_RELEASE_BUS);

	    status = I2C_wait_complete(this_core_i2c, DEMO_I2C_TIMEOUT);

	    return status;
	}

	/*------------------------------------------------------------------------------
	 * Service the I2C timeout functionality.
	 */
	void SysTick_Handler(void)
	{
	    I2C_system_tick(&g_core_i2c0, 10);
	    I2C_system_tick(&g_core_i2c1, 10);
	    I2C_system_tick(&g_core_i2c2, 10);
	    I2C_system_tick(&g_core_i2c3, 10);
	}


	void FabricIrq0_IRQHandler(void)
	{
	    I2C_isr(&g_core_i2c0);
	}

	void FabricIrq1_IRQHandler(void)
	{
	    I2C_isr(&g_core_i2c1);
	}

	void FabricIrq2_IRQHandler(void)
	{
	    I2C_isr(&g_core_i2c2);
	}

	void FabricIrq3_IRQHandler(void)
	{
	    I2C_isr(&g_core_i2c3);
	}


#endif /* I2C_C_ */
