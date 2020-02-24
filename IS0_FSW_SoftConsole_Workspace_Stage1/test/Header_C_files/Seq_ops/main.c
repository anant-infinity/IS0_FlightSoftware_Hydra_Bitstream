
#ifndef _MAIN_SEQ_C
    #define _MAIN_SEQ_C

#include <EPS/eps.h>
#include <PSLV_UART/pslv_uart.h>
#include "Seq_ops/main_seq.h"
#include "CCSDS/ccsds.h"
#include "SD_CARD/sd.h"
#include "RTC/rtc.h"
#include "GPIOs/gpios.h"
#include "Utils/utils.h"
#include "string.h"
#include "Test/test.h"
#include "../../CMSIS/mss_assert.h"
#include "SPI_Flash/spi_flash.h"
#include "SENSOR_BOARD/sensor_board.h"
#include "drivers/mss_uart/mss_uart.h"
#include <stdlib.h>

int main(){

	//	Running the Sequence of Operations
	main_seq();

	//test funtion for testing the SD card
	//test_SD_All_Sectors(0, 128);

}


void main_seq(){

	//Call the Init() function to initialize all the peripherals and RTC
	Init();

	//Starting the two mode switching timers
	Globals.SCIC_SCID_Timer.Start = RTC_Get_Value64();
	Globals.Safe_Pheonix_Timer.Start = RTC_Get_Value64();

	//Starting the timer to get beacon data
	Globals.Beac_Timer.Start = RTC_Get_Value64();

	//Initializing the current satellite mode to Science D
	Globals.Sat_Curr_Mode = SC_SCID_MODE;
	SwitchTo_Mode_Science_D();


	while(1){

		//Calling the Get Beacon Packet function to collect the data from the EPS and Sensor Boards
		//and storing in Beacon packet structure
		Get_Beacon_Packet();

		//Storing the beacon packet in the SD Card
		//TODO: This function is taken as is from IS1 - need to check the working of the same
		//If SD Card fails for any reason, it should skip
		//It should aslo increment write pointer
		//Store_Beacon_Data();


		//At this stage the latest beacon data is in the Beacon_Packet_Array
		//It is also stored in the SD CARD
		//If the SDCARD read pointer is increased or some flag is set, the next sector packet written
		//should be read from the SD card. This then should be loaded into the FIFO next time

		//This function checks if the FIFO of the PSLV UART needs to be filled.
		//If TRUE it fills the FIFO
		Check_and_Fill_FIFO();

		//TODO: Find the exact need of this function
		//SD_FDRI();

		//Call the Decide mode function to Apply the forced simulated modes for IS0
		Decide_Mode();

	}
}

void Init(){

	//Initializing Global Variables
	Globals.Indication_Flags = 0;
    Globals.Flash_SPI_Tries_Limit = 5;
    Globals.Current_SD = MSS_SPI_SLAVE_1;
    Globals.Non_Response_Count_Limit = 20;
    Globals.Forced_Mode_Flag = 0x00;

    // 45 minutes is 0xA0EEBB00
    // 10 seconds is 10 000 000 = 0x989680
    //Globals.SCI_SCID_Thershold_Time = (uint64_t)0x989680;

    // 5 days is 0x‭649534E000
    // 15 seconds is 15 000 000 = 0xE4E1C0
    //Globals.Safe_Pheonix_Threshold_Time = (uint64_t)0xE4E1C0;


	//Initialize the RTC if required
	int i =  RTC_Enable();

	Load_Parameter_Table();
	Globals.Indication_Flags |= SAVE_PARAMETER_MAP_FLAG;
	//Increment the Boot Up Counter
	Globals.Boot_Up_Counter++;

	//Initialize all the GPIOs
	Global_Init_GPIOs();
	//Initialize EPS I2C Channels
	Initialize_EPS_I2C_Channels();
	//Initialize Sensor Board I2C Channel
	Initialize_Sensor_Board_I2C_Channel();

	//Initialize the Flash Memory
	Flash_Init();

	//Reset the FIFO of the PSLV UART
	Reset_FIFO_SM();

	//Initialize the SD Cards
	//TODO: This fucntion is taken directly from IS1 - Test the working of this function
	//SD_Cards_Init();
	//SD_FDRI_Init();


}


