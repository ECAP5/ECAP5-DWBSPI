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

module spi_prescaler #(
)(
  input   logic        clk_i,
  input   logic        rst_i,

  input   logic        prescaler_stb_i,
  input   logic[15:0]  prescaler_i,

  output  logic        high_pulse_o,
  output  logic        low_pulse_o,
  output  logic        prescaled_clk_o
);

/*****************************************/
/*           Internal signals            */
/*****************************************/

logic[15:0] prescaler_d, prescaler_q;
logic[15:0] prescaler_ctr_d, prescaler_ctr_q;

logic high_pulse_d, high_pulse_q;
logic low_pulse_d, low_pulse_q;

logic prescaled_clk_d, prescaled_clk_q, prescaled_clk_qq;

/*****************************************/

always_comb begin : prescaler
  prescaler_d = prescaler_q;
  prescaled_clk_d = prescaled_clk_q;
  high_pulse_d = 0;
  low_pulse_d = 0;

  // if the prescaler has elapsed
  if(prescaler_ctr_q == 16'b0) begin
    // reload the prescaler counter
    prescaler_ctr_d = prescaler_q[15:0] - 1; 

    // toggle the clk
    prescaled_clk_d = ~prescaled_clk_q;

    // toggle the pulse
    if(~prescaled_clk_q) begin
      high_pulse_d = 1;
    end else begin
      low_pulse_d = 1;
    end
  end else begin
    prescaler_ctr_d = prescaler_ctr_q - 1;
  end

  // if the prescaler has been updated
  if(prescaler_stb_i) begin
    prescaler_d = prescaler_i;
    prescaler_ctr_d = prescaler_i[15:0] - 1;
  end 
end

always_ff @(posedge clk_i) begin
  if(rst_i) begin
    prescaler_q <= '0;
    prescaler_ctr_q  <=  '0;

    high_pulse_q     <=  0;
    low_pulse_q      <=  0;

    prescaled_clk_q <= 0;
    prescaled_clk_qq <= 0;
  end else begin
    prescaler_q <= prescaler_d;
    prescaler_ctr_q  <=  prescaler_ctr_d;

    high_pulse_q     <=  high_pulse_d;
    low_pulse_q      <=  low_pulse_d;

    prescaled_clk_q <= prescaled_clk_d;
    // the prescaled clock is delayed by one tick so that the 
    // pulses are detected in sync with the edges
    prescaled_clk_qq <= prescaled_clk_q;
  end
end

assign high_pulse_o = high_pulse_q;
assign low_pulse_o = low_pulse_q;

assign prescaled_clk_o = prescaled_clk_qq;

endmodule // spi_prescaler
