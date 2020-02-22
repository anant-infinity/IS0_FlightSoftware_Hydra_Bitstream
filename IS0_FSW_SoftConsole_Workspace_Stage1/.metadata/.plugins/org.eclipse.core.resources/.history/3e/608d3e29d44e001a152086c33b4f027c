/*
 * utils.h
 *
 *  Created on: Apr 4, 2019
 *      Author: ankit
 */

#ifndef UTILS_H_
#define UTILS_H_

#include "Seq_ops/main_seq.h"
#include "mss_spi/mss_spi.h"

#define SAVE_N_REBOOT	Globals.Indication_Flags |= (SAVE_PARAMETER_MAP_FLAG | SC_REBOOT)

uint16_t Utils_Buffer_to_16_ValueU_Big(uint8_t * buffer);
int16_t Utils_Buffer_to_16_ValueS_Big(uint8_t * buffer);
uint32_t Utils_Buffer_to_32_ValueU_Big(uint8_t * buffer);
int32_t Utils_Buffer_to_32_ValueS_Big(uint8_t * buffer);

uint16_t Utils_Buffer_to_16_ValueU_Little(uint8_t * buffer);
int16_t Utils_Buffer_to_16_ValueS_Little(uint8_t * buffer);
uint32_t Utils_Buffer_to_32_ValueU_Little(uint8_t * buffer);
int32_t Utils_Buffer_to_32_ValueS_Little(uint8_t * buffer);
uint64_t Utils_Buffer_to_52_ValueU_Little(uint8_t * buffer);

/*---------------------------------------------------------------------------*/
/* The Change_Endian_To_BigF32() is a utility function which converts the given
 * float argument to the big endian format.
 *
 * @param
 *  no parameter
 *
 * @return: float
 *  It returns the big endian format of the passed argument.
 *
*/
float Utils_Change_Endian_To_BigF32(float s);

/*---------------------------------------------------------------------------*/
/* The Change_Endian_To_BigU16() is a utility function which converts the given
 * unsigned 16 bits argument to the equivalent big endian format.
 *
 * @param
 *  no parameter
 *
 * @return: uint16_t
 *  It returns the big endian format of the passed argument.
 *
*/
uint16_t Utils_Change_Endian_To_BigU16(uint16_t s);

/*---------------------------------------------------------------------------*/
/* The Change_Endian_To_BigS16() is a utility function which converts the given
 * signed 16 bits argument to the equivalent big endian format.
 *
 * @param
 *  no parameter
 *
 * @return: uint16_t
 *  It returns the big endian format of the passed argument.
 *
*/
int16_t Utils_Change_Endian_To_BigS16(int16_t s);

uint8_t Utils_CMD_Set_Flag(uint8_t * var, uint8_t mask, uint8_t flag);


uint8_t Utils_Handle_Timer16(Timer16_t *  timer, uint32_t mask);
uint8_t Utils_Handle_Timer16_Started(Timer16_t *  timer);

void Utils_Start_Derek_Timer16(Derek_Timer16_t * timer, uint16_t duration);
uint8_t Utils_isStarted_Derek_Timer16(Derek_Timer16_t * timer);
uint8_t Utils_isTimeout_Derek_Timer16(Derek_Timer16_t * timer);

uint8_t Utils_Handle_Timer32(Timer32_t *  timer, uint32_t mask);

uint8_t Utils_Handle_Timer32_Started(Timer32_t *  timer);

void Utils_Manage_State_Machine(Module_Sync_t * module, uint16_t prev_state, uint16_t response_length);

uint8_t Utils_HAL_Set_8bit(addr_t base_addr, uint8_t reg_addr, uint8_t val);

uint16_t Utils_HAL_Set_16bit(addr_t base_addr, uint8_t reg_addr_U, uint8_t reg_addr_L, uint16_t val);

uint16_t Utils_HAL_Get_16bit(addr_t base_addr, uint8_t reg_addr_U, uint8_t reg_addr_L);

void Utils_Delay32(uint32_t delay);
void Utils_Delay32_us(uint32_t delay_us);
void Utils_Reset_Module_Sync(Module_Sync_t * mod,  uint8_t init_state, uint16_t cmd_time, uint16_t response_limit_time);

#endif /* HEADER_C_FILES_UTILS_UTILS_H_ */