//This function collects the Beacon Data from CDH, EPS, Sensor Board every one second.
void Get_Beacon_Packet(){

	//Run the loop once per millisecond
	Globals.Beac_Timer.Time = RTC_Get_Value64();
	if(Globals.Beac_Timer.Time - Globals.Beac_Timer.Start > (uint64_t)0x3E8){
		//Collect the data every one second
		if((Globals.Beac_Timer.Time - Globals.Beac_Timer.Start)>Globals.Beac_Thershold_Time){

			//Get CDH Data
			Get_CDH_Data();

			//Get EPS Data
			Get_EPS_Data();

			//Get VMEL Sensor Board Data
			Get_VEML6075_Data();

			//MSS_GPIO_set_output( GPIO_EN_BUS_TRAN, 1);

			//Get triad sensor data

			Get_AS7265x_Data();

			//MSS_GPIO_set_output( GPIO_EN_BUS_TRAN, 0);


			//Making the beacon packet into CCSDS Format
		    CCSDS_Pack( BEACON_PACKET_APID, 0xC0, Globals.Beacon_Packet_Seq_Counter, (struct CCSDS_Header *)&(Beacon_pack_IS0.beac_head), sizeof(Beacon_pack_IS0));
		    CCSDS_Fletcher_16((uint8_t *)(&Beacon_pack_IS0), sizeof(Beacon_pack_IS0) - 2);

		    //Incrementing the Beacon Packet Sequence Counter
		    Globals.Beacon_Packet_Seq_Counter++;

		    //for ground testing
		    Beacon_Packet_UART_log();
		    //Make the beacon Packet Array
		    Make_Beacon_Packet_Array();
		    Globals.Beac_Timer.Start = RTC_Get_Value64();
		}
	}
}

// This function converts the beacon packet into an array where every entry is a 8 bit value
// This array is then used to pass to the FIFO of the PSLV telemetry UART.
void Make_Beacon_Packet_Array(){


	uint8_t j = 0;
	//CCSDS Header
	Beacon_Pack_Array[j++] = Beacon_pack_IS0.beac_head.Version_ID;
	Beacon_Pack_Array[j++] = Beacon_pack_IS0.beac_head.APID;

	Beacon_Pack_Array[j++] = Beacon_pack_IS0.beac_head.Seq_no >> 8;
	Beacon_Pack_Array[j++] = Beacon_pack_IS0.beac_head.Seq_no & 0x00FF;

	Beacon_Pack_Array[j++] = Beacon_pack_IS0.beac_head.PL >> 8;
	Beacon_Pack_Array[j++] = Beacon_pack_IS0.beac_head.PL & 0x00FF;

	Beacon_Pack_Array[j++] = Beacon_pack_IS0.beac_head.TS2;

	Beacon_Pack_Array[j++] = ((Beacon_pack_IS0.beac_head.TS1 >> 24) & 0xFF);
	Beacon_Pack_Array[j++] = ((Beacon_pack_IS0.beac_head.TS1 >> 16) & 0xFF);
	Beacon_Pack_Array[j++] = ((Beacon_pack_IS0.beac_head.TS1 >> 8) & 0xFF);
	Beacon_Pack_Array[j++] = (Beacon_pack_IS0.beac_head.TS1 & 0xFF);


	//Get the CDH Data
	for(int i=0; i<sizeof(Beacon_pack_IS0.CDH_8); ++i){

		Beacon_Pack_Array[j++] = Beacon_pack_IS0.CDH_8[i];

	}

	for(int i=0; i<(sizeof(Beacon_pack_IS0.CDH_32)/4); ++i){

		Beacon_Pack_Array[j++] = ((Beacon_pack_IS0.CDH_32[i] >> 24) & 0xFF);
		Beacon_Pack_Array[j++] = ((Beacon_pack_IS0.CDH_32[i] >> 16) & 0xFF);
		Beacon_Pack_Array[j++] = ((Beacon_pack_IS0.CDH_32[i] >> 8) & 0xFF);
		Beacon_Pack_Array[j++] = (Beacon_pack_IS0.CDH_32[i] & 0xFF);

	}

	for(int i=0; i<(sizeof(Beacon_pack_IS0.CDH_64)/8); ++i){

		Beacon_Pack_Array[j++] = ((Beacon_pack_IS0.CDH_64[i] >> 56) & 0xFF);
		Beacon_Pack_Array[j++] = ((Beacon_pack_IS0.CDH_64[i] >> 48) & 0xFF);
		Beacon_Pack_Array[j++] = ((Beacon_pack_IS0.CDH_64[i] >> 40) & 0xFF);
		Beacon_Pack_Array[j++] = ((Beacon_pack_IS0.CDH_64[i] >> 32) & 0xFF);
		Beacon_Pack_Array[j++] = ((Beacon_pack_IS0.CDH_64[i] >> 24) & 0xFF);
		Beacon_Pack_Array[j++] = ((Beacon_pack_IS0.CDH_64[i] >> 16) & 0xFF);
		Beacon_Pack_Array[j++] = ((Beacon_pack_IS0.CDH_64[i] >> 8) & 0xFF);
		Beacon_Pack_Array[j++] = (Beacon_pack_IS0.CDH_64[i] & 0xFF);

	}

	//Get the EPS Data
	for(int i=0; i<(sizeof(Beacon_pack_IS0.EPS)/2); ++i){

		Beacon_Pack_Array[j++] = Beacon_pack_IS0.EPS[i] >> 8;
		Beacon_Pack_Array[j++] = Beacon_pack_IS0.EPS[i] & 0x00FF;

	}

	//VMEL6075 Data
	for(int i=0; i<(sizeof(Beacon_pack_IS0.Sensor_Board_VMEL6075)/2); ++i){

		Beacon_Pack_Array[j++] = Beacon_pack_IS0.Sensor_Board_VMEL6075[i] >> 8;
		Beacon_Pack_Array[j++] = Beacon_pack_IS0.Sensor_Board_VMEL6075[i] & 0x00FF;

	}

	//AS765X Data
	for(int i=0; i<sizeof(Beacon_pack_IS0.Sensor_Board_AS7265X); ++i){

		Beacon_Pack_Array[j++] = Beacon_pack_IS0.Sensor_Board_AS7265X[i] ;

	}

	//CCSDS Fletcher Code
	Beacon_Pack_Array[j++] = Beacon_pack_IS0.Fletcher_code >> 8;
	Beacon_Pack_Array[j++] = Beacon_pack_IS0.Fletcher_code & 0x00FF;


}

