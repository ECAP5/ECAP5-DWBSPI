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

module tb_spi_prescaler
(
  input   int          testcase,

  input   logic         clk_i,
  input   logic         rst_i,

  input   logic        prescaler_stb_i,
  input   logic[15:0]  prescaler_i,

  output  logic        high_pulse_o,
  output  logic        low_pulse_o,
  output  logic        prescaled_clk_o
);

spi_prescaler #(
) dut (
  .clk_i           (clk_i),
  .rst_i           (rst_i),

  .prescaler_stb_i (prescaler_stb_i),
  .prescaler_i (prescaler_i),
  .high_pulse_o (high_pulse_o),
  .low_pulse_o (low_pulse_o),
  .prescaled_clk_o (prescaled_clk_o)
);

endmodule // tb_spi_prescaler

