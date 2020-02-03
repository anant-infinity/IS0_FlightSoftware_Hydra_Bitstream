/*
 * main_sch.h
 *
 *  Created on: Jul 9, 2018
 *      Author: Ankit
 */
/*
 * // Default Flags order:
 * 1st bit - UHF
 * 2th bit - ADCS
 * 3th bit - SBAND
 * 4th bit - CIP
 * 5th bit - EPS
 * 6th bit - CDH
 * 7th bit - DAXSS
 */
#ifndef _MAIN_SEQ_H
#define _MAIN_SEQ_H

/***  defines for code state    **********/
#define ENABLE_UART0_COMMAND
#define TEST_STATE // TODO: remove before flight
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
#define UHF_MAX_DATA_SIZE       0x100 		// The maximum size of the packet which can be transmitted ove UHF is 256 bytes (0x100).
#define CCSDS_MAX_DATA_SIZE 	243			// the maximum size of the data that can be put into one CCSDS packet with AX.25 around it. After adding the CCSDS header and the fletcher code
											// the size of the packet becomes 256 bytes.

#define SBAND_SYNC_WORD 				(uint16_t) 0x6948 		// The Sync word for the SBAND (it has to be updated later)
#define SIZEOF_SYNC_WORD				sizeof(SBAND_SYNC_WORD) // The size of the SBand sync word



/*************************************************************************/

/* Defining the APIDs of different packet */
#define  BEACON_PACKET_APID                     0x10

/*************************************************************************/

/* Defining the modes of operation of the spacecraft */
#define SC_PHOENIX_MODE     0x00
#define SC_SAFE_MODE        0x01
//Removed charging mode for IS0
//#define SC_CHARGING_MODE    0x02
#define SC_SCIC_MODE        0x03
#define SC_SCID_MODE        0x04
/*************************************************************************/

/* Defining the flags for avoiding a mode: Globals.Sat_Avoid_Mode */
#define SC_AVOID_SCID       0x03
#define SC_AVOID_SCIC       0x0C
#define SC_AVOID_CHARGING   0x30
#define SC_AVOID_SAFE       0xC0
/*************************************************************************/

/* Globals.Deps_Wait_Tumb_Flag */
#define SP1_DEPLOYMENT_FLAG	0x01
#define SP2_DEPLOYMENT_FLAG	0x02
#define ANT_DEPLOYMENT_FLAG	0x04
#define PSLV_WAIT_FLAG		0x08
#define DE_TUMBLED_FLAG		0x10
/*************************************************************************/

/* Globals.Timers_Started_Flag */
#define PSLV_WAIT_TIMER_STARTED_FLAG			0x001
#define DEPLOYMENT_DURATION_TIMER_STARTED_FLAG	0x002
#define DEPLOYMENT_WAIT_TIMER_STARTED_FLAG		0x004
#define HK_TIMER_STARTED_FLAG					0x008
#define BEAC_TIMER_STARTED_FLAG					0x010
#define BEAC_TRANS_TIMER_STARTED_FLAG			0x020
#define ADCS_PACK_TIMER_STARTED_FLAG			0x040
#define LAST_COMMAND_TIMER_STARTED_FLAG			0x080
#define WATCHDOG_SIGNAL_TIMER_FLAG               0x100
#define ADCS_BOOT_WAIT_TIMER_STARTED            0x200
/*************************************************************************/

