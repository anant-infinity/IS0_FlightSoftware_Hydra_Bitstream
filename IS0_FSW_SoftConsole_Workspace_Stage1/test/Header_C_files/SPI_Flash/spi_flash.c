#ifndef _SPI_FLASH_C
	#define _SPI_FLASH_C

#include <EPS/eps.h>
#include "spi_flash.h"
#include <stdlib.h>

#include "../mss_spi/mss_spi.h"
#include "../mss_pdma/mss_pdma.h"
#include "../CCSDS/ccsds.h"
#include "GPIOs/gpios.h"
#include "string.h"

//Module_Sync_Small_t Globals.Param_Module_sync;
//
//uint16_t Globals.Flash_SPI_Wait_Limit = 0x8000;
uint32_t const addresses[8] = {0x00001000, 0x00002000, 0x00003000, 0x00004000,
                            0x00005000, 0x00006000, 0x00007000, 0x0000F000};


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
	uint8_t res = Flash_Read_Status_Register_1();
	if((res & 0x60) != 0){
    	uint8_t cmd = FLASH_CLSR;
		MSS_SPI_set_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
		Flash_CMD(&cmd , 1, 0, 0);
		MSS_SPI_clear_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
	}
	Globals.Flash_SPI_Wait_Limit = 0x8000;


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
	uint8_t ready_bit;
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

	Globals.Current_SD = t.Param_Current_SD;


	Globals.Beac_Timer.Time = t.Param_Beac_Timer_Time;
	Globals.Beac_Trans_Timer.Time = t.Param_Beac_Trans_Timer_Time;

	Globals.Watchdog_Signal_Timer.Time = t.Param_Watchdog_Signal_Timer_Time;

    Globals.RTC_Sync_Timer.Time = t.Param_RTC_Sync_Timer_Time;

	Globals.Boot_Up_Counter = t.Param_Boot_Up_Counter;




	Globals.Flash_SPI_Tries_Limit = t.Param_Flash_SPI_Tries_Limit;
	Globals.Flash_SPI_Wait_Limit = t.Param_Flash_SPI_Wait_Limit;

	Globals.Non_Response_Count_Limit = t.Param_Non_Response_Count_Limit;

	Globals.Phoe_Safe_Threshold = t.Param_Phoe_Safe_Threshold;
	Globals.Safe_Phoe_Threshold = t.Param_Safe_Phoe_Threshold;
	Globals.Safe_Char_Threshold = t.Param_Safe_Char_Threshold;
	Globals.Char_Safe_Threshold = t.Param_Char_Safe_Threshold;
	Globals.Char_SciC_Threshold = t.Param_Char_SciC_Threshold;
	Globals.SciC_Char_Threshold = t.Param_SciC_Char_Threshold;
	Globals.Char_SciD_Threshold = t.Param_Char_SciD_Threshold;
	Globals.SciD_Char_Threshold = t.Param_SciD_Char_Threshold;



	Globals.Beacon_Sector_Start = t.Param_Beacon_Sector_Start;
	Globals.Beacon_Sector_End = t.Param_Beacon_Sector_End;



	Globals.Beacon_Write_Start = t.Param_Beacon_Write_Start;
	Globals.Beacon_Read_Start = t.Param_Beacon_Read_Start;


	Globals.CMD_Forced_Mode = t.Param_CMD_Forced_Mode;
	Globals.Force_Mode_Timer.Time = t.Param_Force_Mode_Timer_Time;
	Globals.Force_Mode_Update_Rate.Time = t.Param_Force_Mode_Update_Rate_Time;


}

//  /* Require Update */
void Load_Factory_Value(){
	Param_Table_t t;

	t.Param_Current_SD = 0;

	t.Param_Beac_Timer_Time = 1000;
	t.Param_Beac_Trans_Timer_Time = 10000;
	t.Param_Watchdog_Signal_Timer_Time = 200;

    t.Param_RTC_Sync_Timer_Time = 60000;
    t.Param_EPS_CMD_Timer_Time = 400;

	t.Param_Boot_Up_Counter = 0;



	t.Param_UHF_Malfunction_Flags = 0;
	t.Param_ADCS_Malfunction_Flags = 0;



	t.Param_Flash_SPI_Tries_Limit = 240;
	t.Param_Flash_SPI_Wait_Limit = 0x8000;

	t.Param_Non_Response_Count_Limit = 20;

	t.Param_Phoe_Safe_Threshold = 55;
	t.Param_Safe_Phoe_Threshold = 45;
	t.Param_Safe_Char_Threshold = 70;
	t.Param_Char_Safe_Threshold = 75;
	t.Param_Char_SciC_Threshold = 85;
	t.Param_SciC_Char_Threshold = 80;
	t.Param_Char_SciD_Threshold = 85;
	t.Param_SciD_Char_Threshold = 80;



	t.Param_Beacon_Sector_Start = 4;
	t.Param_Beacon_Sector_End = 31536004;



	t.Param_Beacon_Write_Start = t.Param_Beacon_Sector_Start;
	t.Param_Beacon_Read_Start = t.Param_Beacon_Sector_Start;



	t.Param_CMD_Forced_Mode = 0;
    t.Param_Force_Mode_Timer_Time = 0;
    t.Param_Force_Mode_Update_Rate_Time = 0;



	t.Param_Fletcher_code = 0;


	Update_Variables(t);
}

