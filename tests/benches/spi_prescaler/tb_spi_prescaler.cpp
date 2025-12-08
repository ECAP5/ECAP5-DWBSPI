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
  COND_pulse,
  COND_clock,
  __CondIdEnd
};

enum TestcaseId {
  T_RESET = 0,
  T_PRESCALER4 = 1,
  T_PRESCALER10 = 2
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
    this->core->prescaler_stb_i = 0;
    this->core->prescaler_i = 0;
  }
};

void tb_spi_prescaler_reset(TB_Spi_prescaler * tb) {
  Vtb_spi_prescaler * core = tb->core;
  core->testcase = T_RESET;

  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  core->rst_i = 1;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_pulse, (core->high_pulse_o == 0) &&
                        (core->low_pulse_o == 0));

  tb->check(COND_clock, (core->prescaled_clk_o == 0));

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_spi_prescaler.reset.01",
      tb->conditions[COND_pulse],
      "Failed to implement the pulse generation", tb->err_cycles[COND_pulse]);

  CHECK("tb_spi_prescaler.reset.02",
      tb->conditions[COND_clock],
      "Failed to implement the clock generation", tb->err_cycles[COND_clock]);
}

void tb_spi_prescaler_prescaler4(TB_Spi_prescaler * tb) {
  Vtb_spi_prescaler * core = tb->core;
  core->testcase = T_PRESCALER4;

  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  core->prescaler_stb_i = 1;
  core->prescaler_i = 4;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->prescaler_stb_i = 0;

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_pulse, (core->high_pulse_o == 1) &&
                        (core->low_pulse_o == 0));
  tb->check(COND_clock, (core->prescaled_clk_o == 0));

  //=================================
  //      Tick (2-4)
  
  for(size_t i = 0; i < 3; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_pulse, (core->high_pulse_o == 0) &&
                          (core->low_pulse_o == 0));
    tb->check(COND_clock, (core->prescaled_clk_o == 1));
  }

  //=================================
  //      Tick (5)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_pulse, (core->high_pulse_o == 0) &&
                        (core->low_pulse_o == 1));
  tb->check(COND_clock, (core->prescaled_clk_o == 1));

  //=================================
  //      Tick (6-8)
  
  for(size_t i = 0; i < 3; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_pulse, (core->high_pulse_o == 0) &&
                          (core->low_pulse_o == 0));
    tb->check(COND_clock, (core->prescaled_clk_o == 0));
  }

  //=================================
  //      Tick (9)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_pulse, (core->high_pulse_o == 1) &&
                        (core->low_pulse_o == 0));
  tb->check(COND_clock, (core->prescaled_clk_o == 0));

  //=================================
  //      Tick (10)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_pulse, (core->high_pulse_o == 0) &&
                        (core->low_pulse_o == 0));
  tb->check(COND_clock, (core->prescaled_clk_o == 1));

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_spi_prescaler.prescaler4.01",
      tb->conditions[COND_pulse],
      "Failed to implement the pulse generation", tb->err_cycles[COND_pulse]);

  CHECK("tb_spi_prescaler.prescaler4.02",
      tb->conditions[COND_clock],
      "Failed to implement the clock generation", tb->err_cycles[COND_clock]);
}

void tb_spi_prescaler_prescaler10(TB_Spi_prescaler * tb) {
  Vtb_spi_prescaler * core = tb->core;
  core->testcase = T_PRESCALER10;

  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  core->prescaler_stb_i = 1;
  core->prescaler_i = 10;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->prescaler_stb_i = 0;

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_pulse, (core->high_pulse_o == 1) &&
                        (core->low_pulse_o == 0));
  tb->check(COND_clock, (core->prescaled_clk_o == 0));

  //=================================
  //      Tick (2-10)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_pulse, (core->high_pulse_o == 0) &&
                          (core->low_pulse_o == 0));
    tb->check(COND_clock, (core->prescaled_clk_o == 1));
  }

  //=================================
  //      Tick (11)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_pulse, (core->high_pulse_o == 0) &&
                        (core->low_pulse_o == 1));
  tb->check(COND_clock, (core->prescaled_clk_o == 1));

  //=================================
  //      Tick (12-20)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_pulse, (core->high_pulse_o == 0) &&
                          (core->low_pulse_o == 0));
    tb->check(COND_clock, (core->prescaled_clk_o == 0));
  }

  //=================================
  //      Tick (21)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_pulse, (core->high_pulse_o == 1) &&
                        (core->low_pulse_o == 0));
  tb->check(COND_clock, (core->prescaled_clk_o == 0));

  //=================================
  //      Tick (22)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_pulse, (core->high_pulse_o == 0) &&
                        (core->low_pulse_o == 0));
  tb->check(COND_clock, (core->prescaled_clk_o == 1));

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_spi_prescaler.prescaler10.01",
      tb->conditions[COND_pulse],
      "Failed to implement the pulse generation", tb->err_cycles[COND_pulse]);

  CHECK("tb_spi_prescaler.prescaler10.02",
      tb->conditions[COND_clock],
      "Failed to implement the clock generation", tb->err_cycles[COND_clock]);
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
  tb_spi_prescaler_prescaler4(tb);
  tb_spi_prescaler_prescaler10(tb);

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
