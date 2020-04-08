`timescale 1ns / 1ps
////////////////////////////////////////////////////////////////////////////////
//
// Create Date:   12:15:46 05/28/2018
// Module Name:   rtc_counter
// Project Name:  RTC_verilog
// Author:        Ankit Verma
//
// Description: This is the verilog code which implements the Real Time Counter 
//              (RTC) with accuracy of 1 millisecond.
// 
// I/O:
// CLK - A clock of 10 MHz 
// RESET - Active low Reset input
// w_en  - Write Enable (active high).  When set to 1 the value of RTC is updated with the value of 'newYT'.
// newYT - The value to be written to the RTC
// div_clk -  The divided clock output for debugging purposes. Frequency of the clock should be 500 Hz
// YT - The current value of the RTC.    
//
//
// 
////////////////////////////////////////////////////////////////////////////////

module rtc_counter(CLK, RESET, w_en, newYT, div_clk, YT);

input CLK;     
input RESET;    
input w_en; 
input [39:0] newYT;     
output reg div_clk;     
output reg [39:0] YT;       


reg [15:0] c;   // variable for counter

always @(posedge CLK)
begin
    if(RESET == 1)
    begin
        if(w_en == 0)
        begin 
            if(c >= 16'h2710 - 1)   //  dividing clock by 10,000. 0.1 us * 10000 = 1 ms 
            begin
                c <= 0;
                div_clk <= ~div_clk; 
                YT <= YT + 1;    // Incresae the RTC value by 1 as 1 ms have passed
            end
            else
            begin
                c <= c + 1; 
            end
        end
        else
        begin
            c <= 0;      // restart the clock divider the counter
            YT <= newYT; // Write new value to the RTC
            div_clk <= 0;
        end
    end
    else
    begin
        c <= 0;  // reset the clock divider counter
        YT <= 0; // reset the RTC value
        div_clk <= 0;
    end
end
endmodule
