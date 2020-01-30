
// All global variables and functions start with capital letter and separated by '_'.

#ifndef _MAIN_SEQ_C
    #define _MAIN_SEQ_C

#include <EPS/eps.h>
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



void do_nothing() {
	while(1);
}

uint32_t ret_val;
uint8_t sig = 0;
int main(){

//	test_ISO_GPIO_Subsystem_Enable_GPIOs();
//	test_ISO_PeakPower();
//	test_ISO_test_beacon_debug_UART();
//	test_IS0_Read_Sensor_Board_Data();

//	Running the Sequence of Operations
//	main_seq();


}


void main_seq(){

	//Call the Init() function to initialize all the peripherals
	Init();

	//Calling the Get Beacon Packet function to collect the data from the EPS and Sensor Boards
	//and storing in Beacon packet structure
	Get_Beacon_Packet();

	//For ground debugging log the beacon packet over UART
	//Beacon_Packet_UART_log();

	//Storing the beacon packet in the SD Card
	//TODO: This function is taken as is from IS1 - need to check the working of the same
	//Store_Beacon_Data();

	//TODO: Add code for any interaction with the PSLV UART verilog peripheral if needed.

	//Call the Decide mode function to Apply the forced simulated modes for IS0
	//Decide_Mode();

}

void Init(){
	//Initialize all the GPIOs
	Global_Init_GPIOs();
	//Initialize EPS I2C Channels
	Initialize_EPS_I2C_Channels();
	//Initialize Sensor Board I2C Channel
	Initialize_Sensor_Board_I2C_Channel();

	//Initialize the SD Cards
	//TODO: This fucntion is taken directly from IS1 - Test the working of this function
	//SD_Cards_Init();

	//Initialize the RTC if required

	//Initialize the PSLV UART IP if required
}



void Get_Beacon_Packet(){

	//Get EPS Data
	Get_EPS_Data();

	//Get VMEL Sensor Board Data
	Get_VEML6075_Data();

	//TODO: Get triad sensor data
	//Get_AS7265x_Data();

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

	//Start in Science D
	//Switch to Science C in 45 minutes and repeat

	//Every 5 days, force into safe mode for 5 orbits

	//Every 10 days, force into phoenix mode for 5 orbits


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

//TODO: Send Beacon Packet of IS0 over MSS UART
void Beacon_Packet_UART_log(){

	//Updating the Beacon Packet Structure
	Get_Beacon_Packet();

	//Initializing the MSS UART for Logging
	MSS_UART_init(&g_mss_uart0,MSS_UART_9600_BAUD,MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY | MSS_UART_ONE_STOP_BIT);

	//TODO: Complete This. Send all the bytes of the beacon packet using the MSS UART
	//Either send the elements of the structure one-by-one or use the memory address of the structure to achieve the same


}


void RTC_Sync_Timer_Hanlder(){
  if(Utils_Handle_Timer32_Started(&Globals.RTC_Sync_Timer)){
    Globals.Indication_Flags |= UPDATE_RTC_GPS;
  }
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
#endif
