#ifndef _SD_C
	#define _SD_C


#include "../mss_spi/mss_spi.h"
#include "drivers/mss_gpio/mss_gpio.h"
#include "drivers/CoreSPI/core_spi.h"
#include "Utils/utils.h"
#include "sd.h"
#include "SPI_Flash/spi_flash.h"
#include "Seq_ops/main_seq.h"
#include "string.h"
#include "GPIOs/gpios.h"
#include "EPS/eps.h"

#include "../../CMSIS/mss_assert.h"

uint8_t SD_Status;

uint8_t CMD17[8];
uint8_t tmp_buff[SD_CARD_SINGLE_BLOCK_SIZE];

void SD_clear_slave_select(mss_spi_slave_t slave) {
	if (!slave) {
		PWR_Switch(GPIO_SD0_CS,1);
		return;
	}
	PWR_Switch(GPIO_SD1_CS, 1);
}

void SD_set_slave_select(mss_spi_slave_t slave) {
	if (!slave) {
		PWR_Switch(GPIO_SD0_CS,0);
		return;
	}
	PWR_Switch(GPIO_SD1_CS, 0);
}

void SD_toggle_slave_select(mss_spi_slave_t slave) {
	SD_clear_slave_select(slave);
	MSS_SPI_transfer_block(&g_mss_spi1, tmp_buff, 2, &SD_Status,0);
	SD_set_slave_select(slave);
}

void SD_enable(mss_spi_slave_t slave) {
	if (slave == MSS_SPI_SLAVE_0) {
		MSS_GPIO_set_output( GPIO_EN_SD0, 1);
		return;
	}
	// means slave is MSS_SPI_SLAVE_1
	MSS_GPIO_set_output(GPIO_EN_SD1,1);
}

void SD_disable(mss_spi_slave_t slave) {
	if (slave == MSS_SPI_SLAVE_0) {
		MSS_GPIO_set_output( GPIO_EN_SD0, 0);
		return;
	}
	// means slave is MSS_SPI_SLAVE_1
	MSS_GPIO_set_output(GPIO_EN_SD1,0);
}

void Custom_SPI_configure_master_mode
(
    mss_spi_instance_t *    this_spi,
    mss_spi_slave_t         slave,
    mss_spi_protocol_mode_t protocol_mode,
    uint32_t                clk_div,
    uint8_t                 frame_bit_length
) {
	//pass through to normal function
	MSS_SPI_configure_master_mode(this_spi, slave, protocol_mode, clk_div, frame_bit_length);
	/***
	 * The following code was taken from mss_spi.c, the set_slave_select() function.
	 * A better place for this might be in our new slave_select function,
	 * but we'd have to add an input parameter (this_spi pointer), so I chose not to.
	 */
	/* Set the clock rate. */
	this_spi->hw_reg->CONTROL &= ~(uint32_t)CTRL_ENABLE_MASK;
	this_spi->hw_reg->CONTROL = this_spi->slaves_cfg[slave].ctrl_reg;
	this_spi->hw_reg->CLK_GEN = this_spi->slaves_cfg[slave].clk_gen;
	this_spi->hw_reg->TXRXDF_SIZE = this_spi->slaves_cfg[slave].txrxdf_size_reg;
	this_spi->hw_reg->CONTROL |= CTRL_ENABLE_MASK;
}


uint16_t SD_get_status(mss_spi_slave_t slave) {
	uint8_t CMD13[] = {0xFF, 0x4D, 0, 0, 0, 0, 0x0D, 0xFF};
	uint8_t rx_buffer[10];
	uint16_t delay;
	SD_set_slave_select(slave); //tie low
	//TODO: switch to UtilsDelay
	for(delay=0; delay < 500; delay++);
	MSS_SPI_transfer_block(&g_mss_spi1, CMD13, 8, rx_buffer, sizeof(rx_buffer));
	//TODO: switch to UtilsDelay
	for(delay=0; delay < 5000; delay++);
	SD_clear_slave_select(slave); //tie high

	return (uint16_t) (rx_buffer[0] << 8 | rx_buffer[1]);
}

