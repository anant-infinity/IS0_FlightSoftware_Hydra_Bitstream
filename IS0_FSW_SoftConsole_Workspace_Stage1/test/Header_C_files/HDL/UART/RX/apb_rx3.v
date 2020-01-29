`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    10:08:04 07/06/2018 
// Design Name: 
// Module Name:    apb_rx2 
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
module apb_rx3(
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
        
    input RX,

    output [19:0] PADDR_SRAM,
    output PSEL_SRAM,
    output PWRITE_SRAM,
    output PENABLE_SRAM,
    output [7:0] PWDATA_SRAM,
    input [7:0] PRDATA_SRAM,
    input PSLVERR_SRAM, 
    input PREADY_SRAM
    );

    assign PREADY =1;
    assign PSLVERR = 0;
    wire wr_enable,re_enable;
    assign wr_enable = (PENABLE && PWRITE && PSEL);
    assign re_enable = (PENABLE && !PWRITE && PSEL);

    // Inputs
    reg [7:0] resets_reg;
    wire [7:0] Status; 
    reg [7:0] config_reg;

    reg [15:0] r_byte_read_pointer;
    wire [7:0] read_data;

    wire [7:0] w_write_data;
    wire [12:0] w_write_pointer;

    //assign Status[2]= 1'b0;
    uhf_rx_sram_interface uut1(
        .i_PCLK(PCLK),
        .i_Reset_all(PRESETN & resets_reg[0]),
        .i_Reset_Read(PRESETN & resets_reg[2]),
        .i_write_data(w_write_data),
        .i_SAVE_BYTE(Status[3]),
        .i_start_read(config_reg[1]),
        .i_byte_write_pointer(w_write_pointer),
        .i_byte_read_pointer(r_byte_read_pointer[12:0]),

        .o_PADDR_SRAM(PADDR_SRAM),
        .i_PRDATA_SRAM(PRDATA_SRAM),
        .o_PSEL_SRAM(PSEL_SRAM),
        .o_PWRITE_SRAM(PWRITE_SRAM),
        .o_PENABLE_SRAM(PENABLE_SRAM),
        .o_PWDATA_SRAM(PWDATA_SRAM),
        .i_PREADY_SRAM(PREADY_SRAM),
        .i_PSLVERR_SRAM(PSLVERR_SRAM),

        .o_write_completed(Status[4]),
        .o_apb_write_busy(Status[5]),
        .o_read_data_allow(Status[6]),
        .o_read_data_ready(Status[7]),
        .o_read_data(read_data)
        );
    // Instantiate the Unit Under Test (UUT)
    uart_rx_3 uut(
        .i_Clock(PCLK),
        .RESET(PRESETN & resets_reg[0] & resets_reg[1]),
        .EN(config_reg[0]),
        .i_RX_Serial(RX),
        .r_SM_Main(Status[2:0]),

        .r_RX_Byte(w_write_data),
        .o_write_pointer(w_write_pointer),
        .o_save_byte(Status[3])
    );

    always@( posedge wr_enable)
    begin

        if(resets_reg[0] == 0 || PRESETN == 0)
        begin
            r_byte_read_pointer = 16'h0000;
            config_reg = 8'h00;
        end

        if(PADDR == 8'h04)  //RX WRITE
        begin
            config_reg = PWDATA;
        end
        else if(PADDR == 8'h08) //RX WRITE
        begin
            resets_reg = PWDATA;
        end
        else if(PADDR == 8'h0C) //RX WRITE
        begin
            r_byte_read_pointer = {r_byte_read_pointer[15:8], PWDATA};
        end
            else if(PADDR == 8'h10) //RX WRITE
        begin
            r_byte_read_pointer = {PWDATA,r_byte_read_pointer[7:0]};
        end
    end

    always@( posedge re_enable)
    begin
        if(PADDR == 8'h00)  //RX READ
        begin 
            PRDATA = Status;
        end
        else if(PADDR == 8'h04) //RX READ
        begin 
            PRDATA = config_reg;
        end
        else if(PADDR == 8'h08) //RX READ
        begin 
            PRDATA = resets_reg;
        end
        else if(PADDR == 8'h0C) //RX READ
        begin 
            PRDATA = r_byte_read_pointer[7:0];
        end
        else if(PADDR == 8'h10) //RX READ
        begin 
            PRDATA = r_byte_read_pointer[15:8];
        end
        else if(PADDR == 8'h14) //RX READ
        begin 
            PRDATA = read_data;
        end
        else if(PADDR == 8'h18) //RX READ
        begin 
            PRDATA = w_write_data;
        end
        else if(PADDR == 8'h1C) //RX READ
        begin 
            PRDATA = w_write_pointer[7:0];
        end
        else if(PADDR == 8'h20) //RX WRITE
        begin
            PRDATA = {3'b000,w_write_pointer[12:8]};
        end
    end

endmodule
