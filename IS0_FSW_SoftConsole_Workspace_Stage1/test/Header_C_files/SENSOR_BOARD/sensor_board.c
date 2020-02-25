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

uint8_t status_reg_buffer[TX_LENGTH] = {SENSOR_AS72651_STATUS_REG};
uint8_t write_reg_buffer[TX_LENGTH] = {SENSOR_AS72651_WRITE_REG};
uint8_t read_reg_buffer[TX_LENGTH] = {SENSOR_AS72651_READ_REG};

i2c_status_t instance;

uint8_t Read_Array_AS7265x[12];
uint16_t Read_Array_VMEL6075[4];
uint8_t UV_Conf_Buff[3] = {SENSOR_VEML_UV_CONF, 0x00, 0x00};
uint8_t rx_buffer_VMEL[RX_LENGTH_2];
uint8_t rx_buffer_AS7265x[RX_LENGTH] = {0x00};
uint8_t rx_buffer_AS7265x_2[RX_LENGTH];

uint8_t virtual_reg_buffer[TX_LENGTH_2] = {SENSOR_AS72651_WRITE_REG, (SENSOR_AS72651_DEVSEL_ADDR | 0x80)};

void VEML6075AssignInit(){

	int index = 0;
	Read_Array_VMEL6075[index++] = SENSOR_VEML_UVA_Data;
	Read_Array_VMEL6075[index++] = SENSOR_VEML_UVB_Data;
	Read_Array_VMEL6075[index++] = SENSOR_VEML_UVCOMP1_Data;
	Read_Array_VMEL6075[index++] = SENSOR_VEML_UVCOMP2_Data;

}

void AS7265xAssignInit(){

	int k = 0;
	Read_Array_AS7265x[k++] = SENSOR_AS72651_RAW_RGA_H ;
	Read_Array_AS7265x[k++] = SENSOR_AS72651_RAW_RGA_L ;

	Read_Array_AS7265x[k++] = SENSOR_AS72651_RAW_SHB_H ;
	Read_Array_AS7265x[k++] = SENSOR_AS72651_RAW_SHB_L ;

	Read_Array_AS7265x[k++] = SENSOR_AS72651_RAW_TIC_H ;
	Read_Array_AS7265x[k++] = SENSOR_AS72651_RAW_TIC_L ;

	Read_Array_AS7265x[k++] = SENSOR_AS72651_RAW_UJD_H ;
	Read_Array_AS7265x[k++] = SENSOR_AS72651_RAW_UJD_L ;

	Read_Array_AS7265x[k++] = SENSOR_AS72651_RAW_VKE_H ;
	Read_Array_AS7265x[k++] = SENSOR_AS72651_RAW_VKE_L ;

	Read_Array_AS7265x[k++] = SENSOR_AS72651_RAW_WLF_H ;
	Read_Array_AS7265x[k++] = SENSOR_AS72651_RAW_WLF_L ;

}

void Initialize_Sensor_Board(){
    /*-------------------------------------------------------------------------
     * Initialize the Core I2C Drivers
    */
	I2C_init(&g_core_i2c3, COREI2C_3, MASTER_SER_ADDR, I2C_PCLK_DIV_960);
	// Initialize the data structures
	VEML6075AssignInit();
	AS7265xAssignInit();


}


void Get_VEML6075_Data(){

	Utils_Delay32(1000);


	//Writing 0x0000 to the UV_CONF Register to enter into power ON mode
	instance = do_write_transaction(&g_core_i2c3, SENSOR_VML, UV_Conf_Buff, 3);

	//Checking the UV Conf Register Value
	instance = do_write_read_transaction(&g_core_i2c3, SENSOR_VML, SENSOR_VEML_UV_CONF, TX_LENGTH, rx_buffer_VMEL, RX_LENGTH_2);

	for(int i = 0; i < 4; ++i){

		instance = do_write_read_transaction(&g_core_i2c3, SENSOR_VML, Read_Array_VMEL6075[i], TX_LENGTH, rx_buffer_VMEL, RX_LENGTH_2);
		Beacon_pack_IS0.Sensor_Board_VMEL6075[i] = Utils_Buffer_to_16_ValueU_Big(rx_buffer_VMEL);


	}
}


