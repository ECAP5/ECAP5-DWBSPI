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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include <svdpi.h>

#include "Vtb_ecap5_dwbspi.h"
#include "Vtb_ecap5_dwbspi_tb_ecap5_dwbspi.h"
#include "Vtb_ecap5_dwbspi_ecap5_dwbspi.h"
#include "testbench.h"

enum CondId {
  COND_mem,
  COND_rx,
  COND_tx,
  COND_registers,
  __CondIdEnd
};

enum TestcaseId {
  T_IDLE        =  1,
  T_WRITE_CR    =  2,
  T_WRITE_TXDR  =  3,
  T_READ_RXDR   =  4,
  T_PRESCALER   =  5,
  T_SEND_NO_CS  =  6
};

enum StateId {
};

class TB_Ecap5_dwbspi : public Testbench<Vtb_ecap5_dwbspi> {
public:
  void reset() {
    this->_nop();

    this->core->rst_i = 1;
    for(int i = 0; i < 5; i++) {
      this->tick();
    }
    this->core->rst_i = 0;

    Testbench<Vtb_ecap5_dwbspi>::reset();
  }
  
  void _nop() {
    this->core->wb_adr_i = 0;
    this->core->wb_dat_i = 0;
    this->core->wb_we_i = 0;
    this->core->wb_sel_i = 0;
    this->core->wb_stb_i = 0;
    this->core->wb_cyc_i = 0;
  }

  void read(uint32_t addr) {
    this->core->wb_adr_i = addr;
    this->core->wb_dat_i = 0;
    this->core->wb_we_i = 0;
    this->core->wb_sel_i = 0xF;
    this->core->wb_stb_i = 1;
    this->core->wb_cyc_i = 1;
  }

  void write(uint32_t addr, uint32_t data) {
    this->core->wb_adr_i = addr;
    this->core->wb_dat_i = data;
    this->core->wb_we_i = 1;
    this->core->wb_sel_i = 0xF;
    this->core->wb_stb_i = 1;
    this->core->wb_cyc_i = 1;
  }

  uint32_t spi_cr() {
    uint32_t reg = 0;
    reg |= core->tb_ecap5_dwbspi->dut->cr_prescaler_q << 16;
    reg |= core->tb_ecap5_dwbspi->dut->cr_cs_q;
    return reg;
  }

};

void tb_ecap5_dwbspi_idle(TB_Ecap5_dwbspi * tb) {
  Vtb_ecap5_dwbspi * core = tb->core;
  core->testcase = T_IDLE;

  tb->reset();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_tx,        (core->spi_mosi_o == 0) &&
                            (core->spi_cs_o == 0));

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_ecap5_dwbuart.idle.01",
      tb->conditions[COND_tx],
      "Failed to implement the tx interface", tb->err_cycles[COND_tx]);
}

void tb_ecap5_dwbspi_write_cr(TB_Ecap5_dwbspi * tb) {
  Vtb_ecap5_dwbspi * core = tb->core;
  core->testcase = T_WRITE_CR;

  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  // Enable CS
  tb->write(0x4, 1);

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_mem,       (core->wb_ack_o == 1));
  tb->check(COND_registers, (tb->spi_cr() == 0x1));

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();
  core->wb_cyc_i = 1;
  
  //=================================
  //      Tick (2)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_tx,  (core->spi_cs_o == 1) &&
                      (core->spi_clk_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->wb_cyc_i = 0;

  //=================================
  //      Tick (3)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_tx,  (core->spi_cs_o == 1) &&
                      (core->spi_clk_o == 0));

  //=================================
  //      Tick (4)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_tx,  (core->spi_cs_o == 1) &&
                      (core->spi_clk_o == 0));

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_ecap5_dwbuart.write_cr.01",
      tb->conditions[COND_mem],
      "Failed to implement the mem interface", tb->err_cycles[COND_mem]);

  CHECK("tb_ecap5_dwbuart.write_cr.02",
      tb->conditions[COND_registers],
      "Failed to implement the memory-mapped registers", tb->err_cycles[COND_registers]);

  CHECK("tb_ecap5_dwbuart.write_cr.03",
      tb->conditions[COND_tx],
      "Failed to implement the transmit interface", tb->err_cycles[COND_tx]);
}

