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

module apb_uhf_tx3(
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
	output TX_DATA,
    output TX_DATA_NRZI,

	// LSRAM ports
	output [19:0] PADDR_SRAM,
	output PSEL_SRAM,
	output PWRITE_SRAM,
	output PENABLE_SRAM,
	output [7:0] PWDATA_SRAM,
	input [7:0] PRDATA_SRAM,
	input PREADY_SRAM,
	input PSLVERR_SRAM,

    output O_TX_REQ,

    output reset_scrambler
	);

	assign PREADY =1;
	assign PSLVERR = 0;
    wire wr_enable,re_enable;
    assign wr_enable = (PENABLE && PWRITE && PSEL);
    assign re_enable = (PENABLE && !PWRITE && PSEL);

	wire [7:0] status_reg; 
    reg [7:0] config_reg; // 1-EN of transmitter.s
    reg [7:0] resets_reg; 
    reg [12:0] byte_writter_pointer;
    reg [12:0] read_start_byte_pointer;
    reg [12:0] read_end_byte_pointer;
    reg [7:0] write_data;
    wire [7:0] read_data;

    wire w_start_read;
    wire w_reset_read;
    wire [12:0] w_byte_read_pointer;
    wire [12:0] w_read_offset;

    wire [15:0] w_fcs;

    assign  status_reg[5] = w_start_read;
    assign  status_reg[6] = w_reset_read;
    assign  status_reg[7] = 0;
	assign reset_scrambler = resets_reg[0];


	uhf_tx_sram_interface uut1(
    .i_PCLK(PCLK),
    .i_Reset_all(resets_reg[0] && PRESETN),
    .i_Reset_Read(w_reset_read && PRESETN),
    .i_write_data(write_data),
    .i_SAVE_BYTE(config_reg[0]),
    .i_start_read(w_start_read),
    .i_byte_write_pointer(byte_writter_pointer),
    .i_byte_read_pointer(w_byte_read_pointer),

    .o_PADDR_SRAM(PADDR_SRAM),
    .i_PRDATA_SRAM(PRDATA_SRAM),
    .o_PSEL_SRAM(PSEL_SRAM),
    .o_PWRITE_SRAM(PWRITE_SRAM),
    .o_PENABLE_SRAM(PENABLE_SRAM),
    .o_PWDATA_SRAM(PWDATA_SRAM),
    .i_PREADY_SRAM(PREADY_SRAM),
    .i_PSLVERR_SRAM(PSLVERR_SRAM),

    .o_apb_write_busy(status_reg[0]),
    .o_write_completed(status_reg[1]),
    .o_read_data_allow(status_reg[2]),
    .o_read_data_ready(status_reg[3]),
    .o_read_data(read_data)
    );

    uhf_tx_data_v2 uut2(
    .i_EN_TX(config_reg[1]),
    .i_TXRX_CLOCK(TXRX_CLOCK),
    .i_read_data(read_data),
    .i_read_data_ready(status_reg[3]),
    
    .i_Reset_all(resets_reg[0] && PRESETN),

    .i_start_byte_read_pointer(read_start_byte_pointer),
    .i_end_byte_read_pointer(read_end_byte_pointer),

    .o_TX_DATA(TX_DATA),
    .o_TX_DATA_NRZI(TX_DATA_NRZI),
    .o_transmit_done(status_reg[4]),
    .o_start_read(w_start_read),
    .o_reset_read(w_reset_read),
    .o_read_offset(w_read_offset),
    .o_byte_read_pointer(w_byte_read_pointer),
    .o_fcs(w_fcs),
    .o_tx_req(O_TX_REQ)

    );

	

    always @(posedge wr_enable)
	begin
        if(resets_reg[0] == 0 || PRESETN == 0)
        begin
            read_start_byte_pointer = 0;
            read_end_byte_pointer = 0;
            config_reg = 0;
        end

		if(PADDR == 8'h04)	//TX WRITE
        begin
            config_reg = PWDATA;
        end
        if(PADDR == 8'h08)	//TX WRITE
        begin
            resets_reg = PWDATA;
        end
        if(PADDR == 8'h0C)
        begin
            byte_writter_pointer = {byte_writter_pointer[12:8], PWDATA};
        end
        if(PADDR == 8'h10)
        begin
            byte_writter_pointer = {PWDATA[4:0],byte_writter_pointer[7:0]}; 
        end
        if(PADDR == 8'h14)	//TX WRITE
        begin
            read_start_byte_pointer = {read_start_byte_pointer[12:8], PWDATA};
        end
        if(PADDR == 8'h18)	//TX WRITE
        begin
            read_start_byte_pointer = {PWDATA[4:0], read_start_byte_pointer[7:0]};
        end

        if(PADDR == 8'h1C)	//TX WRITE
        begin
            read_end_byte_pointer = {read_end_byte_pointer[12:8], PWDATA};
        end
        if(PADDR == 8'h20)	//TX WRITE
        begin
            read_end_byte_pointer = {PWDATA[4:0], read_end_byte_pointer[7:0]};
        end
        if(PADDR == 8'h24)  //TX WRITE
        begin
            write_data = PWDATA;
        end
	end


	always @(posedge re_enable)
	begin
		if(PADDR == 8'h00)	//TX READ
        begin
            PRDATA = status_reg;
        end
        if(PADDR == 8'h04)	//TX READ
        begin
            PRDATA = config_reg;
        end
        if(PADDR == 8'h08)	//TX READ
        begin
            PRDATA = resets_reg;
        end
        if(PADDR == 8'h0C)	//TX READ
        begin
            PRDATA = byte_writter_pointer[7:0];
        end
        if(PADDR == 8'h10)	//TX READ
        begin
            PRDATA ={3'b000,byte_writter_pointer[12:8]};
        end
        if(PADDR == 8'h14)	//TX READ
        begin
            PRDATA = read_start_byte_pointer[7:0];
        end
        if(PADDR == 8'h18)	//TX READ
        begin
            PRDATA = {3'b000,read_start_byte_pointer[12:8]};
        end

        if(PADDR == 8'h1C)	//TX READ
        begin
            PRDATA = read_end_byte_pointer[7:0];
        end

        if(PADDR == 8'h20)	//TX READ
        begin
            PRDATA = {3'b000,read_end_byte_pointer[12:8]};
        end

        if(PADDR == 8'h24)	//TX READ
        begin
            PRDATA = write_data;
        end

        if(PADDR == 8'h28)  //TX READ
        begin
            PRDATA = read_data;
        end       

        if(PADDR == 8'h2C)  //TX READ
        begin
            PRDATA = w_byte_read_pointer[7:0];
        end

        if(PADDR == 8'h30)  //TX READ
        begin
            PRDATA = {3'b000, w_byte_read_pointer[12:8]};
        end

        if(PADDR == 8'h34)  //TX READ
        begin
            PRDATA = w_read_offset[7:0];
        end

        if(PADDR == 8'h38)  //TX READ
        begin
            PRDATA = {3'b000, w_read_offset[12:8]};
        end

        if(PADDR == 8'h3C)  //TX READ
        begin
            PRDATA = w_fcs[7:0];
        end

        if(PADDR == 8'h40)  //TX READ
        begin
            PRDATA = w_fcs[15:8];
        end
	end

    

endmodule