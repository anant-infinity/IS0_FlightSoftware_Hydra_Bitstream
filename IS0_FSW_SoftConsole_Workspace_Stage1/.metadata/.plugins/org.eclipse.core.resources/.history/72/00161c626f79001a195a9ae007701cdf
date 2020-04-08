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
module uart_rx_3
(
   	input i_Clock,
	input RESET,
	input EN,
   	input i_RX_Serial,

	output reg [2:0] r_SM_Main,

	output reg [7:0] r_RX_Byte,
    output reg [12:0] o_write_pointer,
    output reg o_save_byte
   );
   
	//defining State Machine
	parameter CLKS_PER_BIT = 50;
	parameter IDLE         = 3'b000;
	parameter RX_START_BIT = 3'b001;
	parameter RX_DATA_BITS = 3'b010;
	parameter RX_STOP_BIT  = 3'b011;
	parameter RX_WAIT  = 3'b100;
	
	reg [7:0] r_RX_Byte;
	reg [7:0] r_Clock_Count;
	reg [2:0] r_Bit_Index; //8 bits total
	// Purpose: Control RX state machine
	always @(posedge i_Clock)
	begin
		if (RESET == 1)
		begin
			if(EN == 1)
			begin
				case (r_SM_Main)
					IDLE :
					begin
						r_Clock_Count <= 8'h00;
						o_save_byte <= 1'b0;
						if (i_RX_Serial == 1'b0)          // Start bit detected
						begin
                            r_SM_Main <= RX_START_BIT;
						end
						else
						begin
							r_SM_Main <= IDLE;
						end
					end
					
					// Check middle of start bit to make sure it's still low
					RX_START_BIT :
					begin
						o_save_byte <= 1'b0;
						if (r_Clock_Count == (CLKS_PER_BIT-1)/2)
						begin
							if (i_RX_Serial == 1'b0)
							begin
							  r_Clock_Count <= 8'h00;  // reset counter, found the middle
							  r_SM_Main     <= RX_DATA_BITS;
							end
							else
							  r_SM_Main <= IDLE;
						end
						else
						begin
							r_Clock_Count <= r_Clock_Count + 1;
							r_SM_Main     <= RX_START_BIT;
						end
					end // case: RX_START_BIT
					
					
					// Wait CLKS_PER_BIT-1 clock cycles to sample serial data
					RX_DATA_BITS :
					begin
						o_save_byte <= 1'b0;
						if (r_Clock_Count < CLKS_PER_BIT-1)
						begin
							r_Clock_Count <= r_Clock_Count + 1;
							r_SM_Main     <= RX_DATA_BITS;
						end
						else
						begin
							r_Clock_Count <= 8'h00;
							r_RX_Byte[r_Bit_Index] <= i_RX_Serial;
							
							// Check if we have received all bits
							if (r_Bit_Index < 7)        //Can change number of bits here
							begin
							  r_Bit_Index <= r_Bit_Index + 3'b001;
							  r_SM_Main   <= RX_DATA_BITS;
							end
							else
							begin
							  r_Bit_Index <= 3'b000;
							  r_SM_Main   <= RX_STOP_BIT;
							end
						end
					end // case: RX_DATA_BITS
					
					
					// Receive Stop bit.  Stop bit = 1
					RX_STOP_BIT :
					begin
						// Wait CLKS_PER_BIT-1 clock cycles for Stop bit to finish
						if (r_Clock_Count < CLKS_PER_BIT-1)
						begin
							r_Clock_Count <= r_Clock_Count + 1;
							r_SM_Main     <= RX_STOP_BIT;
						end
						else
						begin
							if(i_RX_Serial == 1'b0)
							begin
								r_SM_Main     <= IDLE;
							end
							else
							begin
								r_SM_Main     <= RX_WAIT;	
							end
							r_Clock_Count <= 8'h00;
						end
				  	end // case: RX_STOP_BIT
					
					RX_WAIT:
					begin
						if (r_Clock_Count < 6)
						begin
							o_save_byte <= 1'b1;
							r_Clock_Count <= r_Clock_Count + 1;
							r_SM_Main     <= RX_WAIT;
						end
						else
						begin 
							r_SM_Main <= IDLE;
							o_save_byte <= 1'b0;
							o_write_pointer <= o_write_pointer + 13'b0000000000001;
							r_Clock_Count <= 8'h00;
						end
					end

					
					default :
					begin
					  	r_SM_Main <= IDLE;
					end

				endcase
			end
			else
			begin
				o_save_byte <= 1'b0;
			end
		end
		else
		begin
			o_write_pointer <= 13'h0000;
			o_save_byte <= 1'b0;
            r_RX_Byte <= 8'h05;

            r_Clock_Count <= 8'h00;
            r_Bit_Index <= 3'b000;
            r_SM_Main <= IDLE;
		end
	 
	end


endmodule
