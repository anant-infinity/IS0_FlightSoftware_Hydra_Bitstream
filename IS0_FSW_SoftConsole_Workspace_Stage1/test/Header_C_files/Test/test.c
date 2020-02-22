
#ifndef _TEST_C
	#define _TEST_C

#include "test.h"
#include <stdint.h>
#include "Seq_ops/main_seq.h"
#include "SPI_Flash/spi_flash.h"
#include "../mss_spi/mss_spi.h"
#include "../mss_uart/mss_uart.h"
#include "EPS/eps.h"
#include "CCSDS/ccsds.h"
#include "SD_CARD/sd.h"
#include "RTC/rtc.h"
#include "GPIOs/gpios.h"
#include "math.h"
#include "string.h"
#include "Utils/utils.h"
#include "I2C/i2c.h"
#include <assert.h>
#include "../../t1_hw_platform.h"
#include "SENSOR_BOARD/sensor_board.h"



#define BUFFER_A_SIZE   2

#define FLASH_MANUFACTURER_ID   (uint8_t)0x01
#define FLASH_DEVICE_ID         (uint8_t)0x17

static uint8_t g_flash_wr_buf[BUFFER_A_SIZE];
static uint8_t g_flash_rd_buf[BUFFER_A_SIZE];


//********************************** Added for EPS Testing  ********************************************\\
//Read the EPS Slaves to get the HK data

/*------------------------------------------------------------------------------
 * Instance data for our CoreI2C devices
 */
i2c_instance_t g_core_i2c0;				// Core I2C for EPS_C0;
i2c_instance_t g_core_i2c1; 			// Core I2C for EPS_C1;
i2c_instance_t g_core_i2c2; 			// Core I2C for EPS_NC;
i2c_instance_t g_core_i2c3; 			// Core I2C for SENSOR_0;






	/*------------------------------------------------------------------------------
	 * MSS UART instance for UART1
	 */
	mss_uart_instance_t * const gp_my_uart = &g_mss_uart1;

	//Main Function of the example project started here
	    i2c_status_t instance;
	    uint8_t rx_size=0;
	    uint8_t rx_buff[1];
	    //uint8_t loop_count;







