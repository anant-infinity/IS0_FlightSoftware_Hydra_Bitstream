/*
 * spi_flash.h
 *
 *  Created on: Sep 4, 2018
 *      Author: Ankit
 */

#ifndef _SPI_FLASH_H
#define _SPI_FLASH_H

#include "../mss_spi/mss_spi.h"
#include "../Seq_ops/main_seq.h"

#undef  GLOBAL
#ifdef _SPI_FLASH_C
#define GLOBAL
#else
#define GLOBAL  extern
#endif

/* Commands */
#define FLASH_READ_ID	0x90
#define FLASH_RDID	0x9F
#define FLASH_RDSR1	0x05
#define FLASH_RDSR2	0x07
#define FLASH_RDCR1	0x35
#define FLASH_WRR	0x01
#define FLASH_WRDI	0x04	// write disable
#define FLASH_WREN	0x06	// write enable
#define FLASH_CLSR	0x30	// Clear Status register-1
#define FLASH_ECCRD	0x18	// Read ECC?
#define FLASH_ABRD	0x14	// Read autoboot register
#define FLASH_ABWR	0x15
#define FLASH_BBRD	0x16	// Bank register read
#define FLASH_BRWR	0x17
#define FLASH_BRAC	0xB9	// Bank register access
#define FLASH_DLPRD	0x41	// Data learning pattern read
#define FLASH_PNVDLR	0x43	// Program NV data learning register
#define FLASH_WVDLR	0x4A	// Write volatile data learning register

#define FLASH_READ	0x03
#define FLASH_PP	0x02	// Page program
#define FLASH_P4E	0x20	// Erase 4KB
#define FLASH_BE	0x60	// Bulk erase
#define FLASH_SE	0xD8	// erase 64KB
#define FLASH_ERSP	0x75	// Suspend erase
#define FLASH_ERRS	0x7A	// Erase resume
#define FLASH_OTRP	0x42	// One time program
#define FLASH_OTPR	0x4B	// One time read

#define FLASH_RESET	0xF0	// Software reset
#define FLASH_MBR	0xFF	// Mode bit reset

/*Status Register Masks Default: MSB 000 000 00 LSB*/
#define FLASH_STATUS1_WIP 		0x01	// Write In Progress: Read Only
#define FLASH_STATUS1_WEL		0x02	// Write enable latch up: Read Only
#define FLASH_STATUS1_BP0		0x04	// Block Protect bit
#define FLASH_STATUS1_BP1		0x08	// Block Protect bit
#define FLASH_STATUS1_BP2		0x10	// Block Protect bit
#define FLASH_STATUS1_E_ERR		0x20	// Erase Error Occurred
#define FLASH_STATUS1_P_ERR		0x40	// Programming Error Occurred
#define FLASH_STATUS1_SRWD		0x80	// Status Register Write Disable

/* Default: MSB 00000000 LSB */
#define FLASH_CONFIG1_FREEZE	0x01	// Lock the current state of BP2-0 in status reg1
#define FLASH_CONFIG1_QUAD		0x02	// Put the device into the quad mode
#define FLASH_CONFIG1_TBPARM	0x04	// Configures parameters sector location OTP
#define FLASH_CONFIG1_BPNV		0x08	// Configures BP2-0 to be volatile or non-volatile
#define FLASH_CONFIG1_TBPROT	0x20	// Configure BP2-0 to be bottom(1) or top(0)
#define FLASH_CONFIG1_LC0		0x40	// Latency Code
#define FLASH_CONFIG2_LC1		0x80	// Latency code

#define FLASH_STATUS2_PS		0X01	// Program Suspend
#define FLASH_STATUS2_ES		0x02	// Erase Suspend

/* Write operation state definitions */
#define FLASH_STATE_ERASE_CMD		0x00
#define	FLASH_STATE_WAIT			0X01
#define FLASH_STATE_ERASE_VERIFY	0x02
#define FLASH_STATE_PROGRAM			0x03
#define FLASH_STATE_IDLE			0x04

