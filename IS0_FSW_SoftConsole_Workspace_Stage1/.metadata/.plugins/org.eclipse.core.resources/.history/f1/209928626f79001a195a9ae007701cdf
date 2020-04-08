///////////////////////////////////////////////////////////////////////////////////////////////////
// Company: <Name>
//
// File: ax_nrzi_decoder2.v
// File history:
//      <Revision number>: <Date>: <Comments>
//      <Revision number>: <Date>: <Comments>
//      <Revision number>: <Date>: <Comments>
//
// Description: 
//
// <Description here>
//
// Targeted device: <Family::SmartFusion2> <Die::M2S060> <Package::484 FBGA>
// Author: <Name>
//
/////////////////////////////////////////////////////////////////////////////////////////////////// 

//`timescale <time_units> / <precision>

module ax_nrzi_decoder2( 
input i_TXRX_CLOCK,
input i_R1,
input i_R2,
output reg o_RX
);

always @(negedge i_TXRX_CLOCK) 
begin
    if(i_R1 == 0 && i_R2 == 0)
    begin
        o_RX <= 1;
    end
    else if(i_R1 == 1 && i_R2 == 1)
    begin
        o_RX <= 1;
    end
    else 
    begin
        o_RX <= 0;
    end
end

//<statements>

endmodule