void test_IS0_Read_Sensor_Board_Data() {


	uint8_t tx_buffer[TX_LENGTH] = {SENSOR_DEVICE_ID};
	uint8_t tx_buffer_2[TX_LENGTH] = {SENSOR_AS72651_STATUS_REG};

	uint8_t write_length = TX_LENGTH;
	uint8_t write_length_2 = TX_LENGTH_2;

	uint16_t rx_buffer[RX_LENGTH] = {0x00};
	uint8_t read_length = RX_LENGTH;

	uint8_t rx_buffer_2[RX_LENGTH_2] = {0x00 , 0x00};
	uint8_t read_length_2 = RX_LENGTH_2;


	     //Logging UART Initialization
		MSS_UART_init(&g_mss_uart0,MSS_UART_115200_BAUD,MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY | MSS_UART_ONE_STOP_BIT);

/*	    -------------------------------------------------------------------------
	     * Initialize the Core I2C Drivers*/

	    I2C_init(&g_core_i2c0, COREI2C_0, MASTER_SER_ADDR, I2C_PCLK_DIV_960);
	    I2C_init(&g_core_i2c1, COREI2C_1, MASTER_SER_ADDR, I2C_PCLK_DIV_960);
	    I2C_init(&g_core_i2c2, COREI2C_2, MASTER_SER_ADDR, I2C_PCLK_DIV_960);
	    I2C_init(&g_core_i2c3, COREI2C_3, MASTER_SER_ADDR, I2C_PCLK_DIV_960);

/*
	    -------------------------------------------------------------------------
	     * Initialize the system tick for 10mS operation or 1 tick every 100th of
	     * a second and also make sure it is lower priority than the I2C IRQs.
*/

	    //NVIC_SetPriority(SysTick_IRQn, 0xFFu); /* Lowest possible priority
	    //SysTick_Config(MSS_SYS_M3_CLK_FREQ / 100);

	    MSS_GPIO_init();
	    MSS_GPIO_config( MSS_GPIO_22, MSS_GPIO_OUTPUT_MODE);
	    MSS_GPIO_config( MSS_GPIO_25, MSS_GPIO_OUTPUT_MODE);
	    MSS_GPIO_config( MSS_GPIO_26, MSS_GPIO_OUTPUT_MODE);
	    MSS_GPIO_config( MSS_GPIO_30, MSS_GPIO_OUTPUT_MODE);



	    	//Sensor Board Testing Code
			//Enable Sensor Board - H1 18
	    	//MSS_GPIO_set_output( MSS_GPIO_22, 0);
			MSS_GPIO_set_output( MSS_GPIO_22, 1);

			//Reset Sensor Sensor Board - H2 50 - ALWAYS PULL HIGH
			MSS_GPIO_set_output( MSS_GPIO_25, 1);

			//Flash Write Protect of Sensor Board - H1 49 - ALWAYS PULL HIGH for reprogramming - CHECK THIS
			//MSS_GPIO_set_output( MSS_GPIO_22, 0);
			MSS_GPIO_set_output( MSS_GPIO_26, 0);

			//MSS_GPIO_30 For Chip select of the SPI
			MSS_GPIO_set_output( MSS_GPIO_30, 1);

			while(1){
			//VEML6075 Interfacing Code
			// By default VEML6075 seems to go into shut down mode , so needs to be turned ON by writing 0x0000 in CONF register
			// Working code for VEML Data read, for some reason write only is not working, so used write read
			// Also verify that the data should be 0x00 0x00

			//uint8_t config_buffer[TX_LENGTH_3] = {SENSOR_VEML_UV_CONF, 0x00, 0x00};
			//instance = do_write_read_transaction(&g_core_i2c3, SENSOR_VML, config_buffer, TX_LENGTH_3, rx_buffer, read_length); // This works!
			// Send the Data over the MSS UART for Logging
			//MSS_UART_polled_tx( &g_mss_uart0, rx_buffer, sizeof(rx_buffer) );

			instance = do_write_read_transaction(&g_core_i2c3, SENSOR_VML, SENSOR_DEVICE_ID, write_length, rx_buffer_2, read_length_2); // This works!
			// Send the Data over the MSS UART for Logging
			MSS_UART_polled_tx( &g_mss_uart0, rx_buffer_2, sizeof(rx_buffer_2) );

			//AS72651 Interfacing Code
			//Read Data from the AS72651
			//uint8_t temp_buffer[TX_LENGTH] = {read_AS7265_reg(SENSOR_AS72651_SWVERSION_L)};

			// Send the Data over the MSS UART for Logging
			//MSS_UART_polled_tx( &g_mss_uart0, temp_buffer, sizeof(temp_buffer) );


	    }


}




	void test_ISO_test_beacon_debug_UART() {


		// Defining the TX and RX Buffers
		uint8_t tx_buffer[TX_LENGTH] = {SENSOR_DEVICE_ID};
		uint8_t tx_buffer_2[TX_LENGTH] = {SENSOR_AS72651_STATUS_REG};

		uint8_t write_length = TX_LENGTH;
		uint8_t write_length_2 = TX_LENGTH_2;

		uint16_t rx_buffer[RX_LENGTH];
		uint8_t read_length = RX_LENGTH;

		uint8_t rx_buffer_2[RX_LENGTH_2] = {0x00, 0x00};
		uint8_t read_length_2 = RX_LENGTH_2;


		//Hello World Test
		//MSS_UART_init(&g_mss_uart0,MSS_UART_9600_BAUD,MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY | MSS_UART_ONE_STOP_BIT);
		//uint8_t message[12] = "Hello World";
		//MSS_UART_polled_tx( &g_mss_uart0, message, sizeof(message) );

		//Enable Sensor Board - H1 18
		MSS_GPIO_set_output( MSS_GPIO_22, 0);
		MSS_GPIO_set_output( MSS_GPIO_22, 1);

		//Reset Sensor Sensor Board - H2 50 - ALWAYS PULL HIGH
		MSS_GPIO_set_output( MSS_GPIO_25, 1);

		//Flash Write Protect of Sensor Board - H1 49 - ALWAYS PULL HIGH
		MSS_GPIO_set_output( MSS_GPIO_26, 1);

		//Hydra Test
		uint8_t write_CCSDS_Dummy_Header[] = {0x08, 0x10 , 0x80 , 0x01 , 0x00, 0x0C , 0x00 , 0x00 , 0x00 ,0xFF, 0x00};
		MSS_UART_polled_tx( &g_mss_uart0, write_CCSDS_Dummy_Header, sizeof(write_CCSDS_Dummy_Header) );

		//Collecting EPS Data - BUS Voltage and Current
		instance = do_write_read_transaction(&g_core_i2c1, EPS_VC5_SLAVE_C1, EPS_VC_CH3_BUS_VOLTAGE, write_length, rx_buffer_2, read_length_2);
		MSS_UART_polled_tx( &g_mss_uart0, rx_buffer_2, sizeof(rx_buffer_2) );
		instance = do_write_read_transaction(&g_core_i2c1, EPS_VC5_SLAVE_C1,EPS_VC_CH3_SHUNT_VOLTAGE , write_length, rx_buffer_2, read_length_2);
		MSS_UART_polled_tx( &g_mss_uart0, rx_buffer_2, sizeof(rx_buffer_2) );
		uint8_t write_CCSDS_Dummy_Fletcher[] = {0x0F, 0x0F};
		MSS_UART_polled_tx( &g_mss_uart0, write_CCSDS_Dummy_Fletcher, sizeof(write_CCSDS_Dummy_Fletcher) );

	    //ADC Testing Code - We operate the ADC in mode 2

	     //Perform write operation
	    //instance = do_write_transaction(&g_core_i2c1, EPS_ADC_SLAVE_C1, tx_buffer, write_length);

	     //Perform write-Read operation
	    //instance = do_write_read_transaction(&g_core_i2c1, EPS_ADC_SLAVE_C1, tx_buffer, write_length, rx_buffer, read_length);

	    // Send the Data over the MSS UART for Logging
	    //MSS_UART_polled_tx( &g_mss_uart0, rx_buffer, sizeof(rx_buffer) );

	    //Disable Sband Switch
	    //MSS_GPIO_set_output( MSS_GPIO_2, 1);

	    // Perform write-Read operation
	    //instance = do_write_read_transaction(&g_core_i2c1, EPS_ADC_SLAVE_C1, tx_buffer_2, write_length, rx_buffer, read_length);

	    // Send the Data over the MSS UART for Logging
	    //MSS_UART_polled_tx( &g_mss_uart0, rx_buffer, sizeof(rx_buffer) );

	}

	void test_ISO_PeakPower() {

		//Reset Sensor Sensor Board - H2 50 - ALWAYS PULL HIGH
		MSS_GPIO_set_output( MSS_GPIO_25, 1);

		//Flash Write Protect of Sensor Board - H1 49 - ALWAYS PULL HIGH
		MSS_GPIO_set_output( MSS_GPIO_26, 1);

		//DSW - 0 - H1 13 - DAXSS SWITCH - Enables 8.4V at H1 pins 3 and 5
		MSS_GPIO_set_output( MSS_GPIO_1, 1);

		//DSW - 1 - H1 14 - CIP SWITCH - Enables 12V at H2 pin 23
		MSS_GPIO_set_output( MSS_GPIO_2, 1);

		//DSW - 2 - H1 15 - ADCS_GPS SWITCH - Enables 12V at H1 pin 30
		//Also Enables 12 V at H2 Pin 15, 17, 19 and enables 3.3V at H2 pin 16
		MSS_GPIO_set_output( MSS_GPIO_3, 1);

		//DSW - 3 - H1 16 - SBAND SWITCH - Enables 6V at H2 Pin 11
		MSS_GPIO_set_output( MSS_GPIO_4, 1);


		//DSW - 4 - H1 17 - BATTERY HEATER SWITCH -
		MSS_GPIO_set_output( MSS_GPIO_5, 1);

		//Enable Sensor Board - H1 18
		MSS_GPIO_set_output( MSS_GPIO_22, 1);

		MSS_GPIO_set_output( MSS_GPIO_22, 0);
		MSS_GPIO_set_output( MSS_GPIO_1, 0);
		MSS_GPIO_set_output( MSS_GPIO_5, 0);
		MSS_GPIO_set_output( MSS_GPIO_4, 0);
		MSS_GPIO_set_output( MSS_GPIO_3, 0);
		MSS_GPIO_set_output( MSS_GPIO_2, 0);

	}

	void test_ISO_GPIO_Subsystem_Enable_GPIOs() {


		//DSW - 0 - H1 13 - DAXSS SWITCH - Enables 8.4V at H1 pins 3 and 5
		MSS_GPIO_set_output( MSS_GPIO_1, 1);
		MSS_GPIO_set_output( MSS_GPIO_1, 0);

		//DSW - 1 - H1 14 - CIP SWITCH - Enables 12V at H2 pin 23
		MSS_GPIO_set_output( MSS_GPIO_2, 1);
		MSS_GPIO_set_output( MSS_GPIO_2, 0);

		//DSW - 2 - H1 15 - ADCS_GPS SWITCH - Enables 12V at H1 pin 30
		//Also Enables 12 V at H2 Pin 15, 17, 19 and enables 3.3V at H2 pin 16
		MSS_GPIO_set_output( MSS_GPIO_3, 1);
		MSS_GPIO_set_output( MSS_GPIO_3, 0);

		//DSW - 3 - H1 16 - SBAND SWITCH - Enables 6V at H2 Pin 11
		MSS_GPIO_set_output( MSS_GPIO_4, 1);
		MSS_GPIO_set_output( MSS_GPIO_4, 0);

		//DSW - 4 - H1 17 - BATTERY HEATER SWITCH -
		MSS_GPIO_set_output( MSS_GPIO_5, 1);
		MSS_GPIO_set_output( MSS_GPIO_5, 0);

		//To Test Tomorrow

		//CONVEST Signal for ADC - H1 6
		MSS_GPIO_set_output( MSS_GPIO_6, 1);
		MSS_GPIO_set_output( MSS_GPIO_6, 0);

		//Enable Sensor Board - H1 18
		MSS_GPIO_set_output( MSS_GPIO_22, 1);
		MSS_GPIO_set_output( MSS_GPIO_22, 0);

		//Reset Sensor Sensor Board - H2 50 - ALWAYS PULL HIGH
		MSS_GPIO_set_output( MSS_GPIO_25, 1);
		MSS_GPIO_set_output( MSS_GPIO_25, 0);
		MSS_GPIO_set_output( MSS_GPIO_25, 1);

		//Flash Write Protect of Sensor Board - H1 49 - ALWAYS PULL HIGH
		MSS_GPIO_set_output( MSS_GPIO_26, 1);
		MSS_GPIO_set_output( MSS_GPIO_26, 0);
		MSS_GPIO_set_output( MSS_GPIO_26, 1);



		}