/* Write operation return definitions */
#define FLASH_WRITE_FAIL			0x00
#define FLASH_WRITE_SUCCESS			0x01
#define FLASH_WRITE_BUSY			0x02
#define FLASH_WRITE_IDLE			0x04
#define FLASH_WRITE_TMP_FAIL		0xff

/*---------------------------------------------------------------------------*/
/* The Flash_Init() function initializes the spi for the flash memory. It checks
 * the status register of the flash, if required then it sends the command to
 * clear the status register. It then initializes the state machine for the 
 * write operation.
 *
 * @param
 * 	No parameter
 *
 * @return: void
 *	
*/
void Flash_Init();

/*---------------------------------------------------------------------------*/
/* The Flash_Wait() keeps on checking the Status register using a do while 
 * loop with the limit on no. of checks by Globals.Flash_Wait_Limit. The function
 * returns the no. of cycle the Status register has been checked before the 
 * ready bit of the status register become 1. The slave select should be called
 * outside this function, as this function doesn't call the slave select.
 *
 * @param
 * 	No parameter
 *
 * @return: uint32_t
 *	It retuns the number of cycle the Status register has been checked before the 
 * ready bit of the status register become 1.
*/
uint32_t Flash_Wait();	

/*---------------------------------------------------------------------------*/
/* The Flash_Write_EN_DIS() is used to write enable or write  disable the flash
 * memory. This function calls the slave select and then sends the command to either
 * write enable or write  disable the flash memory on the basis of the parameter
 * value passed. If en_dis is 1 then it will enable the flash memory, if it is 
 * 0 then it will disable it.
 *
 * @param
 * 	uint8_t en_dis: It can take either one or zero.
 *
 * @return: void
 *	
*/
void Flash_Write_EN_DIS(uint8_t en_dis);	

/*---------------------------------------------------------------------------*/
/* The Flash_CMD() sends the command to the flash memory and receive the response
 * if there are any. After the transaction the Flash_Wait() is called and the value
 * returned by the function is returned. The Flash_CMD() sets the slave select and 
 * clears it any the end of it. 
 *
 * @param
 * 	uint8_t * cmd: The pointer to the command array.
 *	uint8_t cmd_size: The size of command array.
 *	uint8_t * rx: The pointer to the receive buffer
 * 	uint8_t rx_size: The size of the receive buffer. Pass 0 if there is no response
 *						to the command.
 *
 * @return: uint32_t
 *	It returns the value returned by Flash_Wait()
 *	
*/
uint32_t Flash_CMD(uint8_t * cmd, uint8_t cmd_size, uint8_t * rx, uint16_t rx_size);

/*---------------------------------------------------------------------------*/
/* The Flash_Read_Data() sends the command to the flash memory to read the data
 * from a particular sector. Muliple sector read operation is allowed. The 
 * slave select and slave clear is called internally.
 *
 * @param
 * 	uint32_t address: The address of the sector from where the data is to be read
 *	uint8_t * rx_buffer: The pointer to the receive buffer
 * 	uint8_t size_in_bytes: The size of the receive buffer.
 *
 * @return: uint32_t
 *	It returns the value returned by Flash_Wait().
 *	
*/
uint32_t Flash_Read_Data(uint32_t address, uint8_t * rx_buffer, uint16_t size_in_bytes);

/*---------------------------------------------------------------------------*/
/* The Flash_Erase() can be used to erase either whole flash memory or just a
 * particular sector.
 * The slave select and slave clear is called internally.
 *
 * @param
 * 	uint8_t command: The erase command. It can either be FLASH_P4E or FLASH_BE.
 *						In case of FLASH_BE the address value should be greater
 *						than 0x00FFFFFF.
 *	uint32_t address: The address of the sector from which has to be erased. If
 * 						the value is greater than 0x00FFFFFF then whole flash 
 *						memory would be erased.
 *
 * @return: uint32_t
 *	It returns the value returned by Flash_Wait().
 *	
*/
uint32_t Flash_Erase(uint8_t command, uint32_t address);

