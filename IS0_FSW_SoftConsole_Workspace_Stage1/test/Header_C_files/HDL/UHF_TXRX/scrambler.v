module scramber(
    input i_reset,
    input i_TXRX_CLOCK,
    input i_RX,
    input i_TX,

    output reg o_RX,
    output reg o_TX
   );

reg [16:0] scram_tx_reg;
reg [16:0] scram_rx_reg;

reg tx_tmp;

always @(negedge i_TXRX_CLOCK) begin
    if (i_reset) begin
        o_RX = i_RX ^ scram_rx_reg[5] ^ scram_rx_reg[0];
        scram_rx_reg = scram_rx_reg >> 1;
        scram_rx_reg[16] = i_RX;

        o_TX = scram_tx_reg[16];
        tx_tmp = i_TX ^ scram_tx_reg[5] ^ scram_tx_reg[0];
        scram_tx_reg = scram_tx_reg >> 1;
        scram_tx_reg[16] = tx_tmp;
    end
    else
    begin
        scram_rx_reg = 17'b00000000000000000;
        scram_tx_reg = 17'b00000000000000000;
    end
end

endmodule