//*************************** OLD IS1 Tests ******************************//
void test_main_seq(){
	main_seq();
}


void test_SD_Card(){
	Global_Init_GPIOs();
	uint8_t sig = 0;
	Globals.Current_SD = SD_Select_Init();

	uint16_t data_L = 512;uint64_t i;
	uint8_t w_data[data_L], r_data[data_L];
	for (i =0; i<data_L; i++){
		w_data[i]=i%9;
		r_data[i] = 0;
	}

	i = SD_Write_Data(w_data, data_L, 4, Globals.Current_SD);

	while(1){
		sig = 1;
		PWR_Switch(GPIO_DSW0_DAXSS_Mask, sig);
		i = SD_Read_Data(r_data, data_L, 4, Globals.Current_SD);
		sig = 0;
		PWR_Switch(GPIO_DSW0_DAXSS_Mask, sig);
		i = FLASH_Verify_write(w_data, r_data, data_L);
		if( i != data_L){
			sig = !sig;
		}
		memset(r_data, 0, data_L);
	}
}

void test_SD_Write_Packets() {
	Init();
	uint8_t beac_write_addr = Globals.Beacon_Write_Start;
	uint8_t read_data[512], j;
	uint16_t i=0,err=0;
	while(1) {

		if (beac_write_addr < Globals.Beacon_Write_Start) {
			j = SD_Read_Sector(read_data, beac_write_addr, Globals.Current_SD, 512);
			if (!j) {
				j = 1;
				j = 0;
			}
			beac_write_addr = Globals.Beacon_Write_Start;
			for (i=0,err=0; i<BEACON_PACKET_SIZE; i++) {
				if (read_data[i+2] != ((uint8_t *)&Beacon_pack_IS0)[i]){
					err++;
				}
			}
			if (err > 5) {
				return;
			}

		}
	}
}

