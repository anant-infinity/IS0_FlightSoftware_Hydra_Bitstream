module ax_delayer(
   input i_TXRX_CLOCK,
   input i_RX_DATA,
   input i_Reset,

    output reg o_R1,
    output reg o_R2
   );


always @(posedge i_TXRX_CLOCK) begin
    if (i_Reset) begin
        o_R2 = o_R1;
        o_R1 = i_RX_DATA;
    end
    else
    begin
        o_R1 = 0;
        o_R2 = 0;
    end
end


endmodule