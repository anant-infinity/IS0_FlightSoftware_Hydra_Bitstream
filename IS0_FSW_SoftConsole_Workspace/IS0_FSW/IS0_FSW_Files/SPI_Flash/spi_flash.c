/*
 * spi_flash.c
 *
 *  Created on: March 5, 2020
 *      Author: Anant
 */

#ifndef _SPI_FLASH_C
	#define _SPI_FLASH_C

#include <IS0_FSW_Files/CCSDS/ccsds.h>
#include <IS0_FSW_Files/EPS/eps.h>
#include <IS0_FSW_Files/GPIOs/gpios.h>
#include <IS0_FSW_Files/RTC/rtc.h>
#include <IS0_FSW_Files/SPI_Flash/spi_flash.h>
#include <IS0_FSW_Files/Utils/utils.h>
#include <stdlib.h>

#include "../mss_spi/mss_spi.h"
#include "../mss_pdma/mss_pdma.h"
#include "string.h"

uint32_t flash_rd_buf[10];
uint32_t flash_wr_buf[10];
uint8_t flash_packet_rd_buf[156];
uint8_t rtc_array[7];
uint8_t rtc_init_array[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint32_t stat;

void Flash_Init(){

    MSS_SPI_init( &g_mss_spi0 );

    MSS_SPI_configure_master_mode
        (
			&g_mss_spi0,
            MSS_SPI_SLAVE_0,
            MSS_SPI_MODE3,
            2,
            MSS_SPI_BLOCK_TRANSFER_FRAME_SIZE
        );
    uint8_t res = 0xFF;

	res = Flash_Read_Status_Register_1();

	if((res & 0x60) != 0){
    	uint8_t cmd = FLASH_CLSR;
		MSS_SPI_set_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
		Flash_CMD(&cmd , 1, 0, 0);
		MSS_SPI_clear_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
	}
	Globals.Flash_SPI_Wait_Limit = 0x8000;

	Utils_Delay32(100);

	//Only For Erasing Everything on First Boot
	//stat = Flash_Erase(FLASH_BE, 0x0FFFFFFF);

}

uint32_t Flash_Wait(){
	uint32_t k=0;uint8_t ready_bit = 0xff;
	do {
		ready_bit =  Flash_Read_Status_Register_1();
		ready_bit = ready_bit & FLASH_STATUS1_WIP;
		k++;
	} while((ready_bit == 1) && k < Globals.Flash_SPI_Wait_Limit);
	return k;
}

void Flash_Write_EN_DIS(uint8_t k){
	MSS_SPI_set_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
	MSS_SPI_transfer_block( &g_mss_spi0, &k, sizeof(uint8_t), 0, 0);
	MSS_SPI_clear_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
}

uint32_t Flash_CMD(uint8_t * cmd, uint8_t cmd_size, uint8_t * rx, uint16_t rx_size){
	uint32_t k;
	if(rx_size==0){
		Flash_Write_EN_DIS(FLASH_WREN);

		MSS_SPI_set_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
		MSS_SPI_transfer_block( &g_mss_spi0, cmd, cmd_size, 0, 0);
		MSS_SPI_clear_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );

		k = Flash_Wait();

		Flash_Write_EN_DIS(FLASH_WRDI);
	}else{
		MSS_SPI_set_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
		MSS_SPI_transfer_block( &g_mss_spi0, cmd, cmd_size, rx, rx_size);
		MSS_SPI_clear_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
		k = Flash_Wait();
	}

	return k;
}

uint8_t Flash_Read_Status_Register_1(){
	MSS_SPI_set_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
	uint8_t ready_bit = 0xFF ;
	uint8_t command = FLASH_RDSR1;
	MSS_SPI_transfer_block( &g_mss_spi0, &command, sizeof(command), &ready_bit, sizeof(ready_bit) );
	MSS_SPI_clear_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
	return ready_bit;
}

