module uhf_tx_data_v2(
    input i_EN_TX,
    input i_TXRX_CLOCK,
    input [7:0] i_read_data,
    input i_read_data_ready,
    
    input i_Reset_all,
    input [12:0] i_start_byte_read_pointer,
    input [12:0] i_end_byte_read_pointer,
    output reg o_TX_DATA,
    output reg o_TX_DATA_NRZI,
    output reg o_transmit_done,
    output reg o_start_read,
    output reg o_reset_read,
    output reg [12:0] o_read_offset,
    output reg [12:0] o_byte_read_pointer,
    output reg [15:0] o_fcs,
    output reg o_tx_req,
    output reg [2:0] r_transmit_states
    );
localparam p_poly = 16'h8408;
localparam p_flag_times= 16'h08;
localparam s_Transmit_Init = 3'b000;
localparam s_Transmit_Enable_Read = 3'b001;
localparam s_Transmit_Continue = 3'b011;
localparam s_Transmit_FCS = 3'b100;
localparam s_Transmit_Flag = 3'b101;
reg [2:0] r_bit_stuff_count;
reg r_avoid_flag_transmit;
reg [7:0] r_Flag;
reg [15:0] r_flag_transmit_nos;

reg [7:0] r_transmit_buffer;
reg [3:0] r_transmit_bit_offset;
always @(negedge i_TXRX_CLOCK ) begin
    if (i_Reset_all == 1) begin
        
        if(i_EN_TX == 1)
        begin
            case(r_transmit_states)
                s_Transmit_Init:
                begin
                    r_transmit_bit_offset = 0;
                    
                    if(o_transmit_done == 0)
                    begin
                        
                        if(r_avoid_flag_transmit == 0)
                        begin
                            o_read_offset = 0;
                            o_byte_read_pointer = i_start_byte_read_pointer;
                            r_transmit_states = s_Transmit_Flag;
                            r_avoid_flag_transmit = 1;
                            o_tx_req = 0;
                            o_start_read = 1;
                            o_reset_read = 0;
                        end
                        else if(i_read_data_ready == 0)
                        begin
                            o_byte_read_pointer = i_start_byte_read_pointer;
                            r_transmit_states = s_Transmit_Enable_Read;
                            o_TX_DATA = 0;
                            o_tx_req = 0;
                            o_start_read = 1;
                            o_reset_read = 0;
                        end
                        else 
                        begin
                            o_tx_req = 1;
                            if(r_bit_stuff_count >= 5)
                            begin
                                o_TX_DATA = 0;
                                r_bit_stuff_count = 0;
                                r_transmit_states = s_Transmit_Init;
                            end
                            else
                            begin
                                r_transmit_buffer = i_read_data;
                                o_read_offset = o_read_offset + 1;
                                o_byte_read_pointer = i_start_byte_read_pointer + o_read_offset; 
                                o_TX_DATA = r_transmit_buffer[r_transmit_bit_offset];
                                
                                if(o_TX_DATA == 1)
                                begin
                                    r_bit_stuff_count = r_bit_stuff_count + 1;
                                end
                                else begin
                                    r_bit_stuff_count = 0;
                                end
                                
                                if( o_TX_DATA ^  o_fcs[0])
                                begin
                                    o_fcs = o_fcs >> 1;
                                    o_fcs = o_fcs ^ p_poly;
                                end
                                else begin
                                    o_fcs = o_fcs >> 1;
                                end
                            
                                r_transmit_bit_offset = r_transmit_bit_offset + 1;
                                r_transmit_states = s_Transmit_Continue;

                                o_start_read = 1;
                                o_reset_read = 0; 
                            end
                        end
                          
                    end
                    else 
                    begin
                        o_tx_req = 0;
                        r_transmit_states = s_Transmit_Init;
                        o_start_read = 0;
                        o_reset_read = 0;
                    end
                    
                end 
                
                s_Transmit_Enable_Read:
                begin
                    o_reset_read = 1;
                    o_start_read = 1;
                    r_transmit_states = s_Transmit_Init;
                end
                s_Transmit_Continue:
                begin
                    o_tx_req = 1;
                    o_reset_read = 1;
                    o_start_read = 1;
                    if(r_bit_stuff_count >= 5)
                    begin
                        o_TX_DATA = 0;
                        r_bit_stuff_count = 0;
                    end
                    else
                    begin
                        o_TX_DATA = r_transmit_buffer[r_transmit_bit_offset];
                        if( o_TX_DATA ^  o_fcs[0])
                        begin
                            o_fcs = o_fcs >> 1;
                            o_fcs = o_fcs ^ p_poly;
                        end
                        else begin
                            o_fcs = o_fcs >> 1;
                        end
                        if(o_TX_DATA == 1)
                        begin
                            r_bit_stuff_count = r_bit_stuff_count + 1;
                        end
                        else begin
                            r_bit_stuff_count = 0;
                        end
                        r_transmit_states = s_Transmit_Continue;
                        if(r_transmit_bit_offset == 7)
                        begin
                            r_transmit_bit_offset = 0;
                            
                            if(o_byte_read_pointer - 1 >= i_end_byte_read_pointer)
                            begin
                                o_byte_read_pointer = i_end_byte_read_pointer;
                                o_fcs = o_fcs ^ 16'hffff;
                                r_transmit_states = s_Transmit_FCS;
                            end
                            else begin
                                r_transmit_states = s_Transmit_Init;    
                            end
                        end
                        else
                        begin
                            r_transmit_bit_offset = r_transmit_bit_offset + 1;
                        end
                    end
                end
                s_Transmit_FCS:
                begin
                    o_tx_req = 1;
                    o_reset_read = 1;
                    o_start_read = 1;
                    if(r_bit_stuff_count >= 5)
                    begin
                        o_TX_DATA = 0;
                        r_bit_stuff_count = 0;
                    end
                    else
                    begin
                        o_TX_DATA = o_fcs[r_transmit_bit_offset];
                        
                        if(o_TX_DATA == 1)
                        begin
                            r_bit_stuff_count = r_bit_stuff_count + 1;
                        end
                        else begin
                            r_bit_stuff_count = 0;
                        end
                        if(r_transmit_bit_offset == 15)
                        begin
                            r_transmit_bit_offset = 0;
                            r_flag_transmit_nos = 0;
                            r_transmit_states = s_Transmit_Flag;
                        end
                        else begin
                            r_transmit_bit_offset = r_transmit_bit_offset + 1;
                            r_transmit_states = s_Transmit_FCS;
                        end
                    end
                end
                s_Transmit_Flag:
                begin
                    o_tx_req = 1;
                    o_reset_read = 1;
                    o_start_read = 1;
                    r_bit_stuff_count = 0;
                    o_TX_DATA = r_Flag[r_transmit_bit_offset];
                    if(r_transmit_bit_offset == 7)
                    begin
                        r_transmit_bit_offset = 0;
                        r_flag_transmit_nos = r_flag_transmit_nos + 1;
                        if(r_flag_transmit_nos >= p_flag_times)
                        begin
                            if(o_read_offset == 0)
                            begin
                                o_transmit_done = 0;
                            end
                            else begin
                                o_transmit_done = 1;    
                            end
                            r_transmit_states = s_Transmit_Init;                        
                        end
                        else begin
                            r_transmit_states = s_Transmit_Flag;
                        end
                        
                    end
                    else begin
                        r_transmit_bit_offset = r_transmit_bit_offset + 1;
                        r_transmit_states = s_Transmit_Flag;
                    end
                end
                
                default:
                begin
                    r_transmit_states = s_Transmit_Init;
                end
            endcase
        end
        else 
        begin
            
            o_byte_read_pointer = 0;
            o_TX_DATA = 0;
            o_tx_req = 0;
        end
    end
    else
    begin
        
        o_fcs = 16'hffff;
        o_reset_read = 0;
        r_transmit_states = s_Transmit_Init;
        r_transmit_bit_offset = 0;
        r_transmit_buffer = 0;
        o_start_read = 0;
        o_read_offset = 0;
        o_byte_read_pointer = 0;
        o_transmit_done = 0;
        o_TX_DATA = 0;
        o_tx_req = 0;
        r_flag_transmit_nos = 0;
        r_Flag = 8'h7E;
        r_avoid_flag_transmit = 0;
        r_bit_stuff_count = 0;
    end
end
reg r_prev;
always@(negedge i_TXRX_CLOCK)
begin
    if (i_Reset_all == 1)
    begin
        o_TX_DATA_NRZI = ~(o_TX_DATA ^ r_prev);
        r_prev = o_TX_DATA_NRZI;
    end
    else begin
        r_prev = 0;
        o_TX_DATA_NRZI = 0;
    end
end
endmodule