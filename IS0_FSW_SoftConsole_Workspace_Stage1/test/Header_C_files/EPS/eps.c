/*
 * eps.c
 *
 *  Created on: Jan 25, 2020
 *      Author: Anant
 */

#ifndef _PWR_C
	#define _PWR_C

#include <EPS/eps.h>
#include "math.h"
#include "GPIOs/gpios.h"
#include "I2C/i2c.h"
#include "Seq_ops/main_seq.h"
#include "string.h"
#include <stdlib.h>
#include "Utils/utils.h"
#include "drivers/corei2c/core_i2c.h"

/*------------------------------------------------------------------------------
 * Instance data for our CoreI2C devices
 */
i2c_instance_t g_core_i2c0;				// Core I2C for EPS_C0;
i2c_instance_t g_core_i2c1; 			// Core I2C for EPS_C1;
i2c_instance_t g_core_i2c2; 			// Core I2C for EPS_NC;


void Initialize_EPS_I2C_Channels(){

    /*-------------------------------------------------------------------------
     * Initialize the Core I2C Drivers
    */
    I2C_init(&g_core_i2c0, COREI2C_0, MASTER_SER_ADDR, I2C_PCLK_DIV_960);
    I2C_init(&g_core_i2c1, COREI2C_1, MASTER_SER_ADDR, I2C_PCLK_DIV_960);
    I2C_init(&g_core_i2c2, COREI2C_2, MASTER_SER_ADDR, I2C_PCLK_DIV_960);

}

void PWR_Switch(uint32_t config, uint8_t on_off){
	//Gets the Current state of all the GPIOs and stores it in GPIO pattern
	uint32_t gpio_pattern = MSS_GPIO_get_outputs();

	if(on_off){
		//IF on_off bit is set to 1, then turns ON the GPIOs passed in config,
		//while keeping the OLD GPIOs in the same state
		gpio_pattern |= config;
	}else{
		//IF on_off bit is set to 0, then turns OFF the GPIOs passed in config,
		//while keeping the old GPIOs same state
		gpio_pattern &= (~config);
	}
	MSS_GPIO_set_outputs(gpio_pattern);
}



struct GetEpsSeqPoint {
	i2c_instance_t* i2c;
	uint8_t slaveAddr;
	uint8_t regAddr;
};

struct GetEpsSeqPoint points[32];

struct GetEpsSeqPoint assignGetEps(i2c_instance_t* i2c, uint8_t slaveAddr, uint8_t regAddr){
	struct GetEpsSeqPoint result;
	result.i2c = i2c;
	result.slaveAddr = slaveAddr;
	result.regAddr = regAddr;
	return result;
}

void epsAssignInit(){
	int index = 0;
	points[index++] = assignGetEps(&g_core_i2c1, EPS_VC5_SLAVE_C1, EPS_VC_CH3_BUS_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c1, EPS_VC5_SLAVE_C1, EPS_VC_CH3_SHUNT_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c1, EPS_VC5_SLAVE_C1, EPS_VC_CH3_BUS_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c1, EPS_VC5_SLAVE_C1, EPS_VC_CH3_SHUNT_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c0, EPS_FG1_SLAVE_C0, EPS_FG_SOC);
	points[index++] = assignGetEps(&g_core_i2c1, EPS_ADC_SLAVE_C1, EPS_ADC_MASK_MODE2_CH2);
	points[index++] = assignGetEps(&g_core_i2c1, EPS_ADC_SLAVE_C1, EPS_ADC_MASK_MODE2_CH4);
	points[index++] = assignGetEps(&g_core_i2c0, EPS_VC4_SLAVE_C0, EPS_VC_CH1_BUS_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c0, EPS_VC3_SLAVE_C0, EPS_VC_CH3_BUS_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c0, EPS_VC3_SLAVE_C0, EPS_VC_CH1_BUS_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c0, EPS_VC4_SLAVE_C0, EPS_VC_CH1_SHUNT_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c0, EPS_VC3_SLAVE_C0, EPS_VC_CH3_SHUNT_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c0, EPS_VC3_SLAVE_C0, EPS_VC_CH1_SHUNT_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c1, EPS_ADC_SLAVE_C1, EPS_ADC_MASK_MODE2_CH6);
	points[index++] = assignGetEps(&g_core_i2c1, EPS_ADC_SLAVE_C1, EPS_ADC_MASK_MODE2_CH7);
	points[index++] = assignGetEps(&g_core_i2c1, EPS_VC6_SLAVE_C1, EPS_VC_CH3_BUS_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c1, EPS_VC6_SLAVE_C1, EPS_VC_CH3_SHUNT_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c1, EPS_VC6_SLAVE_C1, EPS_VC_CH1_BUS_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c1, EPS_VC6_SLAVE_C1, EPS_VC_CH1_SHUNT_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c0, EPS_VC2_SLAVE_C0, EPS_VC_CH1_BUS_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c0, EPS_VC2_SLAVE_C0, EPS_VC_CH1_SHUNT_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c0, EPS_VC2_SLAVE_C0, EPS_VC_CH2_BUS_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c0, EPS_VC2_SLAVE_C0, EPS_VC_CH2_SHUNT_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c0, EPS_VC1_SLAVE_C0, EPS_VC_CH3_BUS_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c0, EPS_VC1_SLAVE_C0, EPS_VC_CH3_SHUNT_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c1, EPS_VC6_SLAVE_C1, EPS_VC_CH2_BUS_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c1, EPS_VC6_SLAVE_C1, EPS_VC_CH2_SHUNT_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c1, EPS_VC5_SLAVE_C1, EPS_VC_CH2_BUS_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c1, EPS_VC5_SLAVE_C1, EPS_VC_CH2_SHUNT_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c0, EPS_VC2_SLAVE_C0, EPS_VC_CH3_SHUNT_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c0, EPS_VC1_SLAVE_C0, EPS_VC_CH2_BUS_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c0, EPS_VC1_SLAVE_C0, EPS_VC_CH2_SHUNT_VOLTAGE);
}

void Get_EPS_Data(){
	epsAssignInit();

		uint8_t write_length = TX_LENGTH;
		uint8_t rx_buffer[RX_LENGTH_2];
		uint8_t read_length = RX_LENGTH_2;
		i2c_status_t instance;
		for(int i = 0; i < 32; ++i){
			instance = do_write_read_transaction(points[i].i2c, points[i].slaveAddr, points[i].regAddr, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS[i] = Utils_Buffer_to_16_ValueU_Big(rx_buffer);
		}

}

#endif
