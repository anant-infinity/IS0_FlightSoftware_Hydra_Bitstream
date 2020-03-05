/*
 * pslv_uart.h
 *
 *  Created on: Feb 13, 2020
 *      Author: Anant
 */


#ifndef HEADER_C_FILES_PSLV_UART_PSLV_UART_H_
#define HEADER_C_FILES_PSLV_UART_PSLV_UART_H_

#include "Seq_ops/main_seq.h"
#include "SD_CARD/sd.h"
#include "RTC/rtc.h"
#include "Utils/utils.h"
#include "string.h"
#include "../../CMSIS/mss_assert.h"
#include "SPI_Flash/spi_flash.h"
#include "hal/hal.h"
#include "CMSIS/system_m2sxxx.h"

#define PSLV_UART 0x5000E000
#define PacketSize 180
#define CONFIG_REG_OFFSET 0x00
#define START_TRANSFER_BIT 0
#define WR_EN_BIT 1
#define RESET_SM_BIT 2
#define STATUS_REG_OFFSET 0x04
#define TX_DATA_REG_OFFSET 0x08
#define FIFO_RD_CNT_REG_OFFSET 0x0C

void Check_and_Fill_FIFO();

void Set_Data_Byte_FIFO(uint8_t data);

void Set_Reset_Write_Enable_FIFO();

uint8_t Get_Status_Reg();

void Reset_FIFO_SM();




#endif /* HEADER_C_FILES_PSLV_UART_PSLV_UART_H_ */