uint8_t SD_Simul_Init() {
	SD_disable(SPI_SLAVE_0); //Power off both SD cards
	SD_disable(SPI_SLAVE_1);
	Utils_Delay16(3); //

	//Power on both SD cards
	MSS_GPIO_set_output( GPIO_EN_SD0, 1);
	MSS_GPIO_set_output( GPIO_EN_SD1, 1);
	// delay for power-up
	Utils_Delay16(10);

	MSS_SPI_init( &g_mss_spi1 );

	Custom_SPI_configure_master_mode(&g_mss_spi1,
		SPI_SLAVE_0,
		MSS_SPI_MODE0,
		SD_CARD_DEFAULT_SPEED,
		8);

	Custom_SPI_configure_master_mode(&g_mss_spi1,
		SPI_SLAVE_1,
		MSS_SPI_MODE0,
		SD_CARD_DEFAULT_SPEED,
		8);

	uint8_t rx_buffer[2];
	uint8_t CMD0[] = {0x40, 0x00,0x00, 0x00,0x00, 0x95,0xFF};
	uint32_t i = 0;

	//dummy bytes to no one
	SD_clear_slave_select(SPI_SLAVE_0); //tie hi
	SD_clear_slave_select(SPI_SLAVE_1); //tie hi
	MSS_SPI_transfer_block(&g_mss_spi1, tmp_buff, 20, rx_buffer, 0);

	//dummy bytes to SD0
	SD_set_slave_select(SPI_SLAVE_0); //tie lo
	MSS_SPI_transfer_block(&g_mss_spi1, tmp_buff,20,rx_buffer,0);
	SD_clear_slave_select(SPI_SLAVE_0); //tie hi
	MSS_SPI_transfer_frame(&g_mss_spi1, 0xFF);

	//dummy bytes to SD1
	SD_set_slave_select(SPI_SLAVE_1); //tie lo
	MSS_SPI_transfer_block(&g_mss_spi1, tmp_buff,20,rx_buffer,0);
	SD_clear_slave_select(SPI_SLAVE_1); //tie hi
	MSS_SPI_transfer_frame(&g_mss_spi1, 0xFF);

	//Send CMD0 to SD0
	do{		// add counter for number of trying
		Utils_Delay16(3); // since RTC is  1kHz, this could be  anywhere from 2-3ms...
		SD_set_slave_select(SPI_SLAVE_0); //tie low
		MSS_SPI_transfer_block(&g_mss_spi1, CMD0, 7, rx_buffer, 1);
		SD_clear_slave_select(SPI_SLAVE_0); //tie hi
		i += 1;
	}while(rx_buffer[0]!=1 && i < 200);// rx_buffer[0]!=1  or  i!=200 //sometimes CMD0 failed

	if(i >= 200){
		return 0;
	}
	i=0;
	//Send CMD0 to SD1
	do{		// add counter for number of trying
		Utils_Delay16(3); // since RTC is  1kHz, this could be  anywhere from 2-3ms...
		SD_set_slave_select(SPI_SLAVE_0); //tie low
		MSS_SPI_transfer_block(&g_mss_spi1, CMD0, 7, rx_buffer, 1);
		SD_clear_slave_select(SPI_SLAVE_0); //tie hi
		SD_clear_slave_select(SPI_SLAVE_1); //tie hi
		i += 1;
	}while(rx_buffer[0]!=1 && i < 200);// rx_buffer[0]!=1  or  i!=200

	if(i >= 200){
		return 0;
	}

	return 1;
}

void SD_Cards_Init() {

	//send CMD0 to both SD_Cards. Hope they both are working... :(((
	uint8_t stat = SD_Simul_Init();
	if (!stat) {
		//this means we failed
	}

	uint8_t status = SD_Init(0);
	if (status) {
		Globals.Current_SD = 0;
		return;
	}
	status = SD_Init(1);
	if (status) {
		Globals.Current_SD = 1;
		return;
	}
	//here, both SD Cards have failed
	//TODO: LOG/Event message
	SD_FDRI();
	return;
}

