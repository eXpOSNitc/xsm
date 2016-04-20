#include "registers.h"
#include "word.h"

#include <stdlib.h>
#include <string.h>

static
xsm_reg *_registers;

static
xsm_reg *_zero_register;

static
const
char *_register_names[] = {
   "R0",
   "R1",
   "R2",
   "R3",
   "R4",
   "R5",
   "R6",
   "R7",
   "R8",
   "R9",
   "R10",
   "R11",
   "R12",
   "R13",
   "R14",
   "R15",
   "R16",
   "R17",
   "R18",
   "R19",

   "P0",
   "P1",
   "P2",
   "P3",

   "BP",
   "SP",
   "IP",
   "PTBR",
   "PTLR",
   "EIP",
   "EC",
   "EPN",
   "EMA"
};

int
registers_init ()
{
   /* Set up the registers. */
   _registers = (xsm_reg *) malloc (sizeof(xsm_reg) * XSM_NUM_REG);
   
   if (!_registers)
      return XSM_FAILURE;
      
   /* The zero register. */
   _zero_register = (xsm_reg *) malloc (sizeof(xsm_reg));
   
   if (!_zero_register)
      return XSM_FAILURE;
   
   word_store_integer (_zero_register, 0);
      
   return XSM_SUCCESS;   
}

xsm_reg*
registers_get_register (const char *name)
{
   register int i;
   
   for (i = 0; i < XSM_NUM_REG; ++i)
   {
      if (!strcasecmp(name, _register_names[i]))
         return &_registers[i];
   }
   
   return NULL;
}

xsm_reg*
registers_zero_register ()
{
   return _zero_register;
}

void
registers_destroy()
{
   free (_registers);
}

const
char **
registers_names ()
{
   return _register_names; 
}

int
registers_len()
{
   return XSM_NUM_REG;
}

int
registers_get_integer (const char *name)
{
   xsm_word *reg;

   reg = registers_get_register(name);
   return word_get_integer (reg);
}

char*
registers_get_string (const char *name)
{
   xsm_word *reg;

   reg = registers_get_register(name);

   if (!reg)
      return NULL;
   
   return word_get_string(reg);
}

int
registers_store_integer (const char *name, int val)
{
   xsm_word *reg;

   reg = registers_get_register(name);
   return word_store_integer(reg, val);
}

int
registers_store_string(const char *name, char *str)
{
   xsm_word *reg;

   reg = registers_get_register(name);
   return word_store_string(reg, str);
}