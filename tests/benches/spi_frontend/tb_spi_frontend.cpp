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
  __CondIdEnd
};

enum TestcaseId {
  T_RESET = 0,
  T_TRANSMIT = 1
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
}

void tb_spi_frontend_transmit(TB_Spi_frontend * tb) {
  Vtb_spi_frontend * core = tb->core;
  core->testcase = T_TRANSMIT;

  tb->reset();

  core->cs_i = 1;

  core->transmit_i = 1;
  core->transmit_data_i = 0xA5;

  tb->tick();

  core->transmit_i = 0;

  tb->n_tick(10);
}

int main(int argc, char ** argv, char ** env) {
  srand(time(NULL));
  Verilated::traceEverOn(true);

  bool verbose = parse_verbose(argc, argv);

  TB_Spi_frontend * tb = new TB_Spi_frontend;
  tb->open_trace("waves/spi_frontend.vcd");
  tb->open_testdata("testdata/spi_frontend.csv");
  tb->set_debug_log(verbose);
  tb->init_conditions(__CondIdEnd);

  /************************************************************/

  tb_spi_frontend_reset(tb);
  tb_spi_frontend_transmit(tb);

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
