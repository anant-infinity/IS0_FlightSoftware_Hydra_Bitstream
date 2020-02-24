/*
 * pslv_uart.c
 *
 *  Created on: Feb 13, 2020
 *      Author: Anant
 */


#include "PSLV_UART/pslv_uart.h"


uint8_t status;

void Check_and_Fill_FIFO(){


	//check if the STATUS register is 0x01
	//if yes then the FIFO is empty and the current Beacon_Packet_Array
	//Can be loaded onto the FIFO


	if(Get_Status_Reg() == 0x01 ){

		// Reset FIFO and state machine
		Reset_FIFO_SM();

		// Write the beacon packet array to the FIFO byte by byte
		for (int i=0; i<sizeof(Beacon_Pack_Array); i++)
		{
			// Set data value
			Set_Data_Byte_FIFO(Beacon_Pack_Array[i]);
			//Set REset write enable bit
			Set_Reset_Write_Enable_FIFO();
		}

		// Set start transmit bit in config reg 0
		HAL_set_8bit_reg(PSLV_UART, CONFIG, (1<<START_TRANSFER_BIT));
		// Reset start transmit bit in config reg 0
		HAL_set_8bit_reg(PSLV_UART, CONFIG, 0);

	}

	//After filling the FIFO the SD card read pointer should also be increased


}

void Set_Data_Byte_FIFO(uint8_t data){
	HAL_set_8bit_reg(PSLV_UART, TX_DATA, data);
}


void Set_Reset_Write_Enable_FIFO(){
	// Set write enable bit for FIFO
	HAL_set_8bit_reg(PSLV_UART, CONFIG, (1<<WR_EN_BIT));
	// Reset write enable bit for FIFO
	HAL_set_8bit_reg(PSLV_UART, CONFIG, 0);
}

uint8_t Get_Status_Reg(){
	status = HAL_get_8bit_reg(PSLV_UART, STATUS);
	return status;
}

void Reset_FIFO_SM(){
	// Reset FIFO and state machine
	HAL_set_8bit_reg(PSLV_UART,CONFIG, (1<<RESET_SM_BIT));
}