uint8_t SD_Init(mss_spi_slave_t slave){
	uint16_t status, delay;
	memset(tmp_buff, 0xff, SD_CARD_SINGLE_BLOCK_SIZE);
	CMD17[0] = 0xff;
	CMD17[1] = 0x51;
	CMD17[7] = 0xff;
	CMD17[6] = 0xff;

	MSS_SPI_init( &g_mss_spi1 );

	Custom_SPI_configure_master_mode(&g_mss_spi1,
			slave,
	        MSS_SPI_MODE0,
			SD_CARD_1MHZ,
	        8);


	uint8_t rx_buffer[5];
	uint8_t CMD0[] = {0x40, 0x00,0x00, 0x00,0x00, 0x95,0xFF};
	uint32_t i = 0;




//	SD_clear_slave_select(slave); //tie hi
//	MSS_SPI_transfer_block(&g_mss_spi1, tmp_buff, 20, rx_buffer, 0);
//	SD_set_slave_select(slave); //tie low
//	MSS_SPI_transfer_frame(&g_mss_spi1, 0xFF);
	SD_clear_slave_select(slave); //tie hi
	MSS_SPI_transfer_frame(&g_mss_spi1, 0xFF);


	do{		// add counter for number of trying
		Utils_Delay16(3); // since RTC is  1kHz, this could be  anywhere from 2-3ms...
		SD_set_slave_select(slave); //tie low
		MSS_SPI_transfer_block(&g_mss_spi1, CMD0, 7, rx_buffer, 1);
		SD_clear_slave_select(slave); //tie hi
		i += 1;
	}while(rx_buffer[0]!=1 && i < 200);// rx_buffer[0]!=1  or  i!=200 //sometimes CMD0 failed

	if(i >= 200){
		return 0;
	}
	i = 0;

	uint8_t rx_b[5];
	uint8_t CMD8[] = {0xFF, 0x48, 0x00, 0x00, 0x01, 0xAA, 0x87, 0xFF};	// last 0xFF is use to give sd card buffer clock
	do{
		Utils_Delay16(3);
		SD_set_slave_select(slave); //tie low
		MSS_SPI_transfer_block(&g_mss_spi1, CMD8, 8, rx_b, 5);
		//checked ,return 0X1AA
		SD_clear_slave_select(slave); //tie hi
		i = i + 1;

		if(rx_b[4] == 0xaa && rx_b[3] == 0x01 && rx_b[2] == 0x00 && rx_b[1] == 0x00){
			rx_buffer[0] = 1;
			i = 0;
			break;
		}
	}while(rx_buffer[0] && i < 200);

	if (i > 0){
		return 0;
	}

	SD_set_slave_select(slave); //tie low
	MSS_SPI_transfer_frame(&g_mss_spi1, 0xFF);
	MSS_SPI_transfer_frame(&g_mss_spi1, 0xFF);
	MSS_SPI_transfer_frame(&g_mss_spi1, 0xFF);
	SD_clear_slave_select(slave); //tie hi
	MSS_SPI_transfer_frame(&g_mss_spi1, 0xFF);
	MSS_SPI_transfer_frame(&g_mss_spi1, 0xFF);
	MSS_SPI_transfer_frame(&g_mss_spi1, 0xFF);



	// sending CMD55 and then ACMD41 to get it out  of idle mode.
	uint8_t CMD55[] = {0xFF, 0x77, 0x00, 0x00, 0x00, 0x00, 0x65, 0xFF};	// last 0xFF is use to give sd card buffer clock
	uint8_t ACMD41[] = {0xFF, 0x69, 0x40, 0x10, 0x00, 0x00, 0xCD, 0xFF};	// last 0xFF is use to give sd card buffer clockACMD41[] = {0x69, 0x41, 0x00, 0x00, 0x00, 0xFF,0xFF}




	do{
		Utils_Delay16(3);
		SD_set_slave_select(slave); //tie low
		MSS_SPI_transfer_block(&g_mss_spi1, CMD55, 8, rx_buffer, 1);
		MSS_SPI_transfer_frame(&g_mss_spi1, 0xFF);
		SD_clear_slave_select(slave); //tie hi
		for(delay=0; delay < 500; delay++);
		MSS_SPI_transfer_frame(&g_mss_spi1, 0xFF);
		SD_set_slave_select(slave); //tie low
		MSS_SPI_transfer_block(&g_mss_spi1, ACMD41, 8, rx_buffer, 1);
		MSS_SPI_transfer_frame(&g_mss_spi1, 0xFF);
		SD_clear_slave_select(slave); //tie hi
		i = i+1;
	}while(rx_buffer[0] && i < 2000); //any of them is 0 (i=4 or rx_buffer[0]=0), get out of the loop //stucked sometimes

	if(i >= 2000)
		return 0;

	uint8_t CMD58[] = {0xFF, 0x7A, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF};
	MSS_SPI_transfer_block(&g_mss_spi1, CMD58, 8, rx_buffer, 5);
	MSS_SPI_transfer_frame(&g_mss_spi1, 0xFF);

	MSS_SPI_transfer_block(&g_mss_spi1, tmp_buff, 20, rx_buffer, 0);

	SD_clear_slave_select(slave); //tie high

	status = SD_get_status(slave);
	for(delay=0; delay < 5000; delay++);
	status = SD_get_status(slave);
	for(delay=0; delay < 5000; delay++);
	status = SD_get_status(slave);
	for(delay=0; delay < 5000; delay++);

	Custom_SPI_configure_master_mode
	    (
	        &g_mss_spi1,
			slave,
	        MSS_SPI_MODE0,
			SD_CARD_3MHZ,
	        8);

	return 1;
}	// SD card is initialized now
//
//uint8_t SD_Select_Init(){
//	/* curr_slave to be updated */
//	uint8_t curr_slave = MSS_SPI_SLAVE_1;
//	SD_disable(curr_slave); //Power off the selected SD card
//	SD_enable(curr_slave); //Power on the selected SD card
//
//	//Power on both SD cards
//	MSS_GPIO_set_output( GPIO_EN_SD0, 1);
//	MSS_GPIO_set_output( GPIO_EN_SD1, 1);
//
//
//	uint8_t data[SD_TEST_DATA_SIZE];
//	uint8_t rx_data[SD_TEST_DATA_SIZE];
//	uint16_t i;
//	for (i = 0; i < SD_TEST_DATA_SIZE; i++){
//		data[i]=i%128 +0 ;
//		rx_data[i] = 0;
//	}
//	i=0;
//	SD_Init(curr_slave);
//	i = SD_Write_Data(data, SD_TEST_DATA_SIZE, 0x201, curr_slave);
//	i = SD_Read_Data(rx_data, 512, 0x201, curr_slave);
//
//	uint8_t tries = 20;
//    i = 0;
//    do
//    {
//    	i++;
//    	if(SD_Select_Helper(curr_slave)){
//			Globals.Indication_Flags |= SD_CARD0_WORKING;
//			Globals.Indication_Flags &= ~SD_CARD1_WORKING;
//    		return curr_slave;
//    	}
//    	SD_Set_Highest_Clock(curr_slave);
//
//    }while( i < tries);
//    /* At this point the sd0 is not working set the indication flag to 0*/
//    // TODO: uncomment after sdcard2 is ok again.
//	curr_slave = MSS_SPI_SLAVE_1;
//	SD_Init(curr_slave);
//	i = 0;
//    do
//    {
//    	i++;
//    	if(SD_Select_Helper(curr_slave)){
//    		Globals.Indication_Flags |= SD_CARD1_WORKING;
//			Globals.Indication_Flags &= ~SD_CARD0_WORKING;
//    		return curr_slave;
//    	}
//    }while( i < tries);
//	/* None of the sd card is working */
//	Globals.Indication_Flags &= ~SD_CARD0_WORKING;
//	Globals.Indication_Flags &= ~SD_CARD1_WORKING;
//	// Don't want these actually due to power-up time considerations
////	MSS_GPIO_set_output( GPIO_EN_SD0, 0);	/* Turn OFF the sd card 0*/
////	MSS_GPIO_set_output( GPIO_EN_SD1, 0);	/* Turn OFF the sd card 1*/
//
//	//reset clock speeds to much higher.
//	MSS_SPI_configure_master_mode
//	(
//		&g_mss_spi1,
//		MSS_SPI_SLAVE_0,
//		MSS_SPI_MODE0,
//		SD_CARD_3MHZ, // preliminary and under test TODO: Finalize and define
//		8);
//
//	MSS_SPI_configure_master_mode
//	(
//		&g_mss_spi1,
//		MSS_SPI_SLAVE_1,
//		MSS_SPI_MODE0,
//		SD_CARD_3MHZ,
//		8);
//	return 0xff;
//}
//
//uint8_t SD_Select_Helper(uint8_t curr_slave){
//	SD_Status = SD_Set_Highest_Clock(curr_slave);
//	if(SD_Status == 0){
//		MSS_GPIO_set_output( GPIO_EN_SD0, 0);
//		MSS_GPIO_set_output( GPIO_EN_SD1, 0);
//		MSS_GPIO_set_output( GPIO_EN_SD0, 1);
//		//TODO: reset after this works!
//		MSS_GPIO_set_output( GPIO_EN_SD1, 1);
//
//		SD_Status = SD_Init(curr_slave);
//		return 0;
//	}
//	return 1;
//}

