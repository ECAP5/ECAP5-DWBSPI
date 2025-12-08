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

  input   logic        cs_i,
  input   logic        prescaler_stb_i,
  input   logic[15:0]  prescaler_i,

  output  logic        high_pulse_o,
  output  logic        low_pulse_o,
  output  logic        prescaled_clk_o
);

typedef enum {
  HIGH = 0,
  LOW = 1
} state_t;

/*****************************************/
/*           Internal signals            */
/*****************************************/

state_t state_d, state_q;

logic[15:0] prescaler_ctr_d, prescaler_ctr_q;

logic high_pulse_d, high_pulse_q;
logic low_pulse_d, low_pulse_q;

logic prescaled_clk_d, prescaled_clk_q;

/*****************************************/

always_comb begin : prescaler
  prescaler_ctr_d = prescaler_ctr_q;
  high_pulse_d = 0;
  low_pulse_d = 0;

  /* Decrement the counter if CS is high */
  if(cs_i) begin
    prescaler_ctr_d = prescaler_ctr_q - 1;
  end

  /* Load the prescaler if :
   *   - the prescaler value shall be loaded
   *   - the prescaler counter has elapsed
   */ 
  if(prescaler_stb_i || prescaler_ctr_q == 16'b0) begin
    prescaler_ctr_d = {1'b0, prescaler_i[15:1]};

    // The high pulse is as soon as possible
    if(state_q == HIGH) begin
      high_pulse_d = 1;
    end
  end

  // The low pulse is as late as possible
  if(prescaler_ctr_q == 16'b1) begin
    if(state_q == LOW) begin
      low_pulse_d = 1;
    end
  end
end

always_comb begin : state_machine
  state_d = state_q;
  prescaled_clk_d = prescaled_clk_q;

  case(state_q)
    HIGH: begin
      prescaled_clk_d = 1;
      if(prescaler_ctr_q == 16'b1) begin
        state_d = LOW;
      end
    end
    LOW: begin
      prescaled_clk_d = 0;
      if(prescaler_ctr_q == 16'b1) begin
        state_d = HIGH;
      end
    end
    default: begin end
  endcase
end

always_ff @(posedge clk_i) begin
  if(rst_i) begin
    state_q <= HIGH;

    prescaler_ctr_q  <=  '0;

    high_pulse_q     <=  0;
    low_pulse_q      <=  0;

    prescaled_clk_q <= 0;
  end else begin
    state_q <= state_d;

    prescaler_ctr_q  <=  prescaler_ctr_d;

    high_pulse_q     <=  high_pulse_d;
    low_pulse_q      <=  low_pulse_d;

    prescaled_clk_q <= prescaled_clk_d;
  end
end

assign high_pulse_o = high_pulse_q & cs_i;
assign low_pulse_o = low_pulse_q & cs_i;

assign prescaled_clk_o = prescaled_clk_q & cs_i;

endmodule // spi_prescaler