void Get_CDH_Data(){

	int index_8 = 0;
	Beacon_pack_IS0.CDH_8[index_8++] = Globals.Sat_Curr_Mode;
	Beacon_pack_IS0.CDH_8[index_8++] = Globals.I2C_Error_Flag;
	Beacon_pack_IS0.CDH_8[index_8++] = Globals.Current_SD;
	int index_32 = 0;
	Globals.Time_in_msec = RTC_Get_ms();
	Beacon_pack_IS0.CDH_32[index_32++] = Globals.Time_in_msec;
	Beacon_pack_IS0.CDH_32[index_32++] = Globals.Beacon_Read_Start;
	Beacon_pack_IS0.CDH_32[index_32++] = Globals.Beacon_Write_Start;
	int index_64 = 0;
	Beacon_pack_IS0.CDH_64[index_64++] = Globals.Safe_Pheonix_Threshold_Time;
	Beacon_pack_IS0.CDH_64[index_64++] = Globals.SCI_SCID_Thershold_Time;

}

void Store_Beacon_Data(){
  if (Globals.Current_SD == 0xff) {
	  return;
  }
  uint32_t * write_addr = &Globals.Beacon_Write_Start;
  uint16_t sync = SBAND_SYNC_WORD;
  uint16_t * sync_ptr = &sync;
  uint8_t to_write[sizeof(Beacon_pack_IS0)+SIZEOF_SYNC_WORD];
  memcpy(to_write, (uint8_t *)sync_ptr, SIZEOF_SYNC_WORD);
  memcpy(&to_write[SIZEOF_SYNC_WORD], &Beacon_pack_IS0, sizeof(Beacon_pack_IS0));
  uint32_t i = 0;
  uint8_t tries = 0;
  //try up to 4 times
  while((i < (*write_addr + 1)) && (tries < 4)) {
    i = SD_Write_Data(to_write, sizeof(Beacon_pack_IS0) + SIZEOF_SYNC_WORD, *write_addr, Globals.Current_SD);
    tries++;
  }
  if (tries < 4) {
    //update Write_Start if successful
    *write_addr = (i <= Globals.Beacon_Sector_End) ? i : Globals.Beacon_Sector_Start;
    //TODO: figure out when/how often to update Write_Start flag in flash -- being changed every second
    // how about every time we transmit beacon packet, we update parameter table.
//    if (Globals.Beacon_Packet_Seq_Counter % Beacon_Write_Start_Update_Cnt == 0) {
//    Globals.Indication_Flags |= SAVE_PARAMETER_MAP_FLAG;
//    }
  }
  else {
	  Globals.Current_SD = 0xff;
	  return;
  }

}