//pass in what slot the SD CUT (SD Card Under Test) is in (0 or 1), and its size (16 or 128).
int test_SD_All_Sectors(mss_spi_slave_t slot, uint16_t sd_size) {
	Global_Init_GPIOs();
	MSS_UART_init(&g_mss_uart0,MSS_UART_115200_BAUD,MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY | MSS_UART_ONE_STOP_BIT);

//	SD_enable(slot);
	SD_Cards_Init();

	// not sure if this really does anything when running in an automated way,
	// but it does work to randomize the test data in a lab bench setting.
	srand(RTC_Get_Value32());
	int rand_seed = rand();


	uint32_t sector,i;
	uint8_t data[SD_TEST_DATA_SIZE];
	uint8_t rx_data[SD_TEST_DATA_SIZE+2];
	int write_errs=0,read_errs=0,ver_errs=0;

	// init data buffers
	for (i = 0; i < SD_TEST_DATA_SIZE; i++,rand_seed++) {
		data[i] = rand_seed % 128;
		rx_data[i] = 0;
	}

	if(!SD_Init(slot)) {
		MSS_UART_polled_tx(&g_mss_uart0, "Unable to init card", sizeof("Unable to init card"));
	}
	uint32_t max_size = sd_size == 16 ? SD_MAX_SECTOR_VALUE_16 : SD_MAX_SECTOR_VALUE_128;
	for (sector = 4; sector < max_size; sector++) {
		i = SD_Write_Data(data, SD_TEST_DATA_SIZE, sector, slot);
		if (i != sector+1)
			write_errs++;
		i = SD_Read_Data(rx_data, 512, sector, slot);
		if (i != sector + 1)
			read_errs++;
		i = FLASH_Verify_write(data, rx_data, SD_TEST_DATA_SIZE);
		if (i != SD_TEST_DATA_SIZE)
			ver_errs++;

		int total_errs = write_errs + read_errs + ver_errs;

		if(sector%16 == 0){
			if (!total_errs) {
				MSS_UART_polled_tx(&g_mss_uart0, ".", 1);
			}
			else {
				MSS_UART_polled_tx(&g_mss_uart0, (uint8_t *)&sector, sizeof(sector));
				MSS_UART_polled_tx(&g_mss_uart0, (uint8_t *)&write_errs, sizeof(write_errs));
				MSS_UART_polled_tx(&g_mss_uart0, (uint8_t *)&total_errs, sizeof(total_errs));
			}
		}

		//change data buffers for accuracy of test
		for (i = 0; i < SD_TEST_DATA_SIZE; i++) {
				data[i] = i % 128 + sector;
				rx_data[i] = 0;
			}
	}

	SD_Card_Wipe(slot);
	MSS_UART_polled_tx(&g_mss_uart0, (uint8_t *)write_errs, sizeof(write_errs));

	return write_errs;

}

