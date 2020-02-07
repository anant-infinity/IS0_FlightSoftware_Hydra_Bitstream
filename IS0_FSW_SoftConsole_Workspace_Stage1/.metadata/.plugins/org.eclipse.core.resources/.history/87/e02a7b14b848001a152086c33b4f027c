/*
 * rtc.h
 *
 *  Created on: Jun 13, 2018
 *      Author: Ankit
 */

#include "hal/hal.h"

#ifndef _RTC_H
	#define _RTC_H

/* There are two RTCs in the FPGA. RTC0 is for timing various tasks while the RTC1 is for time-stamping the packets.*/
#include "Utils/utils.h"
//#include "Seq_ops/main_seq.h"


/** **************** FPGA Peripheral Memory Map definitions  ******************* **/
/* values to be defined on basis of libero project, defined at top of main_seq.h */
#ifdef ANKIT_OLD_AND_WORKING
#define RTC_APB_ADDR 0x50000000U
#endif

#ifdef MAYURESH_V3_RTC_WORKS
#define RTC_APB_ADDR 0x50007000U
#endif

#ifdef MAYURESH_V4_DOESNT_WORK
#define RTC_APB_ADDR 0x50009000U
#endif

#ifdef MAYURESH_V4
#define RTC_APB_ADDR 0x50009000U
#endif
/** ****************  END FPGA Peripheral Memory Map definitions  ******************* **/



#define RTC_R1_REG_OFFSET 0x00 	// YT[7:0] register
#define RTC_R2_REG_OFFSET 0x04	// YT[15:8] register
#define RTC_R3_REG_OFFSET 0x08	// YT[23:16] register
#define RTC_R4_REG_OFFSET 0x0C	// YT[31:24] register
#define RTC_R5_REG_OFFSET 0x10	// YT[39:32] register
#define RTC_CONFIG_REG_OFFSET	0x14
#define RTC_R1_1_REG_OFFSET 0x18 	// YT2[7:0] register
#define RTC_R2_1_REG_OFFSET 0x1C	// YT2[15:8] register
#define RTC_R3_1_REG_OFFSET 0x20	// YT2[23:16] register
#define RTC_R4_1_REG_OFFSET 0x24	// YT2[31:24] register
#define RTC_R5_1_REG_OFFSET 0x28	// YT2[39:32] register

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
/* The RTC_Get_Value40() function returns the 40 bits of the 
 * 40 bit of RTC0 as 64 bit value with rest of the bit set as 0.
 *
 * @param
 * 	No parameter
 *
 * @return: uint64_t
 *	It returns the 40 bits of RTC0.
*/
uint64_t RTC_Get_Value64();

/*---------------------------------------------------------------------------*/
/* The RTC_Get_Value_Pointer1() function gives all the 40 bits of RTC1 as uint8_t
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


uint8_t RTC_Get_Config_value();
uint8_t RTC_Set_Config_value(uint8_t val);
#endif /* RTC_H_ */