void Decide_Mode(){

	//Sequence of Forcing the modes for IS0.
	//The functions to switch mode are written in a way that switching from any mode to any other mode is possible

	//For IS0 mode decision is on the basis of time, following is the description:
	//1. Start in Science D
	//2. Switch between Science D and Science C every 45 minutes
	//3. On 5th day, force into safe mode for 5 orbits - repeat this after 10 days, i.e 5th, 15th, 25th and so on
	//4. On 10th day, force into phoenix mode for 5 orbits - repeat this after 10 days, i.e 10th, 20th, 30th and so on

	//Code to switch between Science C and Science D
	//Get the Current Time
	Globals.SCIC_SCID_Timer.Time = RTC_Get_Value64();
	//Make sure that the decide mode function is only run once per millisecond. 1 millisecond = 1000 microsec = 0x3E8
	if(Globals.SCIC_SCID_Timer.Time - Globals.SCIC_SCID_Timer.Start > (uint64_t)0x3E8){

		if((Globals.SCIC_SCID_Timer.Time - Globals.SCIC_SCID_Timer.Start)>Globals.SCI_SCID_Thershold_Time){//45 minutes in hexadecimal is 0x0A8C
				if(Globals.Sat_Curr_Mode == SC_SCID_MODE || Globals.Sat_Curr_Mode == SC_SAFE_MODE || Globals.Sat_Curr_Mode == SC_PHEONIX_MODE ){
					SwitchTo_Mode_Science_C();
					Globals.Sat_Curr_Mode = SC_SCIC_MODE;
					Globals.SCIC_SCID_Timer.Start = RTC_Get_Value64();
				}
				else{
					SwitchTo_Mode_Science_D();
					Globals.Sat_Curr_Mode = SC_SCID_MODE;
					Globals.SCIC_SCID_Timer.Start = RTC_Get_Value64();
				}
			}

			//Code to switch to Safe or Pheonix
			//If there is an intersection of time between safe or pheonix mode and SCI_C or SCI_D mode
			//Safe or Pheonix mode will dominate.

			Globals.Safe_Pheonix_Timer.Time = RTC_Get_Value64();
			if((Globals.Safe_Pheonix_Timer.Time - Globals.Safe_Pheonix_Timer.Start)>Globals.Safe_Pheonix_Threshold_Time){//5 days in hexadecimal is 0x069780

				if(Globals.Forced_Mode_Flag == 0x00){
					SwitchTo_Mode_Safe();
					Globals.Sat_Curr_Mode = SC_SAFE_MODE;
					Globals.Forced_Mode_Flag = 0x01;
					Globals.Safe_Pheonix_Timer.Start = RTC_Get_Value64();
				}
				else{
					SwitchTo_Mode_Pheonix();
					Globals.Sat_Curr_Mode = SC_PHEONIX_MODE;
					Globals.Forced_Mode_Flag = 0x00;
					Globals.Safe_Pheonix_Timer.Start = RTC_Get_Value64();
				}
			}
	}



}

void SwitchTo_Mode_Science_D(){
	//In science D - DAXSS, SBAND, ADCS, Battery Heater are ON, CIP is OFF
	//This is the mode we star in so turn ON everything except CIP first
	PWR_Switch((GPIO_DSW0_DAXSS_Mask|GPIO_DSW2_ADCS_Mask|GPIO_DSW3_Sband_Mask|GPIO_DSW4_Bat_heat_Mask),1);
	//Turn OFF CIP
	PWR_Switch(GPIO_DSW1_CIP_Mask,0);
}

void SwitchTo_Mode_Science_C(){
	//In science D - CIP, SBAND, ADCS, Battery Heater are ON, DAXSS is OFF
	//turn ON everything except DAXSS first
	PWR_Switch((GPIO_DSW1_CIP_Mask|GPIO_DSW2_ADCS_Mask|GPIO_DSW3_Sband_Mask|GPIO_DSW4_Bat_heat_Mask),1);
	//Turn OFF DAXSS
	PWR_Switch(GPIO_DSW0_DAXSS_Mask,0);

}

void SwitchTo_Mode_Safe(){
	//In Safe mode all the GPIOs Distribution Switches (Subsytem Enables) are OFF, EXCEPT ADCS
	//Turning OFF everything except ADCS
	PWR_Switch((GPIO_DSW0_DAXSS_Mask|GPIO_DSW1_CIP_Mask|GPIO_DSW3_Sband_Mask|GPIO_DSW4_Bat_heat_Mask),0);
	//Turning ON ADCS
	PWR_Switch(GPIO_DSW2_ADCS_Mask,1);
}

