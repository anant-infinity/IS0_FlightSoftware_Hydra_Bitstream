module UART_TX(
    input reset,
    input       i_Clock,
    input       i_Tx_Sig,
    input [7:0] i_Tx_Byte, 
    input [7:0] CLKS_PER_BIT,
    output      o_Tx_idle,
    output      o_Tx_Active,
    output      o_Tx_Done,
    output reg  o_Tx_Serial,
    output [7:0] o_SM_Main,
    output [7:0] o_Bit_Index
    );


localparam s_idle       = 8'b00000001;
localparam s_start      = 8'b00000010;
localparam s_data_bits  = 8'b00000100;
localparam s_end        = 8'b00001000;

reg [7:0]   r_SM;
reg [7:0]   r_tx_data;
reg [7:0]   r_bit_index;
reg [7:0]  r_counter;
reg         r_tx_idle;
reg         r_tx_active;
reg         r_tx_done;

assign o_Tx_Active  = r_tx_active;
assign o_Tx_Done    = r_tx_done;
assign o_Tx_idle    = r_tx_idle;
assign o_SM_Main    = r_SM;
assign o_Bit_Index  = r_bit_index;

always@(posedge i_Clock )
begin

    if(reset == 1)
    begin
        case(r_SM)
            s_idle :
            begin
                o_Tx_Serial = 1'b1;
                r_bit_index = 8'h00;
                r_tx_data   = 8'h55;    // for testing purpose it has been kept 0x55;
                r_tx_idle   = 1'b1;
                r_tx_active = 1'b0;
                r_counter = 8'h00;
                if(i_Tx_Sig == 1'b1 && r_tx_done == 1'b0)
                begin
                    r_tx_data = i_Tx_Byte;
                    r_SM = s_start;
                end
                else
                begin
                    r_SM = s_idle;
                end
            end

            s_start :
            begin
                o_Tx_Serial = 1'b0;
                r_tx_idle = 1'b0;
                r_tx_done = 1'b0;
                r_tx_active = 1'b1;
                
                if(r_counter < (CLKS_PER_BIT -1 ))
                begin
                    r_counter = r_counter + 1;   
                    r_bit_index = 8'h00; 
                    r_SM = s_start;              
                end
                else begin
                   r_counter =  8'h00;
                   r_SM = s_data_bits;
                end
            end

            s_data_bits :
            begin
                o_Tx_Serial = r_tx_data[r_bit_index];
                r_tx_idle = 1'b0;
                r_tx_done = 1'b0;
                r_tx_active = 1'b1;

                if(r_counter < (CLKS_PER_BIT-1))
                begin
                    r_counter = r_counter + 1;
                    r_SM = s_data_bits;
                end
                else begin
                    if(r_bit_index < 7)
                    begin
                        r_bit_index = r_bit_index + 1;
                        r_SM = s_data_bits;
                        
                    end
                    else begin
                        r_bit_index = 0;
                        r_SM = s_end;
                    end
                    r_counter = 0;
                end
            end

            s_end :
            begin
                o_Tx_Serial = 1'b1;
                r_tx_idle = 1'b0;
                r_tx_done = 1'b0;
                r_tx_active = 1'b1;

                if(r_counter < (CLKS_PER_BIT -1 ))
                begin
                    r_counter = r_counter + 1;
                    r_SM = s_end;              
                end
                else begin
                   r_counter =  8'h00;
                   r_tx_done = 1'b1;
                   r_SM = s_idle;
                end
            end

            default :
            begin
                r_SM = s_idle;
            end
        endcase

    end
    else
    begin
        r_SM = s_idle;
        r_tx_data = 8'h55;
        r_bit_index = 8'h00;
        r_counter = 16'h0000;
        r_tx_idle = 1'b0;
        r_tx_active = 1'b0;
        r_tx_done = 1'b0;
    end

end



endmodule