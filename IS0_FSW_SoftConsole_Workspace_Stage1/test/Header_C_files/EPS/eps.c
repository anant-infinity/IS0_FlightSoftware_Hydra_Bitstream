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


void Get_EPS_Data(){

		uint8_t write_length = TX_LENGTH;
		uint8_t rx_buffer[RX_LENGTH_2];
		uint8_t read_length = RX_LENGTH_2;
		i2c_status_t instance;

			//Reading the Battery Voltage
			instance = do_write_read_transaction(&g_core_i2c1, EPS_VC5_SLAVE_C1, EPS_VC_CH3_BUS_VOLTAGE, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS_Bat_Volt = Utils_Buffer_to_16_ValueU_Big(rx_buffer);

			//Reading the Battery Current
			instance = do_write_read_transaction(&g_core_i2c1, EPS_VC5_SLAVE_C1, EPS_VC_CH3_SHUNT_VOLTAGE, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS_Bat_Curr = Utils_Buffer_to_16_ValueU_Big(rx_buffer);

			//Reading the EPS Bus Voltage
			instance = do_write_read_transaction(&g_core_i2c1, EPS_VC5_SLAVE_C1, EPS_VC_CH3_BUS_VOLTAGE, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS_Bus_Volt = Utils_Buffer_to_16_ValueU_Big(rx_buffer);

			//Reading the EPS Bus Current
			instance = do_write_read_transaction(&g_core_i2c1, EPS_VC5_SLAVE_C1, EPS_VC_CH3_SHUNT_VOLTAGE, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS_Bus_Curr = Utils_Buffer_to_16_ValueU_Big(rx_buffer);

			//TODO: This needs to be updated
			//Reading the EPS Battery SoC
			instance = do_write_read_transaction(&g_core_i2c0, EPS_FG1_SLAVE_C0, EPS_FG_SOC, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS_Bat_SOC = Utils_Buffer_to_16_ValueU_Big(rx_buffer);;

			//Reading the EPS EPS_Bat_Temp1
			instance = do_write_read_transaction(&g_core_i2c1, EPS_ADC_SLAVE_C1, EPS_ADC_MASK_MODE2_CH2, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS_Bat_Temp1 = Utils_Buffer_to_16_ValueU_Big(rx_buffer);

			//Reading the EPS EPS_Bat_Temp2
			instance = do_write_read_transaction(&g_core_i2c1, EPS_ADC_SLAVE_C1, EPS_ADC_MASK_MODE2_CH4, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS_Bat_Temp2 = Utils_Buffer_to_16_ValueU_Big(rx_buffer);

			//Reading the Solar Panel (PV0) Voltage 1
			instance = do_write_read_transaction(&g_core_i2c0, EPS_VC4_SLAVE_C0, EPS_VC_CH1_BUS_VOLTAGE, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS_CIP_Volt = Utils_Buffer_to_16_ValueU_Big(rx_buffer);

			//Reading the Solar Panel (PV1) Voltage 2
			instance = do_write_read_transaction(&g_core_i2c0, EPS_VC3_SLAVE_C0, EPS_VC_CH3_BUS_VOLTAGE, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS_CIP_Volt = Utils_Buffer_to_16_ValueU_Big(rx_buffer);

			//Reading the Solar Panel (PV2) Voltage 3
			instance = do_write_read_transaction(&g_core_i2c0, EPS_VC3_SLAVE_C0, EPS_VC_CH1_BUS_VOLTAGE, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS_CIP_Volt = Utils_Buffer_to_16_ValueU_Big(rx_buffer);

			//Reading the Solar Panel (PV0) Current 1
			instance = do_write_read_transaction(&g_core_i2c0, EPS_VC4_SLAVE_C0, EPS_VC_CH1_SHUNT_VOLTAGE, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS_CIP_Volt = Utils_Buffer_to_16_ValueU_Big(rx_buffer);

			//Reading the Solar Panel (PV1) Current 2
			instance = do_write_read_transaction(&g_core_i2c0, EPS_VC3_SLAVE_C0, EPS_VC_CH3_SHUNT_VOLTAGE, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS_CIP_Volt = Utils_Buffer_to_16_ValueU_Big(rx_buffer);

			//Reading the Solar Panel (PV2) Current 3
			instance = do_write_read_transaction(&g_core_i2c0, EPS_VC3_SLAVE_C0, EPS_VC_CH1_SHUNT_VOLTAGE, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS_CIP_Volt = Utils_Buffer_to_16_ValueU_Big(rx_buffer);


			//TODO: This needs to be checked and confirmed that ADC can work in MODE to by default
			//Reading the Adapter Board Temperature
			instance = do_write_read_transaction(&g_core_i2c1, EPS_ADC_SLAVE_C1, EPS_ADC_MASK_MODE2_CH6, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS_Interface_Temp = Utils_Buffer_to_16_ValueU_Big(rx_buffer);

			//Reading the EPS Temperature
			instance = do_write_read_transaction(&g_core_i2c1, EPS_ADC_SLAVE_C1, EPS_ADC_MASK_MODE2_CH7, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS_EPS_Temp = Utils_Buffer_to_16_ValueU_Big(rx_buffer);

			//Reading the CIP Voltage
			instance = do_write_read_transaction(&g_core_i2c1, EPS_VC6_SLAVE_C1, EPS_VC_CH3_BUS_VOLTAGE, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS_CIP_Volt = Utils_Buffer_to_16_ValueU_Big(rx_buffer);

			//Reading the CIP Current
			instance = do_write_read_transaction(&g_core_i2c1, EPS_VC6_SLAVE_C1, EPS_VC_CH3_SHUNT_VOLTAGE, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS_CIP_Volt = Utils_Buffer_to_16_ValueU_Big(rx_buffer);

			//Reading the ADCS Voltage
			instance = do_write_read_transaction(&g_core_i2c1, EPS_VC6_SLAVE_C1, EPS_VC_CH1_BUS_VOLTAGE, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS_CIP_Volt = Utils_Buffer_to_16_ValueU_Big(rx_buffer);

			//Reading the ADCS Current
			instance = do_write_read_transaction(&g_core_i2c1, EPS_VC6_SLAVE_C1, EPS_VC_CH1_SHUNT_VOLTAGE, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS_CIP_Volt = Utils_Buffer_to_16_ValueU_Big(rx_buffer);

			//Reading the SBAND Voltage
			instance = do_write_read_transaction(&g_core_i2c0, EPS_VC2_SLAVE_C0, EPS_VC_CH1_BUS_VOLTAGE, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS_CIP_Volt = Utils_Buffer_to_16_ValueU_Big(rx_buffer);

			//Reading the SBAND Current
			instance = do_write_read_transaction(&g_core_i2c0, EPS_VC2_SLAVE_C0, EPS_VC_CH1_SHUNT_VOLTAGE, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS_CIP_Volt = Utils_Buffer_to_16_ValueU_Big(rx_buffer);

			//Reading the UHF Voltage
			instance = do_write_read_transaction(&g_core_i2c0, EPS_VC2_SLAVE_C0, EPS_VC_CH2_BUS_VOLTAGE, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS_CIP_Volt = Utils_Buffer_to_16_ValueU_Big(rx_buffer);

			//Reading the UHF Current
			instance = do_write_read_transaction(&g_core_i2c0, EPS_VC2_SLAVE_C0, EPS_VC_CH2_SHUNT_VOLTAGE, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS_CIP_Volt = Utils_Buffer_to_16_ValueU_Big(rx_buffer);


			//Reading the CDH Voltage
			instance = do_write_read_transaction(&g_core_i2c0, EPS_VC1_SLAVE_C0, EPS_VC_CH3_BUS_VOLTAGE, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS_CIP_Volt = Utils_Buffer_to_16_ValueU_Big(rx_buffer);

			//Reading the CDH Current
			instance = do_write_read_transaction(&g_core_i2c0, EPS_VC1_SLAVE_C0, EPS_VC_CH3_SHUNT_VOLTAGE, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS_CIP_Volt = Utils_Buffer_to_16_ValueU_Big(rx_buffer);


			//Reading the GPS 3V3 Voltage
			instance = do_write_read_transaction(&g_core_i2c1, EPS_VC6_SLAVE_C1, EPS_VC_CH2_BUS_VOLTAGE, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS_GPS_33_Volt = Utils_Buffer_to_16_ValueU_Big(rx_buffer);

			//Reading the GPS 3V3 Current
			instance = do_write_read_transaction(&g_core_i2c1, EPS_VC6_SLAVE_C1, EPS_VC_CH2_SHUNT_VOLTAGE, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS_GPS_33_Curr = Utils_Buffer_to_16_ValueU_Big(rx_buffer);

			//Reading the GPS 12 Voltage
			instance = do_write_read_transaction(&g_core_i2c1, EPS_VC5_SLAVE_C1, EPS_VC_CH2_BUS_VOLTAGE, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS_GPS_12_Volt = Utils_Buffer_to_16_ValueU_Big(rx_buffer);

			//Reading the GPS 12 Current
			instance = do_write_read_transaction(&g_core_i2c1, EPS_VC5_SLAVE_C1, EPS_VC_CH2_SHUNT_VOLTAGE, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS_GPS_12_Curr = Utils_Buffer_to_16_ValueU_Big(rx_buffer);

			//Reading the EPS Battery Heater Current
			instance = do_write_read_transaction(&g_core_i2c0, EPS_VC2_SLAVE_C0, EPS_VC_CH3_SHUNT_VOLTAGE, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS_Bat_Heater_Curr = Utils_Buffer_to_16_ValueU_Big(rx_buffer);

			//Reading the DAXSS Voltage
			instance = do_write_read_transaction(&g_core_i2c0, EPS_VC1_SLAVE_C0, EPS_VC_CH2_BUS_VOLTAGE, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS_DAXSS_Volt = Utils_Buffer_to_16_ValueU_Big(rx_buffer);

			//Reading the DAXSS Current
			instance = do_write_read_transaction(&g_core_i2c0, EPS_VC1_SLAVE_C0, EPS_VC_CH2_SHUNT_VOLTAGE, write_length, rx_buffer, read_length);
			Beacon_pack_IS0.EPS_DAXSS_Curr = Utils_Buffer_to_16_ValueU_Big(rx_buffer);


}

#endif