void SwitchTo_Mode_Pheonix(){
	//In Pheonix mode all the GPIOs Distribution Switches (Subsytem Enables are OFF)
	//Turning OFF everything
	PWR_Switch((GPIO_DSW0_DAXSS_Mask|GPIO_DSW1_CIP_Mask|GPIO_DSW2_ADCS_Mask|GPIO_DSW3_Sband_Mask|GPIO_DSW4_Bat_heat_Mask),0);
}


void Beacon_Packet_UART_log(){

	uint16_t EPS_array_1[8];
	uint16_t EPS_array_2[8];
	uint16_t EPS_array_3[8];
	uint16_t EPS_array_4[8];
	uint16_t EPS_array_5[2];

	uint8_t AS7265X_array_1[16];
	uint8_t AS7265X_array_2[16];
	uint8_t AS7265X_array_3[4];

	//Initializing the MSS UART for Logging
	MSS_UART_init(&g_mss_uart0,MSS_UART_115200_BAUD,MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY | MSS_UART_ONE_STOP_BIT);

	//Collecting the Data in Arrays to be printed
	uint8_t CCSDS_header_array_1[2] = {Beacon_pack_IS0.beac_head.Version_ID, Beacon_pack_IS0.beac_head.APID };
	uint16_t CCSDS_header_array_2[2] = {Beacon_pack_IS0.beac_head.Seq_no, Beacon_pack_IS0.beac_head.PL};

	uint8_t CCSDS_header_array_3[1] = {Beacon_pack_IS0.beac_head.TS2};
	uint32_t CCSDS_header_array_4[1] = {Beacon_pack_IS0.beac_head.TS1};


	uint16_t CCSDS_fletcher_array[] = {Beacon_pack_IS0.Fletcher_code};

	//Dividing the EPS Data into arrays of 16 Bytes, Since MSS_UART peripheral cannot handle more than 16 bytes
	for(int i = 0; i < 8; ++i){
		EPS_array_1[i] = Beacon_pack_IS0.EPS[i];
	}
	for(int i = 0; i < 8; ++i){
		EPS_array_2[i] = Beacon_pack_IS0.EPS[i+8];
	}
	for(int i = 0; i < 8; ++i){
		EPS_array_3[i] = Beacon_pack_IS0.EPS[i+16];
	}
	for(int i = 0; i < 8; ++i){
		EPS_array_4[i] = Beacon_pack_IS0.EPS[i+24];
	}
	for(int i = 0; i < 2; ++i){
		EPS_array_5[i] = Beacon_pack_IS0.EPS[i+32];
	}


	for(int i = 0; i < 16; ++i){
		AS7265X_array_1[i] = Beacon_pack_IS0.Sensor_Board_AS7265X[i];
	}
	for(int i = 0; i < 16; ++i){
		AS7265X_array_2[i] = Beacon_pack_IS0.Sensor_Board_AS7265X[i+16];
	}
	for(int i = 0; i < 4; ++i){
		AS7265X_array_3[i] = Beacon_pack_IS0.Sensor_Board_AS7265X[i+32];
	}

	//Printing the data ussing the MSS UART which can print a maximum of 16 bytes at a time
	//Printing the CCSDS Header
	MSS_UART_polled_tx( &g_mss_uart0, (uint8_t*)CCSDS_header_array_1, sizeof(CCSDS_header_array_1) );
	MSS_UART_polled_tx( &g_mss_uart0, (uint8_t*)CCSDS_header_array_2, sizeof(CCSDS_header_array_2) );
	MSS_UART_polled_tx( &g_mss_uart0, (uint8_t*)CCSDS_header_array_3, sizeof(CCSDS_header_array_3) );
	MSS_UART_polled_tx( &g_mss_uart0, (uint8_t*)CCSDS_header_array_4, sizeof(CCSDS_header_array_4) );

	//Printing the CDH Data
	MSS_UART_polled_tx( &g_mss_uart0, (uint8_t*)Beacon_pack_IS0.CDH_8, sizeof(Beacon_pack_IS0.CDH_8) );
	MSS_UART_polled_tx( &g_mss_uart0, (uint8_t*)Beacon_pack_IS0.CDH_32, sizeof(Beacon_pack_IS0.CDH_32) );
	MSS_UART_polled_tx( &g_mss_uart0, (uint8_t*)Beacon_pack_IS0.CDH_64, sizeof(Beacon_pack_IS0.CDH_64) );

	//Printing the EPS Data
	MSS_UART_polled_tx( &g_mss_uart0, (uint8_t*)EPS_array_1, sizeof(EPS_array_1));
	MSS_UART_polled_tx( &g_mss_uart0, (uint8_t*)EPS_array_2, sizeof(EPS_array_2));
	MSS_UART_polled_tx( &g_mss_uart0, (uint8_t*)EPS_array_3, sizeof(EPS_array_3));
	MSS_UART_polled_tx( &g_mss_uart0, (uint8_t*)EPS_array_4, sizeof(EPS_array_4));
	MSS_UART_polled_tx( &g_mss_uart0, (uint8_t*)EPS_array_5, sizeof(EPS_array_5));

	//Printing the Sensor_Board_VMEL6075 Data
	MSS_UART_polled_tx( &g_mss_uart0, (uint8_t*)Beacon_pack_IS0.Sensor_Board_VMEL6075, sizeof(Beacon_pack_IS0.Sensor_Board_VMEL6075) );

	//Printing the Sensor_Board_AS7265X Data
	MSS_UART_polled_tx( &g_mss_uart0, AS7265X_array_1, sizeof(AS7265X_array_1));

	//Printing the Sensor_Board_AS7265X Data
	MSS_UART_polled_tx( &g_mss_uart0, AS7265X_array_2, sizeof(AS7265X_array_2));

	//Printing the Sensor_Board_AS7265X Data
	MSS_UART_polled_tx( &g_mss_uart0, AS7265X_array_3, sizeof(AS7265X_array_3));

	//Printing the CCSDS Fletcher Code
	MSS_UART_polled_tx( &g_mss_uart0, (uint8_t*)CCSDS_fletcher_array, sizeof(CCSDS_fletcher_array) );

}