/* Indication Flags: Indication_Flags */
#define SAVE_PARAMETER_MAP_FLAG     	0x0001
#define SD_CARD0_WORKING            	0x0002
#define SD_CARD1_WORKING            	0x0004
#define AUTO_SBAND_BEACON_TRANS     	0x0008
#define AUTO_SBAND_DATA_TRANS      	 	0x0010
#define SC_REBOOT                   	0x0020
#define IS_ECLIPSE                  	0x0040
#define ADCS_BOOT_WAIT_COMPLETED    	0x0080
#define SBAND_TRANSMIT_ENABLE	    	0x0100
#define UHF_TRANSMIT_ENABLE    			0x0200
#define TRANSMIT_DATA_READY         	0x0400
#define UPDATE_RTC_GPS              	0x0800
#define REQ_BEACON_TRANSMIT         	0x1000
#define REQ_PARAM_TRANSMIT          	0x2000
#define REQ_SD_DATA_TRANSMIT        	0x4000
#define REQ_ADCS_IMG_TRANSMIT       	0x8000
#define REQ_SUBSYS_RESPONSE_TRANSMIT  	0x10000
#define REQ_GLOBAL_TRANSMIT				0x20000
#define TRANSMIT_BAND					0x40000


/*************************************************************************/

/* Command Persistant Flags: Globals.CMD_Persistant_Flags*/
#define AUTO_SBAND_TRANS           	 	0x01
#define CMD_ECHO_ON		            	0x02
#define FORCE_MODE_FLAG            	  	0x04
/*************************************************************************/

/* Data Valid Flags: Beac_pack.CDH_Sub_System_Data_Valid_Flags */
#define UHF_DATA_VALID_FLAG		0x01
#define ADCS_DATA_VALID_FLAG	0x02
#define SBAND_DATA_VALID_FLAG	0x04
#define CIP_DATA_VALID_FLAG		0x08

#define EPS_C0_DATA_VALID_FLAG	0x10
#define EPS_C1_DATA_VALID_FLAG	0x20
#define EPS_NC_DATA_VALID_FLAG	0x40

#define DAXSS_DATA_VALID_FLAG   0x80
#define EPS_DATA_VALID_FLAG		0x10
/*************************************************************************/

/* UHF Malfunction flags: Globals.UHF_Malfunction_Flags */
#define UHF_SOFT_RESTARTED_FLAG 		0x01
/*************************************************************************/

/* ADCS Malfunction flags: ADCS_Malfunction_Flags */
#define ADCS_SOFT_RESTARTED_FLAG		0x01
#define ADCS_RESTARTED_FLAG 			0x02
/*************************************************************************/

/* ADCS Malfunction flags: SBAND_Malfunction_Flags */
#define SBAND_FPGA_RESET_FLAG               0x01
#define SBAND_EN_RESET_FLAG                 0x02
#define SBAND_DSW_RESET_FLAG                0x04
/*************************************************************************/

/* DAXSS Malfunction flags: DAXSS_Malfunction_Flags */
#define DAXSS_RESTARTED_FLAG               0x01
/*************************************************************************/

/* Globals.Transmit_Helper State definitions */
#define TRANS_HELPER_IDLE			0x00

#define TRANS_HELPER_PARAM_READ	    0x01
#define TRANS_HELPER_PARAM_TRANS1	0x02
#define TRANS_HELPER_PARAM_TRANS2	0x03

#define TRANS_HELPER_GLOBAL_TRANS1	0x01
#define TRANS_HELPER_GLOBAL_TRANS2	0x02
#define TRANS_HELPER_GLOBAL_TRANS3	0x03
/*************************************************************************/

/* Packet Sizes (including CCSDS Headers) */
#define CCSDS_TLM_WRAPPER_SIZE			13
#define SCID_PACKET_SIZE 				254
#define BEACON_PACKET_SIZE				254
#define PARAMETER_TABLE1_PACKET_SIZE 	256
#define PARAMETER_TABLE2_PACKET_SIZE	94
#define HK_PACKET_SIZE					0
#define SCIC_PACKET_SIZE				0
#define ADCS_TM_PACKET_0_SIZE			241
#define ADCS_TM_PACKET_1_SIZE			240
#define ADCS_TM_PACKET_2_SIZE			241
#define ADCS_TM_PACKET_3_SIZE			240
#define ADCS_TM_PACKET_4_SIZE			238
#define ADCS_TM_PACKET_5_SIZE			245
#define ADCS_NUM_TM_PACKETS				6
#define LOG_PACKET_SIZE					72
/*************************************************************************/


