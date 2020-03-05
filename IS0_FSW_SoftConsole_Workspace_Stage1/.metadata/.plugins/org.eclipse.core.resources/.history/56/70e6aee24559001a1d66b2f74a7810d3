/*
 * utils.h
 *
 *  Created on: Apr 4, 2019
 *      Author: ankit
 */

#ifndef UTILS_C_
#define UTILS_C_

#include "utils.h"
#include "RTC/rtc.h"
#include "CCSDS/ccsds.h"
#include "string.h"
#include "../../CMSIS/mss_assert.h"


uint16_t Utils_Buffer_to_16_ValueU_Big(uint8_t * buffer){
	return buffer[0] << 8 | buffer[1];
}

int16_t Utils_Buffer_to_16_ValueS_Big(uint8_t * buffer){
	return buffer[0] << 8 | buffer[1];
}

uint32_t Utils_Buffer_to_32_ValueU_Big(uint8_t * buffer){
	return buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 | buffer[3];
}

int32_t Utils_Buffer_to_32_ValueS_Big(uint8_t * buffer){
	return buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 | buffer[3];
}

uint16_t Utils_Buffer_to_16_ValueU_Little(uint8_t * buffer){
	return buffer[1] << 8 | buffer[0];
}

int16_t Utils_Buffer_to_16_ValueS_Little(uint8_t * buffer){
	return buffer[1] << 8 | buffer[0];
}

uint32_t Utils_Buffer_to_32_ValueU_Little(uint8_t * buffer){
	return buffer[3] << 24 | buffer[2] << 16 | buffer[1] << 8 | buffer[0];
}

int32_t Utils_Buffer_to_32_ValueS_Little(uint8_t * buffer){
	return buffer[3] << 24 | buffer[2] << 16 | buffer[1] << 8 | buffer[0];
}

uint64_t Utils_Buffer_to_52_ValueU_Little(uint8_t * buffer){
	uint64_t k = (uint64_t) buffer[6];
	for (uint8_t i = 1; i < 7; i++){
		k <<= 8;
		k |= buffer[6-i];
	}
	return k;
}

float Utils_Change_Endian_To_BigF32(float s){ /* TBC */
    
    uint8_t s1 = (uint8_t)s;
    uint8_t s2 = ((uint8_t)s >> 8);
    uint8_t s3 = ((uint8_t)s >> 16);
    uint8_t s4 = ((uint8_t)s >> 24);

    return (float)((s1 << 24) | (s2 << 16) | (s3 << 8) | s4);
}

uint16_t Utils_Change_Endian_To_BigU16(uint16_t s){ /* TBC */
    return (s << 8) | (s >> 8);
}

int16_t Utils_Change_Endian_To_BigS16(int16_t s){ /* TBC */
    
    int8_t s1 = s;
    int8_t s2 = (s >> 8);

    return (int16_t)((s1 << 8) | s2);
}

uint8_t Utils_CMD_Set_Flag(uint8_t * var, uint8_t mask, uint8_t flag){
	if(flag)
		*var |= mask;
	else
		*var &= (~mask);
	return *var;
}

uint8_t Utils_Handle_Timer16(Timer16_t *  timer, uint32_t mask){
	if((Globals.Timers_Started_Flag & mask) > 0){
		if(Utils_Handle_Timer16_Started(timer)){
			timer->Start = (uint16_t) RTC_Get_ms();
			return 1;
		}
		return 0;
   	}else{
		timer->Start = (uint16_t) RTC_Get_ms();
       	Globals.Timers_Started_Flag |= mask;
   	}
   	return 0;
}
uint8_t Utils_Handle_Timer16_Started(Timer16_t *  timer){
	uint16_t Curr = (uint16_t) RTC_Get_ms();
   	/* Taking care of timer overflow */
   	if (Curr < timer->Start){
       	Curr += (0xffff - timer->Start);
   	}else{
       	Curr -= timer->Start;
   	}

   	if(Curr >= timer->Time){
   		return 1;
   	}
   	return 0;
}

