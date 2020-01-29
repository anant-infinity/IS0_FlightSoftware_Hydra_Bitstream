
#ifndef _CCSDS_C
	#define _CCSDS_C

#include "ccsds.h"
#include "RTC/rtc.h"
#include "Utils/utils.h"

/* The CCSDS header has to be in big endian. The data would be mixed endianess. The AX.25 doesn't matter since all the fields are 
gonna be 8-bit long. During transmission the LSBit of each byte is sent first. */
void CCSDS_Pack( uint8_t APP_ID, uint8_t grouping_flag, uint16_t sequence_count,
		struct CCSDS_Header * k, uint32_t pl){

	uint8_t timestamp[5];
    RTC_Get_Value_Pointer1(timestamp);
	//create header 1
    k->Version_ID =0x08;
    k->APID = APP_ID;
    k->Seq_no = Utils_Change_Endian_To_BigU16((sequence_count & 0x3FFF) | (grouping_flag << 8));
    k->PL = Utils_Change_Endian_To_BigU16(pl - sizeof(struct CCSDS_Header) + CCSDS_SECOND_HEADER_LEN - 1);

    k->TS2 = timestamp[4];
    k->TS1 = Utils_Buffer_to_32_ValueU_Big(timestamp);
    //Packet structure is: (0000 1000) (APP_ID) (GGSS SSSS)(SSSS SSSS) (DDDD DDDD)(DDDD DDDD)(TIME Stamp 5 bytes)(Data)

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

/* Fletcher code checker */
uint16_t CCSDS_Fletcher_16_Checkbytes(uint8_t* msg, uint16_t len, uint8_t op)
{	
	uint8_t ckA,ckB,ckAbyte,ckBbyte;
	uint16_t result, i;
	if(len == 0)
		return 0;

	ckA = 0;
	ckB = 0;
	for(i = 0; i<len; i++)
	{
		ckA = (ckA + msg[i]) % 0xff;
		ckB = (ckB + ckA) % 0xff;
	}
	result = (uint16_t)((ckA << 8) + ckB);

	if(op){
		result = !(Utils_Buffer_to_16_ValueU_Little(&msg[len]) == result);
	}
	return result;
}


#endif
