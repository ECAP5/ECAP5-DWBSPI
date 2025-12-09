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
  input   logic  transmit_data_i,
  output  logic  received_data_o,
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

logic spi_miso_q, spi_miso_qq;

logic transmit_ongoing_d, transmit_ongoing_q;
logic transmit_done_d, transmit_done_q;

logic[7:0] shift_reg_d, shift_reg_q;
logic[7:0] received_data_d, received_data_q;
logic[2:0] bit_cnt_d, bit_cnt_q;

/*****************************************/

always_comb begin
  transmit_ongoing_d = transmit_ongoing_q;
  shift_reg_d = shift_reg_q;
  bit_cnt_d = bit_cnt_q;
  transmit_done_d = 0;
  received_data_d = received_data_q;

  if(transmit_ongoing_q == 0) begin
    if(transmit_i) begin
      transmit_ongoing_d = 1;
      bit_cnt_d = 7;
    end
  end else begin
    if(high_pulse_i) begin
      shift_reg_d = {shift_reg_q[6:0], spi_miso_qq}; 
    end 

    if(low_pulse_i) begin
      // increment the bit counter
      bit_cnt_d = bit_cnt_q - 1; 
      if(bit_cnt_q == 0) begin
        transmit_ongoing_d = 0;
        transmit_done_d = 1;
        received_data_d = {shift_reg_q[6:0], spi_miso_qq};
      end
    end
  end
end

always_ff @(posedge clk_i) begin
  if(rst_i) begin
    spi_miso_q <= 0;
    spi_miso_qq <= 0;
    transmit_ongoing_q <= 0;
    shift_reg_q <= '0;
    bit_cnt_q <= '0;
    transmit_done_q <= 0;
    received_data_q <= '0;
  end else begin
    spi_miso_q <= spi_miso_i;
    spi_miso_qq <= spi_miso_q;

    transmit_ongoing_q <= transmit_ongoing_d;
    shift_reg_q <= shift_reg_d;
    bit_cnt_q <= bit_cnt_d;
    transmit_done_q <= transmit_done_d;
    received_data_q <= received_data_d;
  end
end

assign spi_clk_o = prescaled_clk_i;
assign spi_cs_o = cs_i;
assign spi_mosi_o = (transmit_ongoing_q) ? shift_reg_q[7] : 1'b0;

endmodule // spi_frontend
