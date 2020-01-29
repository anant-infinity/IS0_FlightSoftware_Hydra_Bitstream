#ifndef _SD_C
	#define _SD_C


#include "../mss_spi/mss_spi.h"
#include "drivers/mss_gpio/mss_gpio.h"
#include "drivers/CoreSPI/core_spi.h"
#include "sd.h"
#include "SPI_Flash/spi_flash.h"
#include "Seq_ops/main_seq.h"
#include "string.h"
#include "gpios.h"

#include "../../CMSIS/mss_assert.h"

uint8_t SD_Status;

uint8_t CMD17[7];
uint8_t tmp_buff[SD_CARD_SINGLE_BLOCK_SIZE];

uint8_t SD_Init(mss_spi_slave_t slave){

	memset(tmp_buff, 0xff, SD_CARD_SINGLE_BLOCK_SIZE);

	CMD17[0] = 0x51;
	CMD17[6] = 0xff;
	CMD17[5] = 0xff;

	MSS_SPI_init( &g_mss_spi1 );

    MSS_SPI_configure_master_mode
    (
        &g_mss_spi1,
		slave,
        MSS_SPI_MODE0,
        SD_CARD_SINGLE_BLOCK_SIZE,
        8);

	uint8_t rx_buffer[5];
	uint8_t CMD0[] = {0x40, 0x00,0x00, 0x00,0x00, 0x95,0xFF};
	uint32_t i = 0;

	MSS_SPI_set_slave_select(&g_mss_spi1, slave); //tie low

	Utils_SPI_Transmit_Block(&g_mss_spi1, tmp_buff, 100);
//	MSS_SPI_transfer_block(&g_mss_spi1, tmp_buff, 100, 0, 0);

	do{		// add counter for number of trying
		MSS_SPI_transfer_block(&g_mss_spi1, CMD0, 7, rx_buffer, 1);
		i += 1;
	}while(rx_buffer[0]!=1 && i < 200);// rx_buffer[0]!=1  or  i!=200 //sometimes CMD0 failed

	if(i >= 200){
		return 0;
	}
	i = 0;

	uint8_t rx_b[5];
	uint8_t CMD8[] = {0x48, 0x00, 0x00, 0x01, 0xAA, 0x87, 0xFF};	// last 0xFF is use to give sd card buffer clock
	do{
		MSS_SPI_transfer_block(&g_mss_spi1, CMD8, 7, rx_b, 5);             //checked ,return 0X1AA
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

	//send CMD58
	uint8_t CMD58[] = {0x7A, 0x00, 0x00, 0x00, 0x00, 0xFD, 0xFF};

	MSS_SPI_transfer_block(&g_mss_spi1, CMD58, 7, rx_buffer, 5);
		MSS_SPI_transfer_frame(&g_mss_spi1, 0xFF);

	// sending CMD55 and then ACMD41 to get it out  of idle mode.
	uint8_t CMD55[] = {0x77, 0x00, 0x00, 0x00, 0x00, 0x65, 0xFF};	// last 0xFF is use to give sd card buffer clock
	uint8_t ACMD41[] = {0x69, 0x40, 0x10, 0x00, 0x00, 0xCD, 0xFF};	// last 0xFF is use to give sd card buffer clockACMD41[] = {0x69, 0x41, 0x00, 0x00, 0x00, 0xFF,0xFF}
	do{
		i = i+1;
		MSS_SPI_transfer_block(&g_mss_spi1, CMD55, 7, rx_buffer, 1);
		MSS_SPI_transfer_frame(&g_mss_spi1, 0xFF);
		MSS_SPI_transfer_block(&g_mss_spi1, ACMD41, 7, rx_buffer, 1);
		MSS_SPI_transfer_frame(&g_mss_spi1, 0xFF);

	}while(rx_buffer[0] && i < 5); //any of them is 0 (i=4 or rx_buffer[0]=0), get out of the loop //stucked sometimes

	if(i >= 5)
		return 0;

	MSS_SPI_transfer_block(&g_mss_spi1, CMD58, 7, rx_buffer, 5);
	MSS_SPI_transfer_frame(&g_mss_spi1, 0xFF);

	MSS_SPI_clear_slave_select(&g_mss_spi1, slave); //tie high

	return 1;
}	// SD card is initialized now

uint8_t SD_Select_Init(){
	/* curr_slave to be updated */
	uint8_t curr_slave = MSS_SPI_SLAVE_0;
	MSS_GPIO_set_output( GPIO_EN_SD0, 1);
//	MSS_GPIO_set_output( GPIO_EN_SD1, 1);
	SD_Init(curr_slave);

	uint8_t tries = 20;
    uint8_t i = 0;
    do
    {
    	i++;
    	if(SD_Select_Helper(curr_slave)){
			Globals.Indication_Flags |= SD_CARD0_WORKING;
			Globals.Indication_Flags &= ~SD_CARD1_WORKING;
    		return curr_slave;
    	}
    	SD_Set_Highest_Clock(curr_slave);

    }while( i < tries);
    /* At this point the sd0 is not working set the indication flag to 0*/
    // TODO: uncomment after sdcard2 is ok again.
	curr_slave = MSS_SPI_SLAVE_1;
	SD_Init(curr_slave);
	i = 0;
    do
    {
    	i++;
    	if(SD_Select_Helper(curr_slave)){
    		Globals.Indication_Flags |= SD_CARD1_WORKING;
			Globals.Indication_Flags &= ~SD_CARD0_WORKING;
    		return curr_slave;
    	}
    }while( i < tries);
	/* None of the sd card is working */
	Globals.Indication_Flags &= ~SD_CARD0_WORKING;
	Globals.Indication_Flags &= ~SD_CARD1_WORKING;
	// Don't want these actually due to power-up time considerations
//	MSS_GPIO_set_output( GPIO_EN_SD0, 0);	/* Turn OFF the sd card 0*/
//	MSS_GPIO_set_output( GPIO_EN_SD1, 0);	/* Turn OFF the sd card 1*/
	MSS_SPI_configure_master_mode
	(
		&g_mss_spi1,
		MSS_SPI_SLAVE_0,
		MSS_SPI_MODE0,
		SD_CARD_SINGLE_BLOCK_SIZE,
		8);

	MSS_SPI_configure_master_mode
	(
		&g_mss_spi1,
		MSS_SPI_SLAVE_1,
		MSS_SPI_MODE0,
		SD_CARD_SINGLE_BLOCK_SIZE,
		8);
	return 0xff;
}

uint8_t SD_Select_Helper(uint8_t curr_slave){
	SD_Status = SD_Set_Highest_Clock(curr_slave);
	if(SD_Status == 0){
		MSS_GPIO_set_output( GPIO_EN_SD0, 0);
		MSS_GPIO_set_output( GPIO_EN_SD1, 0);
		MSS_GPIO_set_output( GPIO_EN_SD0, 1);
		//TODO: reset after this works!
//		MSS_GPIO_set_output( GPIO_EN_SD1, 1);
		SD_Status = SD_Init(curr_slave);
		return 0;
	}
	return 1;
}

uint8_t SD_Write_Sector(uint8_t * data, uint32_t addr, mss_spi_slave_t slave, uint16_t write_limit){
	uint8_t loop_ctrl = 1;
	uint8_t rx_buffer[400];
	uint16_t i = 0;uint8_t res = 0;

	uint8_t CMD24[7];
	CMD24[0] = 0x58;
	CMD24[1] = (addr >> 24) & 0xff;
	CMD24[2] = (addr >> 16) & 0xff;
	CMD24[3] = (addr >> 8) & 0xff;
	CMD24[4] = addr & 0xff;
	CMD24[5] = SD_crc7Publish(SD_crc7Update(CMD24, sizeof(CMD24)-2, 0)); // compute CRC for this address.
	CMD24[6] = 0xFF;//address an integer denoting the sector address.
	MSS_SPI_transfer_block(&g_mss_spi1, tmp_buff, 512, 0, 0); //dummy bytes while CS is high (inactive) per: https://electronics.stackexchange.com/questions/303745/sd-card-initialization-problem-cmd8-wrong-response
	MSS_SPI_set_slave_select(&g_mss_spi1, slave); //tie low
	MSS_SPI_transfer_block(&g_mss_spi1, CMD24, 7, rx_buffer, sizeof(rx_buffer));

	i = SD_get_status(slave);
	i = SD_get_status(slave);
	i = SD_get_status(slave);

	uint8_t flag = SD_START_FLAG;
	uint8_t stop = SD_END_FLAG;
	if(rx_buffer[0] == 0){
//		Utils_SPI_Transmit_Block(&g_mss_spi1, tmp_buff, 100);
//		MSS_SPI_transfer_block(&g_mss_spi1, tmp_buff, 10, 0, 0);
		i=0;
		memset(rx_buffer, 0, sizeof(rx_buffer));
		MSS_SPI_transfer_block(&g_mss_spi1, &flag, 1, rx_buffer, 0);
		MSS_SPI_transfer_block(&g_mss_spi1, data, write_limit, rx_buffer, 0);
//		MSS_SPI_transfer_block(&g_mss_spi1, &stop, 1, rx_buffer, sizeof(rx_buffer));
		MSS_SPI_transfer_block(&g_mss_spi1, tmp_buff, SD_CARD_SINGLE_BLOCK_SIZE - write_limit + 2, rx_buffer, sizeof(rx_buffer));
//		while(loop_ctrl) {
//			MSS_SPI_transfer_block(&g_mss_spi1, CMD24, 7, rx_buffer, sizeof(rx_buffer));
//			if(rx_buffer[0] == 0) {
//				MSS_SPI_transfer_block(&g_mss_spi1, &flag, 1, rx_buffer, 0);
//				MSS_SPI_transfer_block(&g_mss_spi1, data, write_limit, rx_buffer, 0);
//		//		MSS_SPI_transfer_block(&g_mss_spi1, &stop, 1, rx_buffer, sizeof(rx_buffer));
//				MSS_SPI_transfer_block(&g_mss_spi1, tmp_buff, SD_CARD_SINGLE_BLOCK_SIZE - write_limit + 2, rx_buffer, sizeof(rx_buffer));
//			}
			i = SD_get_status(slave);
//		}
//		MSS_SPI_transfer_block(&g_mss_spi1, CMD24, 7, rx_buffer, sizeof(rx_buffer));
//		MSS_SPI_transfer_block(&g_mss_spi1, &flag, 1, rx_buffer, 0);
//		MSS_SPI_transfer_block(&g_mss_spi1, data, write_limit, rx_buffer, 0);
//		MSS_SPI_transfer_block(&g_mss_spi1, &stop, 1, rx_buffer, sizeof(rx_buffer));
//		MSS_SPI_transfer_block(&g_mss_spi1, tmp_buff, SD_CARD_SINGLE_BLOCK_SIZE - write_limit + 2, rx_buffer, sizeof(rx_buffer));

		res = (rx_buffer[0] & 0x0f);

//		Utils_SPI_Transmit_Block(&g_mss_spi1, tmp_buff, 192);
		MSS_SPI_transfer_block(&g_mss_spi1, tmp_buff, 512, 0, 0);

//		uint8_t ACMD13[] = {0x4D, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff};
//		MSS_SPI_transfer_block(&g_mss_spi1, ACMD13, 7, rx_buffer, 2);	// rx_buffer should be 0x00 0x00
		memset(rx_buffer, 0, sizeof(rx_buffer));
	}
	MSS_SPI_clear_slave_select(&g_mss_spi1, slave); //tie high
	return res == 0x05;
}

uint8_t SD_Read_Sector(uint8_t * data, uint32_t addr, mss_spi_slave_t slave, uint16_t read_limit){
	uint8_t rx_buffer[100];
	uint16_t i;
	uint8_t loop_ctrl = 1;
	CMD17[1] = (addr >> 24);
	CMD17[2] = (addr >> 16);
	CMD17[3] = (addr >> 8);
	CMD17[4] = addr;

	MSS_SPI_set_slave_select(&g_mss_spi1, slave); //tie low
	while (loop_ctrl){
		MSS_SPI_transfer_block(&g_mss_spi1, CMD17, 7, rx_buffer, sizeof(rx_buffer));
		i = SD_get_status(slave);
	}
	i=0;
	if(rx_buffer[0]!=0){
		return 0;
	}
	do{
		SD_Status = MSS_SPI_transfer_frame(&g_mss_spi1, 0xFF);
		i++;
	}while(SD_Status != SD_START_FLAG && i < 50);
	if(i >= 50){
		return 0;
	}

	MSS_GPIO_set_output( GPIO_CTRL_MOSI_SD, 1);
	Utils_SPI_Receive_Block(&g_mss_spi1, data, read_limit);
//	MSS_SPI_transfer_block(&g_mss_spi1, 0, 0, data, read_limit);
	MSS_GPIO_set_output( GPIO_CTRL_MOSI_SD, 0);

	Utils_SPI_Transmit_Block(&g_mss_spi1, tmp_buff, SD_CARD_SINGLE_BLOCK_SIZE - read_limit + 2);
//	MSS_SPI_transfer_block(&g_mss_spi1, tmp_buff, SD_CARD_SINGLE_BLOCK_SIZE - read_limit + 2, 0, 0);

	MSS_SPI_clear_slave_select(&g_mss_spi1, slave); //tie high
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

uint8_t SD_Set_Highest_Clock(mss_spi_slave_t slave){
	Globals.SD_Clk_Div = 40;
	uint16_t i;	/* The initial clock divider value to be chosen on the basis of the base clock*/
	uint8_t data[SD_TEST_DATA_SIZE+1];
	uint8_t rx_data[SD_TEST_DATA_SIZE];
	data[0]=0xFE;
	for (i =0; i<SD_TEST_DATA_SIZE; i++){
		data[i]=i%10;
		rx_data[i] = 0;
	}
	data[SD_TEST_DATA_SIZE] = 0xff;

	do{
		MSS_SPI_configure_master_mode
		(
			&g_mss_spi1,
			slave,
			MSS_SPI_MODE0,
			Globals.SD_Clk_Div,
			8);

		i = SD_Write_Data(data, SD_TEST_DATA_SIZE, 0x00000001, slave);	/* 0x00000200 and 0x00000400 are dummy addresses to be used for this purpose and not for data storage*/
		if(i == 0x00000001)
			break;
		i = SD_Read_Data(rx_data, SD_TEST_DATA_SIZE, 0x00000001, slave);
		if(i == 0x00000001)
			break;
		i = FLASH_Verify_write(data, rx_data, SD_TEST_DATA_SIZE);
		if(i != SD_TEST_DATA_SIZE)
			break;

		memset(rx_data, 0, SD_TEST_DATA_SIZE);

		Globals.SD_Clk_Div -= 2;
	}while(i == SD_TEST_DATA_SIZE && Globals.SD_Clk_Div >= 10);

	Globals.SD_Clk_Div += 2;
	MSS_SPI_configure_master_mode
	(
		&g_mss_spi1,
		slave,
		MSS_SPI_MODE0,
		Globals.SD_Clk_Div,
		8);
	i = SD_Write_Data(data, SD_TEST_DATA_SIZE, 0x00000001, slave);
	i = SD_Read_Data(rx_data, SD_TEST_DATA_SIZE, 0x00000001, slave);
	i = FLASH_Verify_write(data, rx_data, SD_TEST_DATA_SIZE);
	if(i == SD_TEST_DATA_SIZE)
		return Globals.SD_Clk_Div;
	else
		return 0;
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
	Globals.Current_SD = SD_Select_Init();
	Globals.SD_FDRI_Module_Sync.CMD_Seq_Count = 0;

}

uint16_t SD_get_status(mss_spi_slave_t slave) {
	uint8_t CMD13[] = {0x4D, 0, 0, 0, 0, 0x0D, 0xFF};
	uint8_t rx_buffer[10];
	MSS_SPI_set_slave_select(&g_mss_spi1, slave); //tie low

	MSS_SPI_transfer_block(&g_mss_spi1, CMD13, 7, rx_buffer, sizeof(rx_buffer));
	return (uint16_t) (rx_buffer[0] << 8 | rx_buffer[1]);
}

//TODO: write and test and replace if it works

uint8_t SD_Init_new(mss_spi_slave_t slave) {
	memset(tmp_buff, 0xff, SD_CARD_SINGLE_BLOCK_SIZE);

	CMD17[0] = 0x51;
	CMD17[6] = 0xff;
	CMD17[5] = 0x55;

	MSS_SPI_init( &g_mss_spi1 );

	//select
	MSS_SPI_configure_master_mode
	(
		&g_mss_spi1,
		slave,
		MSS_SPI_MODE0,
		SD_CARD_SINGLE_BLOCK_SIZE,
		8);

	uint8_t rx_buffer[5];
	uint8_t CMD0[] = {0x40, 0x00,0x00, 0x00,0x00, 0x95,0xFF};
	uint32_t i = 0;

	MSS_SPI_set_slave_select(&g_mss_spi1, slave); //tie low

	Utils_SPI_Transmit_Block(&g_mss_spi1, tmp_buff, 100);
//	MSS_SPI_transfer_block(&g_mss_spi1, tmp_buff, 100, 0, 0);

	do{		// add counter for number of trying
		MSS_SPI_transfer_block(&g_mss_spi1, CMD0, 7, rx_buffer, 1);
		i += 1;
	}while(rx_buffer[0]!=1 && i < 200);// rx_buffer[0]!=1  or  i!=200 //sometimes CMD0 failed

	if(i >= 200){
		return 0;
	}
	i = 0;

	uint8_t rx_b[5];
	uint8_t CMD8[] = {0x48, 0x00, 0x00, 0x01, 0xAA, 0x87, 0xFF};	// last 0xFF is use to give sd card buffer clock
	do{
		MSS_SPI_transfer_block(&g_mss_spi1, CMD8, 7, rx_b, 5);             //checked ,return 0X1AA
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

	// sending CMD55 and then ACMD41 to get it out  of idle mode.
	uint8_t CMD55[] = {0x77, 0x00, 0x00, 0x00, 0x00, 0x65, 0xFF};	// last 0xFF is use to give sd card buffer clock
	uint8_t ACMD41[] = {0x69, 0x40, 0x10, 0x00, 0x00, 0xCD, 0xFF};	// last 0xFF is use to give sd card buffer clockACMD41[] = {0x69, 0x41, 0x00, 0x00, 0x00, 0xFF,0xFF}
	do{
		i = i+1;
		MSS_SPI_transfer_block(&g_mss_spi1, CMD55, 7, rx_buffer, 1);
		MSS_SPI_transfer_frame(&g_mss_spi1, 0xFF);
		MSS_SPI_transfer_block(&g_mss_spi1, ACMD41, 7, rx_buffer, 1);
		MSS_SPI_transfer_frame(&g_mss_spi1, 0xFF);

	}while(rx_buffer[0] && i < 5); //any of them is 0 (i=4 or rx_buffer[0]=0), get out of the loop //stucked sometimes

	if(i >= 5)
		return 0;

	uint8_t CMD58[] = {0x7A, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF};
	MSS_SPI_transfer_block(&g_mss_spi1, CMD58, 7, rx_buffer, 5);
	MSS_SPI_transfer_frame(&g_mss_spi1, 0xFF);

	MSS_SPI_clear_slave_select(&g_mss_spi1, slave); //tie high

	return 1;
}


// **************** IS-4 CRC7 Code ****************** //

uint8_t crc7_lookup[] = {
 0x00, 0x12, 0x24, 0x36, 0x48, 0x5A, 0x6C, 0x7E, 0x90, 0x82, 0xB4, 0xA6, 0xD8, 0xCA, 0xFC, 0xEE,
 0x32, 0x20, 0x16, 0x04, 0x7A, 0x68, 0x5E, 0x4C, 0xA2, 0xB0, 0x86, 0x94, 0xEA, 0xF8, 0xCE, 0xDC,
 0x64, 0x76, 0x40, 0x52, 0x2C, 0x3E, 0x08, 0x1A, 0xF4, 0xE6, 0xD0, 0xC2, 0xBC, 0xAE, 0x98, 0x8A,
 0x56, 0x44, 0x72, 0x60, 0x1E, 0x0C, 0x3A, 0x28, 0xC6, 0xD4, 0xE2, 0xF0, 0x8E, 0x9C, 0xAA, 0xB8,
 0xC8, 0xDA, 0xEC, 0xFE, 0x80, 0x92, 0xA4, 0xB6, 0x58, 0x4A, 0x7C, 0x6E, 0x10, 0x02, 0x34, 0x26,
 0xFA, 0xE8, 0xDE, 0xCC, 0xB2, 0xA0, 0x96, 0x84, 0x6A, 0x78, 0x4E, 0x5C, 0x22, 0x30, 0x06, 0x14,
 0xAC, 0xBE, 0x88, 0x9A, 0xE4, 0xF6, 0xC0, 0xD2, 0x3C, 0x2E, 0x18, 0x0A, 0x74, 0x66, 0x50, 0x42,
 0x9E, 0x8C, 0xBA, 0xA8, 0xD6, 0xC4, 0xF2, 0xE0, 0x0E, 0x1C, 0x2A, 0x38, 0x46, 0x54, 0x62, 0x70,
 0x82, 0x90, 0xA6, 0xB4, 0xCA, 0xD8, 0xEE, 0xFC, 0x12, 0x00, 0x36, 0x24, 0x5A, 0x48, 0x7E, 0x6C,
 0xB0, 0xA2, 0x94, 0x86, 0xF8, 0xEA, 0xDC, 0xCE, 0x20, 0x32, 0x04, 0x16, 0x68, 0x7A, 0x4C, 0x5E,
 0xE6, 0xF4, 0xC2, 0xD0, 0xAE, 0xBC, 0x8A, 0x98, 0x76, 0x64, 0x52, 0x40, 0x3E, 0x2C, 0x1A, 0x08,
 0xD4, 0xC6, 0xF0, 0xE2, 0x9C, 0x8E, 0xB8, 0xAA, 0x44, 0x56, 0x60, 0x72, 0x0C, 0x1E, 0x28, 0x3A,
 0x4A, 0x58, 0x6E, 0x7C, 0x02, 0x10, 0x26, 0x34, 0xDA, 0xC8, 0xFE, 0xEC, 0x92, 0x80, 0xB6, 0xA4,
 0x78, 0x6A, 0x5C, 0x4E, 0x30, 0x22, 0x14, 0x06, 0xE8, 0xFA, 0xCC, 0xDE, 0xA0, 0xB2, 0x84, 0x96,
 0x2E, 0x3C, 0x0A, 0x18, 0x66, 0x74, 0x42, 0x50, 0xBE, 0xAC, 0x9A, 0x88, 0xF6, 0xE4, 0xD2, 0xC0,
 0x1C, 0x0E, 0x38, 0x2A, 0x54, 0x46, 0x70, 0x62, 0x8C, 0x9E, 0xA8, 0xBA, 0xC4, 0xD6, 0xE0, 0xF2
};



unsigned char SD_crc7Rotate(unsigned char crc, unsigned char nxt_byte) {
	unsigned char i, highest_bit;
	for (i = 0; i < 8; ++i) {
		highest_bit = crc >> 7;
		crc = (crc << 1) + ((nxt_byte >> (7-i)) & 0x01);
		if(highest_bit) {
			crc ^= SD_CRC7_POLY;
		}
	}
	return crc;
}

unsigned char SD_crc7Update(void* msg, unsigned long length, unsigned char crc) {
 unsigned long i = 0;
 char* message = (char*) msg;
 for (i = 0; i < length; ++i)
   crc = crc7_lookup[crc] ^ message[i];
 return crc;
}

unsigned char SD_crc7Publish(unsigned char crc) {
 crc = SD_crc7Rotate(crc, 0);
 crc |= 0x01;
 return crc;
}


#endif