void Get_Filled_Array(Param_Table_t * t){

	t->Param_Current_SD = Globals.Current_SD;


	t->Param_Beac_Timer_Time = Globals.Beac_Timer.Time;
	t->Param_Beac_Trans_Timer_Time = Globals.Beac_Trans_Timer.Time;

	t->Param_Watchdog_Signal_Timer_Time = Globals.Watchdog_Signal_Timer.Time;


    t->Param_RTC_Sync_Timer_Time = Globals.RTC_Sync_Timer.Time;


	t->Param_Boot_Up_Counter = Globals.Boot_Up_Counter;



	t->Param_Flash_SPI_Tries_Limit = Globals.Flash_SPI_Tries_Limit;
	t->Param_Flash_SPI_Wait_Limit = Globals.Flash_SPI_Wait_Limit;

	t->Param_Non_Response_Count_Limit = Globals.Non_Response_Count_Limit;

	t->Param_Phoe_Safe_Threshold = Globals.Phoe_Safe_Threshold;
	t->Param_Safe_Phoe_Threshold = Globals.Safe_Phoe_Threshold;
	t->Param_Safe_Char_Threshold = Globals.Safe_Char_Threshold;
	t->Param_Char_Safe_Threshold = Globals.Char_Safe_Threshold;
	t->Param_Char_SciC_Threshold = Globals.Char_SciC_Threshold;
	t->Param_SciC_Char_Threshold = Globals.SciC_Char_Threshold;
	t->Param_Char_SciD_Threshold = Globals.Char_SciD_Threshold;
	t->Param_SciD_Char_Threshold = Globals.SciD_Char_Threshold;



	t->Param_Beacon_Sector_Start = Globals.Beacon_Sector_Start;
	t->Param_Beacon_Sector_End = Globals.Beacon_Sector_End;



	t->Param_Beacon_Write_Start = Globals.Beacon_Write_Start;
	t->Param_Beacon_Read_Start = Globals.Beacon_Read_Start;



	t->Param_CMD_Forced_Mode = Globals.CMD_Forced_Mode;
    t->Param_Force_Mode_Timer_Time = Globals.Force_Mode_Timer.Time;
    t->Param_Force_Mode_Update_Rate_Time = Globals.Force_Mode_Update_Rate.Time;



}

// TODO: add handle for triple parameter table implementation
uint8_t Save_Parameter_Table(){
/*	if(Globals.Param_Module_sync.CMD_Seq_Count == FLASH_STATE_IDLE){
		return FLASH_STATE_IDLE;
	}
    Param_Table_t FLASH_Parameter_Arr;
	Get_Filled_Array(&FLASH_Parameter_Arr);
    CCSDS_Fletcher_16((uint8_t *)&FLASH_Parameter_Arr, sizeof(FLASH_Parameter_Arr)-2);

    uint8_t res = Save_Parameter_Table_Arr((uint8_t *)&FLASH_Parameter_Arr, sizeof(FLASH_Parameter_Arr), addresses[7]);
    if(res == 0xff){
    	if(Globals.Param_Module_sync.Response_Read  < Globals.Flash_SPI_Tries_Limit){
    		Globals.Param_Module_sync.Response_Read++;
    		 Send the clear status command
    		uint8_t cmd[] = {FLASH_CLSR};
    		MSS_SPI_transfer_block(&g_mss_spi0, cmd, sizeof(cmd), 0, 0);	 Check it once
    		Globals.Param_Module_sync.CMD_Seq_Count = FLASH_STATE_ERASE_CMD;
    		return 0xff;
    	}
    	else{
    		Globals.Param_Module_sync.CMD_Seq_Count = FLASH_STATE_IDLE;
    		return 0x00;
    	}
    }else if(res == FLASH_STATE_PROGRAM){
    	Globals.Param_Module_sync.Response_Read = 0;
    	return 0x01;
    }else{
    	return 0x02;
    }*/
	return 0;
}