void tb_ecap5_dwbspi_write_txdr(TB_Ecap5_dwbspi * tb) {
  Vtb_ecap5_dwbspi * core = tb->core;
  core->testcase = T_WRITE_TXDR;

  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  // Enable CS + Prescaler = 1
  tb->write(0x4, (1 << 16) | 1);

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();
  core->wb_cyc_i = 1;

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_mem,       (core->wb_ack_o == 1));
  tb->check(COND_registers, (tb->spi_cr() == ((1 << 16) | 1)));
  
  //=================================
  //      Tick (2)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_tx,  (core->spi_cs_o == 1) &&
                      (core->spi_clk_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->wb_cyc_i = 0;

  //=================================
  //      Tick (3)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  uint8_t data = 0xA5;
  tb->write(0xC, data);

  //=================================
  //      Tick (4)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();
  core->wb_cyc_i = 1;

  //=================================
  //      Tick (5)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->wb_cyc_i = 0;

  //=================================
  //      Tick (6)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_registers,  (core->tb_ecap5_dwbspi->dut->sr_txe_q == 0));

  //=================================
  //      Tick (7-22)
  
  // check send
  for(int i = 0; i < 8; i++) {
    tb->tick();

    uint8_t expected_bit = ((data << i) >> 7) & 1;
    tb->check(COND_tx,  (core->spi_clk_o == 1) &&
                        (core->spi_mosi_o == expected_bit));

    tb->tick();

    tb->check(COND_tx,  (core->spi_clk_o == 0) &&
                        (core->spi_mosi_o == expected_bit));
  }

  //=================================
  //      Tick (23)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_tx,  (core->tb_ecap5_dwbspi->dut->transmit_done == 1));

  //=================================
  //      Tick (24)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs

  tb->read(0x0);

  //=================================
  //      Tick (25)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_mem,       (core->wb_ack_o == 1) &&
                            (core->wb_dat_o == 1));

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();
  core->wb_cyc_i = 1;

  //=================================
  //      Tick (26)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->wb_cyc_i = 0;

  //=================================
  //      Tick (27)
  
  tb->tick();

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_ecap5_dwbuart.write_txdr.01",
      tb->conditions[COND_mem],
      "Failed to implement the mem interface", tb->err_cycles[COND_mem]);

  CHECK("tb_ecap5_dwbuart.write_txdr.02",
      tb->conditions[COND_registers],
      "Failed to implement the memory-mapped registers", tb->err_cycles[COND_registers]);

  CHECK("tb_ecap5_dwbuart.write_txdr.03",
      tb->conditions[COND_tx],
      "Failed to implement the transmit interface", tb->err_cycles[COND_tx]);
}

void tb_ecap5_dwbspi_read_rxdr(TB_Ecap5_dwbspi * tb) {
  Vtb_ecap5_dwbspi * core = tb->core;
  core->testcase = T_READ_RXDR;

  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  // Enable CS + Prescaler = 1
  tb->write(0x4, (1 << 16) | 1);

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();
  core->wb_cyc_i = 1;

  //=================================
  //      Tick (2)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->wb_cyc_i = 0;

  //=================================
  //      Tick (3)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  uint8_t data = 0xA5;
  tb->write(0xC, data);

  //=================================
  //      Tick (4)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();
  core->wb_cyc_i = 1;

  //=================================
  //      Tick (5)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->wb_cyc_i = 0;

  //=================================
  //      Tick (6-21)
  
  // check send
  uint8_t receive_data = 0x5A;
  for(int i = 0; i < 8; i++) {
    core->spi_miso_i = ((receive_data << i) >> 7) & 1;

    tb->n_tick(2);
  }

  //=================================
  //      Tick (22)
  
  tb->tick();

  //=================================
  //      Tick (23)
  
  tb->tick();

  //=================================
  //      Tick (24)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs

  tb->read(0x8);

  //=================================
  //      Tick (25)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_mem,       (core->wb_ack_o == 1) &&
                            (core->wb_dat_o == receive_data));

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();
  core->wb_cyc_i = 1;

  //=================================
  //      Tick (26)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->wb_cyc_i = 0;

  //=================================
  //      Tick (27)
  
  tb->tick();

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_ecap5_dwbuart.read_rxdr.01",
      tb->conditions[COND_mem],
      "Failed to implement the mem interface", tb->err_cycles[COND_mem]);

  CHECK("tb_ecap5_dwbuart.read_rxdr.02",
      tb->conditions[COND_registers],
      "Failed to implement the memory-mapped registers", tb->err_cycles[COND_registers]);
}

