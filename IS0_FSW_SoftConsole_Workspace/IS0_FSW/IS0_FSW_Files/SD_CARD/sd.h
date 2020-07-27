/*
 * sd.h
 *
 *  Created on: Jan 25, 2020
 *      Author: Anant
 */

#ifndef _SD_H
	#define _SD_H

#undef  GLOBAL
#ifdef _SD_C
#define GLOBAL
#else
#define GLOBAL  extern
#endif


#define SD_CARD_SPI_BASE_CLOCK 60000000

enum SD_CLOCK_SPEEDS {
	SD_CARD_MINIMUM_SPEED=512,
	SD_CARD_DEFAULT_SPEED=256,
	SD_CARD_1MHZ=60,
	SD_CARD_2MHZ=30,
	SD_CARD_3MHZ=20,
	SD_CARD_5MHZ=12,
	SD_CARD_6MHZ=10,
	SD_CARD_7POINT5MHZ=8,
};

enum SD_CMD_CODES {
	SD_CMD0,
	SD_CMD7,
	SD_CMD14,
};



#define SD_CARD_SINGLE_BLOCK_SIZE 512
#define SD_TEST_DATA_SIZE		512
#define SD_START_FLAG			0xFE

#define SD_MAX_SECTOR_VALUE_128 	249737216		// number pulled from HxD on Transcend 128GB uSD
#define SD_MAX_SECTOR_VALUE_16		30881792		// number pulled from HxD on Transcend 16GB uSD

#define SD_CARD_128GB				128
#define SD_CARD_16GB				16

/*---------------------------------------------------------------------------*/
/* The SD_Init() is responsible for initializing the SD card present on the slave
 * as passed. It configures the SPI as master mode with clock divider value of 
 * 512. Then it sends 100 buffer clocks followed CMD0 to the SD card, at max 200
 * times. If the response of the command is 0x01 then we proceed to send CMD8 to
 * the SD card. The response of the CMD8 should be 0x000001AA. We try CMD8 also
 * at max 200 before exiting the function with value 0. Once the CMD8 is successful
 * the CMD55 followed by ACMD41 is sent. The response of ACMD41 should be 0x00. The
 * CMD55 and ACDM41 is tried 5 times at max before reporting the error. If the 
 * response of the ACMD41 is 0x00 then the function returns 1 indicating the 
 * successful initialization of the SD card. In case of any failure it returns 0.
 * 
 *
 * @param
 * 	mss_spi_slave_t slave: The SD card slave. It should either be 0 or 1.
 *
 * @return: uint8_t
 *	1- if the SD card initialization was successful.
 *	0- if the SD card initialization was unsuccessful.
*/
uint8_t SD_Init(mss_spi_slave_t slave);

/** SD_Simul_Init()
 * All devices on the SPI bus mss_spi1 must be on at the same time, because when
 * powered off, the device will load the bus, meaning we can't communicate
 * with the device that is turned on at the time.
 *
 * Since both SD Cards are on at the same time, we need both to be in SPI mode,
 * because SD mode spits garbage unrequested to the bus
 *
 * SD_Simul_Init() must attempt to put both SD Cards in SPI mode at the same time,
 	 or else one SD Card will spit garbage on the bus, inhibiting comms with the
 	 other devices on the bus (SBAND and other SD)


 */
uint8_t SD_Simul_Init();

uint16_t SD_get_status(mss_spi_slave_t slave);

void SD_clear_slave_select(mss_spi_slave_t slave);
void SD_set_slave_select(mss_spi_slave_t slave);
void SD_toggle_slave_select(mss_spi_slave_t slave);
void SD_enable(mss_spi_slave_t slave);
void SD_disable(mss_spi_slave_t slave);
uint8_t SD_Select_Init();
uint8_t SD_Send_Command(mss_spi_slave_t slave, uint8_t cmd_code);

/*---------------------------------------------------------------------------*/
/* The SD_Write_Sector() writes to a sector of the SD card. It first sends 
 * CMD24, whose response should be 0x00. On getting proper response, the start
 * flag i.e 0xFE is sent followed by the data of size wirte_limit. After that
 * 0xffs equal to the remaining size is sent on which the response 0x05 will  
 * denote the data accepted state of the SD card. Then 512 bytes of buffer clock 
 * is sent. The function returns 1 or 0 depending on the successful write operation
 * 
 *
 * @param
 *	uint8_t * data: The pointer to the array from which the data has to be 
 *					written to the SD card.
 *	uint32_t addr: The address of the sector where the data has to be stored
 * 	mss_spi_slave_t slave: The SD card slave. It should either be 0 or 1.
 *	uint16_t write_limit: The size of data to write. Its maximum value is size of 
 *							a sector i.e. 512.
 *
 * @return: uint8_t
 *	1- The data has been successfully written to the SD card
 *	0- The write operation was unsuccessful.
*/
uint8_t SD_Write_Sector(uint8_t *data, uint32_t addr, mss_spi_slave_t, uint16_t write_limit);

