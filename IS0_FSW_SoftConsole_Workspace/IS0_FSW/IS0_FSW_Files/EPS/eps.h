/*
 * eps.h
 *
 *  Created on: Jan 25, 2020
 *      Author: Anant
 */

#ifndef _EPS_H
	#define _EPS_H


#include <IS0_FSW_Files/MAIN/main.h>

#undef  GLOBAL
#ifdef _EPS_C
#define GLOBAL
#else
#define GLOBAL  extern
#endif

#define MASTER_SER_ADDR     			((uint8_t)0x21)	// not used 

/*-----------------------------------------------------------------------------
	 * All Slave Addresses of the I2C Critical Channel 0 EPS Slaves
	 */

	#define	EPS_FG1_SLAVE_C0				((uint8_t)0x36)	// 0 0110110 -> Battery Gauge Slave (U12) on EPS I2C Critical Channel 0
	#define EPS_VC1_SLAVE_C0 				((uint8_t)0x40)	// 0 1000000 -> VC Sensor (U6) on EPS I2C Critical Channel 0 -> CH1 Contains Nothing, CH2 Contains DAXSS, CH3 Contains CDH
	#define EPS_VC2_SLAVE_C0 				((uint8_t)0x43)	// 0 1000011 -> VC Sensor (U15) on EPS I2C Critical Channel 0 -> CH1 Contains SBAND, CH2 Contains UHF, CH3 Contains Battery Heater
	#define EPS_VC3_SLAVE_C0 				((uint8_t)0x41)	// 0 1000001 -> VC Sensor (U1) on EPS I2C Critical Channel 0 -> CH1 Contains PV2, CH2 Contains Nothing, CH3 Contains PV1
	#define EPS_VC4_SLAVE_C0 				((uint8_t)0x42)	// 0 1000010 -> VC Sensor (U16) on EPS I2C Critical Channel 0 -> CH1 Contains PV0

	/*-----------------------------------------------------------------------------
	 * All Slave Addresses of the I2C Critical Channel 1 EPS Slaves
	 */

	#define	EPS_FG2_SLAVE_C1				((uint8_t)0x36)	// 0 0110110 -> Battery Gauge Slave	(U13) I2C Critical Channel 1
	#define EPS_VC5_SLAVE_C1 				((uint8_t)0x40)	// 0 1000000 -> VC Sensor (U7) on EPS I2C Critical Channel 1 -> CH1 Contains GPS_12V_2 , CH2 Contains GPS_12V_1, CH3 Contains Battery Sense
	#define EPS_VC6_SLAVE_C1 				((uint8_t)0x41)	// 0 1000001 -> VC Sensor (U2) on EPS I2C Critical Channel 1 -> CH1 Contains ADCS, CH2 Contains GPS3V3, CH3 Contains CIP
	#define EPS_ADC_SLAVE_C1				((uint8_t)0x20)	// 0 0100000 -> ADC slave (U8) on EPS I2C Critical Channel 1
	/*-----------------------------------------------------------------------------
	 * All Slave Addresses of the I2C Non-Critical Channel 0 EPS Slaves
	 */

	#define	EPS_FG3_SLAVE_NC				((uint8_t)0x36)	// 0 0110110 -> Battery Gauge Slave	(U11) I2C Critical Channel 1

	/*-----------------------------------------------------------------------------
	 * Defining the Register Map of the VC Sensors - Shunt Voltage Indicates Current
	 */
	#define	Read_Manufacture_ID				((uint8_t)0xFE)		// Reading the Manufacturer ID of the IC - should be 0x5449- Works!
	#define Read_Config_Register 			((uint8_t)0x00)		// Reading default value of the configuration register
	#define	EPS_VC_CH1_SHUNT_VOLTAGE		((uint8_t)0x01)		// 16 bits Channel 1 Shunt Voltage reading;	Ops -> R
	#define	EPS_VC_CH1_BUS_VOLTAGE			((uint8_t)0x02)		// 16 bits Channel 1 Bus Voltage reading;	Ops -> R
	#define	EPS_VC_CH2_SHUNT_VOLTAGE		((uint8_t)0x03)		// 16 bits Channel 2 Shunt Voltage reading;	Ops -> R
	#define	EPS_VC_CH2_BUS_VOLTAGE			((uint8_t)0x04)		// 16 bits Channel 2 Bus Voltage reading;	Ops -> R
	#define	EPS_VC_CH3_SHUNT_VOLTAGE		((uint8_t)0x05)		// 16 bits Channel 3 Shunt Voltage reading;	Ops -> R
	#define	EPS_VC_CH3_BUS_VOLTAGE			((uint8_t)0x06)		// 16 bits Channel 3 Bus Voltage reading;	Ops -> R

	/*-----------------------------------------------------------------------------
	 * Defining the Register Map of the Fuel Gauges
	 */

	#define	EPS_FG_VCELL		((uint8_t)0x02)		// 16 bits Voltage of Cell reading;			OPs -> R
	#define	EPS_FG_SOC			((uint8_t)0x04)		// 16 bits State of Charge reading;	Ops -> R
	#define	EPS_FG_MODE			((uint8_t)0x06)		// 16 bits Mode Determination Register;	Ops -> Wnot
	#define	EPS_FG_VERSION		((uint8_t)0x08)		// 16 bits Version of IC;	Ops -> R
	#define	EPS_FG_HIBRT		((uint8_t)0x0A)		// 16 bits Hibernate entering and exiting thresholds register;	Ops -> R/Wnot
	#define	EPS_FG_CONFIG		((uint8_t)0x0C)		// 16 bits Configuration register.	Ops -> R/Wnot
	#define	EPS_FG_VALRT		((uint8_t)0x14)		// 16 bits Configures the VCELL range outside of which alerts are generated;Ops -> R/Wnot
	#define	EPS_FG_CRATE		((uint8_t)0x16)		// 16 bits Approximate charge or discharge rate of the battery; Ops -> R
	#define	EPS_FG_VRESET_ID	((uint8_t)0x18)		// 16 bits Configures VCELL threshold below which the IC resets itself; Ops->R/Wnot
	#define	EPS_FG_STATUS		((uint8_t)0x1A)		// 16 bits Indicates overvoltage, undervoltage, SOC change, SOC low, and reset alerts; Ops->R


	/***************************************************************************
	 *
	 * Defining Register Map for ADC
	 */
	#define	EPS_ADC_CONV_RES			((uint8_t)0x00)		// 16 bits
	#define	EPS_ADC_ALRT				((uint8_t)0x01)		// 16 bits
	#define	EPS_ADC_CONFIG				((uint8_t)0x02)		// 16 bits
	#define	EPS_ADC_CYC_TIME			((uint8_t)0x03)		// 16 bits
	#define	EPS_ADC_DATA_LOW_CH1		((uint8_t)0x04)		// 16 bits
	#define	EPS_ADC_DATA_HIGH_CH1		((uint8_t)0x05)		// 16 bits
	#define	EPS_ADC_DATA_HYS_CH1		((uint8_t)0x06)		// 16 bits
	#define	EPS_ADC_DATA_LOW_CH2		((uint8_t)0x07)		// 16 bits
	#define	EPS_ADC_DATA_HIGH_CH2		((uint8_t)0x08)		// 16 bits
	#define	EPS_ADC_DATA_HYS_CH2		((uint8_t)0x09)		// 16 bits
	#define	EPS_ADC_DATA_LOW_CH3		((uint8_t)0x0A)		// 16 bits
	#define	EPS_ADC_DATA_HIGH_CH3		((uint8_t)0x0B)		// 16 bits
	#define	EPS_ADC_DATA_HYS_CH3		((uint8_t)0x0C)		// 16 bits
	#define	EPS_ADC_DATA_LOW_CH4		((uint8_t)0x0D)		// 16 bits
	#define	EPS_ADC_DATA_HIGH_CH4		((uint8_t)0x0E)		// 16 bits
	#define	EPS_ADC_DATA_HYS_CH4		((uint8_t)0x0F)		// 16 bits

	// Masks for Mode 2 of ADC should be masked to pointer register.
	#define EPS_ADC_MASK_MODE2_CH1		((uint8_t)0x80)	// 1000-0000 Select CH1 in command mode
	#define EPS_ADC_MASK_MODE2_CH2		((uint8_t)0x90)	// 1001-0000 Select CH2 in command mode - Contains Battery 0
	#define EPS_ADC_MASK_MODE2_CH3		((uint8_t)0xA0)	// 1010-0000 Select CH3 in command mode
	#define EPS_ADC_MASK_MODE2_CH4		((uint8_t)0xB0)	// 1011-0000 Select CH4 in command mode - Contains Battery 1
	#define EPS_ADC_MASK_MODE2_CH5		((uint8_t)0xC0)	// 1100-0000 Select CH5 in command mode
	#define EPS_ADC_MASK_MODE2_CH6		((uint8_t)0xD0)	// 1101-0000 Select CH6 in command mode - Contains Thermistor on the OBC
	#define EPS_ADC_MASK_MODE2_CH7		((uint8_t)0xE0)	// 1110-0000 Select CH7 in command mode - Contains Thermistor on the EPS
	#define EPS_ADC_MASK_MODE2_CH8		((uint8_t)0xF0)	// 1111-0000 Select CH8 in command mode - Contains Thermistor on the Adapter Board
	#define EPS_ADC_MASK_MODE2_SEQCH	((uint8_t)0x70)	// Sequence of channels selected in configuration register
	/***************************************************************************/

void Initialize_EPS();
void Get_EPS_Data();
void PWR_Switch(uint32_t, uint8_t);
void epsAssignInit();

#endif