/***************************** Global Variables ***************************************************/

//GLOBAL uint8_t Sat_Curr_Mode;   // Holds the current mode of the spacecraft
//GLOBAL uint8_t Sat_Avoid_Mode;  // Holds the data regarding the avoiding modes.

//GLOBAL uint32_t Timers_Started_Flag;    // Holds the data of which timer has started.


//GLOBAL uint32_t Indication_Flags;   // Used for indicating various actions to different modules

 /* Sequence counter for packets */
//GLOBAL uint16_t Beacon_Packet_Seq_Counter;  // The CCSDS sequence counter for beacon packet
//GLOBAL uint16_t Science_Packet_Seq_Counter; // The CCSDS sequence counter for Science Packet
//GLOBAL uint16_t ADCS_Packet_Seq_Counter;    // The CCSDS sequence counter for ADCS Packet
//GLOBAL uint16_t ADCS_IMG_Packet_Seq_Counter;    // The CCSDS sequence counter for ADCS image Packet
//GLOBAL uint16_t Log_Packet_Seq_Counter; // The CCSDS sequence counter for Log Packet
//GLOBAL uint16_t HK_Packet_Seq_Counter;  // The CCSDS sequence counter for HK Packet
//GLOBAL uint16_t Parameter_Table_Packet_Seq_Counter;
//GLOBAL uint16_t Subsys_Response_Packet_Seq_Counter; // The CCSDS sequence counter for Subsystem Response Packet

/*-------------------------- All the ENVM parameters -------------------------- */

/* LSB  0: SP1, 1: SP2, 2: ANT, 3: PSLV_WAIT_FLAG, 4: Tumble Flag,  5: xx, 6: xx, 7: Sys_Reset MSB */
//GLOBAL uint8_t  Deps_Wait_Tumb_Flag;    // Holds flags for PSLV wait; detumble; and solar panels and antenna deployment

/* Variables for timers and time*/
//GLOBAL Timer32_t PSLV_Wait_Timer;   // Timer for 40 minutes PSLV delay
//GLOBAL Timer32_t Deployment_Wait_Timer; // Timer for 30 minutes delay between deployment attempts.
//GLOBAL Timer16_t Deployment_Duration_Timer; // Timer for 30 seconds of deployment.
//GLOBAL Timer32_t HK_Timer;  // Timer for 30 seconds delay to store HK packet
//GLOBAL Timer32_t Beac_Timer;    // Timer for 5 seconds delay to store beacon packet
//GLOBAL Timer32_t Beac_Trans_Timer;  // Timer for 10 seconds delay for beacon transmission.
//GLOBAL Timer32_t ADCS_Pack_Timer;   // Timer for 30 seconds delay to store ADCS packet
//GLOBAL Timer32_t Last_Command_Timer;    // Timer for 2 days delay before restarting the spacecraft if no command is received from GS.
//GLOBAL Timer16_t Watchdog_Signal_Timer; // Timer for 700 milliseconds delay between two consecutive signals to the external watchdog
//GLOBAL Timer32_t RTC_Sync_Timer;    // Timer for 60 seconds for updating the RTC value with GPS time

/* Malfunction flags */
//GLOBAL uint8_t UHF_Malfunction_Flags;   // Holds the information regarding the UHF malfunction
//GLOBAL uint8_t ADCS_Malfunction_Flags;  // Holds the information regarding the ADCS malfunction
//GLOBAL uint8_t SBAND_Malfunction_Flags; // Holds the information regarding the SBand malfunction
//GLOBAL uint8_t DAXSS_Malfunction_Flags; // Holds the information regarding the DAXSS malfunction

