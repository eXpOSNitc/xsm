#ifndef XSM_MACHINE_H

#define XSM_MACHINE_H

#include "types.h"

typedef 
struct _xsm_cpu
{
   xsm_reg *regs;
   xsm_timer_t timer;
   xsm_mode_t mode;
}
xsm_cpu;

typedef
struct _xsm_mem
{
   xsm_page *pages;
   int num_pages;
}
xsm_mem;

typedef
struct _xsm_options
{
   int timer;
   int debug;
   int disk;
   int console;
}
xsm_options;

typedef
struct _xsm_instruction
{
   int op_opcode;
   xsm_reg *reg_dest;
   xsm_reg *reg_src;
   xsm_word info;
}
xsm_instruction;

#endif
