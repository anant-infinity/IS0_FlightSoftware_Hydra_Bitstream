/*
 * ccsds.c
 *
 *  Created on: Jan 25, 2020
 *      Author: Anant
 */

#ifndef _CCSDS_C
	#define _CCSDS_C

#include <IS0_FSW_Files/CCSDS/ccsds.h>
#include <IS0_FSW_Files/RTC/rtc.h>
#include <IS0_FSW_Files/Utils/utils.h>

void CCSDS_Pack( uint8_t APP_ID, uint8_t grouping_flag, uint16_t sequence_count,
		struct CCSDS_Header * k, uint32_t pl){

	//Packet structure is following CCSDS Protocol is as follows: 
	//(0000 1000) (APP_ID) (GGSS SSSS)(SSSS SSSS) (DDDD DDDD)(DDDD DDDD)(TIME Stamp 5 bytes)(Data)
	uint8_t timestamp[5];
    RTC_Get_Value_Pointer1(timestamp);

    k->Version_ID =0x08; // 0x08 corresponds to 0000 1000
	// Bits 0-2 Indicates Space Packet 
	// Bit 3 indicated thath packet is used for telemetry 
	// Bit 4 indicates that secondary header is present in the packet. 
	// Bits 5-6 are set to 0
    k->APID = APP_ID;
    //Next byte contains the APID
    k->Seq_no = Utils_Change_Endian_To_BigU16((sequence_count & 0x3FFF) | (grouping_flag << 8));
    
    k->PL = Utils_Change_Endian_To_BigU16(pl - sizeof(struct CCSDS_Header) + CCSDS_SECOND_HEADER_LEN - 1);

    k->TS2 = timestamp[4];
    k->TS1 = Utils_Buffer_to_32_ValueU_Big(timestamp);
    

}


/* Fletcher code generator */
void CCSDS_Fletcher_16(uint8_t* msg,uint32_t len)
{
	uint32_t chkA = len;
	uint32_t chkB = len+1;

	msg[chkA] = 0;
	msg[chkB] = 0;

	uint8_t sum1=0,sum2=0;
	uint32_t i;
	for(i = 0; i<len; i++)
	{
		sum1 = (sum1 + msg[i]) % 0xff;
		sum2 = (sum2 + sum1) % 0xff;
	}

	msg[chkA] = 0xff - (sum1 + sum2) % 0xff;
	msg[chkB] = 0xff - (sum1 + msg[chkA]) % 0xff;

}


#endif