uint8_t SD_Write_Sector(uint8_t * data, uint32_t addr, mss_spi_slave_t slave, uint16_t write_limit){
	uint16_t status;
	uint8_t rx_buffer[6];
	uint16_t i = 0;uint8_t res = 0;
	uint16_t delay=0;

	uint8_t CMD24[] = {0xFF, 0x58, (addr >> 24) & 0xff, (addr >> 16) & 0xff, (addr >> 8) & 0xff, addr & 0xff, 0xFF, 0xFF};//address an integer denoting the sector address.


	status = SD_get_status(slave);

	//Send dummy clocks
	SD_set_slave_select(slave); //tie low
	MSS_SPI_transfer_block(&g_mss_spi1, tmp_buff, 8, rx_buffer, 0);


	//Toggle chip select
	SD_clear_slave_select(slave); //tie high
	for(delay=0; delay < 100; delay++);
	SD_set_slave_select(slave); //tie low
	for(delay=0; delay < 100; delay++);


	i = 0;
	do {
		//Start write command
		MSS_SPI_transfer_block(&g_mss_spi1, CMD24, 8, rx_buffer, 1);
		i++;
		for(delay=0; delay < 100; delay++);
	} while(rx_buffer[0] != 0 && i < 2000);

	if (i >= 2000) {
		return 0;
	}



	uint8_t flag = SD_START_FLAG;
	if(rx_buffer[0] == 0){
//		Utils_SPI_Transmit_Block(&g_mss_spi1, tmp_buff, 100);
//		MSS_SPI_transfer_block(&g_mss_spi1, tmp_buff, 10, 0, 0);
		i=0;

		MSS_SPI_transfer_block(&g_mss_spi1, &flag, 1, rx_buffer, 0);
		MSS_SPI_transfer_block(&g_mss_spi1, data, 512, rx_buffer, 0);
		//Send Dummy CRC
		MSS_SPI_transfer_block(&g_mss_spi1, tmp_buff, 2, rx_buffer, 0);
		//MSS_SPI_transfer_block(&g_mss_spi1, data, write_limit, rx_buffer, 1);
		//MSS_SPI_transfer_block(&g_mss_spi1, tmp_buff, SD_CARD_SINGLE_BLOCK_SIZE - write_limit, rx_buffer, 1);

		//Wait for data response token
		do {
			MSS_SPI_transfer_block(&g_mss_spi1, tmp_buff, 0, rx_buffer, 1);
			i++;
		}while(rx_buffer[0] == 0xff && i < 200);
		res = (rx_buffer[0] & 0x0f);

		//Wait for SD card to finish programming and come out of BUSY
		i = 0;
		do {
			MSS_SPI_transfer_block(&g_mss_spi1, tmp_buff, 0, rx_buffer, 1);
			i++;
			for(delay=0; delay < 500; delay++); //Delay needed here for Sandisk
		} while(rx_buffer[0] == 0 && i < 2000);

		if (i >= 2000) {
			return 0;
		}

//
////		Utils_SPI_Transmit_Block(&g_mss_spi1, tmp_buff, 192);
//		MSS_SPI_transfer_block(&g_mss_spi1, tmp_buff, 512, 0, 0);

//		uint8_t ACMD13[] = {0x4D, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff};
//		MSS_SPI_transfer_block(&g_mss_spi1, ACMD13, 7, rx_buffer, 2);	// rx_buffer should be 0x00 0x00
	}
	SD_clear_slave_select(slave); //tie high

	status = SD_get_status(slave);

	return res == 0x05;
}