void Watchdog_Timer_Handler(){
    if(Utils_Handle_Timer16(&Globals.Watchdog_Signal_Timer, WATCHDOG_SIGNAL_TIMER_FLAG)){
        Watchdog_Pet();
    }
}

void Watchdog_Pet() {
	uint32_t k = (MSS_GPIO_get_outputs() & GPIO_RESET_IC_Mask);
	//toggle k
	k = k == 0;
	MSS_GPIO_set_output(GPIO_RESET_IC, k);
}



void SC_Restart(){
   uint8_t i = 0, j = 0;
   for(i = 0; i < 10; i++){
       j = 0;
       MSS_GPIO_set_output(GPIO_RESET_IC, 0);
       while(j < SC_REBOOT_SIGNAL_DELAY){
           j++;
       }
       MSS_GPIO_set_output(GPIO_RESET_IC, 1);
   }
}


void Utils_SPI_Custom_Setup(mss_spi_instance_t * this_spi, uint16_t frame_count){
    ASSERT((this_spi == &g_mss_spi0) || (this_spi == &g_mss_spi1));

    /* This function is only intended to be used with an SPI master. */
    ASSERT((this_spi->hw_reg->CONTROL & CTRL_MASTER_MASK) == CTRL_MASTER_MASK);

    /* Flush the Tx and Rx FIFOs. */
    this_spi->hw_reg->COMMAND |= (TX_FIFO_RESET_MASK | RX_FIFO_RESET_MASK);

    /* Set frame size and the frame count .*/
    this_spi->hw_reg->CONTROL &= ~(uint32_t)CTRL_ENABLE_MASK;
    this_spi->hw_reg->CONTROL = (this_spi->hw_reg->CONTROL & ~TXRXDFCOUNT_MASK) | ( (frame_count << TXRXDFCOUNT_SHIFT) & TXRXDFCOUNT_MASK);
    this_spi->hw_reg->TXRXDF_SIZE = MAX_FRAME_LENGTH;
    this_spi->hw_reg->CONTROL |= CTRL_ENABLE_MASK;

}


/* This function is suppose to be in Utils.c file but something is going wrong with it in when kept in the file
 * hence it is kept here.*/
