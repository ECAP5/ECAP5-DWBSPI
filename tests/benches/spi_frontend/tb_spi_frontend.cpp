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

#include "Vtb_spi_frontend.h"
#include "testbench.h"

enum CondId {
  COND_transmit,
  COND_receive,
  __CondIdEnd
};

enum TestcaseId {
  T_RESET = 0,
  T_TRANSMIT = 1,
  T_RECEIVE = 2,
  T_PRESCALER = 3,
  T_NO_CS = 4,
};

enum StateId {
};

class TB_Spi_frontend : public Testbench<Vtb_spi_frontend> {
public:
  void reset() {
    this->_nop();

    this->core->rst_i = 1;
    for(int i = 0; i < 5; i++) {
      this->tick();
    }
    this->core->rst_i = 0;

    Testbench<Vtb_spi_frontend>::reset();
  }

  void _nop() {
    core->cs_i = 0;
    core->prescaled_clk_i = 0;
    core->high_pulse_i = 0;
    core->low_pulse_i = 0;
    core->transmit_i = 0;
    core->transmit_data_i = 0;
    core->spi_miso_i = 0;
  }
};

void tb_spi_frontend_reset(TB_Spi_frontend * tb) {
  Vtb_spi_frontend * core = tb->core;
  core->testcase = T_RESET;

  tb->reset();

  tb->n_tick(10);

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_spi_frontend.reset.01",
      tb->conditions[COND_transmit],
      "Failed to implement the frontend transmit", tb->err_cycles[COND_transmit]);
}

void tb_spi_frontend_transmit(TB_Spi_frontend * tb) {
  Vtb_spi_frontend * core = tb->core;
  core->testcase = T_TRANSMIT;

  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  core->cs_i = 1;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  uint8_t data = 0xA5;
  core->transmit_i = 1;
  core->transmit_data_i = data;

  //=================================
  //      Tick (2)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->transmit_i = 0;
  core->high_pulse_i = 1;

  //=================================
  //      Tick (3)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_transmit, (core->spi_mosi_o == ((data >> 7) & 1)));

  //=================================
  //      Tick (4-6)
  
  core->prescaled_clk_i = 1;
  core->high_pulse_i = 0;
  tb->tick();
  core->low_pulse_i = 1;
  tb->tick();
  core->prescaled_clk_i = 0;
  core->low_pulse_i = 0;
  tb->tick();

  //=================================
  //      Tick (7-34)
  
  for(int i = 0; i < 7; i++) {
    core->high_pulse_i = 1;

    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_transmit, (core->spi_mosi_o == ((data >> (6-i)) & 1)));

    core->prescaled_clk_i = 1;
    core->high_pulse_i = 0;
    tb->tick();
    core->low_pulse_i = 1;
    tb->tick();
    core->prescaled_clk_i = 0;
    core->low_pulse_i = 0;
    tb->tick();
  }

  core->high_pulse_i = 1;

  //=================================
  //      Tick (35)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_transmit, (core->transmit_done_o == 1));

  //`````````````````````````````````
  //      Set inputs
  
  core->prescaled_clk_i = 1;
  core->high_pulse_i = 0;

  //=================================
  //      Tick (36)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_transmit, (core->transmit_done_o == 0));

  //=================================
  //      Tick (37-38)
  
  core->low_pulse_i = 1;
  tb->tick();
  core->prescaled_clk_i = 0;
  core->low_pulse_i = 0;
  tb->tick();

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_spi_frontend.transmit.01",
      tb->conditions[COND_transmit],
      "Failed to implement the frontend transmit", tb->err_cycles[COND_transmit]);
}

