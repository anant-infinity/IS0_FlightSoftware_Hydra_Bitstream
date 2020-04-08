`timescale 1ns / 1ps
////////////////////////////////////////////////////////////////////////////////
//
// Create Date:   12:15:46 05/28/2018
// Module Name:   apb_rtc.v
// Project Name:  RTC_verilog
// Author:        Ankit Verma
//
// Description: This is the verilog code which implements the APB3 wrapper around the 
//				Real Time Counter (RTC). 
// 
// I/O:
// PCLK - A clock of 10 MHz from APB3
// PRESETN - Active low Reset input from APB3
// PADDR  - Address line from APB3 with address of the register to write to / read from 
// PSEL - Slave select line from APB3
// PENABLE - Slave enable line from APB3
// PWDATA - Write Data line from APB3 which to be written 
// PRDATA - Read data line on which the slave would put the data
// PSVERR - The error indication line (0 by default)
// PREADY - Ready signal line (1 by default)
// div_clk - The divided clock output for debugging purposes from 'rtc_counter.v'. 
//			Frequency of the clock should be 500 Hz
//
// Address Map:
// newYT[7:0] - 0x00 (r/w)
// newYT[15:8] - 0x04 (r/w)
// newYT[23:16] - 0x08 (r/w)
// newYT[31:24] - 0x0c (r/w)
// newYT[39:32] - 0x10 (r/w)
////////////////////////////////////////////////////////////////////////////////

module apb_rtc(
    input PCLK,
    input PRESETN,
    input [7:0] PADDR,
    input PSEL,
    input PENABLE,
    input PWRITE,
    input [7:0] PWDATA,
    output reg [7:0] PRDATA,
	 output PSLVERR,
    output PREADY,
	 output div_clk
    );

    assign PREADY =1;
    assign PSLVERR = 0;
	
	reg [7:0] config_reg;
	reg [39:0] newYT; // to be instantiated
    wire [39:0] YT;
    wire [39:0] YT2;

    wire div_clk2;

    wire wr_enable,rd_enable;
    assign wr_enable = (PENABLE && PWRITE && PSEL);
    assign rd_enable = (PENABLE && !PWRITE && PSEL);

	always @(posedge wr_enable)
	begin
		if(PRESETN)
		begin
			if(PADDR == 8'h00)
	        begin
	            newYT[7:0] = PWDATA;
	        end
			else if(PADDR == 8'h04)
			begin
				newYT[15:8] = PWDATA;
			end
			else if(PADDR == 8'h08)
			begin
				newYT[23:16] = PWDATA;
			end
			else if(PADDR == 8'h0C)
			begin
				newYT[31:24] = PWDATA;
			end
			else if(PADDR == 8'h10)
			begin
				newYT[39:32] = PWDATA;
			end
			else if(PADDR == 8'h14)
			begin
				config_reg = PWDATA;
			end
		end
		else 
	    begin
	    	config_reg = 0;
	        newYT =0;
	    end
	end

	always @(posedge rd_enable)
	begin
		if(PADDR == 8'h00)
		begin 
			PRDATA = YT[7:0];
		end
		else if(PADDR == 8'h04)
		begin 
			PRDATA = YT[15:8];
		end
		else if(PADDR == 8'h08)
		begin 
			PRDATA = YT[23:16];
		end
		else if(PADDR == 8'h0C)
		begin 
			PRDATA = YT[31:24];
		end
		else if(PADDR == 8'h10)
		begin 
			PRDATA = YT[39:32];
		end
		else if(PADDR == 8'h14)
		begin
			PRDATA = config_reg;
		end
		else if(PADDR == 8'h18)
		begin 
			PRDATA = YT2[7:0];
		end
		else if(PADDR == 8'h1C)
		begin 
			PRDATA = YT2[15:8];
		end
		else if(PADDR == 8'h20)
		begin 
			PRDATA = YT2[23:16];
		end
		else if(PADDR == 8'h24)
		begin 
			PRDATA = YT2[31:24];
		end
		else if(PADDR == 8'h28)
		begin 
			PRDATA = YT2[39:32];
		end

	end	
	rtc_counter c1(.CLK(PCLK),.RESET((PRESETN) & config_reg[0]), .w_en(1'b0), .newYT(newYT),.div_clk(div_clk), .YT(YT));
    rtc_counter c2(.CLK(PCLK),.RESET((PRESETN) & config_reg[1]), .w_en(config_reg[2]), .newYT(newYT),.div_clk(div_clk2), .YT(YT2));
endmodule
