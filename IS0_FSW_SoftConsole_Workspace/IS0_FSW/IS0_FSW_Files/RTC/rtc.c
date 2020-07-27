/*
 * rtc.c
 *
 *  Created on: 8 Nov 2019
 *      Author: mayuresh.sarpotdar
 */

#include <IS0_FSW_Files/RTC/rtc.h>

uint8_t RTC_Set_Value(uint8_t * v, uint8_t len){	//function to set the value of RTC
	if(len!=7){
		return 0;
	}
	HAL_set_8bit_reg( RTC_APB_ADDR, RTC_R7,v[6]);
	HAL_set_8bit_reg( RTC_APB_ADDR, RTC_R6,v[5]);
	HAL_set_8bit_reg( RTC_APB_ADDR, RTC_R5,v[4]);
	HAL_set_8bit_reg( RTC_APB_ADDR, RTC_R4,v[3]);
	HAL_set_8bit_reg( RTC_APB_ADDR, RTC_R3,v[2]);
	HAL_set_8bit_reg( RTC_APB_ADDR, RTC_R2,v[1]);
	HAL_set_8bit_reg( RTC_APB_ADDR, RTC_R1,v[0]);

	HAL_set_8bit_reg( RTC_APB_ADDR, RTC_CONFIG, 7);	// Used to set the value in the register hence bit 2 of config register is set to high
	HAL_set_8bit_reg( RTC_APB_ADDR, RTC_CONFIG, 3); // Reset bit 2 to enable the counter to start incrementing from newly assigned value
	return 1;
}

uint8_t RTC_Get_Value8(){	// function to get the 8 LSB of RTC
	return HAL_get_8bit_reg( RTC_APB_ADDR, RTC_R1);
}

uint16_t RTC_Get_Value16(){	// function to get the 16 LSB of RTC
	uint8_t v[2];
	v[0] = HAL_get_8bit_reg( RTC_APB_ADDR, RTC_R1);
	v[1] = HAL_get_8bit_reg( RTC_APB_ADDR, RTC_R2);

	return Utils_Buffer_to_16_ValueU_Little(v);
}

uint32_t RTC_Get_Value32(){	// function to get the 32 LSB of RTC
	uint8_t v[4];
	v[0] = HAL_get_8bit_reg( RTC_APB_ADDR, RTC_R1);
	v[1] = HAL_get_8bit_reg( RTC_APB_ADDR, RTC_R2);
	v[2] = HAL_get_8bit_reg( RTC_APB_ADDR, RTC_R3);
	v[3] = HAL_get_8bit_reg( RTC_APB_ADDR, RTC_R4);

	return  Utils_Buffer_to_32_ValueU_Little(v);
}

uint64_t RTC_Get_Value64(){	// function to get the 64 of RTC
	uint8_t v[7];
	//Modified this to get RTC 1 Value which can be written to
	v[0] = HAL_get_8bit_reg( RTC_APB_ADDR, RTC_R1_1);
	v[1] = HAL_get_8bit_reg( RTC_APB_ADDR, RTC_R2_1);
	v[2] = HAL_get_8bit_reg( RTC_APB_ADDR, RTC_R3_1);
	v[3] = HAL_get_8bit_reg( RTC_APB_ADDR, RTC_R4_1);
	v[4] = HAL_get_8bit_reg( RTC_APB_ADDR, RTC_R5_1);
	v[5] = HAL_get_8bit_reg( RTC_APB_ADDR, RTC_R6_1);
	v[6] = HAL_get_8bit_reg( RTC_APB_ADDR, RTC_R7_1);

	return Utils_Buffer_to_52_ValueU_Little(v);
}

void RTC_Get_Value_Pointer1(uint8_t * v){	// function to return the RTC pointer
	v[0] = HAL_get_8bit_reg( RTC_APB_ADDR, RTC_R3_1);
	v[1] = HAL_get_8bit_reg( RTC_APB_ADDR, RTC_R4_1);
	v[2] = HAL_get_8bit_reg( RTC_APB_ADDR, RTC_R5_1);
	v[3] = HAL_get_8bit_reg( RTC_APB_ADDR, RTC_R6_1);
	v[4] = HAL_get_8bit_reg( RTC_APB_ADDR, RTC_R7_1);
}

uint8_t RTC_Get_Config_value(){
	return HAL_get_8bit_reg(RTC_APB_ADDR, RTC_CONFIG);
}

uint8_t RTC_Set_Config_value(uint8_t val){
	HAL_set_8bit_reg( RTC_APB_ADDR, RTC_CONFIG, val);
	return RTC_Get_Config_value();
}

uint32_t RTC_Get_us() {
	uint64_t rtc_val = RTC_Get_Value64();
	return (uint32_t) (rtc_val & (uint64_t) 0x00000000FFFFFFFF);
}

uint32_t RTC_Get_ms() {
	uint64_t rtc_val = RTC_Get_Value64();
	uint64_t rtc_ms = rtc_val / 1000;
	//truncate to 32 bits
	return (uint32_t) (rtc_ms & (uint64_t) 0x00000000FFFFFFFF);
}

uint32_t RTC_Get_sec() {
    uint64_t rtc_val = RTC_Get_Value64();
    uint64_t rtc_sec = rtc_val / 1000000;

    return (uint32_t) rtc_sec;
}
uint16_t RTC_Get_ts_ms() {

    uint64_t rtc_ms = RTC_Get_ms();
    uint64_t rtc_ts = rtc_ms % 1000;

    return (uint16_t) rtc_ts;
}