uint16_t Utils_SPI_Transmit_Block(mss_spi_instance_t * this_spi, uint8_t * cmd_buffer, uint16_t cmd_byte_size){
    if(cmd_byte_size == 0)
        return 0;

    uint16_t transfer_idx = 0u;
    int16_t tx_idx;
    uint16_t c;
    uint32_t frame_count;
    uint32_t tx_fifo_empty;

    Utils_SPI_Custom_Setup(this_spi, cmd_byte_size);

    tx_idx = 0u;c = 0;
    /* Perform the remainder of the transfer by sending 4 bytes at a time.
     * This should ensure that no Rx overflow can happen in
     * case of an interrupt occurs during this function. */
    while(transfer_idx < cmd_byte_size)
    {

        tx_fifo_empty = this_spi->hw_reg->STATUS & TX_FIFO_EMPTY_MASK;
        if(TX_FIFO_EMPTY_MASK == tx_fifo_empty)
        {
            tx_idx = cmd_byte_size - transfer_idx;
            /* Send another 4 bytes. */
            if(tx_idx >= MAX_FRAME_LENGTH/8)
            {
                this_spi->hw_reg->TX_DATA = Utils_Buffer_to_32_ValueS_Big(&cmd_buffer[transfer_idx]);
                transfer_idx += 4;
            }
            else
            {
                this_spi->hw_reg->CONTROL &= ~(uint32_t)CTRL_ENABLE_MASK;
                this_spi->hw_reg->TXRXDF_SIZE = MSS_SPI_BLOCK_TRANSFER_FRAME_SIZE;
                this_spi->hw_reg->CONTROL |= CTRL_ENABLE_MASK;

                this_spi->hw_reg->TX_DATA = cmd_buffer[transfer_idx];
                ++transfer_idx;
            }
            c=0;
            continue;
        }
        c++;
        if(c > SPI_ESCAPE_LIMIT){
            break;
        }

    }
    return transfer_idx;

}

/* This function is suppose to be in Utils.c file but something is going wrong with it in when kept in the file
 * hence it is kept here.*/
uint16_t Utils_SPI_Receive_Block(mss_spi_instance_t * this_spi, uint8_t * rec_buffer, uint16_t rec_byte_size){
    if(rec_byte_size == 0)
        return 0;

    uint16_t transfer_idx = 0u;
    int16_t tx_idx;
    uint16_t rx_idx;
    uint16_t c;
    uint32_t frame_count;
    uint32_t rx_fifo_empty, tx_fifo_full;uint32_t rx_raw;
    int16_t transit = 0u;

    Utils_SPI_Custom_Setup(this_spi, rec_byte_size);

    tx_idx = 0u; c = 0; rx_idx = 0;uint8_t comp = RX_FIFO_SIZE;
    /* Perform the remainder of the transfer by sending 4 bytes at a time.
     * This should ensure that no Rx overflow can happen in
     * case of an interrupt occurs during this function. */
    while(transfer_idx < rec_byte_size)
    {

        rx_fifo_empty = this_spi->hw_reg->STATUS & RX_FIFO_EMPTY_MASK;
        if(0u == rx_fifo_empty)
        {
            /* Process received byte. */
            rx_raw = this_spi->hw_reg->RX_DATA;

            if(rx_idx < rec_byte_size)
            {
                frame_count = rec_byte_size - rx_idx;
                if(frame_count < RX_FIFO_SIZE){
                    rec_buffer[rx_idx] = rx_raw  & 0xff;
                    ++rx_idx;

                    transfer_idx++;
                }else{
                    rec_buffer[rx_idx] = (rx_raw >> 24) & 0xff;
                    ++rx_idx;
                    rec_buffer[rx_idx] = (rx_raw >> 16) & 0xff;
                    ++rx_idx;
                    rec_buffer[rx_idx] = (rx_raw >> 8) & 0xff;
                    ++rx_idx;
                    rec_buffer[rx_idx] = rx_raw & 0xff;
                    ++rx_idx;
                    comp  = RX_FIFO_SIZE;

                    transfer_idx+=4;
                }
                transit = 0;
            }

        }

        tx_fifo_full = this_spi->hw_reg->STATUS & TX_FIFO_FULL_MASK;
        if(0u == tx_fifo_full)
        {
            if(transit == 0)
            {
                if(tx_idx < rec_byte_size)
                {
                    frame_count = rec_byte_size - tx_idx;
                    if(frame_count < RX_FIFO_SIZE){
                        this_spi->hw_reg->CONTROL &= ~(uint32_t)CTRL_ENABLE_MASK;
                        this_spi->hw_reg->TXRXDF_SIZE = MSS_SPI_BLOCK_TRANSFER_FRAME_SIZE;
                        this_spi->hw_reg->CONTROL |= CTRL_ENABLE_MASK;
                        tx_idx++;
                    }else{
                        tx_idx+=4;
                    }
                    transit = 1;
                    this_spi->hw_reg->TX_DATA = 0x00u;
                }
            }
        }
    }
    return transfer_idx;

}

uint32_t Store_Pkt(uint32_t addr, uint32_t size, uint8_t *pkt);

