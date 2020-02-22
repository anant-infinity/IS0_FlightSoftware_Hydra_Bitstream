#ifndef t1_HW_PLATFORM_H_
#define t1_HW_PLATFORM_H_
/*****************************************************************************
*
*Created by Microsemi SmartDesign  Mon Nov 18 18:06:05 2019
*
*Memory map specification for peripherals in t1
*/

/*-----------------------------------------------------------------------------
* CM3 subsystem memory map
* Master(s) for this subsystem: CM3 
*---------------------------------------------------------------------------*/
#define ADDR_APB_RTC                    0x50000000U
#define ADCS_UART                       0x50001000U
#define CIP_UART                        0x50002000U
#define DAXSS_UART                      0x50003000U
#define UHF_UART_9600                   0x50004000U
#define UART_0                          0x50005000U
#define UHF_DATA_RXTX_0_DATA_RX_SLAVE   0x50006000U
#define UHF_DATA_RXTX_0_DATA_TX_SLAVE   0x50007000U

#define I_EPS_I2C_C_0                   0x50008000U
#define J_EPS_I2C_C_1                   0x50009000U
#define K_EPS_I2C_NC                  	0x5000A000U
#define L_SENSOR_BOARD_I2C_0            0x5000B000U


#define Reset_Generator					0x5000C000U
#define Sensor_Board_SPI				0x5000D000U

#endif /* t1_HW_PLATFORM_H_*/