/*---------------------------------------------------------------------------*/
/* The SD_Read_Sector() reads the data from one sector of the SD card. The no.
 * of bytes read is equal to the read_limit. It uses the SD card for the read
 * operation whose slave address is passed as slave parameter. It sends CMD17 
 * to read one sector of the SD card. If the response is 0x00 then we proceed 
 * to read the data. Once 0x00 response is received, at max 50 bytes data is 
 * read with one byte at a time and check if the byte is 0XFE, the start flag.
 * The 0xFE denotes the start of the data. Hence next byte will be data. Hence
 * the data of size equal to read_limit is read and put into the data pointer
 * passed as argument. Then no. of clock equivalent to the remaining no. of 
 * bytes is transfered to complete the read operation.
 * 
 *
 * @param
 *	uint8_t * data: The pointer to the array to which the read data has to be 
 *					stored.
 *	uint32_t addr: The address of the sector from where the data has to be read
 * 	mss_spi_slave_t slave: The SD card slave. It should either be 0 or 1.
 *	uint16_t read_limit: The size of data to read. Its maximum value is size of 
 *							a sector i.e. 512.
 *
 * @return: uint8_t
 *	1- The data has been successfully read from the SD card
 *	0- The read operation was unsuccessful.
*/
uint8_t SD_Read_Sector(uint8_t * data, uint32_t addr, mss_spi_slave_t slave, uint16_t read_limit);

/*---------------------------------------------------------------------------*/
/* The SD_Write_Data() is responsible for writing data from the SD card over 
 * multiple sectors by using SD_Write_Sector() function. The job of this 
 * function is basically pass proper size of the "data" array to the 
 * SD_Write_Sector() function with proper write_limit.
 * 
 *
 * @param
 *	uint8_t * data: The pointer to the array from which the data has to be 
 *					written to the SD card.
 *	uint16_t dataLength: The size of data to write. There is no limit on the 
 *							maximum size of this parameter.
 *	uint32_t addr: The address of the sector where the data has to be stored
 * 	mss_spi_slave_t slave: The SD card slave. It should either be 0 or 1.
 *
 * @return: uint32_t
 *	It returns the address of the SD card sector to which the next write 
 *	operation should be performed.
*/
uint32_t SD_Write_Data(uint8_t * data, uint16_t dataLength, uint32_t addr, mss_spi_slave_t slave);

/*---------------------------------------------------------------------------*/
/* The SD_Read_Data() is responsible for reading data from the SD card over 
 * multiple sectors by using SD_Read_Sector() function. The job of this 
 * function is basically pass proper offset of the "data" array to the 
 * SD_Read_Sector() function with proper read_limit.
 * 
 *
 * @param
 *	uint8_t * data: The pointer to the array to which the read data has to be 
 *					stored.
 *	uint16_t dataLength: The size of data to read. There is no limit on the 
 *							maximum size of this parameter.
 *	uint32_t addr: The address of the sector from where the data has to be read
 * 	mss_spi_slave_t slave: The SD card slave. It should either be 0 or 1.
 *
 * @return: uint32_t
 *	It returns the address of the SD card sector from where the next read 
 *	operation should start.
 *	
*/
uint32_t SD_Read_Data(uint8_t * data, uint16_t dataLength, uint32_t addr, mss_spi_slave_t);

uint8_t SD_Set_Highest_Clock(mss_spi_slave_t slave);

uint8_t SD_Select_Helper(uint8_t curr_slave);

uint32_t SD_Card_Wipe(mss_spi_slave_t slave);

// This function is necessary in order to update the clock divider.
//Normally, the MSS_set_slave_select() updates the clock div before setting the pin low
// But IS-1 got rid of that function in favor of manually controlling SS pin.
// That means that we never set the clock div to a new value unless we manually
// do it.
// This implementation is a call to the normal configure_master_mode function
// while adding in the code from set_slave_select that dealt with the clk_div.
void Custom_SPI_configure_master_mode
(
    mss_spi_instance_t *    this_spi,
    mss_spi_slave_t         slave,
    mss_spi_protocol_mode_t protocol_mode,
    uint32_t                clk_div,
    uint8_t                 frame_bit_length
);


//new SD_Card_Init function to replace Select_Init
void SD_Cards_Init(void);
void SD_FDRI(void);

void SD_Write_Read_Verify();

//GLOBAL uint8_t SD_Clk_Div;
#endif