/* Limits */
//GLOBAL uint8_t Non_Response_Count_Limit;    // The limit on no. of times non-response should be received before peforming FDRI
//GLOBAL uint8_t Subsystem_Restart_Limit; // The number of times the spacecraft can be power cycled for a subsystem.
//GLOBAL float SBAND_Auto_Lat_Beac1_i_Limit;  // Limit for auto sband transmission
//GLOBAL float SBAND_Auto_Lat_Beac1_f_Limit;  // Limit for auto sband transmission
//GLOBAL float SBAND_Auto_Long_Beac1_i_Limit; // Limit for auto sband transmission
//GLOBAL float SBAND_Auto_Long_Beac1_f_Limit; // Limit for auto sband transmission


//GLOBAL float SBAND_Auto_Lat_Data1_i_Limit;  // Limit for auto sband transmission
//GLOBAL float SBAND_Auto_Lat_Data1_f_Limit;  // Limit for auto sband transmission
//GLOBAL float SBAND_Auto_Long_Data1_i_Limit; // Limit for auto sband transmission
//GLOBAL float SBAND_Auto_Long_Data1_f_Limit; // Limit for auto sband transmission
//GLOBAL float SBAND_Auto_Lat_Beac2_i_Limit;  // Limit for auto sband transmission
//
//
//GLOBAL float SBAND_Auto_Lat_Beac2_f_Limit;  // Limit for auto sband transmission
//GLOBAL float SBAND_Auto_Long_Beac2_i_Limit; // Limit for auto sband transmission
//GLOBAL float SBAND_Auto_Long_Beac2_f_Limit; // Limit for auto sband transmission


//GLOBAL float SBAND_Auto_Lat_Data2_i_Limit;  // Limit for auto sband transmission
//GLOBAL float SBAND_Auto_Lat_Data2_f_Limit;  // Limit for auto sband transmission
//GLOBAL float SBAND_Auto_Long_Data2_i_Limit; // Limit for auto sband transmission
//GLOBAL float SBAND_Auto_Long_Data2_f_Limit; // Limit for auto sband transmission

/* Thresholds */
//GLOBAL uint16_t Phoe_Safe_Threshold;    //Phoenix to Safe to threshold value
//GLOBAL uint16_t Safe_Phoe_Threshold;    //Safe to phoenix to threshold value
//GLOBAL uint16_t Safe_Char_Threshold;    //Safe to Charging to threshold value
//GLOBAL uint16_t Char_Safe_Threshold;    //Charging to Safe to threshold value
//GLOBAL uint16_t Char_SciD_Threshold;    //Charging to SciD to threshold value
//GLOBAL uint16_t SciD_Char_Threshold;    //SciD to Charging to threshold value
//GLOBAL uint16_t Char_SciC_Threshold;    //Charging to SciC to threshold value
//GLOBAL uint16_t SciC_Char_Threshold;    //SciC to Charging to threshold value
//GLOBAL uint16_t SP_Deployment_Stop_Volt_Threshold;  // The voltage level of solar panels above which the deployment attempts of the solar panel will stop
//GLOBAL float De_tumble_Threshold;   // The net body rate below which the de-tumble flag will be set
//GLOBAL float ADCS_Eclipse_Threshold;    // The angle of sun vector to the nadir vector; in body frame below which the eclipse flag will be set

/* Counters */
//GLOBAL uint8_t UHF_SC_Restart_Counter;  // Counts number of time the spacecraft is restart due to UHF malfunction
//GLOBAL uint8_t UHF_VC_SC_Restart_Counter;   // Counts number of time the spacecraft is restart due to VC sensor for UHF
//GLOBAL uint8_t ADCS_SC_Restart_Counter; // Counts number of time the spacecraft is restart due to ADCS malfunction
//GLOBAL uint8_t ADCS_VC_SC_Restart_Counter;  // Counts number of time the spacecraft is restart due to VC sensor for ADCS
//GLOBAL uint8_t SBAND_SC_Restart_Counter;    // Counts number of time the spacecraft is restart due to SBand malfunction
//GLOBAL uint8_t SBAND_VC_SC_Restart_Counter; // Counts number of time the spacecraft is restart due to VC sensor for SBand
//GLOBAL uint8_t DAXSS_SC_Restart_Counter;    // Counts number of time the spacecraft is restart due to DAXSS malfunction
//GLOBAL uint8_t DAXSS_VC_SC_Restart_Counter; // Counts number of time the spacecraft is restart due to VC sensor for DAXSS

