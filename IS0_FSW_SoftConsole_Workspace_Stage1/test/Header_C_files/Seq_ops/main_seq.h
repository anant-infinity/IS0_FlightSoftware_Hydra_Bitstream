/*
 * main_seq.h
 *
 *  Created on: Jan 25, 2020
 *      Author: Anant
 */
#ifndef _MAIN_SEQ_H
#define _MAIN_SEQ_H

/***  defines for code state    **********/
#define ENABLE_UART0_COMMAND

/***************************/

/***
 * Defines for which FPGA configuration we are running!
 */
//#define ANKIT_OLD_AND_WORKING
//#define MAYURESH_V3_RTC_WORKS
//#define MAYURESH_V4_DOESNT_WORK
#define MAYURESH_V4			// as of 10/22/2019 --  clock tree didn't match in C and in FPGA
/********************************/



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

#define SC_REBOOT_SIGNAL_DELAY	240 		// Number of cycles which will be given between each toggle of the line going to the 4 bit counter IC.
#define PI 						(22/7) 		// The value of PI
#define SPI_ESCAPE_LIMIT		0xff00 		// The counter value to escape the loop in the SPI transactions
#define CCSDS_MAX_DATA_SIZE 	243			// the maximum size of the data that can be put into one CCSDS packet with AX.25 around it. After adding the CCSDS header and the fletcher code
											// the size of the packet becomes 256 bytes.
#define SBAND_SYNC_WORD 				(uint16_t) 0x6948 		// The Sync word for the SBAND (it has to be updated later)
#define SIZEOF_SYNC_WORD				sizeof(SBAND_SYNC_WORD) // The size of the SBand sync word
/*************************************************************************/

/* Defining the APIDs of different packet */
#define  BEACON_PACKET_APID                     0x10

/*************************************************************************/

/* Defining the modes of operation of the spacecraft */
#define SC_PHEONIX_MODE     0x00
#define SC_SAFE_MODE        0x01
#define SC_SCIC_MODE        0x02
#define SC_SCID_MODE        0x03
/*************************************************************************/


/* Indication Flags: Indication_Flags */
#define SAVE_PARAMETER_MAP_FLAG     	0x0001
#define SD_CARD0_WORKING            	0x0002
#define SD_CARD1_WORKING            	0x0004
#define SC_REBOOT                   	0x0020

/*************************************************************************/

/* Data Valid Flags: Beac_pack.CDH_Sub_System_Data_Valid_Flags */

#define EPS_C0_DATA_VALID_FLAG	0x10
#define EPS_C1_DATA_VALID_FLAG	0x20
#define EPS_NC_DATA_VALID_FLAG	0x40
#define EPS_DATA_VALID_FLAG		0x10
/*************************************************************************/


/* Packet Sizes (including CCSDS Headers) */
#define CCSDS_TLM_WRAPPER_SIZE			13

/*************************************************************************/

//Space between sectors
#define SD_SECTOR_BUFFER      10

//Start of all sectors
#define SD_SECTOR_START       4

//Size of each sector, in bytes
#define SD_SECTOR_SIZE_BEACON 31536000
#define SD_SECTOR_SIZE_SCID   10512000
#define SD_SECTOR_SIZE_SCIC   157680000
#define SD_SECTOR_SIZE_ADCS   3153600
#define SD_SECTOR_SIZE_IMG    75600
#define SD_SECTOR_SIZE_LOG    546000
#define SD_SECTOR_SIZE_HK     0

#define SD_FAIL_THRESHOLD 5
#define SD_RESTART_LIMIT  20

#define SD_CARD_WRITE_NUM_TRIES 4
/*************************************************************************/



/***************************** Global Variables ***************************************************/

GLOBAL uint64_t statusG; 	// a global Variable for looking at the return values of various functions. It is used only in main.c file (See main.c file)


/********************************* Global Variable Ends ***********************************************/

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
/* The SC_Restart() does the spacecraft restart. It has to give 8 positive edges
 * on a GPIO line going to the external 4-bit counter IC on-board. It sets the 
 * GPIO low then give a delay of SC_REBOOT_SIGNAL_DELAY cycles using for loop
 * then sets the GPIO high. Repeat the process 8 times and the spacecraft should
 * get restarted before or on the 8th cycle.
 *
 * @param
 * 	No parameter
 *
 * @return: void
 *
*/
void SC_Restart();



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


typedef struct
{
    uint32_t start;
    uint32_t end;
    uint32_t write;
    uint32_t read;


} SD_Sector_t;


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



// Definition of a general packet containing the maximum data size array
struct CCSDS_Max_Data_size_Packet
{
    struct CCSDS_Header ccsds_head;	// the ccsds header 
    uint8_t data[CCSDS_MAX_DATA_SIZE];	// the array with maximum data size allowed
    uint16_t Fletcher_code;
};

// typedef the CCSDS Max size packet as the Paramter table packet1
typedef struct CCSDS_Max_Data_size_Packet Param_table_packet1_t;


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

typedef struct Derek_Timer16 {
	uint16_t start;
	uint16_t time;
	uint8_t isStarted;
}Derek_Timer16_t;

typedef struct Timer8{
	uint8_t Start;
	uint8_t Time;
} Timer8_t;

typedef struct 	// a struct for managing the state machine of different subsystems and modules
{
    uint16_t CMD_Seq_Count;
    uint16_t Prev_CMD_Seq_Count;
    uint16_t Response_Length;
    uint16_t Response_Read;
    Timer16_t CMD_Period_Timer;
    Timer16_t Response_Limit_Timer;
    uint8_t Non_response_count;
    uint8_t GS_CMD_Length;
    uint8_t GS_CMD_Response_Length;
} Module_Sync_t;


typedef struct 	// a small struct for the managing the less complex state machines.
{
    uint16_t CMD_Seq_Count;
    uint16_t Prev_CMD_Seq_Count;
    uint16_t Response_Length;
    uint16_t Response_Read;
} Module_Sync_Small_t;

typedef struct
{
    uint16_t failCount;
    uint16_t restartCount;
    uint16_t failThreshold;
    uint16_t restartLimit;

} SD_FDRI_t;


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

   SD_FDRI_t					SD_FDRI_Module_Sync;

   Module_Sync_Small_t         	Param_Module_sync;

} Globals;




typedef struct CCSDS_Max_Data_size_Packet Global_packet_Full_t;	// Tyepdef of the CCSDS Max size packet as global packet 1
typedef struct Global_Table_Packet{	// Struct definition of last global packet 

    struct CCSDS_Header global_head;	// the CCSDS header

    uint8_t data[sizeof(Globals)];

    uint16_t Fletcher_code;

} Global_packet_Last_t;


#endif /* MAIN_SCH_H_ */