uint32_t Flash_Erase(uint8_t command, uint32_t address){
	uint8_t cmd[4];
	cmd[0] = command;
	cmd[1] = (address >> 16) & 0xFF;
	cmd[2] = (address >> 8 ) & 0xFF;
	cmd[3] = address & 0xFF;

	if( address > 0x00FFFFFF){
		uint32_t k = Flash_CMD(cmd, 1, 0, 0);
		return k;
	}else{
		uint32_t k = Flash_CMD(cmd, 4, 0, 0);;
		return k;
	}
}

uint32_t Flash_Read_Data(uint32_t address, uint8_t * rx_buffer, uint16_t size_in_bytes){
	uint8_t cmd_buffer[6];
	cmd_buffer[0] = FLASH_READ;
	cmd_buffer[1] = (uint8_t)((address >> 16) & 0xFF);
	cmd_buffer[2] = (uint8_t)((address >> 8) & 0xFF);
	cmd_buffer[3] = (uint8_t)(address & 0xFF);
	cmd_buffer[4] = 0;
	cmd_buffer[5] = 0;
	uint32_t k=0;

    k = Flash_Wait();

    MSS_SPI_set_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
    MSS_SPI_transfer_block( &g_mss_spi0, cmd_buffer, 4, rx_buffer, size_in_bytes );
    MSS_SPI_clear_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );

    k = Flash_Wait();
	return k;
}

uint32_t Flash_write_cmd_data
(
    mss_spi_instance_t * this_spi,
    const uint8_t * cmd_buffer,
    uint16_t cmd_byte_size,
    uint8_t * data_buffer,
    uint16_t data_byte_size
){
    uint8_t tx_buffer[516];
    uint16_t transfer_size;
    uint16_t idx = 0;

    transfer_size = cmd_byte_size + data_byte_size;


    memcpy(tx_buffer, cmd_buffer, cmd_byte_size);
    memcpy(&tx_buffer[cmd_byte_size], data_buffer, data_byte_size);
    MSS_SPI_transfer_block( &g_mss_spi0, tx_buffer, transfer_size, 0, 0 );

    uint32_t k=0;
    while ( !MSS_SPI_tx_done(this_spi) && k < Globals.Flash_SPI_Wait_Limit )
    {
        k++;
    }
    return k;
}

uint32_t Flash_Program(uint32_t address, uint8_t * write_buffer, uint32_t size_in_bytes){

	uint8_t cmd_buffer[4];
	uint32_t k;
	uint32_t in_buffer_idx;
	uint32_t nb_bytes_to_write;
	uint32_t target_addr;
	/**/
	in_buffer_idx = 0;
	nb_bytes_to_write = size_in_bytes;
	target_addr = address;


	while ( in_buffer_idx < size_in_bytes )
	{
		Flash_Write_EN_DIS(FLASH_WREN);
		MSS_SPI_set_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
		uint32_t size_left;
		/* max possible size that can be written to a page. */
		nb_bytes_to_write = 0x100 - (target_addr & 0xFF);
		size_left = size_in_bytes - in_buffer_idx;	// how many bytes are remaining to write
		if ( size_left < nb_bytes_to_write )
		{
			nb_bytes_to_write = size_left;	// if it fits in the page then write size left
		}

		cmd_buffer[0] = FLASH_PP;
		cmd_buffer[1] = (target_addr >> 16) & 0xFF;
		cmd_buffer[2] = (target_addr >> 8 ) & 0xFF;
		cmd_buffer[3] = target_addr & 0xFF;

		k = Flash_write_cmd_data
		  (
			&g_mss_spi0,
			cmd_buffer,
			sizeof(cmd_buffer),
			&write_buffer[in_buffer_idx],
			nb_bytes_to_write
		  );
		k = Flash_Wait();
		target_addr += nb_bytes_to_write;
		in_buffer_idx += nb_bytes_to_write;
		MSS_SPI_clear_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );

		Flash_Write_EN_DIS(FLASH_WRDI);
	}


	return k;
}

uint16_t FLASH_Verify_write(uint8_t* write_buff, uint8_t* read_buff, uint16_t size)
{
    uint16_t index = 0;
    uint16_t counts=0;
    while(size != 0)
    {
        if(write_buff[index] == read_buff[index])
        {
            counts++;
        }
        index++;
        size--;
    }

    return counts;
}