void test_save_parameter_character(){
	Global_Init_GPIOs();
	Flash_Init();
	while(1){
		Load_Factory_Value();
//		if(Globals.Param_Module_sync.CMD_Seq_Count == FLASH_STATE_IDLE){
//			Globals.Param_Module_sync.CMD_Seq_Count = FLASH_STATE_ERASE_CMD;
//		}
		statusG = Save_Parameter_Table();
//		statusG = Load_Parameter_Table();

		if(statusG == 0){
			continue;
		}
		if(Globals.Subsystem_Restart_Limit != 0x03){
			continue;
		}
	}
}


void test_RTC(){
	uint8_t v[] = {0x01, 0x02, 0x03, 0x04, 0x05};
	uint8_t r[] = {0x00, 0x00, 0x00, 0x00, 0x00};
	uint64_t val = 0;
	RTC_Reset();
	val = RTC_Get_Value64();	// Gets value from counter 1
	RTC_Get_Value_Pointer1(r);	// Gets value from counter 2

	RTC_Enable();
	RTC_Set_Value(v,5);
	while(1){
		// Test to check if reset and enable function works properly
		/*RTC_Get_Value_Pointer1(r);	// Fetch absolute rtc value
		RTC_Set_Value(uint8_t * v, uint8_t len)
		d = RTC_Get_Value64();	// Fetch process rtc value
		d = RTC_Reset();
		d = RTC_Enable();
		RTC_Get_Value_Pointer1(r);	// Fetch absolute rtc value
		d = RTC_Get_Value64();	// Fetch process rtc value*/

		// Test to check if enable function works properly
		/*d = RTC_Enable();
		d = RTC_Get_Value64();
		d = RTC_Get_Value64();
		d = RTC_Get_Value64();
		d = RTC_Get_Value64();
		RTC_Get_Value_Pointer1(r);
		RTC_Get_Value_Pointer1(r);
		RTC_Get_Value_Pointer1(r);*/

		// Test to check if Set Value fnction works properly
		/*RTC_Disable();
		d = RTC_Set_Value(v, sizeof(v));
		d = RTC_Get_Value64();
		d = RTC_Get_Value64();
		d = RTC_Get_Value64();*/
		/*d = RTC_Enable();
		for (int i=0;i<10;i++){
		RTC_Get_Value_Pointer1(r);
		//r[0] = 0x00; r[1] =  0x00; r[2] = 0x00; r[3] = 0x00; r[4] = 0x00;
		}*/

		/*RTC_Get_Value_Pointer1(r);
		RTC_Get_Value_Pointer1(r);*/

		// Keep reading values and see if increments
		/*RTC_Get_Value_Pointer1(r);
		d = RTC_Get_Value64();
		d = RTC_Get_Value64();
		d = RTC_Get_Value64();
		d = RTC_Get_Value64();
		RTC_Get_Value_Pointer1(r);*/

		// Generic RTC test routine
		val = RTC_Get_Value64();	// Gets value from counter 1
		RTC_Get_Value_Pointer1(r);	// Gets value from counter 2

		//RTC_Set_Value(v,5);			// Resets counter 2 value to v


	}
}




