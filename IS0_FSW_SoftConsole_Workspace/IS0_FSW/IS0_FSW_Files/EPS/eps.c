/*
 * eps.c
 *
 *  Created on: Jan 25, 2020
 *      Author: Anant
 */

#ifndef _EPS_C
	#define _EPS_C

#include <IS0_FSW_Files/EPS/eps.h>
#include <IS0_FSW_Files/GPIOs/gpios.h>
#include <IS0_FSW_Files/I2C/i2c.h>
#include <IS0_FSW_Files/MAIN/main.h>
#include <IS0_FSW_Files/Utils/utils.h>
#include "math.h"
#include "string.h"
#include <stdlib.h>
#include "drivers/corei2c/core_i2c.h"

/*------------------------------------------------------------------------------
 * Instance data for our CoreI2C devices
 */
i2c_instance_t g_core_i2c0;				// Core I2C 0 for EPS_C0;
i2c_instance_t g_core_i2c1; 			// Core I2C 1 for EPS_C1;
i2c_instance_t g_core_i2c2; 			// Core I2C 2 for EPS_NC;

uint8_t rx_buffer_EPS[RX_LENGTH_2];
i2c_status_t instance;


//Structure written for ease of data collection from the I2C slaves.
struct GetEpsSeqPoint {
	i2c_instance_t* i2c;
	uint8_t slaveAddr;
	uint8_t regAddr;
};

struct GetEpsSeqPoint points[34];

struct GetEpsSeqPoint assignGetEps(i2c_instance_t* i2c, uint8_t slaveAddr, uint8_t regAddr){
	struct GetEpsSeqPoint result;
	result.i2c = i2c;
	result.slaveAddr = slaveAddr;
	result.regAddr = regAddr;
	return result;
}


//Function to control the various GPIOs for IS0
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

