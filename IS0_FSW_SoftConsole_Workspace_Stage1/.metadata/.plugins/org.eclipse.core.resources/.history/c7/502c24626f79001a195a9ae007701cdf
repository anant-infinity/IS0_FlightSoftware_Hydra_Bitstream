//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    14:21:29 05/31/2018 
// Design Name: 
// Module Name:    comb_test 
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

/* To be Updated

*/
//
//////////////////////////////////////////////////////////////////////////////////
module uhf_rx_v2(
	input i_EN,
	input i_Reset_all,
	input i_TX_req,
    input i_TXRX_CLOCK,
    input i_RX_DATA,
    input i_write_completed,

    output reg o_FRAME_STARTED,
    output reg o_FRAME_DEST_MATCHED,
    output reg o_FRAME_ENDED,
    output reg o_FRAME_FAULT,
    output reg o_SAVE_FAULT,

    output reg [15:0] o_fcs,
    
    output reg [7:0] o_write_data,
    output reg [12:0] o_write_pointer,
    output reg o_save_byte
    );

	reg [7:0] p_dest_addr[6:0];
	
	localparam p_poly = 16'h8408;

	localparam s_IDLE = 3'b000;
	localparam s_FLAG_HUNTING = 3'b001;
	localparam s_DEST_ADDR_SCOUTING = 3'b010;
	localparam s_DATA_COLLECTION = 3'b011;
	localparam s_KICK_OUT_BIT_STUFF = 3'b100;

	wire w_reset;
	//assign w_reset = i_Reset_all & ~i_TX_req;		// we need to use this
	assign w_reset = i_Reset_all;	// for testing purposes only

	reg [7:0] r_rx_buff;
	
	reg [2:0] r_RX_SM;
	reg [2:0] r_RX_GOTO_SM;
	
	reg [7:0] r_bit_stuff_searcher;
	reg r_kicking_initiated;
	reg r_bit_stuff_kicked;
	
	reg [7:0] r_bit_counter;
	reg [7:0] r_addr_cmp_counter;

	reg [15:0] r_fcs_tmp;

	always @ (posedge i_TXRX_CLOCK)
	begin
        if(w_reset == 1)
        begin
        	if (i_EN == 1)
			begin
				case(r_RX_SM)
					
					s_IDLE:
					begin
						r_bit_stuff_searcher = 0;
						r_addr_cmp_counter = 0;
						r_bit_counter = 0;
						if(o_FRAME_ENDED == 0)
						begin
							r_RX_SM = s_FLAG_HUNTING;	
						end
						

						r_kicking_initiated = 0;
						r_fcs_tmp = 16'hffff;
					end

					s_FLAG_HUNTING:
					begin
						if(o_save_byte == 1)
						begin
							if(i_write_completed == 1)
							begin
								o_save_byte = 0;
								o_write_pointer = (o_write_pointer + 1) % 1024;	
							end
							else begin
								o_SAVE_FAULT = 1;
							o_FRAME_ENDED = 1;
							r_RX_SM = s_IDLE;
							end
						end
						o_FRAME_ENDED = 0;
						o_FRAME_STARTED = 0;
						o_FRAME_DEST_MATCHED = 0;
						r_RX_GOTO_SM = s_FLAG_HUNTING;
						r_fcs_tmp = 16'hffff;
						if(r_rx_buff == 8'h7E)
						begin
							r_RX_SM = s_DEST_ADDR_SCOUTING;
							r_bit_counter = 1;
							o_FRAME_STARTED = 1;
							if(i_RX_DATA == 1)
							begin
								r_bit_stuff_searcher = r_bit_stuff_searcher + 1;
							end
							else begin
								r_bit_stuff_searcher = 0;
							end

						end
						else begin
							r_RX_SM = s_FLAG_HUNTING;
						end
						r_rx_buff = r_rx_buff >> 1;
						r_rx_buff[7] = i_RX_DATA;
						if( i_RX_DATA ^  r_fcs_tmp[0])
						begin
							r_fcs_tmp = r_fcs_tmp >> 1;
							r_fcs_tmp = r_fcs_tmp ^ p_poly;
						end
						else begin
							r_fcs_tmp = r_fcs_tmp >> 1;
						end
						
					end

					s_DEST_ADDR_SCOUTING:
					begin
						if(o_save_byte == 1)
						begin
							if(i_write_completed == 1)
							begin
								o_save_byte = 0;
								o_write_pointer = (o_write_pointer + 1) % 1024;	
							end
							else begin
								o_SAVE_FAULT = 1;
								o_FRAME_ENDED = 1;
								r_RX_SM = s_IDLE;
							end
						end

						if(r_bit_counter == 8)
						begin
							r_bit_counter = 0;
							if(r_kicking_initiated == 1 && r_bit_stuff_kicked == 0)
							begin
								r_fcs_tmp = 16'hffff;
								if(r_rx_buff == 8'h7E)
								begin
									if(r_addr_cmp_counter == 0)
									begin
										r_RX_SM = s_DEST_ADDR_SCOUTING;
									end
									else begin
										o_FRAME_FAULT = 1;
										r_RX_SM = s_FLAG_HUNTING;
									end	
								end
								else begin
									o_FRAME_FAULT = 1;
									r_RX_SM = s_FLAG_HUNTING;
								end	
							end
							else begin
								if(p_dest_addr[r_addr_cmp_counter] == r_rx_buff) 
								begin
									/* Store it in the sram */
									o_write_data = r_rx_buff;
									o_save_byte = 1;

									r_addr_cmp_counter = r_addr_cmp_counter + 1;
									if(r_addr_cmp_counter == 7)
									begin
										o_FRAME_DEST_MATCHED = 1;
										r_RX_SM = s_DATA_COLLECTION;
									end
									else begin
										r_RX_SM = s_DEST_ADDR_SCOUTING;
									end
								end
								else begin
									r_RX_SM = s_FLAG_HUNTING;
									o_FRAME_FAULT = 1;
								end
							end
							o_fcs = r_fcs_tmp;

							r_bit_stuff_kicked = 0;
							r_kicking_initiated = 0;
						end

						if(i_RX_DATA == 1)
						begin
							r_bit_stuff_searcher = r_bit_stuff_searcher + 1;
						end
						else begin
							r_bit_stuff_searcher = 0;
						end

						if(r_bit_stuff_searcher == 5)
						begin
						 	r_RX_GOTO_SM = r_RX_SM;
						 	r_RX_SM = s_KICK_OUT_BIT_STUFF;
						end
						
						r_rx_buff = r_rx_buff >> 1;
						r_rx_buff[7] = i_RX_DATA;
						r_bit_counter = r_bit_counter + 1;

						if( i_RX_DATA ^  r_fcs_tmp[0])
						begin
							r_fcs_tmp = r_fcs_tmp >> 1;
							r_fcs_tmp = r_fcs_tmp ^ p_poly;
						end
						else begin
							r_fcs_tmp = r_fcs_tmp >> 1;
						end
					end

					s_DATA_COLLECTION:
					begin
						if(o_save_byte == 1)
						begin
							if(i_write_completed == 1)
							begin
								o_save_byte = 0;
								o_write_pointer = (o_write_pointer + 1) % 1024;	
							end
							else begin
								o_SAVE_FAULT = 1;
							o_FRAME_ENDED = 1;
							r_RX_SM = s_IDLE;
							end
						end


						if(r_bit_counter == 8)
						begin
							r_bit_counter = 0;
							if(r_kicking_initiated == 1 && r_bit_stuff_kicked == 0)
							begin
								if(r_rx_buff == 8'h7E)
								begin
									/* End flag captured */
									o_fcs = o_fcs ^ 16'hffff;
									r_fcs_tmp = 16'hffff;
									o_FRAME_ENDED = 1;
									r_RX_SM = s_IDLE;
								end
								else begin
									o_FRAME_FAULT = 1;
									r_RX_SM = s_FLAG_HUNTING;
								end	
							end
							else begin
								
								o_fcs = r_fcs_tmp;

								o_write_data = r_rx_buff;
								o_save_byte = 1;

								r_RX_SM = s_DATA_COLLECTION;
							end

							r_bit_stuff_kicked = 0;
							r_kicking_initiated = 0;
						end

						if(i_RX_DATA == 1)
						begin
							r_bit_stuff_searcher = r_bit_stuff_searcher + 1;
						end
						else begin
							r_bit_stuff_searcher = 0;
						end

						if(r_bit_stuff_searcher == 5)
						begin
						 	r_RX_GOTO_SM = s_DATA_COLLECTION;
						 	r_RX_SM = s_KICK_OUT_BIT_STUFF;
						end
						
						r_rx_buff = r_rx_buff >> 1;
						r_rx_buff[7] = i_RX_DATA;
						r_bit_counter = r_bit_counter + 1;
						if( i_RX_DATA ^  r_fcs_tmp[0])
						begin
							r_fcs_tmp = r_fcs_tmp >> 1;
							r_fcs_tmp = r_fcs_tmp ^ p_poly;
						end
						else begin
							r_fcs_tmp = r_fcs_tmp >> 1;
						end
					end

					s_KICK_OUT_BIT_STUFF:
					begin
						r_bit_stuff_searcher = 0;
						r_kicking_initiated = 1;
						if(i_RX_DATA == 0)
						begin
							r_bit_stuff_kicked = 1;
						end
						else begin
							r_rx_buff = r_rx_buff >> 1;
							r_rx_buff[7] = i_RX_DATA;
							r_bit_counter = r_bit_counter + 1;

							if( i_RX_DATA ^  r_fcs_tmp[0])
							begin
								r_fcs_tmp = r_fcs_tmp >> 1;
								r_fcs_tmp = r_fcs_tmp ^ p_poly;
							end
							else begin
								r_fcs_tmp = r_fcs_tmp >> 1;
							end

							r_bit_stuff_searcher = r_bit_stuff_searcher + 1;	
						end
						r_RX_SM = r_RX_GOTO_SM;
					end

					default:
					begin
						r_RX_SM = s_IDLE;
					end

				endcase
			end
			else
			begin
	        	r_rx_buff = 0;
	        	o_FRAME_STARTED  = 0;
	        	o_FRAME_ENDED = 0;
				o_SAVE_FAULT = 0;
	        	r_RX_SM = s_IDLE;
	        	r_bit_stuff_searcher = 0;
	        	o_FRAME_FAULT = 0;
	        	r_addr_cmp_counter = 0;
	        	o_write_data = 0;
	        	o_FRAME_DEST_MATCHED = 0;
			end	
        end
        else begin
        	r_rx_buff = 0;
        	o_FRAME_STARTED  = 0;
        	o_FRAME_ENDED = 0;
        	r_RX_SM = s_IDLE;
        	r_bit_stuff_searcher = 0;
        	r_bit_counter = 0;
        	o_SAVE_FAULT = 0;
        	o_FRAME_FAULT = 0;
        	r_addr_cmp_counter = 0;
        	r_kicking_initiated = 0;
        	r_bit_stuff_kicked = 0;
        	r_RX_GOTO_SM = s_IDLE;
        	o_write_data = 0;
        	o_write_pointer = 0;
        	o_save_byte = 0;
        	r_fcs_tmp = 16'hffff; 
        	o_fcs = 16'hffff; 
        	o_FRAME_DEST_MATCHED = 0;

        	p_dest_addr[0] = 8'h92;
            p_dest_addr[1] = 8'hA6;
            p_dest_addr[2] = 8'h5A;
            p_dest_addr[3] = 8'h62;
            p_dest_addr[4] = 8'h40;
            p_dest_addr[5] = 8'h40;
            p_dest_addr[6] = 8'h60;

        end
		
	end

	
endmodule