uint8_t Save_Parameter_Table_Arr(uint8_t * FLASH_Parameter_Arr, uint16_t FLASH_Parameter_Arr_Size, uint32_t address){
/*    uint8_t rx_buff[FLASH_Parameter_Arr_Size];
    uint32_t status=0;
    uint8_t ready_bit = 0xff; uint8_t cmd[4];
	switch(Globals.Param_Module_sync.CMD_Seq_Count){
		case FLASH_STATE_ERASE_CMD:	 Sector erase command
			Globals.Param_Module_sync.Response_Length = 0;
			cmd[0] = FLASH_P4E;
			cmd[1] = (address >> 16) & 0xFF;
			cmd[2] = (address >> 8 ) & 0xFF;
			cmd[3] = address;
			Flash_Write_EN_DIS(FLASH_WREN);
			MSS_SPI_set_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
			MSS_SPI_transfer_block( &g_mss_spi0, cmd, sizeof(cmd), 0, 0);
			MSS_SPI_clear_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );

    		Globals.Param_Module_sync.CMD_Seq_Count = FLASH_STATE_WAIT;
    		Globals.Param_Module_sync.Prev_CMD_Seq_Count = FLASH_STATE_ERASE_CMD;
    		return FLASH_STATE_ERASE_CMD;

    	case FLASH_STATE_WAIT:	 Wait state for erase (only)
    		ready_bit =  Flash_Read_Status_Register_1();
			ready_bit = ready_bit & FLASH_STATUS1_WIP;
			Globals.Param_Module_sync.Prev_CMD_Seq_Count = FLASH_STATE_WAIT;
			if(ready_bit == 0){
				Globals.Param_Module_sync.CMD_Seq_Count = FLASH_STATE_ERASE_VERIFY;
    			Flash_Write_EN_DIS(FLASH_WRDI);
    			return FLASH_STATE_WAIT;
			}else{
				Globals.Param_Module_sync.CMD_Seq_Count = FLASH_STATE_WAIT;
				Globals.Param_Module_sync.Response_Length++;
				if(Globals.Param_Module_sync.Response_Length >= Globals.Flash_SPI_Wait_Limit){
					Flash_Write_EN_DIS(FLASH_WRDI);
					return 0xff;
				}else{
					return FLASH_STATE_WAIT;
				}
			}
    		break;

    	case FLASH_STATE_ERASE_VERIFY:	 Cross check the erase
    		status = Flash_Read_Data(address, rx_buff, FLASH_Parameter_Arr_Size);
	        status = FLASH_Verify_write_count(rx_buff, 0xff, FLASH_Parameter_Arr_Size);
	        Globals.Param_Module_sync.CMD_Seq_Count = FLASH_STATE_PROGRAM;
    		Globals.Param_Module_sync.Prev_CMD_Seq_Count = FLASH_STATE_ERASE_VERIFY;
	        if(status != FLASH_Parameter_Arr_Size){
	        	return 0xff;
	        }else{
	        	return FLASH_STATE_ERASE_VERIFY;
	        }

	    case FLASH_STATE_PROGRAM:	 Program the sector and cross check
	    	status = Flash_Program(address, FLASH_Parameter_Arr, FLASH_Parameter_Arr_Size);
	        status = Flash_Read_Data(address, rx_buff, FLASH_Parameter_Arr_Size);
	        status = FLASH_Verify_write(rx_buff, FLASH_Parameter_Arr, FLASH_Parameter_Arr_Size);

			Globals.Param_Module_sync.CMD_Seq_Count = FLASH_STATE_IDLE;
			Globals.Param_Module_sync.Prev_CMD_Seq_Count = FLASH_STATE_PROGRAM;
	        if(status != FLASH_Parameter_Arr_Size){
				return 0xff;
	        }else{
	            return FLASH_STATE_PROGRAM;
	        }

	    default:	 IDLE state
	    	return FLASH_STATE_IDLE;
	}*/
	return 0;
}

uint8_t Load_Parameter_Table(){
	Param_Table_t FLASH_Parameter;
	if(Read_Parameter_Table(&FLASH_Parameter)){
		Update_Variables(FLASH_Parameter);
		return 1;
	}

	Load_Factory_Value();
	return 0;
}


// TODO: add handle for triple parameter table implementation
uint8_t Read_Parameter_Table(Param_Table_t * FLASH_Parameter){

/*	int8_t i=7; uint32_t status; uint8_t reading_errors = 0;
	uint16_t FLASH_Parameter_Arr_Size = sizeof(Param_Table_t);
	while(i > 6){
		status = Flash_Read_Data(addresses[i], (uint8_t *)FLASH_Parameter, FLASH_Parameter_Arr_Size);
		if(status < Globals.Flash_SPI_Wait_Limit){
        	status = CCSDS_Fletcher_16_Checkbytes((uint8_t *)FLASH_Parameter, FLASH_Parameter_Arr_Size, 0);
			if(status == 0 && (FLASH_Parameter->Param_PSLV_Wait_Timer_Time) != 0xffffffff){
				// Update_Variables(FLASH_Parameter);
				return 1; 	 The "break" means we won't be correcting the corrupted sectors.
			}
        }
		i--;
	}
	return 0;*/
	return 0;
}

#endif