/* SPI Flash Parameters */
//GLOBAL uint8_t Flash_SPI_Tries_Limit;   // No. of times the write operation on flash memory will take place in case of failure

/* Command Persistent flags*/
//GLOBAL uint8_t CMD_Persistant_Flags;    // Holds information of commands from GS which are to be persistent

 /* other counters */
//GLOBAL uint32_t Boot_Up_Counter;    // Number of times the flight software restarted.

/* Sectors pointers*/
//GLOBAL uint32_t Beacon_Sector_Start;    // Indicate the start sector of beacon packet.
//GLOBAL uint32_t Beacon_Sector_End;  // Indicate the end sector of beacon packet.
//GLOBAL uint32_t SciC_Sector_Start;  // Indicate the start sector of SciC packet.
//GLOBAL uint32_t SciC_Sector_End;    // Indicate the end sector of SciC packet.
//GLOBAL uint32_t SciD_Sector_Start;  // Indicate the start sector of SciD packet.
//GLOBAL uint32_t SciD_Sector_End;    // Indicate the end sector of SciD packet.
//GLOBAL uint32_t ADCS_Sector_Start;  // Indicate the start sector of ADCS packet.
//GLOBAL uint32_t ADCS_Sector_End;    // Indicate the end sector of ADCS packet.
//GLOBAL uint32_t ADCS_Image_Sector_Start;       // Indicate the start sector of Star tracker image.
//GLOBAL uint32_t ADCS_Image_Sector_End;     // Indicate the end sector of Star tracker image.
//GLOBAL uint32_t Log_Sector_Start;   // Indicate the start sector of Log packet.
//GLOBAL uint32_t Log_Sector_End; // Indicate the end sector of Log packet.
//GLOBAL uint32_t HK_Sector_Start;    // Indicate the start sector of HK packet.
//GLOBAL uint32_t HK_Sector_End;  // Indicate the end sector of HK packet.

/* Real time read and write sector indicator */
//GLOBAL uint32_t Beacon_Write_Start; // Indicate the sector address where next write operation of beacon packet will take place
//GLOBAL uint32_t Beacon_Read_Start;  // Indicate the sector address from where next transmission of beacon packet will take place
//GLOBAL uint32_t SciC_Write_Start;   // Indicate the sector address where next write operation of SciC packet will take place
//GLOBAL uint32_t SciC_Read_Start;    // Indicate the sector address from where next transmission of SciC packet will take place
//GLOBAL uint32_t SciD_Write_Start;   // Indicate the sector address where next write operation of SciD packet will take place
//GLOBAL uint32_t SciD_Read_Start;    // Indicate the sector address from where next transmission of SciD packet will take place
//GLOBAL uint32_t ADCS_Write_Start;   // Indicate the sector address where next write operation of ADCS packet will take place
//GLOBAL uint32_t ADCS_Read_Start;    // Indicate the sector address from where next transmission of ADCS packet will take place
//GLOBAL uint32_t ADCS_Image_Write_Start; // Indicate the sector address where next write operation of star tracker will take place
//GLOBAL uint32_t ADCS_Image_Read_Start;  // Indicate the sector address from where next transmission of star tracker will take place
//GLOBAL uint32_t Log_Write_Start;    // Indicate the sector address where next write operation of Log packet will take place
//GLOBAL uint32_t Log_Read_Start; // Indicate the sector address from where next transmission of Log packet will take place
//GLOBAL uint32_t HK_Write_Start; // Indicate the sector address where next write operation of HK packet will take place
//GLOBAL uint32_t HK_Read_Start;  // Indicate the sector address from where next transmission of HK packet will take place

