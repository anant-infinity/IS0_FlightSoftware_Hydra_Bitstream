/*
 * sensor_board.c
 *
 *  Created on: Jan 25, 2020
 *      Author: Anant
 */


// ******************************** Sensor Board Read and Write Data Functions ****************************************************

//	This function is used to read a virtual register from the ASM7265 following the algorithm:
//	Poll I²C slave STATUS register;
//	If TX_VALID bit is 0, the virtual register address for the read may be written;
//	Send a virtual register address;
//	Poll I²C slave STATUS register;
//	If RX_VALID bit is 1, the read data is ready;
//	Read the data.
#include "SENSOR_BOARD/sensor_board.h"
#include <EPS/eps.h>
#include "Seq_ops/main_seq.h"
#include "CCSDS/ccsds.h"
#include "SD_CARD/sd.h"
#include "RTC/rtc.h"
#include "GPIOs/gpios.h"
#include "Utils/utils.h"
#include "string.h"
#include "Test/test.h"
#include "../../CMSIS/mss_assert.h"
#include "SPI_Flash/spi_flash.h"
#include "I2C/i2c.h"
#include "../mss_uart/mss_uart.h"
#include "drivers/corei2c/core_i2c.h"

/*------------------------------------------------------------------------------
 * Instance data for our CoreI2C devices
 */
i2c_instance_t g_core_i2c3; 			// Core I2C for SENSOR_0;

void Initialize_Sensor_Board_I2C_Channel(){
    /*-------------------------------------------------------------------------
     * Initialize the Core I2C Drivers
    */
	I2C_init(&g_core_i2c3, COREI2C_3, MASTER_SER_ADDR, I2C_PCLK_DIV_960);

}

void Get_VEML6075_Data(){
	uint8_t write_length = TX_LENGTH;
	uint8_t rx_buffer[RX_LENGTH_2];
	uint8_t read_length = RX_LENGTH_2;
	i2c_status_t instance;

	//TODO: Check if configuration of VMEL (by writing 0x0000, is really required every time
	//Reading the VMEL6075 UVA Data
	instance = do_write_read_transaction(&g_core_i2c3, SENSOR_VML, SENSOR_VEML_UVA_Data, write_length, rx_buffer, read_length);
	Beacon_pack_IS0.Sensor_Board_VMEL6075_UVA_DATA = Utils_Buffer_to_16_ValueU_Big(rx_buffer);

	//Reading the VMEL6075 UVB Data
	instance = do_write_read_transaction(&g_core_i2c3, SENSOR_VML, SENSOR_VEML_UVB_Data, write_length, rx_buffer, read_length);
	Beacon_pack_IS0.Sensor_Board_VMEL6075_UVA_DATA = Utils_Buffer_to_16_ValueU_Big(rx_buffer);

	//Reading the VMEL6075 COMP1 Data
	instance = do_write_read_transaction(&g_core_i2c3, SENSOR_VML, SENSOR_VEML_UVCOMP1_Data, write_length, rx_buffer, read_length);
	Beacon_pack_IS0.Sensor_Board_VMEL6075_UVCOMP1_DATA = Utils_Buffer_to_16_ValueU_Big(rx_buffer);

	//Reading the VMEL6075 UVB Data
	instance = do_write_read_transaction(&g_core_i2c3, SENSOR_VML, SENSOR_VEML_UVCOMP2_Data, write_length, rx_buffer, read_length);
	Beacon_pack_IS0.Sensor_Board_VMEL6075_UVCOMP2_DATA = Utils_Buffer_to_16_ValueU_Big(rx_buffer);

}

//TODO: Complete this function
void Get_AS7265x_Data(){

}

//TODO: Test the "Reliable Working of this Function"
uint8_t read_AS7265_reg(uint8_t virtual_reg_address){

	 uint8_t write_length = TX_LENGTH;
	 uint8_t write_length_2 = TX_LENGTH_2;
	 uint8_t rx_buffer[RX_LENGTH];
	 uint8_t read_length = RX_LENGTH;
	 i2c_status_t instance;

	 uint8_t status_reg_buffer[TX_LENGTH] = {SENSOR_AS72651_STATUS_REG};
	 uint8_t write_reg_buffer[TX_LENGTH] = {SENSOR_AS72651_WRITE_REG};
	 uint8_t read_reg_buffer[TX_LENGTH] = {SENSOR_AS72651_READ_REG};

	 I2C_init(&g_core_i2c3, COREI2C_3, MASTER_SER_ADDR, I2C_PCLK_DIV_960);

	 //Can Uncomment for step by step logging
	 //mss_uart_instance_t * const gp_my_uart = &g_mss_uart1;
	 //MSS_UART_init(&g_mss_uart0,MSS_UART_115200_BAUD,MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY | MSS_UART_ONE_STOP_BIT);

	 while(1){
		 // Read slave I²C status to see if the read buffer is ready.
		 instance = do_write_transaction(&g_core_i2c3, SENSOR_MASTER, status_reg_buffer, write_length);

		 instance = do_read_transaction(&g_core_i2c3, SENSOR_MASTER, rx_buffer, read_length);

		 //Can Uncomment for step by step logging
		 //MSS_UART_polled_tx( &g_mss_uart0, rx_buffer_2, sizeof(rx_buffer_2) );

		 if((rx_buffer[0] & I2C_AS72651_SLAVE_TX_VALID) == 0){
			 // No inbound TX pending at slave. Okay to write now.
			 break;
		 }
	 }

	 // Send the virtual register address to the write buffer
	 uint8_t virtual_reg_buffer[TX_LENGTH_2] = {SENSOR_AS72651_WRITE_REG, virtual_reg_address};
	 instance = do_write_transaction(&g_core_i2c3, SENSOR_MASTER, virtual_reg_buffer, write_length_2);

	 while(1){
		 // Read the slave I²C status to see if our read data is available.
		 //instance = do_write_read_transaction(&g_core_i2c3, SENSOR_MASTER, status_reg_buffer, write_length, rx_buffer_2, read_length_2);

		 // Read slave I²C status to see if the read buffer is ready.
		 instance = do_write_transaction(&g_core_i2c3, SENSOR_MASTER, status_reg_buffer, write_length);

		 instance = do_read_transaction(&g_core_i2c3, SENSOR_MASTER, rx_buffer, read_length);

		 if((rx_buffer[0] & I2C_AS72651_SLAVE_RX_VALID)!=0){
		 		// Read data is ready.
		 		break;
		 }
	 }
	 // Read the data to complete the operation.
	 instance = do_write_read_transaction(&g_core_i2c3, SENSOR_MASTER, SENSOR_AS72651_READ_REG, write_length, rx_buffer, read_length);

	 //Can Uncomment for step by step logging
	 //MSS_UART_polled_tx( &g_mss_uart0, rx_buffer_2, sizeof(rx_buffer_2) );
	 return rx_buffer[0];
}

