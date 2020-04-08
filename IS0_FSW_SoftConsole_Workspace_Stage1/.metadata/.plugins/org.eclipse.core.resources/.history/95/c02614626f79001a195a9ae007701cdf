`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    17:55:36 02/14/2018 
// Design Name: 
// Module Name:    PPS_B_GEN 
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
module PPS_B_GEN(
    input Reset,
    input CLK,
	 output reg CDH_PPS,
	 output reg CIP_PPS
	 );
	 
	reg [1:0] state;
	localparam HL_Large = 24'h895440;   // 24'h895440 for 10 MHz
	localparam HL_Small = 20'h7A120;    // 20'h7A120 for 10 MHz  
	 
	localparam s_HH = 2'b00;
	localparam s_HL = 2'b01;
	localparam s_LL = 2'b10;

	reg [23:0] cL;
	reg [19:0] cS;


	always @ (posedge CLK)
	begin
        if(Reset)
        begin
            case(state)
                s_HH:
                begin
                    CDH_PPS <= 1;
                    CIP_PPS <= 1;

                    cS <= 0;			
                    if(cL >= HL_Large - 1)
                    begin
                        state <= s_HL;
                        cL <= 0;
                    end
                    else
                    begin
                        state <= s_HH;
                        cL <= cL + 1;
                    end
                end

                s_HL:
                begin
                    CIP_PPS <= 1;
                    CDH_PPS <= 0;
                    
                    cL <= 0;			
                    if(cS >= HL_Small - 1)
                    begin
                        state <= s_LL;
                        cS <= 0;
                    end
                    else
                    begin
                        state <= s_HL;
                        cS <= cS + 1;
                    end
                end

                s_LL:
                begin
                    CIP_PPS <= 0;
                    CDH_PPS <= 0;
                    
                    cL <= 0;	
                    if(cS >= HL_Small - 1)
                    begin
                        state <= s_HH;
                        cS <= 0;
                    end	
                    else
                    begin
                        state <= s_LL;
                        cS <= cS + 1;
                    end
                end

                default:
                begin
                    cS <= 0;
                    state <= s_HH;
                end
            endcase
        end
        else
        begin
            cL <= 0;
            cS <= 0;
        end
		
	end
	 
endmodule