uint8_t SD_Read_Sector(uint8_t * data, uint32_t addr, mss_spi_slave_t slave, uint16_t read_limit){
	uint8_t rx_buffer;
	uint8_t data_buffer[514];
	uint16_t status;
	uint16_t delay=0;
	uint16_t i=0;

	uint8_t CMD16[8]= {0xFF, 0x50, 0x00, 0x00, 0x02, 0x00, 0xFF, 0xFF};



	CMD17[2] = (addr >> 24);
	CMD17[3] = (addr >> 16);
	CMD17[4] = (addr >> 8);
	CMD17[5] = addr;

	//Send 8 bytes of dummy clocks to the SD card to empty out any leftover
	//data from previous commands
	SD_clear_slave_select(slave); //tie high
	for(delay=0; delay < 100; delay++);
	SD_set_slave_select(slave); //tie low

	MSS_SPI_transfer_block(&g_mss_spi1, tmp_buff, 64, &rx_buffer, 0);

	SD_clear_slave_select(slave); //tie high
	for(delay=0; delay < 100; delay++);

	//status = SD_get_status(slave);


	//SD_toggle_slave_select(slave);


	MSS_SPI_transfer_block(&g_mss_spi1, tmp_buff, 64, &rx_buffer, 0);

	SD_clear_slave_select(slave); //tie high
	for(delay=0; delay < 100; delay++);
	SD_set_slave_select(slave); //tie low
	for(delay=0; delay < 100; delay++);

	//Send CMD17 to initiate a single sector read
	MSS_SPI_transfer_block(&g_mss_spi1, CMD17, 8, &rx_buffer, 1);


	//We expect a 00 to be returned by the CMD17, indicating that the read command was valid.
	if(rx_buffer){
		//Poll for 0x00
		do {
			MSS_SPI_transfer_block(&g_mss_spi1, tmp_buff, 0, &rx_buffer, 1);
			i++;
			for(delay=0; delay < 100; delay++);
		}while(rx_buffer && i < 2000);
		if(i >= 2000){
			return 0;
		}
	};

	i=0;
	// Poll for 0xFE.
	//Every read data block starts with FE
	//It could take up to 100ms for the card to return a sector
	do{
		MSS_SPI_transfer_block(&g_mss_spi1, tmp_buff, 0, &rx_buffer, 1);
		i++;
		for(delay=0; delay < 100; delay++);

	}while(rx_buffer != SD_START_FLAG && i < 2000);

	if(i >= 2000){
		return 0;
	}

	//MSS_GPIO_set_output( GPIO_CTRL_MOSI_SD, 1);
//	for (i=0; i<512; i++) {
//		MSS_SPI_transfer_block(&g_mss_spi1, tmp_buff, 0, &data[i], 1);
//	}
    //MSS_SPI_transfer_block(&g_mss_spi1, tmp_buff, 0, &data_buffer, 514);
    MSS_SPI_transfer_block(&g_mss_spi1, tmp_buff, 0, data, 514);
    //Utils_SPI_Receive_Block(&g_mss_spi1, data, read_limit);

//	MSS_GPIO_set_output( GPIO_CTRL_MOSI_SD, 1);

	//Utils_SPI_Transmit_Block(&g_mss_spi1, tmp_buff, SD_CARD_SINGLE_BLOCK_SIZE - read_limit + 2);
//	MSS_SPI_transfer_block(&g_mss_spi1, tmp_buff, SD_CARD_SINGLE_BLOCK_SIZE - read_limit + 2, 0, 0);

	status = SD_get_status(slave);
	SD_clear_slave_select(slave); //tie high

	return 1;
}

