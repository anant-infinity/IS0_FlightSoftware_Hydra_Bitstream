

#ifndef _GPIOS_C
	#define _GPIOS_C

#include "gpios.h"



void Global_Init_GPIOs(){


    MSS_GPIO_init();

    MSS_GPIO_config( GPIO_DSW0_DAXSS, MSS_GPIO_OUTPUT_MODE); /* configuring GPIO_1 for DSW_0 BCR Enable - default 0 */
    MSS_GPIO_config( GPIO_DSW1_CIP, MSS_GPIO_OUTPUT_MODE); /* configuring GPIO_2 for DSW_1 CIP - default 0 */
    MSS_GPIO_config( GPIO_DSW2_ADCS, MSS_GPIO_OUTPUT_MODE); /* configuring GPIO_3 for DSW_2 ADCS & GPS - default 0 */
    MSS_GPIO_config( GPIO_DSW3_Sband, MSS_GPIO_OUTPUT_MODE); /* configuring GPIO_4 for DSW_3 SBand - default 0 */
    MSS_GPIO_config( GPIO_DSW4_Bat_heat, MSS_GPIO_OUTPUT_MODE); /* configuring GPIO_5 for DSW_4 bat heater - default 0 */

    MSS_GPIO_config( GPIO_CONVEST, MSS_GPIO_OUTPUT_MODE); /* configuring for Convest Signal - default 0 */

    MSS_GPIO_config( GPIO_PWR_CYCLE_SC, MSS_GPIO_OUTPUT_MODE); /* configuring for giving spacecraft power cycle signal - default 1*/
    MSS_GPIO_config( GPIO_RESET_IC, MSS_GPIO_OUTPUT_MODE); /* configuring for giving signal to on board Reset IC. - default 1 */
	MSS_GPIO_config( GPIO_EN_BUS_TRAN,MSS_GPIO_OUTPUT_MODE); /* UHF_Wait_Response_Limitconfiguring for giving EN to bus transceiver. - default 1 */
	MSS_GPIO_config( GPIO_EN_SD0,MSS_GPIO_OUTPUT_MODE); /* configuring for giving EN switch of sd card0 - default 0*/
	MSS_GPIO_config( GPIO_EN_SD1,MSS_GPIO_OUTPUT_MODE); /* configuring for giving EN switch of sd card1 - default 0 */
    MSS_GPIO_config( GPIO_CTRL_MOSI_SD,MSS_GPIO_OUTPUT_MODE ); /*configuring for FPGA reset pin of SBand - default 0 */
    MSS_GPIO_set_output( GPIO_CTRL_MOSI_SD, 0);


    MSS_GPIO_config( GPIO_EN_SENSOR_BOARD,MSS_GPIO_OUTPUT_MODE); /* configuring for deployment Antenna - default 0 */
    MSS_GPIO_config( GPIO_RESET_SENSER_BOARD,MSS_GPIO_OUTPUT_MODE); /*configuring for EN pin of SBand - default 1 */
    MSS_GPIO_config( GPIO_SENSER_BOARD_WRITE_PROTECT,MSS_GPIO_OUTPUT_MODE ); /*configuring for FPGA reset pin of SBand - default 1 */


    //Setting the GPIO default outputs
	MSS_GPIO_set_output( GPIO_DSW0_DAXSS, 0);
	MSS_GPIO_set_output( GPIO_DSW1_CIP, 0);
	MSS_GPIO_set_output( GPIO_DSW2_ADCS, 0);
	MSS_GPIO_set_output( GPIO_DSW3_Sband, 0);
	MSS_GPIO_set_output( GPIO_DSW4_Bat_heat, 0);
	MSS_GPIO_set_output( GPIO_CONVEST, 0);

	MSS_GPIO_set_output( GPIO_EN_SENSOR_BOARD, 1);
	MSS_GPIO_set_output( GPIO_RESET_SENSER_BOARD, 1);
	MSS_GPIO_set_output( GPIO_SENSER_BOARD_WRITE_PROTECT, 1);

	MSS_GPIO_set_output( GPIO_PWR_CYCLE_SC, 1); /* Make sure that the spacecraft power cycle signal is tie high by default*/
	MSS_GPIO_set_output( GPIO_RESET_IC, 1); /* Pulling the input of the external Watchdog timer high*/
	MSS_GPIO_set_output( GPIO_EN_BUS_TRAN, 0); /* Enable the bus transceivers - By default high pulling it low now to enable the bus transceiver*/
	MSS_GPIO_set_output( GPIO_EN_SD0, 1);	/* Turn ON the sd card 0*/
	// TODO: Turn this back on once we have one that isn't blown!
	//	MSS_GPIO_set_output( GPIO_EN_SD1, 1);	/* Turn ON the sd card 1*/



}

#endif
