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

module tb_spi_frontend
(
  input   int          testcase,

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

spi_frontend #(
) dut (
  .clk_i           (clk_i),
  .rst_i           (rst_i),

  .cs_i (cs_i),
  .prescaled_clk_i (prescaled_clk_i),
  .high_pulse_i (high_pulse_i),
  .low_pulse_i (low_pulse_i),

  .transmit_i (transmit_i),
  .transmit_data_i (transmit_data_i),
  .received_data_o (received_data_o),
  .transmit_done_o (transmit_done_o),

  .spi_cs_o (spi_cs_o),
  .spi_clk_o (spi_clk_o),
  .spi_mosi_o (spi_mosi_o),
  .spi_miso_i (spi_miso_i)
);

endmodule // tb_spi_frontend