void tb_spi_frontend_receive(TB_Spi_frontend * tb) {
  Vtb_spi_frontend * core = tb->core;
  core->testcase = T_RECEIVE;

  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  core->cs_i = 1;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  uint8_t data = 0xFF;
  core->transmit_i = 1;
  core->transmit_data_i = data;

  //=================================
  //      Tick (2)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->transmit_i = 0;
  core->high_pulse_i = 1;

  //=================================
  //      Tick (3)
  
  tb->tick();

  //=================================
  //      Tick (4-6)
  
  core->prescaled_clk_i = 1;
  core->high_pulse_i = 0;

  uint8_t send_data = 0xA5;
  core->spi_miso_i = (send_data >> 7) & 1;

  tb->tick();
  core->low_pulse_i = 1;
  tb->tick();
  core->prescaled_clk_i = 0;
  core->low_pulse_i = 0;
  tb->tick();

  //=================================
  //      Tick (7-34)
  
  for(int i = 0; i < 7; i++) {
    core->high_pulse_i = 1;

    tb->tick();

    core->prescaled_clk_i = 1;
    core->high_pulse_i = 0;

    core->spi_miso_i = (send_data >> (6-i)) & 1;

    tb->tick();
    core->low_pulse_i = 1;
    tb->tick();
    core->prescaled_clk_i = 0;
    core->low_pulse_i = 0;
    tb->tick();
  }

  core->high_pulse_i = 1;

  //=================================
  //      Tick (35)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_receive, (core->transmit_done_o == 1) &&
                          (core->received_data_o == send_data));

  //`````````````````````````````````
  //      Set inputs
  
  core->prescaled_clk_i = 1;
  core->high_pulse_i = 0;

  core->spi_miso_i = 0;

  //=================================
  //      Tick (36)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_receive, (core->transmit_done_o == 0));

  //=================================
  //      Tick (37-38)
  
  core->low_pulse_i = 1;
  tb->tick();
  core->prescaled_clk_i = 0;
  core->low_pulse_i = 0;
  tb->tick();

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_spi_frontend.receive.01",
      tb->conditions[COND_receive],
      "Failed to implement the frontend receive", tb->err_cycles[COND_receive]);
}

void tb_spi_frontend_prescaler(TB_Spi_frontend * tb) {
  Vtb_spi_frontend * core = tb->core;
  core->testcase = T_PRESCALER;

  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  core->cs_i = 1;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  uint8_t data = 0xA5;
  core->transmit_i = 1;
  core->transmit_data_i = data;

  //=================================
  //      Tick (2)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->transmit_i = 0;
  core->high_pulse_i = 1;

  //=================================
  //      Tick (3)
  
  tb->tick();

  tb->check(COND_transmit, (core->spi_mosi_o == ((data >> 7) & 1)));

  //=================================
  //      Tick (4-6)
  
  core->prescaled_clk_i = 1;
  core->high_pulse_i = 0;

  uint8_t send_data = 0x5A;
  core->spi_miso_i = (send_data >> 7) & 1;

  tb->n_tick(3);
  core->low_pulse_i = 1;
  tb->tick();
  core->prescaled_clk_i = 0;
  core->low_pulse_i = 0;
  tb->n_tick(3);

  //=================================
  //      Tick (7-34)
  
  for(int i = 0; i < 7; i++) {
    core->high_pulse_i = 1;

    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_transmit, (core->spi_mosi_o == ((data >> (6-i)) & 1)));

    core->prescaled_clk_i = 1;
    core->high_pulse_i = 0;

    core->spi_miso_i = (send_data >> (6-i)) & 1;

    tb->n_tick(3);
    core->low_pulse_i = 1;
    tb->tick();
    core->prescaled_clk_i = 0;
    core->low_pulse_i = 0;
    tb->n_tick(3);
  }

  core->high_pulse_i = 1;

  //=================================
  //      Tick (35)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_transmit, (core->transmit_done_o == 1) &&
                           (core->received_data_o == send_data));

  //`````````````````````````````````
  //      Set inputs
  
  core->prescaled_clk_i = 1;
  core->high_pulse_i = 0;

  core->spi_miso_i = 0;

  //=================================
  //      Tick (36)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_transmit, (core->transmit_done_o == 0));

  //=================================
  //      Tick (37-38)
  
  core->low_pulse_i = 1;
  tb->tick();
  core->prescaled_clk_i = 0;
  core->low_pulse_i = 0;
  tb->tick();

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_spi_frontend.prescaler.01",
      tb->conditions[COND_transmit],
      "Failed to implement the frontend transmit", tb->err_cycles[COND_transmit]);

  CHECK("tb_spi_frontend.prescaler.02",
      tb->conditions[COND_receive],
      "Failed to implement the frontend receive", tb->err_cycles[COND_receive]);
}

