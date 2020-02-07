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


void Initialize_Sensor_Board_I2C_Channel(){
    /*-------------------------------------------------------------------------
     * Initialize the Core I2C Drivers
    */
	I2C_init(&g_core_i2c3, COREI2C_3, MASTER_SER_ADDR, I2C_PCLK_DIV_960);

}

struct GetVEML6075SeqPoint {
	uint8_t regAddr;
};

struct GetVEML6075SeqPoint points[4];

struct GetVEML6075SeqPoint assignGetVEML6075(uint8_t regAddr){
	struct GetVEML6075SeqPoint result;
	result.regAddr = regAddr;
	return result;
}

void VEML6075AssignInit(){
	int index = 0;
	points[index++] = assignGetVEML6075(SENSOR_VEML_UVA_Data);
	points[index++] = assignGetVEML6075(SENSOR_VEML_UVB_Data);
	points[index++] = assignGetVEML6075(SENSOR_VEML_UVCOMP1_Data);
	points[index++] = assignGetVEML6075(SENSOR_VEML_UVCOMP2_Data);

}

void Get_VEML6075_Data(){
	uint8_t write_length = TX_LENGTH;
	uint8_t rx_buffer[RX_LENGTH_2];
	uint8_t read_length = RX_LENGTH_2;
	i2c_status_t instance;

	for(int i = 0; i < 4; ++i){
				instance = do_write_read_transaction(&g_core_i2c3, SENSOR_VML, points[i].regAddr, write_length, rx_buffer, read_length);
				Beacon_pack_IS0.Sensor_Board_VMEL6075[i] = Utils_Buffer_to_16_ValueU_Big(rx_buffer);
	}
}