//FIXME! SD_Card FDRI
//TODO: Finish function, decide how often to try again and using what structure
//NOTE: on 6/18/19, this function was called (noticed only via breakpoint),
// and the system was able to recover, and keep incrementing Beaocn_Write_Start thereafter
//TODO: Add timer to parameter table as well as use timer for FDRI based on state
void SD_FDRI() {

    //No failures, so nothing to do
    if (Globals.SD_FDRI_Module_Sync.failCount == 0 ||
        Globals.Current_SD == 0xFF) {
        return;
    }

    //We have restarted too many times, so stop trying to fix it
    if (Globals.SD_FDRI_Module_Sync.restartCount > Globals.SD_FDRI_Module_Sync.restartLimit) {
        return;
    }

    //now we know we need to do something
    if (Globals.SD_FDRI_Module_Sync.failCount > Globals.SD_FDRI_Module_Sync.failThreshold) {

        Globals.SD_FDRI_Module_Sync.restartCount++;

//        Make_Log_packet(LOG_ID_SD_RESTART,
//                Globals.SD_FDRI_Module_Sync.failCount >> 8,
//                Globals.SD_FDRI_Module_Sync.failCount,
//                Globals.SD_FDRI_Module_Sync.failThreshold >> 8,
//                Globals.SD_FDRI_Module_Sync.failThreshold,
//                Globals.SD_FDRI_Module_Sync.restartCount >> 8,
//                Globals.SD_FDRI_Module_Sync.restartCount, 0);

        SD_Cards_Init();
        Globals.SD_FDRI_Module_Sync.failCount = 0;

    }

    //TODO request a s/c power cycle??
}

void SD_FDRI_Init() {

    Globals.SD_FDRI_Module_Sync.failCount = 0;
    Globals.SD_FDRI_Module_Sync.restartCount = 0;
    Globals.SD_FDRI_Module_Sync.failThreshold = SD_FAIL_THRESHOLD;
    Globals.SD_FDRI_Module_Sync.restartLimit = SD_RESTART_LIMIT;
}

uint32_t SD_Write_To_Sector(SD_Sector_t *sector, uint32_t size, uint8_t *pkt) {

    //Cannot write if both cards are failed
    if (Globals.Current_SD == 0xFF) return;

    uint32_t addr_to_write = sector->start + sector->write;

    uint32_t new_addr = Store_Pkt(addr_to_write, size, pkt);

    //Write did not succeed
    if (new_addr == addr_to_write) {
        Globals.SD_FDRI_Module_Sync.failCount++;
//        Make_Log_packet(LOG_ID_SD_WRITE_FAIL,
//                (uint8_t)(Globals.SD_FDRI_Module_Sync.failCount >> 8),
//                (uint8_t)(Globals.SD_FDRI_Module_Sync.failCount),
//                (uint8_t)(addr_to_write >> 24),
//                (uint8_t)(addr_to_write >> 16),
//                (uint8_t)(addr_to_write >> 8),
//                (uint8_t)(addr_to_write),
//                0);

        return 0;
    }

    // Rollover write pointer if we have reached the end of the sector
    if (new_addr > sector->end) {
      sector->write = 0;
//      Make_Log_packet(LOG_ID_SD_ROLLOVER,
//              (uint8_t)(sector->start >> 24),
//              (uint8_t)(sector->start >> 16),
//              (uint8_t)(sector->start >> 8),
//              (uint8_t)(sector->start),0,0,0);
    }
    else {
      sector->write++;
    }

    //TODO: signal parameter table to be updated?
    return 1;
}

uint32_t Store_Pkt(uint32_t addr, uint32_t size, uint8_t *pkt) {

    static uint8_t storeBuffer[512];
    memset(storeBuffer, 0, sizeof(storeBuffer));

    memcpy(storeBuffer, (const void * restrict) SBAND_SYNC_WORD, SIZEOF_SYNC_WORD);
    memcpy(&storeBuffer[SIZEOF_SYNC_WORD], pkt, size);

    uint32_t new_addr = addr, tries = 0;

    //try up to 4 times, SD_Write_Data will return the next address if the write was successful
    while((new_addr == addr) && (tries < SD_CARD_WRITE_NUM_TRIES)) {
      new_addr = SD_Write_Data(storeBuffer, size + SIZEOF_SYNC_WORD, addr, Globals.Current_SD);
      tries++;
    }
    if (tries != 4) {
      return new_addr;
    }

    //Otherwise, return old address so can try again
    return addr;
}

#endif
