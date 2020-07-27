/*
 * main_seq.h
 *
 *  Created on: Jan 25, 2020
 *      Author: Anant
 */
#ifndef _MAIN_SEQ_H
#define _MAIN_SEQ_H


#include "CortexM3/GNU/cpu_types.h"
#include "mss_spi/mss_spi.h"

#undef  GLOBAL
#ifdef _MAIN_SEQ_C
#define GLOBAL
#else
#define GLOBAL extern
#endif

/*************************************************************************/
/* Some general definitions*/
#define SPI_ESCAPE_LIMIT		0xff00 		// The counter value to escape the loop in the SPI transactions
#define CCSDS_MAX_DATA_SIZE 	243			// the maximum size of the data that can be put into one CCSDS packet with AX.25 around it. After adding the CCSDS header and the fletcher code
											// the size of the packet becomes 256 bytes.
/*************************************************************************/

/* Defining the APIDs of different packet */
#define  BEACON_PACKET_APID                     0x10

/*************************************************************************/

/* Defining the modes of operation of the spacecraft */
#define SC_PHEONIX_MODE     0x00
#define SC_SAFE_MODE        0x01
#define SC_SCIC_MODE        0x02
#define SC_SCID_MODE        0x03

/* Packet Sizes (including CCSDS Headers) */
#define CCSDS_TLM_WRAPPER_SIZE			13

/*************************************************************************/

//Space between sectors
#define SD_SECTOR_BUFFER      10

//Start of all sectors
#define SD_SECTOR_START       4
#define SD_FAIL_THRESHOLD 5
#define SD_RESTART_LIMIT  20
#define SD_CARD_WRITE_NUM_TRIES 4


/*---------------------------------------------------------------------------*/
/* The main_seq() is the main sequence of operation. Call this function in the
 * main() function to run the flight software.
 *
 * @param
 * 	No parameter
 *
 * @return: void
 *
*/
void main_seq();

/*---------------------------------------------------------------------------*/
/* The Init() initializes all the modules of the flight software. It also reads
 * the parameter table from the flash memory and updates all the variables
 * appropriately.
 *
 * @param
 * 	No parameter
 *
 * @return: void
 *
*/
void Init();

/*---------------------------------------------------------------------------*/
/* The Get_Beacon_Packet() function forms the beacon packet structure by collecting
 * data from the sensor board and EPS. It also forms the CCSDS version of the packet
 * by added the CCSDS Header and fletcher code
 *
 * @param
 * 	No parameter
 *
 * @return: void
 *
*/
void Get_Beacon_Packet();

/*---------------------------------------------------------------------------*/
/* The Store_Beacon_Data() stores the beacon packet in appropriate SD card
 * and put the request to save the parameter table. If the write operation fails
 * it sets the Current SD card to 0xff indicating the failure of the SD card.
 *
 * This function is under development.
 *
 * @param
 * 	No parameter
 *
 * @return: void
 *
*/

void Make_Beacon_Packet_Array();

void Get_CDH_Data();

/*---------------------------------------------------------------------------*/
/* This function is used to collect the important global variables from the CDH.
 * It then adds them to the beacon packet.
 *
 *
 *
 * @param
 * 	No parameter
 *
 * @return: void
 *
*/

void Store_Beacon_Data();


/*---------------------------------------------------------------------------*/
/* The Decide_Mode() decides the mode of the spacecraft. This
 * is called either at the beginning or at the end of the sequence of operation.
 * It decides the mode on the basis of two timer values, one for SCIC and SCID
 * and the other for Safe and Pheonix
 *
 * @param
 * 	No parameter
 *
 * @return: void
 *
*/
void Decide_Mode();

void SwitchTo_Mode_Science_C();
void SwitchTo_Mode_Science_D();
void SwitchTo_Mode_Safe();
void SwitchTo_Mode_Pheonix();

/*---------------------------------------------------------------------------*/
/* The Beacon_Packet_UART_log() is used for ground testing only. It uss MSS_UART_0
 * To transimit the beacon packet formed to a ground PC.
 *
 * @param
 * 	No parameter
 *
 * @return: void
 *
*/
void Beacon_Packet_UART_log();



/*---------------------------------------------------------------------------*/
/* The Watchdog_Timer_Handler() manages the Watchdog timer. Once it expires then
 * a signal to the external watchdog IC is sent and the timer is restarted.
 *
 * @param
 * 	No parameter
 *
 * @return: void
 *
*/


/*---------------------------------------------------------------------------*/
/* The Watchdog_Pet() toggles a GPIO to send the "PET" signal to the watchdog
 * timer handler verilog peripheral
 *
 * @param
 * 	No parameter
 *
 * @return: void
 *
*/

void Watchdog_Pet(void);



/*---------------------------------------------------------------------------*/
/* The Utils_SPI_Custom_Setup() sets up the SPI for the transaction of data.
 *
 * @param
 * 	No parameter
 *
 * @return: void
 *
*/
void Utils_SPI_Custom_Setup(mss_spi_instance_t * this_spi, uint16_t frame_count);

/*---------------------------------------------------------------------------*/
/* The Utils_SPI_Transmit_Block() transmits block of data.
 *
 * @param
 * 	No parameter
 *
 * @return: void
 *
*/
uint16_t Utils_SPI_Transmit_Block(mss_spi_instance_t * this_spi, uint8_t * cmd_buffer, uint16_t cmd_byte_size);

/*---------------------------------------------------------------------------*/
/* The Utils_SPI_Receive_Block() receives the block of data.
 *
 * @param
 * 	No parameter
 *
 * @return: void
 *
*/
uint16_t Utils_SPI_Receive_Block(mss_spi_instance_t * this_spi, uint8_t * rec_buffer, uint16_t rec_byte_size);