//GLOBAL uint8_t Current_SD;  // Indicate which SD card to use


GLOBAL uint64_t statusG; 	// a global Variable for looking at the return values of various functions. It is used only in main.c file (See main.c file)

GLOBAL uint8_t GS_Pipeline_CMD[238];    // A global array to store the command from the ground station which is to be forwarded to a subsystem

GLOBAL uint8_t Transmit_MSG[2][512];   // Two global array which act like buffers for the packet to be transmitted. Any data which has to be transmitted either over UHF 
										// or the SBand has to be put into this buffer in appropriate manner. The function Manage_Data_Transmission() in main.c takes care of it.




/********************************* Global Variable Ends ***********************************************/


void main_seq();

void Init();

void Get_Beacon_Packet();

void Beacon_Packet_UART_log();

void Store_Beacon_Data();

void Beacon_Transmit_Timer_Handler();

void Watchdog_Timer_Handler();

void Watchdog_Pet(void);

void SC_Restart();

void RTC_Sync_Timer_Hanlder();

void Decide_Mode();

void Handle_Power_Supply();

void Manage_Data_Transmission();

void Handle_Transmit();

void Utils_SPI_Custom_Setup(mss_spi_instance_t * this_spi, uint16_t frame_count);
uint16_t Utils_SPI_Transmit_Block(mss_spi_instance_t * this_spi, uint8_t * cmd_buffer, uint16_t cmd_byte_size);
uint16_t Utils_SPI_Receive_Block(mss_spi_instance_t * this_spi, uint8_t * rec_buffer, uint16_t rec_byte_size);

#pragma pack(1)


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
void main_seq();	// The function 

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

/*------------------------Task function's definitions------------------------*/

/*---------------------------------------------------------------------------*/
/* The Last_Command_Timer_Handler() manages the timer for the last command 
 * received. Once the timer expires it does the spacecraft restart.
 *
 * @param
 * 	No parameter
 *
 * @return: void
 *
*/

void Beacon_Timer_Handler();

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
void Store_Beacon_Data();

/*---------------------------------------------------------------------------*/
/* The Store_Beacon_Data() manages the Beacon transmit timer. Once this timer
 * expires the request to transmit the beacon packet is put and the timer is 
 * restarted.
 *
 * @param
 * 	No parameter
 *
 * @return: void
 *
*/
void Beacon_Transmit_Timer_Handler();

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
void Watchdog_Timer_Handler();

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

void RTC_Sync_Timer_Hanlder();

/*---------------------------------------------------------------------------*/
/* The Decide_Mode() decides the mode of the spacecraft for the next cycle. This
 * is called either at the beginning or at the end of the sequence of operation.
 * This takes the account of the battery SOC, eclipse/sun, mode avoidance and 
 * force mode to decide the next mode of the spacecraft.
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
/* The Handle_Power_Supply() manages the power supply to different subsystem
 * depending on the mode of the spacecraft. This is called after the Decide_mode()
 * function.
 *
 * @param
 * 	No parameter
 *
 * @return: void
 *
*/
void Handle_Power_Supply();

/*---------------------------------------------------------------------------*/
/* The Manage_Data_Transmission() manages the transmission of data over different
 * band. Depending on the request the data is transmitted and the request is cleared.
 * At present, the priority order is as follows:
 * 1) SD card data
 * 2) Global packet transmission
 * 3) Parameter packet transmission
 * 4) Subsystem response packet transmission
 * 5) Beacon packet transmission
 *
 * As per above order, if there is a request for SD card data transmission (any data
 * from sd data, science, beacon, log etc..), also there is a request for Parameter
 * packet transmission, and also beacon packet transmission then only after completing
 * the SD card data transmission the Parameter packet transmission occurs and only after
 * Parameter packet transmission is over then the beacon packet transmission occurs.
 *
 * @param
 * 	No parameter
 *
 * @return: void
 *
*/
void Manage_Data_Transmission();

