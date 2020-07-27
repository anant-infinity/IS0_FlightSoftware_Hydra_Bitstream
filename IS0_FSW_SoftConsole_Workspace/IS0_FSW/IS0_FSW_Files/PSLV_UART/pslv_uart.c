/*
 * pslv_uart.c
 *
 *  Created on: Feb 13, 2020
 *      Author: Anant
 */


#include <IS0_FSW_Files/PSLV_UART/pslv_uart.h>


uint8_t status;
uint8_t fifo_packet_wr_buf[156];

//This file contains the C functions that are used to interface  
//with the PSLV Interface Protocol IP developed in verilog.  
//The verilog IP needs as input a packet array and stores it in its FIFO. 
//It then handles sending the packet to the PSLV two bytes at a time. 
//This is done whenever it recieves commands from the PSLV. 
//Further the the IP contains a status reg , which is set to 0x01 if the
//FIFO of the IP is empty and needs new packet from the C Code. 

void Check_and_Fill_FIFO(){


	// Checking if the STATUS register is 0x01
	// If yes then the FIFO is Empty 
	// Then current Beacon_Packet_Array can be loaded onto the FIFO


	if(Get_Status_Reg() == 0x01 ){

		// Reset FIFO and state machine
		Reset_FIFO_SM();

		if(Globals.Flash_Packet_Read_Counter> (uint32_t)53746){
			Globals.Flash_Packet_Read_Counter = 0;
		}

		if(Globals.Flash_Packet_Read_Counter>Globals.Flash_Packet_Write_Counter){
			return;
		}
		Flash_Read_Data( 0x00001000 + 156*Globals.Flash_Packet_Read_Counter, (uint8_t*)fifo_packet_wr_buf, sizeof(fifo_packet_wr_buf));
		Globals.Flash_Packet_Read_Counter++;

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