void spi_flash_test2(){
    volatile uint32_t errors = 0;
    uint32_t address = 0;
    uint16_t loop_count;

    for(loop_count = 0; loop_count < (BUFFER_A_SIZE/2); loop_count++)
    {
        g_flash_wr_buf[loop_count] = 0+loop_count;
        g_flash_rd_buf[loop_count] = 0x00;
    }
    for(loop_count = (BUFFER_A_SIZE/2); loop_count < BUFFER_A_SIZE; loop_count++)
    {
        g_flash_wr_buf[loop_count] = 0x33;
        g_flash_rd_buf[loop_count] = 0x00;
    }
    Flash_Init();

    while(1){
        uint8_t res[10];
        uint8_t cmd[10];

        MSS_SPI_set_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
        Flash_Write_EN_DIS(FLASH_WREN);
        MSS_SPI_clear_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );

        cmd[0] = FLASH_RDSR1;
        Flash_CMD(cmd, 1, res, 1);

        uint8_t res2 = Flash_Read_Status_Register_1();
        res2 = Flash_Read_Status_Register_1();

        MSS_SPI_set_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
        Flash_Write_EN_DIS(FLASH_WRDI);
        MSS_SPI_clear_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );

        res2 = Flash_Read_Status_Register_1();
        res2 = Flash_Read_Status_Register_1();



