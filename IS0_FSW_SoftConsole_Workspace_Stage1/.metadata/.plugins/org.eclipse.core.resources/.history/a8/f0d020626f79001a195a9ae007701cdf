`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Institute: IIST 
// Engineer: Ankit Verma
// 
// Create Date:    11:17:07 06/26/2018 
// Design Name: 
// Module Name:    apb_uart_tx 
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
// ADDR: Comment
//	0x00: Config	(write/read)
//	0x08: Status (read only)
//	0x04: TX_DATA 
//  0x0C: COUNT_LIMIT[7:0]
//  0x10: COUNT_LIMIT[15:0]
//	Procedure for using it
//  0. Read the Status(0x08) if Status[0] == 1 then proceed forward else wait.
//	1. Write to Config (0x00) value 0.
//	2. Write data to TX_DATA (0x04).
//	3. Write to Config (0x00) the value 0x01.
// Repeat above 

// The Status Register:
// Status[0]: isEnded
//	Status[1]: isStarted
//////////////////////////////////////////////////////////////////////////////////
module apb_uart_tx(
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
	output TX
    );
	 
	wire wr_enable,rd_enable;
    assign wr_enable = (PENABLE && PWRITE && PSEL);
    assign rd_enable = (PENABLE && !PWRITE && PSEL);
	
    assign PSLVERR = 0;
    assign PREADY = 1;

    reg [7:0] COUNT_LIMIT;
	reg [7:0] tx_Config;
	reg [7:0] TX_DATA;
	
    
	wire [7:0] tx_Status;
    wire [7:0] SM_Main;
    wire [7:0] Bit_Index;
	assign tx_Status[7:3] = 5'b00000;

	always@(posedge wr_enable)
	begin
        if( PRESETN == 0)
        begin 
            TX_DATA = 8'h00;
            tx_Config = 0;
            COUNT_LIMIT = 50;
        end

        if(PADDR == 8'h00)	//TX WRITE
        begin
            tx_Config = PWDATA;
        end
        else if(PADDR == 8'h04)	//TX WRITE
        begin
            TX_DATA = PWDATA;
        end
        else if(PADDR == 8'h0C)	//TX READ	tx-data
        begin 
            COUNT_LIMIT[7:0] = PWDATA;
        end

	end
	
    always@(posedge rd_enable)
    begin           
        if(PADDR == 8'h00)  //TX READ   tx-config
        begin 
            PRDATA = tx_Config;
        end
        else if(PADDR == 8'h04) //TX READ   status
        begin 
            PRDATA = TX_DATA;
        end
        else if(PADDR == 8'h08) //TX READ   tx-data
        begin 
            PRDATA = tx_Status;
        end
        else if(PADDR == 8'h0C) //TX READ   tx-data
        begin 
            PRDATA = COUNT_LIMIT[7:0];
        end
        else if(PADDR == 8'h14) //TX READ   tx-data
        begin 
            PRDATA = SM_Main;
        end
        else if(PADDR == 8'h18) //TX READ   tx-data
        begin 
            PRDATA = Bit_Index;
        end
    end
    UART_TX  uut_tx
    (
    .reset(PRESETN & tx_Config[0]),
    .i_Clock(PCLK),
    .i_Tx_Sig(tx_Config[1]),
    .i_Tx_Byte(TX_DATA), 
    .o_Tx_Active(tx_Status[1]),
    .o_Tx_Serial(TX),
    .o_Tx_Done(tx_Status[2]),
    .o_Tx_idle(tx_Status[0]),
    .CLKS_PER_BIT(COUNT_LIMIT),
    .o_SM_Main(SM_Main),
    .o_Bit_Index(Bit_Index)
    );


endmodule