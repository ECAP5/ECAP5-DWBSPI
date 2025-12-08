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

#include "Vtb_spi_prescaler.h"
#include "testbench.h"

enum CondId {
  __CondIdEnd
};

enum TestcaseId {
  T_RESET = 0,
  T_CHIP_SELECT = 1,
  T_PULSE = 1
};

enum StateId {
};

class TB_Spi_prescaler : public Testbench<Vtb_spi_prescaler> {
public:
  void reset() {
    this->_nop();

    this->core->rst_i = 1;
    for(int i = 0; i < 5; i++) {
      this->tick();
    }
    this->core->rst_i = 0;

    Testbench<Vtb_spi_prescaler>::reset();
  }

  void _nop() {
    this->core->cs_i = 0;
    this->core->prescaler_stb_i = 0;
    this->core->prescaler_i = 0;
  }
};

void tb_spi_prescaler_reset(TB_Spi_prescaler * tb) {
  Vtb_spi_prescaler * core = tb->core;
  core->testcase = T_RESET;

  tb->reset();

  tb->n_tick(10);
}

void tb_spi_prescaler_chip_select(TB_Spi_prescaler * tb) {
  Vtb_spi_prescaler * core = tb->core;
  core->testcase = T_CHIP_SELECT;

  tb->reset();

  core->cs_i = 1;
  core->prescaler_stb_i = 1;
  core->prescaler_i = 10;

  tb->tick();

  core->prescaler_stb_i = 0;

  tb->n_tick(120);
}

void tb_spi_prescaler_pulse(TB_Spi_prescaler * tb) {
  Vtb_spi_prescaler * core = tb->core;
  core->testcase = T_PULSE;

  tb->reset();
}


int main(int argc, char ** argv, char ** env) {
  srand(time(NULL));
  Verilated::traceEverOn(true);

  bool verbose = parse_verbose(argc, argv);

  TB_Spi_prescaler * tb = new TB_Spi_prescaler;
  tb->open_trace("waves/spi_prescaler.vcd");
  tb->open_testdata("testdata/spi_prescaler.csv");
  tb->set_debug_log(verbose);
  tb->init_conditions(__CondIdEnd);

  /************************************************************/

  tb_spi_prescaler_reset(tb);
  tb_spi_prescaler_chip_select(tb);
  tb_spi_prescaler_pulse(tb);

  /************************************************************/

  printf("[spi_prescaler]: ");
  if(tb->success) {
    printf("Done\n");
  } else {
    printf("Failed\n");
  }

  delete tb;
  exit(EXIT_SUCCESS);
}
