
//ccsds.h

#ifndef _CCSDS_H
	#define _CCSDS_H

#include "stdint.h"
#include "../Seq_ops/main_seq.h"

#undef  GLOBAL
#ifdef _CCSDS_C
#define GLOBAL
#else
#define GLOBAL  extern
#endif

#define CCSDS_PRIMARY_HEADER_LEN	6
#define CCSDS_SECOND_HEADER_LEN 	5			// just the time stamp

/*---------------------------------------------------------------------------*/
/* The CCSDS_Pack() function is use to make a CCSDS packet. It does it by making
 * the CCSDS primay and secondary header, for CRC the CCSDS_Fletcher_16() is
 * suppose to be used. The function makes the primary and secondary headers
 * in the big endian format. After making the CCSDS headers it calls
 * CCSDS_AX_25_Wrap() so that the AX.25 wrapper is made. The AX.25 wrapper is
 * made but not integrated with the packet. The ax.25 wrapper is suppose to be
 * passed separately if it is to be transmitted. More details on it can be
 * found in the uhf_tx.h.
 * @param
 * 	uint8_t APP_ID: The APID of the CCSDS packet is to be passed.
 * 	uint8_t grouping_flag: The value of grouping flag is suppose to be passed
 * 	the values it can take are 0x03, 0x01 or 0x02.
 * 	uint16_t sequence_count: A 14 bits long sequence count is suppose to be
 * 	input in the CCSDS packet, this is done by masking the 15th and 16th bit to
 * 	0 and putting the passed grouping flag in those position.
 * 	struct CCSDS_Header * k: This is the pointer to the CCSDS header struct
 * 	where all the data of the primary header and the secondary header is put.
 * 	The struct is defined in the main_seq.h.
 * 	uint32_t pl: The length of the data to be passed in this argument. The
 * 	length should be less than 256 if the AX.25 wrapping is done. The length
 * 	should exclude the length of the secondary header as it is taken care of
 * 	in the function itself. The length put into the primary header is based on
 * 	the following formula
 *						Primary_Header->PL = pl + 5 - 1;
 *
 *
 *
 * @return: void
 *	It doesn't return any value, hence care should be taken to make sure that
 *	correct parameters are passed in the argument.
*/
void CCSDS_Pack( uint8_t APP_ID, uint8_t grouping_flag, uint16_t sequence_count, struct CCSDS_Header * k, uint32_t pl);

/*---------------------------------------------------------------------------*/
/* The CCSDS_Fletcher_16() function which is used to calculate the Fletcher code
 * for the given packet whose pointer is passed in the argument. The fletcher
 * code is calculated including the headers of the packet. The
 * code is calculated in a way that if the same code is calculated on the other
 * side of the communication including this fletcher code then the value will be
 * zero.
 * @param
 * uint8_t * msg: This is the pointer to the CCSDS packet. The typecast would
 * be required to pass the data
 * uint32_t len: This is the length of the data "msg" passed, this should
 * include the length of primary header, secondary header and the data (data
 * includes the fletcher code) minus 2,
 * since the fletcher code is of 2 bytes.

 * @return: void
 *	It doesn't return any value.
*/
void CCSDS_Fletcher_16(uint8_t* msg, uint32_t len);

/*---------------------------------------------------------------------------*/
/* The CCSDS_Fletcher_16_Checkbytes() function which is used to check the
 * Fletcher code of the received packet whose pointer is passed in the argument.
 * The function has the argument "op" which is to be set to 1 when the command
 * accepted was sent by Hydra. This is done because of the way Hydra implements
 * fletcher code. The "op" may be zero when the packet is from
 * DAXSS, it is not clear at the moment.
 * @param
 * uint8_t * msg: This is the pointer to the received CCSDS packet.
 * The typecast would be required to pass the data
 * uint32_t len: This is the length of the data "msg" passed, this should
 * include the length of primary header, secondary header and the data(data
 * includes the fletcher code).
 * uint8_t op: This field should be set to 1 if they data received is from Hydra
 * else it should be 0.
 *
 * @return: uint16_t
 *	It returns 0 if the code is correct else it returns 1.
*/
uint16_t CCSDS_Fletcher_16_Checkbytes(uint8_t* msg, uint16_t len, uint8_t op);


#endif






