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

module ecap5_dwbspi #(
  localparam SPI_SR = 0,
  localparam SPI_CR = 1,
  localparam SPI_RXDR = 2,
  localparam SPI_TXDR = 3
)(
  input   logic         clk_i,
  input   logic         rst_i,

  //=================================
  //    Memory interface

  input   logic[31:0]  wb_adr_i,
  output  logic[31:0]  wb_dat_o,
  input   logic[31:0]  wb_dat_i,
  input   logic        wb_we_i,
  input   logic[3:0]   wb_sel_i,
  input   logic        wb_stb_i,
  output  logic        wb_ack_o,
  input   logic        wb_cyc_i,
  output  logic        wb_stall_o,

  //=================================
  //    SPI interface

  output logic spi_cs_o,
  output logic spi_clk_o,
  output logic spi_mosi_o,
  input  logic spi_miso_i
);

/*****************************************/
/*           Internal signals            */
/*****************************************/

logic[31:0] mem_addr;
logic       mem_read, mem_write;
logic[31:0] mem_read_data_d, mem_read_data_q, 
            mem_write_data;

logic transmit_d, transmit_q;
logic mem_write_cr_d, mem_write_cr_q;

logic prescaled_clk, high_pulse, low_pulse;
logic[7:0] received_data;
logic transmit_done;

/*****************************************/
/*        Memory mapped registers        */
/*****************************************/

logic[15:0] cr_prescaler_d, cr_prescaler_q;
logic       cr_cs_d, cr_cs_q;
logic      sr_txe_d, sr_txe_q;
logic[7:0] txdr_txd_d, txdr_txd_q;
logic[7:0] rxdr_rxd_d, rxdr_rxd_q;

/*****************************************/

spi_prescaler prescaler_inst (
  .clk_i            (clk_i),
  .rst_i            (rst_i),

  .prescaler_stb_i  (mem_write_cr_q),
  .prescaler_i      (cr_prescaler_q),

  .high_pulse_o     (high_pulse),
  .low_pulse_o      (low_pulse),
  .prescaled_clk_o  (prescaled_clk)
);

spi_frontend frontend_inst (
  .clk_i            (clk_i),
  .rst_i            (rst_i),

  .cs_i             (cr_cs_q),
  .prescaled_clk_i  (prescaled_clk),
  .high_pulse_i     (high_pulse),
  .low_pulse_i      (low_pulse),
  .transmit_i       (transmit_q),
  .transmit_data_i  (txdr_txd_q),

  .received_data_o  (received_data),
  .transmit_done_o  (transmit_done),

  .spi_cs_o         (spi_cs_o),
  .spi_clk_o        (spi_clk_o),
  .spi_mosi_o       (spi_mosi_o),
  .spi_miso_i       (spi_miso_i)
);

ecap5_dwbmmsc wb_interface_inst (
  .clk_i (clk_i),   .rst_i (rst_i),
  
  .wb_adr_i (wb_adr_i),  .wb_dat_o (wb_dat_o),  .wb_dat_i   (wb_dat_i),
  .wb_we_i  (wb_we_i),   .wb_sel_i (wb_sel_i),  .wb_stb_i   (wb_stb_i),
  .wb_ack_o (wb_ack_o),  .wb_cyc_i (wb_cyc_i),  .wb_stall_o (wb_stall_o),

  .addr_o       (mem_addr),
  .read_o       (mem_read),
  .read_data_i  (mem_read_data_q),
  .write_o      (mem_write),
  .write_data_o (mem_write_data),
  .sel_o        ()
);

always_comb begin : register_access
  sr_txe_d        =  sr_txe_q;
  cr_prescaler_d  =  cr_prescaler_q;
  cr_cs_d         =  cr_cs_q;
  rxdr_rxd_d      =  rxdr_rxd_q;
  txdr_txd_d      =  txdr_txd_q;

  transmit_d = 0;
  mem_write_cr_d = 0;

  // Set the data output for read requests
  case(mem_addr[7:2])
    SPI_SR:   mem_read_data_d = {31'b0, sr_txe_q};
    SPI_CR:   mem_read_data_d = {cr_prescaler_q, 15'b0, cr_cs_q};
    SPI_RXDR: mem_read_data_d = {24'b0, rxdr_rxd_q};
    default:  mem_read_data_d = '0;
  endcase

  // Set the register data for write requests
  if(mem_write) begin
    case(mem_addr[7:2])
      SPI_CR: begin
        cr_prescaler_d  =  mem_write_data[31:16];
        cr_cs_d         =  mem_write_data[0];
        mem_write_cr_d  =  1;
      end
      SPI_TXDR: begin
        txdr_txd_d = mem_write_data[7:0];
        transmit_d = 1;
      end
    endcase
  end

  // Priority to the memory request
  if(mem_write && (mem_addr[4:2] == SPI_TXDR)) begin
    sr_txe_d = 0;
  end else if (transmit_done) begin
    sr_txe_d = 1;
  end

  // Priority to the hardware
  if(transmit_done) begin
    rxdr_rxd_d = received_data;
  // When the memory request occurs but no data was received
  // we clear the previously received data
  end else if(mem_read && mem_addr[4:2] == SPI_RXDR) begin
    rxdr_rxd_d = '0;
  end
end

always_ff @(posedge clk_i) begin
  if(rst_i) begin
    cr_prescaler_q <= '0;
    cr_cs_q <= 0;
    sr_txe_q <= 1;
    txdr_txd_q <= '0;
    rxdr_rxd_q <= '0;

    mem_read_data_q <= '0;

    transmit_q <= 0;
    mem_write_cr_q <= 0;
  end else begin
    cr_prescaler_q <= cr_prescaler_d;
    cr_cs_q <= cr_cs_d;
    sr_txe_q <= sr_txe_d;
    txdr_txd_q <= txdr_txd_d;
    rxdr_rxd_q <= rxdr_rxd_d;

    mem_read_data_q <= mem_read_data_d;

    transmit_q <= transmit_d;
    mem_write_cr_q <= mem_write_cr_d;
  end
end

endmodule // ecap5_dwbspi