uint32_t SD_Write_Data(uint8_t * data, uint16_t dataLength, uint32_t addr, mss_spi_slave_t slave){
	uint16_t j = 0;uint32_t addr_tmp = addr;
//    uint8_t data_tmp[515];
//
//    data_tmp[0] = 0xfe;
//
//    data_tmp[513]=0xFF;
//    data_tmp[514]=0xFF;
    uint16_t sent_bytes = 0, rem_bytes  = dataLength, bytes_to_send = 0;
    uint16_t i;
    while(sent_bytes < dataLength){
        bytes_to_send = rem_bytes < SD_CARD_SINGLE_BLOCK_SIZE ? rem_bytes : SD_CARD_SINGLE_BLOCK_SIZE;


//        memcpy(&data_tmp[1], &data[sent_bytes], bytes_to_send);
//
//        if(bytes_to_send + 1 < 513){
//        	memset(&data_tmp[bytes_to_send + 1], 0, SD_CARD_SINGLE_BLOCK_SIZE - bytes_to_send);
//        }
		SD_Status = SD_Write_Sector(&data[sent_bytes], addr, slave, bytes_to_send);
        if(SD_Status == 0)
        	return addr_tmp;

        sent_bytes += bytes_to_send;
        rem_bytes = dataLength - sent_bytes;
        addr += 1;

    }
    //  10 bytes of 0xff

    return addr;    /* Returns from which address the next data to be written */

}