uint8_t read_AS7265x_reg(uint8_t virtual_reg_address){

	 uint8_t count = 0;

	 while(count<10){
		 count++;
		 Utils_Delay32(1);
		 // Read slave I²C status to see if the read buffer is ready.
		 instance = do_write_read_transaction(&g_core_i2c3, SENSOR_MASTER, SENSOR_AS72651_STATUS_REG, TX_LENGTH, rx_buffer_AS7265x, RX_LENGTH);
		 Utils_Delay32(1);
		 if((rx_buffer_AS7265x[0] & I2C_AS72651_SLAVE_TX_VALID) == 0){
			 // No inbound TX pending at slave. Okay to write now.
			 count = 0;
			 break;
		 }
	 }
	 Utils_Delay32(1);
	 // Send the virtual register address to the write buffer
	 uint8_t virtual_reg_buffer[TX_LENGTH_2] = {SENSOR_AS72651_WRITE_REG, virtual_reg_address};
	 instance = do_write_transaction(&g_core_i2c3, SENSOR_MASTER, virtual_reg_buffer, TX_LENGTH_2);
	 count = 0;
	 while(count<10){
		 count++;
		 // Read the slave I²C status to see if our read data is available.
		 instance = do_write_read_transaction(&g_core_i2c3, SENSOR_MASTER, SENSOR_AS72651_STATUS_REG, TX_LENGTH, rx_buffer_AS7265x, RX_LENGTH);
		 Utils_Delay32(1);
		 if((rx_buffer_AS7265x[0] & I2C_AS72651_SLAVE_RX_VALID)!=0){
		 	 // Read data is ready.
			 count = 0;
		 	 break;
		 }
	 }
	 count = 0;
	 // Read the data to complete the operation.
	 Utils_Delay32(1);
	 instance = do_write_read_transaction(&g_core_i2c3, SENSOR_MASTER, SENSOR_AS72651_READ_REG, TX_LENGTH, rx_buffer_AS7265x, RX_LENGTH);
	 return rx_buffer_AS7265x[0];

}

	 void write_AS7265x_reg(uint8_t data_to_write){

		//Poll I²C slave STATUS register;
		//If TX_VALID bit is 0, a write can be performed on the interface;
		//Send a virtual register address and set the MSB of the register address to 1 to indicate the pending write;
		//Poll I²C slave STATUS register;
		//If TX_VALID bit is 0, the virtual register address for the write has been received and the data may now be written;
		//Write the data.

		 while(1){
			 // Read slave I²C status to see if the read buffer is ready.
			 instance = do_write_read_transaction(&g_core_i2c3, SENSOR_MASTER, SENSOR_AS72651_STATUS_REG, TX_LENGTH, rx_buffer_AS7265x_2, RX_LENGTH);

			 if((rx_buffer_AS7265x_2[0] & I2C_AS72651_SLAVE_TX_VALID) == 0){
				 // No inbound TX pending at slave. Okay to write now.
				 break;
			 }
		 }

		 // Send the virtual register address (enabling bit 7 to indicate a write).
		 instance = do_write_transaction(&g_core_i2c3, SENSOR_MASTER, virtual_reg_buffer, TX_LENGTH_2);

		 while(1){
			 // Read the slave I²C status to see if our read data is available.
			 instance = do_write_read_transaction(&g_core_i2c3, SENSOR_MASTER, SENSOR_AS72651_STATUS_REG, TX_LENGTH, rx_buffer_AS7265x_2, RX_LENGTH);

			 if((rx_buffer_AS7265x_2[0] & I2C_AS72651_SLAVE_TX_VALID)==0){
				 	// No inbound TX pending at slave. Okay to write data now
			 		break;
			 }
		 }

		 // Write the data to complete the operation.
		 uint8_t data_write_buffer[TX_LENGTH_2] = {SENSOR_AS72651_WRITE_REG, data_to_write};
		 instance = do_write_transaction(&g_core_i2c3, SENSOR_MASTER, data_write_buffer, TX_LENGTH_2);

	 }

	 /*
	 	The AS7265x consists of three sensors , which can be selected by writing the either 0x01, 0x02 or 0x03 to the DEV_SEL register
	 	There are 6 registers which contain this data these are-
	 	1. Channel R, G, A
	 	2. Channel S, H, B
	 	3. Channel T, I, C
	 	4. Channel U, J, D
	 	5. Channel V, K, E
	 	6. Channel W, L, F

	 	Thus the procedure to read that will be followed is
	 	1. Select Sensor 1 (by writing 0x01 into the DEV_SEL register
	 	2. Read the data from the 6 register corresponding to channel 1
	 	3. Repeat the procedure for channel 2 and 3
	 */

	 void Get_AS7265x_Data(){

		Utils_Delay32(1);

		//Set the DEV_SEL register to sensor 1
		write_AS7265x_reg(SENSOR_AS72651_DEVSEL_MASTER);
		Utils_Delay32(10);

		//Read all the data from sensor 1
		for(int i = 0; i < 12; ++i){

			Beacon_pack_IS0.Sensor_Board_AS7265X[i] = read_AS7265x_reg(Read_Array_AS7265x[i]);

		}

		write_AS7265x_reg(SENSOR_AS72651_DEVSEL_FIRSTSLAVE);
		Utils_Delay32(10);

		//Read all the data from sensor 2
		for(int i = 0; i < 12; ++i){

			Beacon_pack_IS0.Sensor_Board_AS7265X[i+12] = read_AS7265x_reg(Read_Array_AS7265x[i]);

		}

		//Set the DEV_SEL register to sensor 3
		write_AS7265x_reg(SENSOR_AS72651_DEVSEL_SECONDSLAVE);
		Utils_Delay32(10);

		//Read all the data from sensor 3
		for(int i = 0; i < 12; ++i){

			Beacon_pack_IS0.Sensor_Board_AS7265X[i+24] = read_AS7265x_reg(Read_Array_AS7265x[i]);

		}
	 }