//TODO: Test the "Reliable" Working of this Function
uint8_t read_AS7265x_reg(uint8_t virtual_reg_address){

	 uint8_t write_length = TX_LENGTH;
	 uint8_t write_length_2 = TX_LENGTH_2;
	 uint8_t rx_buffer[RX_LENGTH];
	 uint8_t read_length = RX_LENGTH;


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

	 //FIXME: Uncomment this after the reliable working of the read data functions
	 ///return rx_buffer[0];
	 return 0x0A;
}



	 //TODO: Test the "Reliable" Working of this Function
	 void write_AS7265x_reg(uint8_t data_to_write){

		uint8_t write_length = TX_LENGTH;
		uint8_t write_length_2 = TX_LENGTH_2;
		uint8_t rx_buffer[RX_LENGTH];
		uint8_t read_length = RX_LENGTH;
	 //Poll I²C slave STATUS register;
	 //If TX_VALID bit is 0, a write can be performed on the interface;
	 //Send a virtual register address and set the MSB of the register address to 1 to indicate the pending write;
	 //Poll I²C slave STATUS register;
	 //If TX_VALID bit is 0, the virtual register address for the write has been received and the data may now be written;
	 //Write the data.

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

		 // Send the virtual register address (enabling bit 7 to indicate a write).
		 uint8_t virtual_reg_buffer[TX_LENGTH_2] = {SENSOR_AS72651_WRITE_REG, (SENSOR_AS72651_DEVSEL_ADDR || 0x80)};
		 instance = do_write_transaction(&g_core_i2c3, SENSOR_MASTER, virtual_reg_buffer, write_length_2);

		 while(1){
			 // Read the slave I²C status to see if our read data is available.
			 //instance = do_write_read_transaction(&g_core_i2c3, SENSOR_MASTER, status_reg_buffer, write_length, rx_buffer_2, read_length_2);

			 // Read slave I²C status to see if the read buffer is ready.
			 instance = do_write_transaction(&g_core_i2c3, SENSOR_MASTER, status_reg_buffer, write_length);

			 instance = do_read_transaction(&g_core_i2c3, SENSOR_MASTER, rx_buffer, read_length);

			 if((rx_buffer[0] & I2C_AS72651_SLAVE_RX_VALID)==0){
				 	// No inbound TX pending at slave. Okay to write data now
			 		break;
			 }
		 }

		 // Write the data to complete the operation.
		 uint8_t data_write_buffer[TX_LENGTH_2] = {SENSOR_AS72651_WRITE_REG, data_to_write};
		 instance = do_write_transaction(&g_core_i2c3, SENSOR_MASTER, data_write_buffer, write_length_2);

	 }

	 struct GetAS72651SeqPoint {
	 	uint8_t regAddr;
	 };

	 struct GetAS72651SeqPoint pointsAS72651[12];

	 struct GetAS72651SeqPoint assignGetAS72651(uint8_t regAddr){
	 	struct GetAS72651SeqPoint result;
	 	result.regAddr = regAddr;
	 	return result;
	 }

	 void AS72651AssignInit(){
	 	int index = 0;
	 	pointsAS72651[index++] = assignGetAS72651(SENSOR_AS72651_RAW_RGA_H);
	 	pointsAS72651[index++] = assignGetAS72651(SENSOR_AS72651_RAW_RGA_L);

	 	pointsAS72651[index++] = assignGetAS72651(SENSOR_AS72651_RAW_SHB_H);
	 	pointsAS72651[index++] = assignGetAS72651(SENSOR_AS72651_RAW_SHB_L);

	 	pointsAS72651[index++] = assignGetAS72651(SENSOR_AS72651_RAW_TIC_H);
	 	pointsAS72651[index++] = assignGetAS72651(SENSOR_AS72651_RAW_TIC_L);

	 	pointsAS72651[index++] = assignGetAS72651(SENSOR_AS72651_RAW_UJD_H);
	 	pointsAS72651[index++] = assignGetAS72651(SENSOR_AS72651_RAW_UJD_L);

	 	pointsAS72651[index++] = assignGetAS72651(SENSOR_AS72651_RAW_VKE_H);
	 	pointsAS72651[index++] = assignGetAS72651(SENSOR_AS72651_RAW_VKE_L);

	 	pointsAS72651[index++] = assignGetAS72651(SENSOR_AS72651_RAW_WLF_H);
	 	pointsAS72651[index++] = assignGetAS72651(SENSOR_AS72651_RAW_WLF_L);

	 }


	 //FIXME : Test the "Reliable Working of this Function"
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

		//Set the DEV_SEL register to sensor 1
		//TODO: Uncomment this later
		//write_AS7265x_reg(SENSOR_AS72651_DEVSEL_MASTER);
		//Read all the data from sensor 1
		for(int i = 0; i < 12; ++i){
			//TODO: Uncomment this later
			//Beacon_pack_IS0.Sensor_Board_AS7265X[i] = read_AS7265x_reg(pointsAS72651[i].regAddr);
			Beacon_pack_IS0.Sensor_Board_AS7265X[i] = 0x0A;
		}
		//Set the DEV_SEL register to sensor 2
		//TODO: Uncomment this later
		//write_AS7265x_reg(SENSOR_AS72651_DEVSEL_FIRSTSLAVE);
		//Read all the data from sensor 2
		for(int i = 0; i < 12; ++i){
			//TODO: Uncomment this later
			//Beacon_pack_IS0.Sensor_Board_AS7265X[i+12] = read_AS7265x_reg(pointsAS72651[i].regAddr);
			Beacon_pack_IS0.Sensor_Board_AS7265X[i+12] = 0x0B;
		}

		//Set the DEV_SEL register to sensor 3
		//TODO: Uncomment this later
		//write_AS7265x_reg(SENSOR_AS72651_DEVSEL_SECONDSLAVE);
		//Read all the data from sensor 3
		for(int i = 0; i < 12; ++i){
			//TODO: Uncomment this later
			//Beacon_pack_IS0.Sensor_Board_AS7265X[i+24] = read_AS7265x_reg(pointsAS72651[i].regAddr);
			Beacon_pack_IS0.Sensor_Board_AS7265X[i+24] = 0x0C;
		}
	 }





