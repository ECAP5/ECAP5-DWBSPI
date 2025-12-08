/*           __        _
 *  ________/ /  ___ _(_)__  ___
 * / __/ __/ _ \/ _ `/ / _ \/ -_)
 * \__/\__/_//_/\_,_/_/_//_/\__/
 * 
 * Copyright (C) Clément Chaine
 * This file is part of ECAP5-DWBSPI <https://github.com/ecap5/ECAP5-DWBSPI>
 *
 * ECAP5-DWBSPI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ECAP5-DWBSPI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ECAP5-DWBSPI.  If not, see <http://www.gnu.org/licenses/>.
 */

module spi_frontend #(
)(
  input   logic  clk_i,
  input   logic  rst_i,

  //=================================
  //    Internal interface
  
  input   logic  cs_i,
  input   logic  prescaled_clk_i,
  input   logic  high_pulse_i,
  input   logic  low_pulse_i,

  input   logic  transmit_i,
  input   logic[7:0]  transmit_data_i,
  output  logic[7:0]  received_data_o,
  output  logic  transmit_done_o,

  //=================================
  //    SPI interface

  output  logic  spi_cs_o,
  output  logic  spi_clk_o,
  output  logic  spi_mosi_o,
  input   logic  spi_miso_i
);

/*****************************************/
/*           Internal signals            */
/*****************************************/

typedef enum {
  IDLE,
  READY,
  RUNNING
} state_t;

state_t state_d, state_q;

logic spi_miso_q, spi_miso_qq;

logic transmit_done_d, transmit_done_q;

logic low_pulse_q, low_pulse_qq;

logic[7:0] tx_shift_reg_d, tx_shift_reg_q;
logic[7:0] rx_shift_reg_d, rx_shift_reg_q;
logic[3:0] bit_cnt_d, bit_cnt_q;

/*****************************************/

always_comb begin
  state_d = state_q;

  transmit_done_d = 0;

  tx_shift_reg_d = tx_shift_reg_q;
  rx_shift_reg_d = rx_shift_reg_q;
  bit_cnt_d = bit_cnt_q;

  case(state_q)
    IDLE: begin
      // If a transmit request has been raised
      if(cs_i && transmit_i) begin
        state_d = READY;
        bit_cnt_d = 8;
        tx_shift_reg_d = transmit_data_i;
        rx_shift_reg_d = '0;
      end
    end
    READY: begin
      // Only start sending on the reception of the next high pulse
      if(high_pulse_i) begin
        state_d = RUNNING; 
      end
    end
    RUNNING: begin
      if(high_pulse_i) begin
        tx_shift_reg_d = {tx_shift_reg_q[6:0], 1'b0}; 

        if(bit_cnt_q == 0) begin
          state_d = IDLE;
          transmit_done_d = 1;
        end
      end

      if(low_pulse_i) begin
        // decrement the bit counter
        bit_cnt_d = bit_cnt_q - 1; 
      end

      // The sampling is delayed to account for the registering of the input
      if(low_pulse_qq) begin
        rx_shift_reg_d = {rx_shift_reg_q[6:0], spi_miso_qq};
      end
    end
    default: begin end
  endcase
end

always_ff @(posedge clk_i) begin
  if(rst_i) begin
    state_q <= IDLE;

    spi_miso_q <= 0;
    spi_miso_qq <= 0;

    transmit_done_q <= 0;

    tx_shift_reg_q <= '0;
    rx_shift_reg_q <= '0;
    bit_cnt_q <= '0;
  end else begin
    state_q <= state_d;

    spi_miso_q <= spi_miso_i;
    spi_miso_qq <= spi_miso_q;

    low_pulse_q <= low_pulse_i;
    low_pulse_qq <= low_pulse_q;

    transmit_done_q <= transmit_done_d;

    tx_shift_reg_q <= tx_shift_reg_d;
    rx_shift_reg_q <= rx_shift_reg_d;
    bit_cnt_q <= bit_cnt_d;
  end
end

assign transmit_done_o = transmit_done_q;

assign spi_clk_o = (state_q == RUNNING) ? prescaled_clk_i : 1'b0;
assign spi_cs_o = cs_i;
assign spi_mosi_o = (state_q == RUNNING) ? tx_shift_reg_q[7] : 1'b0;
assign received_data_o = rx_shift_reg_q;

endmodule // spi_frontend