// duration in ms
void Utils_Start_Derek_Timer16(Derek_Timer16_t * timer, uint16_t duration) {
	// if already started, do nothing.
	if (Utils_isStarted_Derek_Timer16(timer)) return;
	timer->isStarted = 1;
	timer->time = duration;
	timer->start = (uint16_t) RTC_Get_ms();


}
uint8_t Utils_isStarted_Derek_Timer16(Derek_Timer16_t * timer) {
	return timer->isStarted;
}
uint8_t Utils_isTimeout_Derek_Timer16(Derek_Timer16_t * timer) {
	// convention here is to say that a timer is
	// timed out when it is ready to start again.
	if (!timer->isStarted) {
		uint8_t test = 1;
		test++;
		return 1;
	}

	if ((uint16_t)(((uint16_t) RTC_Get_ms() - timer->start) % 0xFFFF) > timer->time) {
		timer->isStarted = 0;
		return 1;
	}
	return 0;
}

uint8_t Utils_Handle_Timer32(Timer32_t *  timer, uint32_t mask){
	if((Globals.Timers_Started_Flag & mask) > 0){
		if(Utils_Handle_Timer32_Started(timer)){
			timer->Start = RTC_Get_ms();
			return 1;
		}
		return 0;
   	}else{
		timer->Start = RTC_Get_ms();
       	Globals.Timers_Started_Flag |= mask;
   	}
   	return 0;
}
uint8_t Utils_Handle_Timer32_Started(Timer32_t *  timer){
	uint32_t Curr = RTC_Get_ms();
   	/* Taking care of timer overflow */
   	if (Curr < timer->Start){
       	Curr += (0xffffffff - timer->Start);
   	}else{
       	Curr -= timer->Start;
   	}

   	if(Curr >= timer->Time){
   		return 1;
   	}
   	return 0;
}

void Utils_Manage_State_Machine(Module_Sync_t * module, uint16_t prev_state, uint16_t response_length){
    module->Response_Length = response_length;
	module->Response_Read = 0;
    module->Prev_CMD_Seq_Count = prev_state;
}


uint8_t Utils_HAL_Set_8bit(addr_t base_addr, uint8_t reg_addr, uint8_t val){
    HW_set_8bit_reg(base_addr+reg_addr, val);
    return HW_get_8bit_reg(base_addr+reg_addr);
}

uint16_t Utils_HAL_Set_16bit(addr_t base_addr, uint8_t reg_addr_U, uint8_t reg_addr_L, uint16_t val){
    uint8_t test_start_lower = val,test_start_upper = val >> 8;
    uint16_t start_p=0;
    HW_set_8bit_reg(base_addr + reg_addr_L, test_start_lower);
    HW_set_8bit_reg(base_addr + reg_addr_U, test_start_upper);

    return Utils_HAL_Get_16bit(base_addr, reg_addr_U, reg_addr_L);
}

uint16_t Utils_HAL_Get_16bit(addr_t base_addr, uint8_t reg_addr_U, uint8_t reg_addr_L){
    uint8_t test_start_upper = 0;
    uint16_t start_p=0;

    start_p = HW_get_8bit_reg(base_addr + reg_addr_L);
    test_start_upper = HW_get_8bit_reg(base_addr + reg_addr_U);

    start_p = start_p | (test_start_upper << 8);
    return start_p;
}

void Utils_Reset_Module_Sync(Module_Sync_t * mod,  uint8_t init_state, uint16_t cmd_time, uint16_t response_limit_time){
    mod->CMD_Seq_Count = init_state;
    mod->Response_Length = 0;
    mod->Prev_CMD_Seq_Count = 0;
    mod->CMD_Period_Timer.Time = cmd_time;
    mod->Response_Limit_Timer.Time = response_limit_time;
    mod->Response_Read = 0;
    mod->CMD_Period_Timer.Start = 0;
    mod->GS_CMD_Length = 0;
    mod->GS_CMD_Response_Length = 0;
}

void Utils_Delay32(uint32_t delay) {
	uint32_t start = RTC_Get_us();
	while ((uint32_t)(RTC_Get_us() - start) < (delay*1000));
	return;
}

void Utils_Delay32_us(uint32_t delay_us) {
	uint32_t start = RTC_Get_us();
	while((uint32_t)(RTC_Get_us() - start) < (delay_us));
	return;
}
#endif /* HEADER_C_FILES_UTILS_UTILS_H_ */