uint32_t SD_Read_Data(uint8_t * data, uint16_t dataLength, uint32_t addr, mss_spi_slave_t slave){
	uint32_t addr_tmp = addr;
    uint16_t read_bytes = 0, rem_bytes = dataLength, bytes_to_read = 0;
    uint16_t i = 0;
    while(read_bytes < dataLength){
    	bytes_to_read = rem_bytes < SD_CARD_SINGLE_BLOCK_SIZE ? rem_bytes : SD_CARD_SINGLE_BLOCK_SIZE;
        i = SD_Read_Sector(&data[read_bytes], addr, slave, bytes_to_read);
        if(i == 0)
        	return addr_tmp;

        read_bytes += bytes_to_read;
        rem_bytes = dataLength - read_bytes;
        addr += 1;
    }
    return addr;    /* Returns from which address the next read operation can be done */
}

//uint8_t SD_Set_Highest_Clock(mss_spi_slave_t slave){
//	Globals.SD_Clk_Div = 5;
//	uint16_t i;	/* The initial clock divider value to be chosen on the basis of the base clock*/
//	uint8_t data[SD_TEST_DATA_SIZE];
//	uint8_t rx_data[SD_TEST_DATA_SIZE];
//	data[0]=0xFE;
//	for (i =0; i<SD_TEST_DATA_SIZE; i++){
//		data[i]=i % 10;
//		rx_data[i] = 0;
//	}
//
//	do{
//		Custom_SPI_configure_master_mode
//		(
//			&g_mss_spi1,
//			slave,
//			MSS_SPI_MODE0,
//			SD_CARD_MINIMUM_SPEED,//Globals.SD_Clk_Div,
//			8);
//
//		i = SD_Write_Data(data, SD_TEST_DATA_SIZE, 0x00000001, slave);	/* 0x00000200 and 0x00000400 are dummy addresses to be used for this purpose and not for data storage*/
//		if(i == 0x00000001)
//			break;
//		i = SD_Read_Data(rx_data, SD_TEST_DATA_SIZE, 0x00000001, slave);
//		if(i == 0x00000001)
//			break;
//		i = FLASH_Verify_write(data, rx_data, SD_TEST_DATA_SIZE);
//		if(i != SD_TEST_DATA_SIZE)
//			break;
//
//		memset(rx_data, 0, SD_TEST_DATA_SIZE);
//
////		Globals.SD_Clk_Div -= 2;
//	}while(i == SD_TEST_DATA_SIZE && Globals.SD_Clk_Div >= 10);
//
////	Globals.SD_Clk_Div += 2;
//	MSS_SPI_configure_master_mode
//	(
//		&g_mss_spi1,
//		slave,
//		MSS_SPI_MODE0,
//		SD_CARD_MINIMUM_SPEED,//Globals.SD_Clk_Div,
//		8);
//	i = SD_Write_Data(data, SD_TEST_DATA_SIZE, 0x00000001, slave);
//
//	i = SD_Read_Data(rx_data, SD_TEST_DATA_SIZE, 0x00000001, slave);
//	i = FLASH_Verify_write(data, rx_data, SD_TEST_DATA_SIZE);
//	if(i == SD_TEST_DATA_SIZE)
//		return Globals.SD_Clk_Div;
//	else
//		return 0;
//}