//      cmd[0] = FLASH_WRR;cmd[1] = 0;
//      Flash_CMD(cmd, 2, 0,0,0);

        Flash_CMD(cmd, 1, res, 1);
        cmd[0] = FLASH_RDSR2;
        Flash_CMD(cmd, 1, res, 1);
        cmd[0] = FLASH_RDCR1;
        Flash_CMD(cmd, 1, res, 1);

        cmd[0] = FLASH_READ_ID; cmd[1] = 0; cmd[2] = 0; cmd[3] = 0; cmd[4] = 0;
        Flash_CMD(cmd, 5, res, 2);


        address = 0x00020000;
        uint32_t stat = Flash_Erase(FLASH_SE, address);
        uint8_t p;
        cmd[0] = FLASH_RDSR1;
//      while(1){
            MSS_GPIO_set_output(MSS_GPIO_1, 1);

            Flash_CMD(cmd, 1, res, 1);
//          p = Flash_CMD_Read();
//          MSS_SPI_set_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
//          p=FLASH_get_status();
//          MSS_SPI_clear_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
//          Flash_Read_Data(0x00020000, g_flash_rd_buf, sizeof(g_flash_wr_buf));
            MSS_GPIO_set_output(MSS_GPIO_1, 0);
//      }

        address = 0x00030000;
        stat = Flash_Erase(FLASH_SE, address);
        Flash_Read_Data(0x00030000, g_flash_rd_buf, sizeof(g_flash_wr_buf));
        address = 0x00040000;
        stat = Flash_Erase(FLASH_SE, address);
        Flash_Read_Data(0x00040000, g_flash_rd_buf, sizeof(g_flash_wr_buf));

        address = 0x00020000;
        Flash_Program(address, g_flash_wr_buf, sizeof(g_flash_wr_buf));
        Flash_Read_Data(address, g_flash_rd_buf, sizeof(g_flash_wr_buf));
        errors = FLASH_Verify_write(g_flash_rd_buf, g_flash_wr_buf, sizeof(g_flash_wr_buf));

        address = 0x00030000;
        Flash_Program(address, g_flash_wr_buf, sizeof(g_flash_wr_buf));
        Flash_Read_Data(address, g_flash_rd_buf, sizeof(g_flash_wr_buf));
        errors = FLASH_Verify_write(g_flash_rd_buf, g_flash_wr_buf, sizeof(g_flash_wr_buf));

        address = 0x00040000;
        Flash_Program(address, g_flash_wr_buf, sizeof(g_flash_wr_buf));
        Flash_Read_Data(address, g_flash_rd_buf, sizeof(g_flash_wr_buf));
        errors = FLASH_Verify_write(g_flash_rd_buf, g_flash_wr_buf, sizeof(g_flash_wr_buf));

        address= 0x00030000;
        Flash_Erase(FLASH_SE, address);
        Flash_Read_Data(address, g_flash_rd_buf, sizeof(g_flash_wr_buf));

        address= 0x00020000;
        Flash_Read_Data(address, g_flash_rd_buf, sizeof(g_flash_wr_buf));
        errors = FLASH_Verify_write(g_flash_rd_buf, g_flash_wr_buf, sizeof(g_flash_wr_buf));

        address= 0x00030000;
        Flash_Read_Data(address, g_flash_rd_buf, sizeof(g_flash_wr_buf));
        errors = FLASH_Verify_write(g_flash_rd_buf, g_flash_wr_buf, sizeof(g_flash_wr_buf));

        address= 0x00040000;
        Flash_Read_Data(address, g_flash_rd_buf, sizeof(g_flash_wr_buf));
        errors = FLASH_Verify_write(g_flash_rd_buf, g_flash_wr_buf, sizeof(g_flash_wr_buf));


    }

}






#endif
