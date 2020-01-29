`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    12:43:37 07/02/2018 
// Design Name: 
// Module Name:    uart_rx_2 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module clk_div_9600
(
	input RESET,
   	input i_Clock,
   	output reg o_clock
   );
   
	reg [7:0] r_Clock_Count;
	always @(posedge i_Clock)
	begin
		if (RESET == 1)
		begin
			if(r_Clock_Count < 5)
			begin
				r_Clock_Count <= r_Clock_Count + 1;
			end
			else begin
				o_clock <= ~o_clock;
				r_Clock_Count <= 0;
			end
		end
		else
		begin
			o_clock <= 0;
			r_Clock_Count <= 8'b00;
		end
	 
	end


endmodule
