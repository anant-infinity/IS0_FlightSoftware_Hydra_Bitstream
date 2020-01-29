module apb_i2c_master(
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

	// I2C ports
	inout SCL,
    inout SDA,

	// LSRAM ports
	output [19:0] PADDR_SRAM,
	output PSEL_SRAM,
	output PWRITE_SRAM,
	output PENABLE_SRAM,
	output [7:0] PWDATA_SRAM,
	input [7:0] PRDATA_SRAM,
	input PREADY_SRAM,
	input PSLVERR_SRAM,

    output read_data_ready_o
	);

	assign PREADY =1;
	assign PSLVERR = 0;

    wire wr_enable;
    wire re_enable;
    assign wr_enable = (PENABLE && PWRITE && PSEL);
    assign re_enable = (!PWRITE && PSEL && PENABLE);

	wire [7:0] status_reg; 
    reg [7:0] config_reg; 
    reg [7:0] resets_reg; 

    // LSRAM to uP wires and regs
    reg [12:0] byte_writter_pointer;
    reg [7:0] write_data;

    reg [7:0] r_count_limit;

    reg [12:0] read_end_byte_pointer;
    

    // LSRAM to i2c master wires
    wire [7:0] w_read_data;
    wire w_start_read;
    wire w_reset_read;
    wire w_read_data_ready;
    wire [12:0] w_byte_read_pointer;
    wire [15:0] w_rec_data;

    // i2c wires
    wire w_SCL, w_i2c_scl_shifted;

    assign status_reg[7] = SCL;
    assign status_reg[6] = SDA;
    assign read_data_ready_o = w_read_data_ready;
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
    .o_read_data_ready(w_read_data_ready),
    .o_read_data(w_read_data)
    );

    i2c_masterv2 uut_i2c(

    .i_SCL(w_SCL),
    .i_SCL_Shifted(w_i2c_scl_shifted),
    .i_Reset(PRESETN && resets_reg[0]),
    .i_EN_TX(config_reg[1]),
    .i_EN_RX(config_reg[2]),
    .o_SCL(SCL),
    .o_SDA(SDA),
    
    .i_tx_end(read_end_byte_pointer),
    .o_rx_buffer(w_rec_data),
    .large_rec(config_reg[3]),
    
    .o_tx_done(status_reg[4]),
    .o_tx_fault(status_reg[5]),
    .o_rx_done(status_reg[3]),
        
    .i_read_data(w_read_data),
    .i_read_data_ready(w_read_data_ready),
    .o_read_offset(w_byte_read_pointer),
    .o_start_read(w_start_read),
    .o_reset_read(w_reset_read)

    );

    
 i2c_clk_gen uut2(
     .CLK(PCLK),
     .reset(PRESETN),
     .limit(r_count_limit),
     .I2C_SCL(w_SCL),
     .I2C_SCL_shifted(w_i2c_scl_shifted)
     );

	
	always@(posedge wr_enable )
	begin
		
		if(resets_reg[0] == 0 || PRESETN == 0)
		begin
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
        if(PADDR == 8'h14)
        begin
            r_count_limit = PWDATA;
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
        if(PADDR == 8'h14)
        begin
            PRDATA = r_count_limit;
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
            PRDATA = w_read_data;
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
            PRDATA = w_rec_data[7:0];
        end
        if(PADDR == 8'h38)  //TX READ
        begin
            PRDATA = w_rec_data[15:8];
        end
	end

    

endmodule