void tb_spi_frontend_no_cs(TB_Spi_frontend * tb) {
  Vtb_spi_frontend * core = tb->core;
  core->testcase = T_NO_CS;

  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  core->cs_i = 0;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  uint8_t data = 0xA5;
  core->transmit_i = 1;
  core->transmit_data_i = data;

  //=================================
  //      Tick (2)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->transmit_i = 0;
  core->high_pulse_i = 1;

  //=================================
  //      Tick (3)
  
  tb->tick();

  tb->check(COND_transmit, (core->spi_mosi_o == 0));

  //=================================
  //      Tick (4-6)
  
  core->prescaled_clk_i = 1;
  core->high_pulse_i = 0;

  uint8_t send_data = 0x5A;
  core->spi_miso_i = (send_data >> 7) & 1;

  tb->tick();
  core->low_pulse_i = 1;
  tb->tick();
  core->prescaled_clk_i = 0;
  core->low_pulse_i = 0;
  tb->tick();

  //=================================
  //      Tick (7-34)
  
  for(int i = 0; i < 7; i++) {
    core->high_pulse_i = 1;

    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_transmit, (core->spi_mosi_o == 0));

    core->prescaled_clk_i = 1;
    core->high_pulse_i = 0;

    core->spi_miso_i = (send_data >> (6-i)) & 1;

    tb->tick();
    core->low_pulse_i = 1;
    tb->tick();
    core->prescaled_clk_i = 0;
    core->low_pulse_i = 0;
    tb->tick();
  }

  core->high_pulse_i = 1;

  //=================================
  //      Tick (35)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_transmit, (core->transmit_done_o == 0) &&
                           (core->received_data_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->prescaled_clk_i = 1;
  core->high_pulse_i = 0;

  core->spi_miso_i = 0;

  //=================================
  //      Tick (36)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_transmit, (core->transmit_done_o == 0));

  //=================================
  //      Tick (37-38)
  
  core->low_pulse_i = 1;
  tb->tick();
  core->prescaled_clk_i = 0;
  core->low_pulse_i = 0;
  tb->tick();

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_spi_frontend.no_cs.01",
      tb->conditions[COND_transmit],
      "Failed to implement the frontend transmit", tb->err_cycles[COND_transmit]);

  CHECK("tb_spi_frontend.no_cs.02",
      tb->conditions[COND_receive],
      "Failed to implement the frontend receive", tb->err_cycles[COND_receive]);
}

int main(int argc, char ** argv, char ** env) {
  srand(time(NULL));
  Verilated::traceEverOn(true);

  bool verbose = parse_verbose(argc, argv);
  verbose = 1;

  TB_Spi_frontend * tb = new TB_Spi_frontend;
  tb->open_trace("waves/spi_frontend.vcd");
  tb->open_testdata("testdata/spi_frontend.csv");
  tb->set_debug_log(verbose);
  tb->init_conditions(__CondIdEnd);

  /************************************************************/

  tb_spi_frontend_reset(tb);
  tb_spi_frontend_transmit(tb);
  tb_spi_frontend_receive(tb);
  tb_spi_frontend_prescaler(tb);
  tb_spi_frontend_no_cs(tb);

  /************************************************************/

  printf("[spi_frontend]: ");
  if(tb->success) {
    printf("Done\n");
  } else {
    printf("Failed\n");
  }

  delete tb;
  exit(EXIT_SUCCESS);
}