/*---------------------------------------------------------------------------*/
/* The Flash_Program() writes data to erased sectors. The erase operation has 
 * to be carried out separately. It can write the data spanned over multiple
 * sectors
 * The slave select and slave clear is called internally.
 *
 * @param
 *	uint32_t address: The address of the sector from which has to be erased. If
 * 						its value should be  less than equal to 0x00FFFFFF.
 * 	uint8_t * write_buffer: The pointer to the data to be written.
 *	uint32_t size_in_bytes: The size of the data to be written.
 *
 * @return: uint32_t
 *	It returns the value returned by Flash_Wait().
 *	
*/
uint32_t Flash_Program(uint32_t address, uint8_t * write_buffer, uint32_t size_in_bytes);

/*---------------------------------------------------------------------------*/
/* The Flash_Read_Status_Register_1() returns the value of the status register
 * of the flash memory.
 * The slave select and slave clear is called internally.
 *
 * @param
 *	no parameter
 *
 * @return: uint8_t
 *	It returns the value of the status register.
 *	
*/
uint8_t Flash_Read_Status_Register_1();

/*---------------------------------------------------------------------------*/
/* The FLASH_Verify_write() verifies the data stored in two arrays and returns
 * the number of bytes matching.
 *
 * @param
 *	uint8_t * write_buff: The pointer to the first array.
 *	uint8_t * read_buff: The pointer to the second array.
 *	uint16_t size: The size of the arrays.
 *
 * @return: uint8_t
 *	It returns the number of bytes matching.
 *	
*/
uint16_t FLASH_Verify_write(uint8_t * write_buff, uint8_t * read_buff, uint16_t size);

/*---------------------------------------------------------------------------*/
/* The FLASH_Verify_write_count() verifies if the data stored in the arrays is
 *  matching the passed parameter.
 *
 * @param
 *	uint8_t * write_buff: The pointer to the array.
 *	uint8_t read_buff: The value to which all the array's element will be 
 *						compared with.
 *	uint16_t size: The size of the arrays.
 *
 * @return: uint8_t
 *	It returns the number of bytes matching.
 *	
*/
uint16_t FLASH_Verify_write_count(uint8_t* write_buff, uint8_t read_buff, uint16_t size);

/*---------------------------------------------------------------------------*/
/* The Update_Variables() updates all the global variables, which are stored in
 * the parameter table, with the parameter table passed as parameter. 
 *
 * @param
 *	Param_Table_t t: The struct containing the values of all the parameter 
 *						variable
 *
 * @return: void
 *	
*/
void Update_Variables(Param_Table_t t);

/*---------------------------------------------------------------------------*/
/* The Load_Factory_Value() loads the factory value to the global variables, 
 * which are stored in the parameter table.
 *
 * @param
 *	no parameter
 *
 * @return: void
 *	
*/
void Load_Factory_Value();

/*---------------------------------------------------------------------------*/
/* The Get_Filled_Array() copy the global variables value to the corresponding
 * field of the Paramter_Table_T struct which is passed as the parameter to the
 * function.
 *
 * @param
 *	Param_Table_t * t: The pointer to the parameter struct to which all the 
 *						global variables' value will be copied.
 *
 * @return: void
 *	
*/
void Get_Filled_Array(Param_Table_t * t);

/*---------------------------------------------------------------------------*/
/* The Save_Parameter_Table() is responsible for handling the state machine
 * of the function Save_Parameter_Table_Arr(), Param_Module_sync, and 
 * retuning appropriate values. When the Param_Module_sync is in IDLE state
 * then this function returns FLASH_WRITE_IDLE value. Otherwise it will 
 * fill the parameter table with the current values of the global variables
 * and pass it to the Save_Parameter_Table_Arr() function. If the value 
 * returned by the Save_Parameter_Table_Arr() function is FLASH_WRITE_TMP_FAIL then
 * this function it sets the state machine to FLASH_STATE_ERASE_CMD and checks 
 * the Param_Module_sync.Response_Read value if it is less than 
 * Globals.Flash_SPI_Tries_Limit then it sends the clear status register  
 * command and returns FLASH_WRITE_TMP_FAIL, else it sets the state machine to 
 * the IDLE state and returns FLASH_WRITE_FAIL. If the value returned by the 
 * Save_Parameter_Table_Arr is FLASH_STATE_PROGRAM then this function resets the
 * Globals.Param_Module_sync.Response_Read to 0 and returns FLASH_WRITE_SUCCESS.
 * If the function Save_Parameter_Table_Arr() returns any other value then this 
 * function returns the value FLASH_WRITE_BUSY.
 *
 * @param
 *	no parameters
 *
 * @return: uint8_t
 *	It returns one of the following follows:
 *		FLASH_WRITE_FAIL
 *		FLASH_WRITE_SUCCESS
 *		FLASH_WRITE_BUSY
 *		FLASH_WRITE_IDLE
 *		FLASH_WRITE_TMP_FAIL
*/
uint8_t Save_Parameter_Table();