//Assigning the points array each element of which contains a GetEpsSeqPoint structure 
//This array is then used to collect telemetry from the different I2C slaves of the EPS
void epsAssignInit(){
	int index = 0;

	//Critical Channel 0 Slaves
	//Fuel Gauge 1
	points[index++] = assignGetEps(&g_core_i2c0, EPS_FG1_SLAVE_C0, EPS_FG_SOC);

	//See EPS.h to see what voltage and current each of the channels contain.
	//VC Sensor 1
	points[index++] = assignGetEps(&g_core_i2c0, EPS_VC1_SLAVE_C0, EPS_VC_CH2_SHUNT_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c0, EPS_VC1_SLAVE_C0, EPS_VC_CH2_BUS_VOLTAGE);

	points[index++] = assignGetEps(&g_core_i2c0, EPS_VC1_SLAVE_C0, EPS_VC_CH3_SHUNT_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c0, EPS_VC1_SLAVE_C0, EPS_VC_CH3_BUS_VOLTAGE);

	//VC Sensor 2
	points[index++] = assignGetEps(&g_core_i2c0, EPS_VC2_SLAVE_C0, EPS_VC_CH1_SHUNT_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c0, EPS_VC2_SLAVE_C0, EPS_VC_CH1_BUS_VOLTAGE);

	points[index++] = assignGetEps(&g_core_i2c0, EPS_VC2_SLAVE_C0, EPS_VC_CH2_SHUNT_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c0, EPS_VC2_SLAVE_C0, EPS_VC_CH2_BUS_VOLTAGE);

	points[index++] = assignGetEps(&g_core_i2c0, EPS_VC2_SLAVE_C0, EPS_VC_CH3_SHUNT_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c0, EPS_VC2_SLAVE_C0, EPS_VC_CH3_BUS_VOLTAGE);

	//VC Sensor 3
	points[index++] = assignGetEps(&g_core_i2c0, EPS_VC3_SLAVE_C0, EPS_VC_CH1_SHUNT_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c0, EPS_VC3_SLAVE_C0, EPS_VC_CH1_BUS_VOLTAGE);

	points[index++] = assignGetEps(&g_core_i2c0, EPS_VC3_SLAVE_C0, EPS_VC_CH3_SHUNT_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c0, EPS_VC3_SLAVE_C0, EPS_VC_CH3_BUS_VOLTAGE);

	//VC Sensor 4
	//NOTE: This sensor is not present in the v4 Version of the EPS Board
	//It is only present in the flight board.
	points[index++] = assignGetEps(&g_core_i2c0, EPS_VC4_SLAVE_C0, EPS_VC_CH1_SHUNT_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c0, EPS_VC4_SLAVE_C0, EPS_VC_CH1_BUS_VOLTAGE);


	//Critical Channel 1 Slaves
	//Fuel Gauge 2
	points[index++] = assignGetEps(&g_core_i2c1, EPS_FG2_SLAVE_C1, EPS_FG_SOC);

	//VC Sensor 5
	points[index++] = assignGetEps(&g_core_i2c1, EPS_VC5_SLAVE_C1, EPS_VC_CH1_SHUNT_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c1, EPS_VC5_SLAVE_C1, EPS_VC_CH1_BUS_VOLTAGE);

	points[index++] = assignGetEps(&g_core_i2c1, EPS_VC5_SLAVE_C1, EPS_VC_CH2_SHUNT_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c1, EPS_VC5_SLAVE_C1, EPS_VC_CH2_BUS_VOLTAGE);

	points[index++] = assignGetEps(&g_core_i2c1, EPS_VC5_SLAVE_C1, EPS_VC_CH3_SHUNT_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c1, EPS_VC5_SLAVE_C1, EPS_VC_CH3_BUS_VOLTAGE);

	//VC Sensor 6
	points[index++] = assignGetEps(&g_core_i2c1, EPS_VC6_SLAVE_C1, EPS_VC_CH1_SHUNT_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c1, EPS_VC6_SLAVE_C1, EPS_VC_CH1_BUS_VOLTAGE);

	points[index++] = assignGetEps(&g_core_i2c1, EPS_VC6_SLAVE_C1, EPS_VC_CH2_SHUNT_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c1, EPS_VC6_SLAVE_C1, EPS_VC_CH2_BUS_VOLTAGE);

	points[index++] = assignGetEps(&g_core_i2c1, EPS_VC6_SLAVE_C1, EPS_VC_CH3_SHUNT_VOLTAGE);
	points[index++] = assignGetEps(&g_core_i2c1, EPS_VC6_SLAVE_C1, EPS_VC_CH3_BUS_VOLTAGE);

	//ADC
	points[index++] = assignGetEps(&g_core_i2c1, EPS_ADC_SLAVE_C1, EPS_ADC_MASK_MODE2_CH2);
	points[index++] = assignGetEps(&g_core_i2c1, EPS_ADC_SLAVE_C1, EPS_ADC_MASK_MODE2_CH4);
	points[index++] = assignGetEps(&g_core_i2c1, EPS_ADC_SLAVE_C1, EPS_ADC_MASK_MODE2_CH7);

	//Non-Critical Channel Slaves
	points[index++] = assignGetEps(&g_core_i2c2, EPS_FG3_SLAVE_NC, EPS_FG_SOC);

}

void Initialize_EPS(){

    /*-------------------------------------------------------------------------
     * Initialize the Core I2C Drivers
    */
    I2C_init(&g_core_i2c0, COREI2C_0, MASTER_SER_ADDR, I2C_PCLK_DIV_960);		// Core I2C 0 for EPS_C0;
    I2C_init(&g_core_i2c1, COREI2C_1, MASTER_SER_ADDR, I2C_PCLK_DIV_960);		// Core I2C 1 for EPS_C1;
    I2C_init(&g_core_i2c2, COREI2C_2, MASTER_SER_ADDR, I2C_PCLK_DIV_960);		// Core I2C 1 for EPS_C1;

    //Initialize the Data Structures
    epsAssignInit();

}

void Get_EPS_Data(){


		for(int i = 0; i < 34; ++i){
			instance = do_write_read_transaction(points[i].i2c, points[i].slaveAddr, points[i].regAddr, TX_LENGTH, rx_buffer_EPS, RX_LENGTH_2);
			Beacon_pack_IS0.EPS[i] = Utils_Buffer_to_16_ValueU_Big(rx_buffer_EPS);
			//Doing Data Processing on the EPS ADC Data , to remove the 4 MSBs Refer AD7998 Datasheet Page 20
			if(i==30 || i==31 || i==32 ){
				Beacon_pack_IS0.EPS[i] = Beacon_pack_IS0.EPS[i]&0x0FFF;
			}
		}

}

#endif