#pragma pack(1)	// this pragma makes sure that all the structs are tightly packed

// The CCSDS header structure
struct CCSDS_Header{
        uint8_t Version_ID;
        uint8_t APID;
        uint16_t Seq_no;
        uint16_t PL;
        uint8_t TS2;    /* 1 lSB byte */
        uint32_t TS1;   /* 4 byte of MSB */
};

// The Enum defined EPS data of the beacon packet
typedef enum BeaconEpsPacketEnum {
    EPS_Bat_Volt = 0,
    EPS_Bat_Curr,
    EPS_Bus_Volt,
    EPS_Bus_Curr,
    EPS_Bat_SOC,
    EPS_Bat_Temp1,
    EPS_Bat_Temp2,
    EPS_SP1_Volt,
    EPS_SP2_Volt,
    EPS_SP3_Volt,

    EPS_SP1_Curr,
    EPS_SP2_Curr,
    EPS_SP3_Curr,
    EPS_Interface_Temp,
    EPS_EPS_Temp,
    EPS_CIP_Volt,
    EPS_CIP_Curr,
    EPS_ADCS_Volt,
    EPS_ADCS_Curr,
    EPS_SBAND_Volt,

    EPS_SBAND_Curr,
    EPS_UHF_Volt,
    EPS_UHF_Curr,
    EPS_CDH_Volt,
    EPS_CDH_Curr,
    EPS_GPS_33_Volt,
    EPS_GPS_33_Curr,
    EPS_GPS_12_Volt,
    EPS_GPS_12_Curr,
    EPS_Bat_Heater_Curr,

    EPS_DAXSS_Volt,
    EPS_DAXSS_Curr,
} BeaconEpsPacket;

// The Enum defined Sensor Board VMEL6075 data of the beacon packet

typedef enum BeaconVMEL6075PacketEnum {
	VMEL6075_UVA_DATA = 0,
	VMEL6075_UVB_DATA,
	VMEL6075_UVCOMP1_DATA,
	VMEL6075_UVCOMP2_DATA,

}BeaconVMEL6075Packet;

// The beacon packet structure
struct Beacon_packet_IS0{
	//To Add CCSDS Header
    struct CCSDS_Header beac_head;	// The CCSDS header

    uint8_t CDH_8[3];
    uint32_t CDH_32[3];
    uint64_t CDH_64[2];

    //EPS HK Data
    uint16_t EPS[34];

    //Sensor Board VMEL Data
    uint16_t Sensor_Board_VMEL6075[4];

    //Sensor Board AS7265X Data
    uint8_t Sensor_Board_AS7265X[36];

    //To Add the Fletcher Code
    uint16_t Fletcher_code;


} Beacon_pack_IS0;

GLOBAL uint8_t Beacon_Pack_Array[sizeof(Beacon_pack_IS0)];


typedef struct Parameter_Table 	// The struct definition for all the variables which are in the parameter table.
{

    /* Timer data */
    uint64_t Param_Watchdog_Signal_Threshold_Time;
    uint64_t Param_Beac_Timer_Threshold_Time;
    uint64_t Param_SCI_SCID_Thershold_Time;
    uint64_t Param_Safe_Pheonix_Threshold_Time;

    /* Limits */
    uint8_t Param_Flash_SPI_Tries_Limit;
    uint16_t Param_Flash_SPI_Wait_Limit;

    uint16_t Param_Fletcher_code;

} Param_Table_t;


/* Structs for Timers */
typedef struct Timer64 {
	uint64_t Start;
	uint64_t Time;
} Timer64_t;

typedef struct Timer32{
	uint32_t Start;
	uint32_t Time;
} Timer32_t;

typedef struct Timer16{
	uint16_t Start;
	uint16_t Time;
} Timer16_t;


typedef struct Timer8{
	uint8_t Start;
	uint8_t Time;
} Timer8_t;



GLOBAL struct Global_Variables { 	// Struct containing all the global variables

   uint32_t						Time_in_msec;

   //Timers
   Timer64_t                   	Watchdog_Signal_Timer;
   Timer64_t                   	Beac_Timer;
   Timer64_t                   	SCIC_SCID_Timer;
   Timer64_t					Safe_Pheonix_Timer;

   //Timer Thresholds
   uint64_t 					Beac_Thershold_Time;
   uint64_t 					SCI_SCID_Thershold_Time;
   uint64_t						Safe_Pheonix_Threshold_Time;
   uint64_t						Watchdog_Signal_Threshold_Time;

   //Flags
   uint8_t                     	I2C_Error_Flag;

   //Variables
   uint8_t                     	Sat_Curr_Mode;
   uint8_t 						Forced_Mode_Flag;
   uint32_t                    	Beacon_Packet_Seq_Counter;
   uint8_t                     	Flash_SPI_Tries_Limit;
   uint16_t                    	Flash_SPI_Wait_Limit;
   uint32_t                    	Boot_Up_Counter;

   //Flash Packet Write Counter
   uint32_t                    	Flash_Packet_Write_Counter;
   uint32_t                    	Flash_Packet_Read_Counter;

   //SD Card Sector Addresses
   uint32_t                    	Beacon_Sector_Start;
   uint32_t                    	Beacon_Sector_End;
   uint32_t                    	Beacon_Write_Start;
   uint32_t                    	Beacon_Read_Start;
   uint8_t                     	SD_Clk_Div;
   uint8_t                     	Current_SD;
   uint32_t						SD_Write_Read_Verify_Count;

} Globals;


#endif /* MAIN_SCH_H_ */