uint16_t FLASH_Verify_write_count(uint8_t* write_buff, uint8_t read_buff, uint16_t size)
{
    uint16_t index = 0;
    uint16_t counts=0;
    while(size != 0)
    {
        if(write_buff[index] == read_buff)
        {
        	counts++;
        }
        index++;
        size--;
    }

    return counts;
}

void Update_Variables(Param_Table_t t){

    Globals.Watchdog_Signal_Threshold_Time = t.Param_Watchdog_Signal_Threshold_Time;
	Globals.Beac_Thershold_Time = t.Param_Beac_Timer_Threshold_Time;
    Globals.SCI_SCID_Thershold_Time = t.Param_SCI_SCID_Thershold_Time;
    Globals.Safe_Pheonix_Threshold_Time = t.Param_Safe_Pheonix_Threshold_Time;

	Globals.Flash_SPI_Tries_Limit = t.Param_Flash_SPI_Tries_Limit;
	Globals.Flash_SPI_Wait_Limit = t.Param_Flash_SPI_Wait_Limit;

}


void Load_Factory_Value(){
	Param_Table_t t;

	//This corresponds to 1000000 (hex 0x0F4240) microseconds, which is 1 second
	t.Param_Watchdog_Signal_Threshold_Time = (uint64_t)0x0F4240;

	//This corresponds to 1000000 (hex 0x0F4240) microseconds, which is 1 second
	t.Param_Beac_Timer_Threshold_Time = (uint64_t)0x0F4240;

	//45 minutes = 2700 seconds = 2700 000 000 micro seconds = 0xA0EEBB00
	//5 seconds = 5000 000 microseconds = 0x4C4B40
	// 10 seconds is 10 000 000 = 0x989680
	// 10 minutes is 10*60*1000000 = 600000000
	t.Param_SCI_SCID_Thershold_Time = (uint64_t)0xA0EEBB00;

	//5 days = 7200 minutes = 432000 seconds = 432000 000 000 micro seconds = 0x649534E000
	//12 seconds = 12 000 000 microseconds = 0xB71B00
	// 15 seconds is 15 000 000 = 0xE4E1C0
	//15 minutes is 15*60*1000000 = 600000000
	t.Param_Safe_Pheonix_Threshold_Time = (uint64_t)0x649534E000;


	t.Param_Flash_SPI_Tries_Limit = 240;
	t.Param_Flash_SPI_Wait_Limit = 0x8000;

	t.Param_Fletcher_code = 0;


	Update_Variables(t);
}

void Update_Parameter_Table_IS0(){
    //Need to Erase before updating
    stat = Flash_Erase(FLASH_P4E, 0x00000000);
    //Add a delay here
    Utils_Delay32(100);
    //Updating the Flash parameters
    int j = 0;
    flash_wr_buf[j++] = Globals.Boot_Up_Counter;
    flash_wr_buf[j++] = Globals.Beacon_Packet_Seq_Counter;

    flash_wr_buf[j++] = Globals.Flash_Packet_Write_Counter;
    flash_wr_buf[j++] = Globals.Flash_Packet_Read_Counter;

    flash_wr_buf[j++] = Globals.Beacon_Read_Start;
    flash_wr_buf[j++] = Globals.Beacon_Write_Start;
    flash_wr_buf[j++] = Globals.SD_Write_Read_Verify_Count;

    Globals.Time_in_msec = RTC_Get_ms();
    flash_wr_buf[j++] = Globals.Time_in_msec;

    flash_wr_buf[j++] = (uint32_t)Globals.Sat_Curr_Mode;
    flash_wr_buf[j++] = (uint32_t)Globals.Current_SD;

    //Writing the Variables to the Flash Memory
    Flash_Program(0x00000000, (uint8_t*)flash_wr_buf, sizeof(flash_wr_buf));
    //Add a delay here
    Utils_Delay32(100);

    //Just Reading to Check
    Flash_Read_Data(0x00000000, (uint8_t*)flash_rd_buf, sizeof(flash_rd_buf));
}

