///////////////////////////////////////////////////////////////////////////////////////////////////
// Company: <Name>
//
// File: apb_uhf_rx3.v
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

module apb_uhf_rx3(
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

	// UHF ports
	input TXRX_CLOCK,
	input RX_DATA,
    input TX_REQ,

    // LSRAM ports
    output [19:0] PADDR_SRAM,
    output PSEL_SRAM,
    output PWRITE_SRAM,
    output PENABLE_SRAM,
    output [7:0] PWDATA_SRAM,
    input [7:0] PRDATA_SRAM,
    input PREADY_SRAM,
    input PSLVERR_SRAM
	);

	assign PREADY =1;
	assign PSLVERR = 0;
    wire wr_enable,re_enable;
    assign wr_enable = (PENABLE && PWRITE && PSEL);
    assign re_enable = (PENABLE && !PWRITE && PSEL);

	wire [7:0] status_reg; 
    reg [7:0] config_reg; // 1-EN of transmitter.s
    reg [7:0] resets_reg; 
    
    reg [12:0] r_byte_read_pointer;
    wire [7:0] read_data;

    wire [15:0] w_fcs;
    wire [12:0] w_write_pointer;

    wire [7:0] w_write_data;
    wire w_write_completed, w_save_byte;

	uhf_rx_sram_interface uut1(
    .i_PCLK(PCLK),
    .i_Reset_all(resets_reg[0] && PRESETN),
    .i_Reset_Read(resets_reg[1] && PRESETN),
    .i_write_data(w_write_data),
    .i_SAVE_BYTE(w_save_byte),
    .i_start_read(config_reg[1]),
    .i_byte_write_pointer(w_write_pointer),
    .i_byte_read_pointer(r_byte_read_pointer),

    .o_PADDR_SRAM(PADDR_SRAM),
    .i_PRDATA_SRAM(PRDATA_SRAM),
    .o_PSEL_SRAM(PSEL_SRAM),
    .o_PWRITE_SRAM(PWRITE_SRAM),
    .o_PENABLE_SRAM(PENABLE_SRAM),
    .o_PWDATA_SRAM(PWDATA_SRAM),
    .i_PREADY_SRAM(PREADY_SRAM),
    .i_PSLVERR_SRAM(PSLVERR_SRAM),

    .o_write_completed(w_write_completed),
    .o_apb_write_busy(status_reg[6]),
    .o_read_data_allow(status_reg[4]),
    .o_read_data_ready(status_reg[5]),
    .o_read_data(read_data)
    );

    uhf_rx_v2 uut2(
    .i_EN(config_reg[0]),
    .i_Reset_all(resets_reg[0] && PRESETN),
    .i_TX_req(TX_REQ),
    .i_TXRX_CLOCK(TXRX_CLOCK),
    .i_RX_DATA(RX_DATA),
    .i_write_completed(w_write_completed),

    .o_FRAME_STARTED(status_reg[0]),
    .o_FRAME_DEST_MATCHED(status_reg[7]),
    .o_FRAME_ENDED(status_reg[1]),
    .o_FRAME_FAULT(status_reg[2]),
    .o_SAVE_FAULT(status_reg[3]),

    .o_fcs(w_fcs),

    .o_write_data(w_write_data),
    .o_write_pointer(w_write_pointer),
    .o_save_byte(w_save_byte)
    );
	
    always @ (posedge wr_enable)
	begin
        if(resets_reg[0] == 0 || PRESETN == 0)
        begin
            r_byte_read_pointer = 0;
            config_reg = 0;
        end

		if(PADDR == 8'h04)	//RX WRITE
        begin
            config_reg = PWDATA;
        end
        if(PADDR == 8'h08)	//RX WRITE
        begin
            resets_reg = PWDATA;
        end
        if(PADDR == 8'h0C)
        begin
            r_byte_read_pointer = {r_byte_read_pointer[12:8], PWDATA};
        end
        if(PADDR == 8'h10)
        begin
            r_byte_read_pointer = {PWDATA[4:0],r_byte_read_pointer[7:0]}; 
        end
	end

	always @ (posedge re_enable)
	begin
		if(PADDR == 8'h00)	//RX READ
        begin
            PRDATA = status_reg;
        end
        if(PADDR == 8'h04)	//RX READ
        begin
            PRDATA = config_reg;
        end
        if(PADDR == 8'h08)	//RX READ
        begin
            PRDATA = resets_reg;
        end
        if(PADDR == 8'h0C)	//RX READ
        begin
            PRDATA = r_byte_read_pointer[7:0];
        end
        if(PADDR == 8'h10)	//RX READ
        begin
            PRDATA ={3'b000,r_byte_read_pointer[12:8]};
        end
        if(PADDR == 8'h14)	//RX READ
        begin
            PRDATA = w_write_data;
        end
        if(PADDR == 8'h18)  //RX READ
        begin
            PRDATA = read_data;
        end       
        if(PADDR == 8'h1C)  //RX READ
        begin
            PRDATA = w_fcs[7:0];
        end
        if(PADDR == 8'h20)  //RX READ
        begin
            PRDATA = w_fcs[15:8];
        end       
        if(PADDR == 8'h24)  //RX READ
        begin
            PRDATA = w_write_pointer[7:0];
        end
        if(PADDR == 8'h28)  //RX READ
        begin
            PRDATA = {3'b000,w_write_pointer[12:8]};
        end
	end

    

endmodule