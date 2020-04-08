///////////////////////////////////////////////////////////////////////////////////////////////////
// Company: <Name>
//
// File: uhf_rxtx_test.v
// File history:
//      <Revision number>: <Date>: <Comments>
//      <Revision number>: <Date>: <Comments>
//      <Revision number>: <Date>: <Comments>
//
// Description: 
//
// <Description here>
//
// Targeted device: <Family::SmartFusion2> <Die::M2S025> <Package::484 FBGA>
// Author: <Name>
//
/////////////////////////////////////////////////////////////////////////////////////////////////// 

//`timescale <time_units> / <precision>

module spi_manipulator(
    input i_MOSI_uP,
    output o_MOSI_SD,

    input i_Read_SD_CTRL
	);

    assign o_MOSI_SD = i_Read_SD_CTRL == 1 ? 1 : i_MOSI_uP;
	
endmodule