void Store_Data_Packet_Flash(){

	//Writing the Data Packet to the Flash Memory
	//Total number of Packets that the Flash memory can store is
	//(8192-4)*1024/156 = 53746 packets

	if(Globals.Flash_Packet_Write_Counter> (uint32_t)53746){
		if(Globals.Flash_Packet_Read_Counter<(uint32_t)53746){
			return;
		}
		Globals.Flash_Packet_Write_Counter = 0;

	}

	Flash_Program(0x00001000 + 156*Globals.Flash_Packet_Write_Counter , Beacon_Pack_Array, sizeof(Beacon_Pack_Array));


	//Added Here for Testing
	Flash_Read_Data(0x00001000 + 156*Globals.Flash_Packet_Write_Counter, (uint8_t*)flash_packet_rd_buf, sizeof(flash_packet_rd_buf));
	Globals.Flash_Packet_Write_Counter++ ;

}



void Read_Parameter_Table_IS0(){

	//Load the factory value for all the variables not stored in the Flash
	Load_Factory_Value();

	//Call this read function first During Initialization
    Flash_Read_Data(0x00000000, (uint8_t*)flash_rd_buf, sizeof(flash_rd_buf));


    if (flash_rd_buf[0]==(uint32_t)0x00 || flash_rd_buf[0]==0xFFFFFFFF ){

    	//Need to Erase The entire Flash on the First Boot
    	stat = Flash_Erase(FLASH_BE, 0x0FFFFFFF);

    	//Load the Factory Value if first boot
    	Globals.Boot_Up_Counter = 0;
    	Globals.Beacon_Packet_Seq_Counter = 0;

    	Globals.Flash_Packet_Write_Counter = 0;
    	Globals.Flash_Packet_Read_Counter = 0;

    	Globals.Beacon_Read_Start = 4;
    	Globals.Beacon_Write_Start = 4;
    	Globals.SD_Write_Read_Verify_Count = 0;

    	uint8_t rtc_init = RTC_Set_Value(rtc_init_array, 7);
    	uint64_t rtc_val = RTC_Get_Value64();

    	Globals.Time_in_msec = RTC_Get_ms();

    	Globals.Sat_Curr_Mode = SC_SCID_MODE;
    	Globals.Current_SD = 1;


    }
    else{

    	//Update the Variables with the value in the Flash Memory, if not first boot
    	int j = 0;
    	Globals.Boot_Up_Counter = flash_rd_buf[j++];
    	Globals.Beacon_Packet_Seq_Counter = flash_rd_buf[j++];

    	Globals.Flash_Packet_Write_Counter = flash_rd_buf[j++];
    	Globals.Flash_Packet_Read_Counter = flash_rd_buf[j++];

    	Globals.Beacon_Read_Start = flash_rd_buf[j++];
    	Globals.Beacon_Write_Start = flash_rd_buf[j++];
    	Globals.SD_Write_Read_Verify_Count = flash_rd_buf[j++];

    	//Globals.Time_in_msec = RTC_Get_ms();

    	Globals.Time_in_msec = flash_rd_buf[j++];
    	uint32_t Time_64_Bit = Globals.Time_in_msec*1000;

    	rtc_array[6] = 0;
    	rtc_array[5] = 0;
    	rtc_array[4] = 0;
    	rtc_array[3] = ((Time_64_Bit >> 24) & 0xFF);
    	rtc_array[2] = ((Time_64_Bit >> 16) & 0xFF);
    	rtc_array[1] = ((Time_64_Bit >> 8) & 0xFF);
    	rtc_array[0] = (Time_64_Bit & 0xFF);

    	//Update the RTC to set the value read from the Flash
    	uint8_t rtc_load = RTC_Set_Value(rtc_array, 7);

    	//uint64_t rtc_val = RTC_Get_Value64();

    	Globals.Sat_Curr_Mode = (uint8_t)flash_rd_buf[j++];
    	Globals.Current_SD = (uint8_t)flash_rd_buf[j++];
    }
}






#endif