void tb_ecap5_dwbspi_prescaler(TB_Ecap5_dwbspi * tb) {
  Vtb_ecap5_dwbspi * core = tb->core;
  core->testcase = T_PRESCALER;

  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  // Enable CS + Prescaler = 2
  tb->write(0x4, (2 << 16) | 1);

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();
  core->wb_cyc_i = 1;
  
  //=================================
  //      Tick (2)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->wb_cyc_i = 0;

  //=================================
  //      Tick (3)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  uint8_t data = 0xA5;
  tb->write(0xC, data);

  //=================================
  //      Tick (4)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();
  core->wb_cyc_i = 1;

  //=================================
  //      Tick (5)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->wb_cyc_i = 0;

  //=================================
  //      Tick (6-37)
  
  // check send
  uint8_t receive_data = 0x5A;
  for(int i = 0; i < 8; i++) {
    core->spi_miso_i = ((receive_data << i) >> 7) & 1;

    tb->n_tick(2);

    uint8_t expected_bit = ((data << i) >> 7) & 1;
    tb->check(COND_tx,  (core->spi_clk_o == 1) &&
                        (core->spi_mosi_o == expected_bit));

    tb->n_tick(2);

    tb->check(COND_tx,  (core->spi_clk_o == 0) &&
                        (core->spi_mosi_o == expected_bit));
  }

  //=================================
  //      Tick (40-41)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_tx,  (core->tb_ecap5_dwbspi->dut->transmit_done == 1));

  //=================================
  //      Tick (24)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs

  tb->read(0x8);

  //=================================
  //      Tick (25)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_mem,       (core->wb_ack_o == 1) &&
                            (core->wb_dat_o == receive_data));

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();
  core->wb_cyc_i = 1;

  //=================================
  //      Tick (26)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->wb_cyc_i = 0;

  //=================================
  //      Tick (27)
  
  tb->tick();

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_ecap5_dwbuart.prescaler.01",
      tb->conditions[COND_mem],
      "Failed to implement the mem interface", tb->err_cycles[COND_mem]);

  CHECK("tb_ecap5_dwbuart.prescaler.02",
      tb->conditions[COND_registers],
      "Failed to implement the memory-mapped registers", tb->err_cycles[COND_registers]);

  CHECK("tb_ecap5_dwbuart.prescaler.03",
      tb->conditions[COND_tx],
      "Failed to implement the transmit interface", tb->err_cycles[COND_tx]);
}

void tb_ecap5_dwbspi_send_no_cs(TB_Ecap5_dwbspi * tb) {
  Vtb_ecap5_dwbspi * core = tb->core;
  core->testcase = T_SEND_NO_CS;

  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  // Disable CS + Prescaler = 2
  tb->write(0x4, (2 << 16) | 0);

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();
  core->wb_cyc_i = 1;
  
  //=================================
  //      Tick (2)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->wb_cyc_i = 0;

  //=================================
  //      Tick (3)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  uint8_t data = 0xA5;
  tb->write(0xC, data);

  //=================================
  //      Tick (4)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();
  core->wb_cyc_i = 1;

  //=================================
  //      Tick (5)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->wb_cyc_i = 0;

  //=================================
  //      Tick (6-37)
  
  // check send
  uint8_t receive_data = 0x5A;
  for(int i = 0; i < 8; i++) {
    core->spi_miso_i = ((receive_data << i) >> 7) & 1;

    tb->n_tick(2);

    tb->check(COND_tx,  (core->spi_clk_o == 0) &&
                        (core->spi_mosi_o == 0));

    tb->n_tick(2);

    tb->check(COND_tx,  (core->spi_clk_o == 0) &&
                        (core->spi_mosi_o == 0));
  }

  //=================================
  //      Tick (40-41)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_tx,  (core->tb_ecap5_dwbspi->dut->transmit_done == 0));

  //=================================
  //      Tick (24)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs

  tb->read(0x8);

  //=================================
  //      Tick (25)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_mem,       (core->wb_ack_o == 1) &&
                            (core->wb_dat_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();
  core->wb_cyc_i = 1;

  //=================================
  //      Tick (26)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->wb_cyc_i = 0;

  //=================================
  //      Tick (27)
  
  tb->tick();

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_ecap5_dwbuart.send_no_cs.01",
      tb->conditions[COND_mem],
      "Failed to implement the mem interface", tb->err_cycles[COND_mem]);

  CHECK("tb_ecap5_dwbuart.send_no_cs.02",
      tb->conditions[COND_registers],
      "Failed to implement the memory-mapped registers", tb->err_cycles[COND_registers]);

  CHECK("tb_ecap5_dwbuart.send_no_cs.03",
      tb->conditions[COND_tx],
      "Failed to implement the transmit interface", tb->err_cycles[COND_tx]);
}

int main(int argc, char ** argv, char ** env) {
  srand(time(NULL));
  Verilated::traceEverOn(true);

  bool verbose = parse_verbose(argc, argv);

  TB_Ecap5_dwbspi * tb = new TB_Ecap5_dwbspi;
  tb->open_trace("waves/ecap5_dwbspi.vcd");
  tb->open_testdata("testdata/ecap5_dwbspi.csv");
  tb->set_debug_log(verbose);
  tb->init_conditions(__CondIdEnd);

  /************************************************************/

  tb_ecap5_dwbspi_idle(tb);
  tb_ecap5_dwbspi_write_cr(tb);
  tb_ecap5_dwbspi_write_txdr(tb);
  tb_ecap5_dwbspi_read_rxdr(tb);
  tb_ecap5_dwbspi_prescaler(tb);
  tb_ecap5_dwbspi_send_no_cs(tb);

  /************************************************************/

  printf("[ECAP5_DWBSPI]: ");
  if(tb->success) {
    printf("Done\n");
  } else {
    printf("Failed\n");
  }

  delete tb;
  exit(EXIT_SUCCESS);
}