/*---------------------------------------------------------------------------*/
/* The Save_Parameter_Table_Arr() is responsible for executing the state machine
 * on the basis of the Param_Module_sync's state. All the states returns the value
 * equal to their state number if the state was executed successfully else they 
 * return the value 0xFF (FLASH_WRITE_TMP_FAIL). In the FLASH_STATE_ERASE_CMD 
 * it sends the erase command of the sector of which the address is passed and 
 * then change the state machine to the FLASH_STATE_WAIT. In FLASH_STATE_WAIT 
 * state, it checks for the status register and wait for the ready bit to become
 * 0. If it is zero then it changes the state to the FLASH_STATE_ERASE_VERIFY else
 * it doesn't change its state and increases the Param_Module_sync.Response_Length 
 * and if the value of Param_Module_sync.Response_Length exceeds 
 * Globals.Flash_SPI_Wait_Limit then it returns 0xFF. The FLASH_STATE_ERASE_VERIFY
 * state read back the value from the sector and verify if it has been erased or not.
 * This state returns either its state value or 0xFF depending on the verification.
 * It then changes the state to the FLASH_STATE_PROGRAM. The FLASH_STATE_PROGRAM 
 * state programs the sector, read back the data and verify its integrity. If any 
 * the data verification fails then it return 0xFF else it returns the value equal 
 * to its state machine.
 *
 * @param
 *	uint8_t * FLASH_Parmaeter_Arr: The array to be saved in the parameter table
 *	uint16_t FLASH_Parameter_Arr_size: The size of the array to be saved.
 *	uint32_t address: The sector address where the data is to be stored.
 *
 * @return: uint8_t
 *	It either returns the value equal to the state which was executed or 0xFF
*/
uint8_t Save_Parameter_Table_Arr(uint8_t * FLASH_Parameter_Arr, uint16_t FLASH_Parameter_Arr_Size, uint32_t address);

/*---------------------------------------------------------------------------*/
/* The Load_Parameter_Table() uses Read_Parameter_Table to read the parameter
 * table from the flash memory and load it to the relevant global variables.
 * If the Read_Parameter_Table() returns 0, which denote the failure to read 
 * the parameter table, then it loads the factory values to the parameter table.
 *
 * @param
 *	no parameter
 *
 * @return: uint8_t
 *	It returns 1 if the parameter has been successfully loaded from the flash
 *	memory else it returns 0.
*/
uint8_t Load_Parameter_Table();

/*---------------------------------------------------------------------------*/
/* The Read_Parameter_Table() reads data from the passed addresses[7] and verifies
 * its checksum along with Param_PSLV_Wait_Timer_Time to not to be 0xFFFFFFFF.
 * If both the conditions meet then it returns 1 else it returns 0.
 *
 * @param
 *	Param_Table_t * FLASH_Parameter: The pointer to the struct where the read 
 *									data is stored.
 *
 * @return: uint8_t
 *	It returns 1 if the parameter has been successfully loaded from the flash
 *	memory else it returns 0.
*/
uint8_t Read_Parameter_Table(Param_Table_t * FLASH_Parameter);

GLOBAL mss_spi_instance_t * FLASH_SPI;	// The instance of spi for flash memory
//GLOBAL uint16_t Flash_SPI_Wait_Limit;	// The no. of times FS will wait for an operation

//GLOBAL Module_Sync_Small_t Param_Module_sync;	// Variable for handling the flash write operation
#endif /* SPI_FLASH_H_ */	

