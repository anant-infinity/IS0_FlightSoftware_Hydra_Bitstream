module i2c_masterv2(

    input i_SCL,
    input i_SCL_Shifted,
    input i_Reset,
    input i_EN_TX,
    input i_EN_RX,
    inout o_SCL,
    inout o_SDA,
    
    input [12:0] i_tx_end,
    output reg [15:0] o_rx_buffer,
    input large_rec,
    output reg o_tx_done,
    output reg o_tx_fault,
    output reg o_rx_done,
    
    
    input [7:0] i_read_data,
    input i_read_data_ready,
    output reg [12:0] o_read_offset,
    output reg o_start_read,
    output reg o_reset_read

);


localparam s_Idle = 4'b0000;
localparam s_Transmit_Init = 4'b0001;
localparam s_Transmit_Data = 4'b0010;
localparam s_Transmit_Rec_Ack = 4'b0011;
localparam s_Transmit_End = 4'b0100;
localparam s_Receive_Data = 4'b0101; 
localparam s_Receive_ACK = 4'b0110;
localparam s_Receive_NACK = 4'b0111;
localparam s_Enable_Read = 4'b1000;

reg [3:0] r_SM;
reg [3:0] r_transmit_local_offset, r_receive_local_offset;

reg rx_counter;

reg delay_sig;
reg [7:0] r_tx_buffer;

reg r_scl_sig;
reg r_sda_sig;

assign o_SDA = r_sda_sig ? 1'bz : 1'b0;
wire i_SDA;
assign i_SDA = o_SDA;
assign o_SCL = r_scl_sig ? 1'bz : (i_SCL ? 1'bz: 1'b0);


always @(posedge i_SCL_Shifted) 
begin
    if (i_Reset == 1) 
    begin
        case(r_SM)

            s_Idle: // 0
            begin
                r_scl_sig = 1;
                r_sda_sig = 1;
                if(i_EN_TX  == 1 && o_tx_done == 0) 
                begin
                    if(i_read_data_ready == 0)
                    begin
                        o_read_offset = 0;
                        r_SM = s_Enable_Read;
                    end
                    else begin
                        r_tx_buffer = i_read_data;
                        r_transmit_local_offset = 0;
                        r_SM = s_Transmit_Init; 
                    end
                end
                else begin
                    r_SM = s_Idle;
                end             
                o_start_read = 1;
                o_reset_read = 0;
            end

            s_Enable_Read:  // 8
            begin
                o_start_read = 1;
                o_reset_read = 1;
                r_SM = s_Idle;
            end

            s_Transmit_Init:    // 1
            begin
                o_start_read = 1;
                o_reset_read = 1;
                if(i_SCL == 1)
                begin
                    r_scl_sig = 0;
                    r_sda_sig = 0;
                    r_SM = s_Transmit_Data;
                end
                else begin
                    r_SM = s_Transmit_Init;
                end
            end

            s_Transmit_Data:    // 2
            begin
                if(r_transmit_local_offset == 0)
                begin
                    r_tx_buffer = i_read_data;
                    o_read_offset = o_read_offset + 1;
                    o_start_read = 1;
                    o_reset_read = 0;
                end
                
                if(r_transmit_local_offset < 8)
                begin
                    if(i_SCL == 0)
                    begin
                        r_sda_sig = r_tx_buffer[7];
                        r_tx_buffer = r_tx_buffer << 1;

                        r_transmit_local_offset = r_transmit_local_offset + 1;
                        
                    end
                    r_SM = s_Transmit_Data;
                end
                else
                begin
                    o_start_read = 1;
                    o_reset_read = 1;
                    r_SM = s_Transmit_Rec_Ack;
                end
                
            end

            s_Transmit_Rec_Ack: // 3
            begin
                o_start_read = 1;
                o_reset_read = 1;   
                if(i_SCL == 0)
                begin
                    r_sda_sig = 1;
                end
                else begin
                    if(o_read_offset < i_tx_end)
                    begin
                        r_transmit_local_offset = 0;
                        r_SM = s_Transmit_Data;
                    end
                    else begin
                        o_tx_done = 1;
                        if(i_EN_RX == 1)
                        begin
                            r_sda_sig = 1;
                            rx_counter = large_rec;
                            r_SM = s_Receive_Data;
                        end
                        else begin
                            r_SM = s_Transmit_End;  
                        end
                    end
                    if(i_SDA == 1)
                    begin
                        o_tx_fault = 1;
                        // r_SM = s_Transmit_End;
                    end
                end

            end

            s_Transmit_End: // 4
            begin
                if (i_SCL == 0) 
                begin
                    r_sda_sig = 0;
                    r_SM = s_Transmit_End;                  
                end
                else begin
                    r_sda_sig = 1;
                    r_scl_sig = 1;
                    r_SM = s_Idle;

                end
            end

            s_Receive_Data: // 5
            begin
                delay_sig = 0;
                r_sda_sig = 1;
                if(i_SCL == 1)
                begin
                    o_rx_buffer = o_rx_buffer << 1;
                    o_rx_buffer[0] = i_SDA;
                    r_receive_local_offset = r_receive_local_offset + 1;
                end 
                if(r_receive_local_offset < 8)
                begin
                    r_SM = s_Receive_Data;
                end
                else begin
                    r_receive_local_offset = 0;
                    r_sda_sig = 1;
                    if(rx_counter == 0)
                    begin
                        rx_counter = 1;
                        r_SM = s_Receive_ACK;
                    end
                    else begin
                        r_SM = s_Receive_NACK;
                    end
                end
            end

            s_Receive_ACK:  // 6
            begin
                if(i_SCL == 0)
                begin
                    
                    if(delay_sig == 0)
                    begin
                        r_sda_sig = 0;
                        delay_sig = 1;
                        r_SM = s_Receive_ACK;
                    end
                    else begin    
                        r_sda_sig = 1;   
                        r_SM = s_Receive_Data;
                    end
                end
            end

            s_Receive_NACK: // 7
            begin
                if(i_SCL == 0)
                begin
                    o_rx_done = 1;
                    if(delay_sig == 0)
                    begin
                        r_sda_sig = 1; 
                        delay_sig = 1;
                        r_SM = s_Receive_NACK;   
                    end
                    else begin
                        r_sda_sig = 0;
                        r_SM = s_Transmit_End;
                    end
                end
            end

            default:
            begin
                r_SM = s_Idle;
            end

        endcase
    end
    else 
    begin
        rx_counter = 0;
        r_scl_sig = 1;
        r_sda_sig = 1;
        r_SM = s_Idle;
        r_transmit_local_offset = 0;
        r_receive_local_offset = 0;
        r_tx_buffer = 0;
        o_rx_buffer = 0;
        o_tx_done = 0;
        o_rx_done = 0;
        o_tx_fault = 0;
        o_read_offset = 0;
        delay_sig = 0;
    end
end
endmodule