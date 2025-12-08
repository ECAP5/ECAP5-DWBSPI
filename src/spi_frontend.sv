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
  input   logic         clk_i,
  input   logic         rst_i,

  //=================================
  //    Internal interface
  
  input logic cs_i,
  
  input   logic        transmit_i,
  input  logic        high_pulse_i,
  input  logic        low_pulse_i,

  //=================================
  //    SPI interface

  output logic spi_cs_o,
  output logic spi_clk_o,
  output logic spi_mosi_o,
  input  logic spi_miso_i
);

typedef enum {
  IDLE,    // 0
  TX_ONLY, // 1
  TX,      // 2
  RX,      // 3
  RX_ONLY  // 4
} state_t;

/*****************************************/
/*           Internal signals            */
/*****************************************/

state_t state_d, state_q;

logic spi_miso_q, spi_miso_qq;

logic[9:0] bit_cnt_d, bit_cnt_q;
logic[7:0] tx_shift_reg_d, tx_shift_reg_q;
logic[7:0] rx_shift_reg_d, rx_shift_reg_q;

logic spi_clk_d, spi_clk_q;

/*****************************************/

always_comb begin : state_machine
  state_d = state_q;

  case(state_q)
    IDLE: begin
      if(transmit_i) begin
        state_d = TX_ONLY;
      end
    end
    TX_ONLY: begin
      if(high_pulse_i) begin
        // Transmit the first two bits while the input metastability is dealt with
        if(bit_cnt_q[7] == 1) begin
          state_d = RX;
        end
      end
    end
    TX: begin
      if(high_pulse_i) begin
        state_d = RX;
      end
    end
    RX: begin
      if(low_pulse_i) begin
        if(bit_cnt_q[2] == 1) begin
          state_d = RX_ONLY;
        end else begin
          state_d = TX;
        end
      end
    end
    RX_ONLY: begin
      if(low_pulse_i) begin
        // Receive the last two bits
        if(bit_cnt_q[0] == 1) begin
          state_d = IDLE;
        end
      end
    end
  endcase
end

always_comb begin : bit_cnt
  bit_cnt_d = bit_cnt_q;

  // Initialize the bit counter
  if(state_q == IDLE && transmit_i) begin
    bit_cnt_d = {1'b1, 9'b0};;
  end

  // Shift the bit counter
  if(state_q != IDLE) begin
    bit_cnt_d = {1'b0, bit_cnt_q[9:1]};
  end
end

always_comb begin : clk_generation
  spi_clk_d = spi_clk_q;

  if(state_q != IDLE) begin
    if(high_pulse_i) begin
      spi_clk_d = 1; 
    end
    if(low_pulse_i) begin
      spi_clk_d = 0;
    end
  end 
end

always_ff @(posedge clk_i) begin
  if(rst_i) begin
    state_q <= IDLE;

    spi_clk_q <= 0;

    spi_miso_q <= 0;
    spi_miso_qq <= 0;
  end else begin
    state_q <= state_d;

    spi_clk_q <= spi_clk_d;

    spi_miso_q <= spi_miso_i;
    spi_miso_qq <= spi_miso_q;
  end
end

assign spi_cs_o = cs_i;
assign spi_clk_o = spi_clk_q;

endmodule // spi_frontend