/*---------------------------------------------------------------------------*/
/* The Handle_Transmit() is responsible for transferring data to the 
 * appropriate module of the flight software (UHF or SBand) and starts the module
 * to transmit data.
 *
 * @param
 * 	No parameter
 *
 * @return: void
 *
*/
void Handle_Transmit();

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



struct CCSDS_Header{	// The CCSDS header struct
        uint8_t Version_ID;
        uint8_t APID;
        uint16_t Seq_no;
        uint16_t PL;
        uint8_t TS2;    /* 1 lSB byte */
        uint32_t TS1;   /* 4 byte of MSB */
};

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

typedef enum BeaconVMEL6075PacketEnum {
	VMEL6075_UVA_DATA = 0,
	VMEL6075_UVB_DATA,
	VMEL6075_UVCOMP1_DATA,
	VMEL6075_UVCOMP2_DATA,

}BeaconVMEL6075Packet;


struct Beacon_packet_IS0{
	//To Add CCSDS Header
    struct CCSDS_Header beac_head;	// The CCSDS header

    //To Add CDH HK Data
    //uint8_t CDH_Subsystem_ON_Stat;
    //uint16_t CDH_Temp;
    //uint8_t CDH_Primary_Info;
    //uint8_t CDH_Sub_System_Data_Valid_Flags;

    //EPS HK Data
    uint16_t EPS[32];

    //Sensor Board VMEL Data
    uint16_t Sensor_Board_VMEL6075[4];
    //TODO: Update the hydra code to combine DATA_L and DATA_H

    //Sensor Board AS7265X Data
    uint8_t Sensor_Board_AS7265X[36];

    //To Add the Fletcher Code
    uint16_t Fletcher_code;


} Beacon_pack_IS0;




typedef struct Parameter_Table 	// The stuct definition for all the variables which are in the parameter table.
{
	//Simply modified the IS1 Parameter table - Removed all the variables that seemed to be useless for IS0
	//Currently these parameters are not used in IS0, need to
    uint8_t Param_Current_SD;
    /* Timer data */
    uint32_t Param_Beac_Timer_Time;
    uint32_t Param_Beac_Trans_Timer_Time;
    uint16_t Param_Watchdog_Signal_Timer_Time;
    uint32_t Param_RTC_Sync_Timer_Time;

    /* Counters */
    uint32_t Param_Boot_Up_Counter;

    /* Flags */
    uint8_t Param_UHF_Malfunction_Flags;
    uint8_t Param_ADCS_Malfunction_Flags;

    /* Limits */
    uint8_t Param_Flash_SPI_Tries_Limit;
    uint16_t Param_Flash_SPI_Wait_Limit;
    uint8_t Param_Non_Response_Count_Limit;

    /* Thresholds */
    uint16_t Param_Phoe_Safe_Threshold;
    uint16_t Param_Safe_Phoe_Threshold;
    uint16_t Param_Safe_Char_Threshold;
    uint16_t Param_Char_Safe_Threshold;
    uint16_t Param_Char_SciC_Threshold;
    uint16_t Param_SciC_Char_Threshold;
    uint16_t Param_Char_SciD_Threshold;
    uint16_t Param_SciD_Char_Threshold;


    /* SD card data */
    uint32_t Param_Beacon_Sector_Start;
    uint32_t Param_Beacon_Sector_End;


    /* Write and transmit data indicators */
    uint32_t Param_Beacon_Write_Start;
    uint32_t Param_Beacon_Read_Start;

    /* Persistent commands data*/
    uint8_t Param_CMD_Forced_Mode;
    uint32_t Param_Force_Mode_Timer_Time;
    uint32_t Param_Force_Mode_Update_Rate_Time;

    uint16_t Param_EPS_CMD_Timer_Time;
    uint16_t Param_Fletcher_code;

} Param_Table_t;