uint32_t SD_Card_Wipe(mss_spi_slave_t slave) {
	uint32_t max_size = slave ? SD_MAX_SECTOR_VALUE_16: SD_MAX_SECTOR_VALUE_128;
	uint32_t sector, i, errs=0;
	uint8_t data[SD_TEST_DATA_SIZE], retries = 0;
	for (i=0;i<SD_TEST_DATA_SIZE;i++){
		data[i] = 0;
	}

	for (sector=0; sector < max_size; sector++) {
		i = SD_Write_Data(data, SD_TEST_DATA_SIZE, sector, slave);
		if (i != sector + 1 && retries < 5){
			errs++;
			retries++;
			sector--;
		}
		retries = 0;
	}
	return errs;

}

//FIXME! SD_Card FDRI
//TODO: Finish function, decide how often to try again and using what structure
//NOTE: on 6/18/19, this function was called (noticed only via breakpoint),
// and the system was able to recover, and keep incrementing Beaocn_Write_Start thereafter
//TODO: Add timer to parameter table as well as use timer for FDRI based on state
void SD_FDRI() {
	if (Globals.Current_SD != 0xff)
		return;
	//now we know we need to do something
	if (Globals.SD_FDRI_Module_Sync.CMD_Seq_Count < Globals.Non_Response_Count_Limit) {
		if (Globals.Indication_Flags & SD_CARD0_WORKING > 0) {
			Globals.Current_SD = MSS_SPI_SLAVE_0;
		}
		else if (Globals.Indication_Flags & SD_CARD1_WORKING > 0) {
			Globals.Current_SD = MSS_SPI_SLAVE_1;
		}
		else {

		}
		Globals.SD_FDRI_Module_Sync.CMD_Seq_Count++;
		return;
	}
//	Globals.Current_SD = SD_Select_Init();
	SD_Cards_Init();
	Globals.SD_FDRI_Module_Sync.CMD_Seq_Count = 0;

}

#endif
