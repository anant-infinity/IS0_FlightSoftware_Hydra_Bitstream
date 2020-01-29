
///////////////////////////////////////////////////////////////////////////////////////////////////
// Company: <Name>
//
// File: uhf_tx_sram_interface.v
// File history:
//      <Revision number>: <Date>: <Comments>
//      <Revision number>: <Date>: <Comments>
//      <Revision number>: <Date>: <Comments>
//
// Description: 
//
// <Description here>
//
// Targeted device: <Family::SmartFusion2> <Die::M2S060> <Package::484 FBGA>
// Author: <Name>
//
/////////////////////////////////////////////////////////////////////////////////////////////////// 

//`timescale <time_units> / <precision>

module uhf_tx_sram_interface( input i_PCLK,
    input i_Reset_all,
    input i_Reset_Read,
    input [7:0] i_write_data,
    input i_SAVE_BYTE,
    input i_start_read,
    input [12:0] i_byte_write_pointer,
    input [12:0] i_byte_read_pointer,
    
    output reg[19:0] o_PADDR_SRAM,
    input [7:0] i_PRDATA_SRAM,
    output reg o_PSEL_SRAM,
    output reg o_PWRITE_SRAM,
    output reg o_PENABLE_SRAM,
    output reg[7:0] o_PWDATA_SRAM,
    input i_PREADY_SRAM,
    input i_PSLVERR_SRAM,

    output reg o_apb_write_busy,
    output reg o_write_completed,
    output reg o_read_data_allow,
    output reg o_read_data_ready,
    output reg [7:0] o_read_data
);

localparam s_SRAM_IDLE = 2'b00;
localparam s_WRITE_STATE1 = 2'b01;
localparam s_WRITE_STATE2 = 2'b10;
reg [1:0] r_write_state;

localparam s_READ_STATE1 = 2'b01;
localparam s_READ_STATE2 = 2'b10;
reg [1:0] r_read_state;


always @(posedge i_PCLK ) begin

    if (i_Reset_all == 1'b1) 
    begin

        if(i_SAVE_BYTE == 1'b1)
        begin
            case(r_write_state)
            
                s_SRAM_IDLE:
                begin
                    o_PENABLE_SRAM = 0;
                    o_PSEL_SRAM = 0;

                    o_apb_write_busy = 0;
                
                    if(o_write_completed == 0)begin
                        r_write_state = s_WRITE_STATE1;
                    end
                    else begin
                        r_write_state = s_SRAM_IDLE;
                    end
                end

                s_WRITE_STATE1:
                begin
                    o_PADDR_SRAM = {7'b0000000, i_byte_write_pointer};
                    o_PWRITE_SRAM  = 1;
                    o_PSEL_SRAM = 1;
                    o_PWDATA_SRAM = i_write_data;
                    o_PENABLE_SRAM = 0;
                    
                    o_apb_write_busy = 1;
                    o_write_completed = 0;
                    r_write_state = s_WRITE_STATE2;
                end

                s_WRITE_STATE2:
                begin
                    o_PENABLE_SRAM = 1;
                    o_PWRITE_SRAM  = 1;
                    o_PSEL_SRAM = 1;
                    o_apb_write_busy = 1;
                    o_write_completed = 1;

                    r_write_state = s_SRAM_IDLE;
                end

                default:
                begin
                    r_write_state = s_SRAM_IDLE;                    
                end
                
            endcase
        end
        else 
        begin
            o_write_completed = 0;
            r_write_state = s_SRAM_IDLE;
            if(i_start_read == 1 && o_read_data_ready == 0)
            begin
                case(r_read_state)

                    s_READ_STATE1:
                    begin
                        o_PADDR_SRAM = {7'b0000000, i_byte_read_pointer};
                        o_PWRITE_SRAM = 0;
                        o_PSEL_SRAM = 1;
                        o_PENABLE_SRAM = 0;

                        o_read_data_allow = 0;
                        r_read_state = s_READ_STATE2;
                    end

                    s_READ_STATE2:
                    begin
                        o_PENABLE_SRAM = 1;
                        o_PSEL_SRAM = 1;
                        o_PWRITE_SRAM = 0;
                        o_read_data_allow = 1;
                        r_read_state = s_READ_STATE1;
                    end
                        
                    default:
                    begin
                        o_PENABLE_SRAM = 0;
                        r_read_state = s_READ_STATE1;
                    end
                endcase
            end
            else begin
                r_read_state = s_READ_STATE1;
            end
            
        end
        
    end
    else
    begin
        r_read_state = s_READ_STATE1;
        r_write_state = s_SRAM_IDLE;
        o_read_data_allow = 0;
        
        o_write_completed = 0;
        o_apb_write_busy = 0;

        o_PADDR_SRAM = 0;
        o_PSEL_SRAM = 0;
        o_PWDATA_SRAM = 0;
        o_PENABLE_SRAM = 0;
        o_PWRITE_SRAM = 0;
    end
end


always @(negedge i_PCLK ) begin
    if(o_read_data_allow == 1 && o_read_data_ready == 0)
    begin
        o_read_data = i_PRDATA_SRAM;
        o_read_data_ready = 1;
    end
    if(i_Reset_Read == 0 || i_Reset_all == 0)
    begin
        o_read_data_ready = 0;
        o_read_data = 0;
    end
end

endmodule