struct CCSDS_Max_Data_size_Packet	// Definition of a general packet containing the maximum data size array
{
    struct CCSDS_Header ccsds_head;	// the ccsds header 

    uint8_t data[CCSDS_MAX_DATA_SIZE];	// the array with maximum data size allowed

    uint16_t Fletcher_code;
};

typedef struct CCSDS_Max_Data_size_Packet Param_table_packet1_t;	// typedef the CCSDS Max size packet as the Paramter table packet1

typedef struct Parameter_Table_Packet{ // the struct definition of the second parameter table packet

    struct CCSDS_Header param_head;	// the CCSDS header

    uint8_t data[sizeof(Param_Table_t)];

    uint16_t Fletcher_code;

} Param_table_packet2_t;



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

typedef struct 	// a small struct for the managing the less complex state machines.
{
    uint16_t CMD_Seq_Count;
    uint16_t Prev_CMD_Seq_Count;
    uint16_t Response_Length;
    uint16_t Response_Read;
} Module_Sync_Small_t;


GLOBAL struct Global_Variables { 	// Struct containing all the global variables
   uint8_t                     	Forced_Mode_Flag;
   Timer32_t                   	SCIC_SCID_Timer;
   Timer32_t					Safe_Pheonix_Timer;
   Timer32_t                   	Force_Mode_Update_Rate;
   Timer16_t                   	DAXSS_Sci_Packet_Timer;
   uint8_t                     	I2C_Error_Flag;
   uint8_t                     	Sat_Curr_Mode;
   uint8_t                     	Sat_Avoid_Mode;
   uint32_t                    	Timers_Started_Flag;
   uint32_t                    	Indication_Flags;
   uint16_t                    	Beacon_Packet_Seq_Counter;
   uint16_t                    	Science_Packet_Seq_Counter;
   Timer32_t                   	Beac_Timer;
   Timer32_t                   	Beac_Trans_Timer;
   Timer16_t                   	Watchdog_Signal_Timer;
   Timer32_t                   	RTC_Sync_Timer;
   uint8_t                     	Non_Response_Count_Limit;
   uint8_t                     	Subsystem_Restart_Limit;
   uint16_t                    	Phoe_Safe_Threshold;
   uint16_t                    	Safe_Phoe_Threshold;
   uint16_t                    	Safe_Char_Threshold;
   uint16_t                    	Char_Safe_Threshold;
   uint16_t                   	Char_SciD_Threshold;
   uint16_t                    	SciD_Char_Threshold;
   uint16_t                    	Char_SciC_Threshold;
   uint16_t                    	SciC_Char_Threshold;
   uint8_t                     	Flash_SPI_Tries_Limit;
   uint8_t                     	CMD_Persistant_Flags;
   uint32_t                    	Boot_Up_Counter;
   uint32_t                    	Beacon_Sector_Start;
   uint32_t                    	Beacon_Sector_End;
   uint32_t                    	Beacon_Write_Start;
   uint32_t                    	Beacon_Read_Start;
   uint32_t                    	HK_Read_Start;
   uint8_t                     	PWR_I2C_Error_Count[3];
   uint16_t                    	PWR_Fuel_Guage_Data[3];
   uint16_t                    	Flash_SPI_Wait_Limit;
   uint8_t                     	SD_Clk_Div;
   uint8_t                     	Current_SD;
   uint16_t 					Globals_Table_Packet_Seq_Counter;
   Module_Sync_Small_t			SD_FDRI_Module_Sync;
} Globals;


typedef struct CCSDS_Max_Data_size_Packet Global_packet_Full_t;	// Tyepdef of the CCSDS Max size packet as global packet 1
typedef struct Global_Table_Packet{	// Struct definition of last global packet 

    struct CCSDS_Header global_head;	// the CCSDS header

    uint8_t data[sizeof(Globals)];

    uint16_t Fletcher_code;

} Global_packet_Last_t;


#endif /* MAIN_SCH_H_ */
