/*
 * rtc.c
 *
 *  Created on: 8 Nov 2019
 *      Author: mayuresh.sarpotdar
 */

#ifndef _RTC_H
#define _RTC_H

/* There are two RTCs in the FPGA. RTC0 is for timing various tasks while the RTC1 is for time-stamping the packets.*/
#include <IS0_FSW_Files/Utils/utils.h>
#include "hal/hal.h"
#include "../t1_hw_platform.h"
//#include "Seq_ops/main_seq.h"


/** **************** FPGA Peripheral Memory Map definitions  ******************* **/
#define RTC_APB_ADDR			ADDR_APB_RTC
/** ****************  END FPGA Peripheral Memory Map definitions  ******************* **/

#define RTC_BYTE_LENGTH		7	//RTC byte length. Current version is 52 bytes.

#define RTC_R1_REG_OFFSET 		0x00
#define RTC_R2_REG_OFFSET		0x04
#define RTC_R3_REG_OFFSET		0x08
#define RTC_R4_REG_OFFSET		0x0C
#define RTC_R5_REG_OFFSET		0x10
#define RTC_R6_REG_OFFSET		0x14
#define RTC_R7_REG_OFFSET		0x18

#define RTC_CONFIG_REG_OFFSET	0x1C

#define RTC_R1_1_REG_OFFSET 	0x20
#define RTC_R2_1_REG_OFFSET 	0x24
#define RTC_R3_1_REG_OFFSET 	0x28
#define RTC_R4_1_REG_OFFSET 	0x2C
#define RTC_R5_1_REG_OFFSET 	0x30
#define RTC_R6_1_REG_OFFSET		0x34
#define RTC_R7_1_REG_OFFSET		0x38

/*---------------------------------------------------------------------------*/
/* The RTC_Reset() function resets both the RTCs of the FPGA by writing 0x00 to
 * the reset register of the Verilog module.. The RTCs will 
 * remain in the reset state until they are enabled again.
 *
 * @param
 * 	No parameter
 *
 * @return: uint8_t
 *	It returns 0 indicating the reset of the RTCs
*/
#define RTC_Reset(...) Utils_HAL_Set_8bit(RTC_APB_ADDR, RTC_CONFIG_REG_OFFSET, 0x00)

/*---------------------------------------------------------------------------*/
/* The RTC_Enable() function enables both the RTCs by writing 0x03 to the reset
 * register of the Verilog module.
 *
 * @param
 * 	No parameter
 *
 * @return: uint8_t
 *	It returns 3 indicating that both the RTCs are enabled.
*/
#define RTC_Enable(...) Utils_HAL_Set_8bit(RTC_APB_ADDR, RTC_CONFIG_REG_OFFSET, 0X03)
#define RTC_Disable(...) Utils_HAL_Set_8bit(RTC_APB_ADDR, RTC_CONFIG_REG_OFFSET, 0x00)
/*---------------------------------------------------------------------------*/
/* The RTC_Get_Value8() function returns the least significant 8 bits of the 
 * 40 bit of RTC0.
 *
 * @param
 * 	No parameter
 *
 * @return: uint8_t
 *	It returns least significant 8 bits of RTC0.
*/
uint8_t RTC_Get_Value8();

/*---------------------------------------------------------------------------*/
/* The RTC_Get_Value16() function returns the least significant 16 bits of the 
 * 40 bit of RTC0.
 *
 * @param
 * 	No parameter
 *
 * @return: uint16_t
 *	It returns least significant 16 bits of RTC0.
*/
uint16_t RTC_Get_Value16();

/*---------------------------------------------------------------------------*/
/* The RTC_Get_Value32() function returns the least significant 32 bit of the 
 * 40 bit of RTC0.
 *
 * @param
 * 	No parameter
 *
 * @return: uint32_t
 *	It returns least significant 32 bits of RTC0.
*/
uint32_t RTC_Get_Value32();


/*---------------------------------------------------------------------------*/
/* The RTC_Get_Value64() function returns the 40 bits of the
 * 40 bit of RTC0 as 64 bit value with rest of the bit set as 0.
 *
 * @param
 * 	No parameter
 *
 * @return: uint64_t
 *	It returns the 52 bits of RTC0.
*/
uint64_t RTC_Get_Value64();

/*---------------------------------------------------------------------------*/
/* The RTC_Get_Value_Pointer1() function gives all the 52 bits of RTC1 as uint8_t
 * array for time-stamping.
 *
 * @param
 * 	No parameter
 *
 * @return: void
 *	
*/
void RTC_Get_Value_Pointer1(uint8_t * v);

/*---------------------------------------------------------------------------*/
/* The RTC_Set_Value() function sets the RTC1 value with the 5 byte array 
 * passed as parameter. While the RTC1 value is set the RTC0 time keeps ticking
 * hence it doesn't hamper the tasks scheduling. The RTC_Enable() function has
 * to be called after this function to make the RTC1 tick again after its updation.
 *
 * @param
 * 	No parameter
 *
 * @return: void
 *	It returns 1 or 0 depending on if the write operation was successful or not.
*/
uint8_t RTC_Set_Value(uint8_t *, uint8_t );

uint32_t RTC_Get_us();
uint32_t RTC_Get_ms();
uint32_t RTC_Get_sec();
uint16_t RTC_Get_ts_ms();

#endif /* RTC_H_ */
