%{
#include <stdlib.h>

#include "machine.h"
%}

%union{
   char *str;
   xsm_instruction instr;
   xsm_operand opr;
   int val;
}

%type <instr> instruction opcode
%type <opr> operand register string number dref_location special_reg gen_reg

%token TOKEN_NUMBER TOKEN_STRING TOKEN_DREF_L TOKEN_DREF_R TOKEN_SP TOKEN_BP
%token TOKEN_IP TOKEN_PTBR TOKEN_PTLR TOKEN_EIP TOKEN_EC TOKEN_EPN TOKEN_EMA
%token TOKEN_PORT TOKEN_REGISTER TOKEN_INSTRUCTION TOKEN_COMMA
%type <str> TOKEN_INSTRUCTION TOKEN_REGISTER TOKEN_PORT TOKEN_STRING
%type <val> TOKEN_NUMBER

instruction_stream:
   instruction_stream instruction
   |
;

instruction:
   opcode operand TOKEN_COMMA operand
   |
   opcode operand
   |
   opcode
;

operand:
   register
   |
   string
   |
   number
   |
   dref_location
;

number:
   TOKEN_NUMBER {$$.regis
;

register:
   gen_reg {$$ = $1; $$.mode = PRIVILEGE_USER; }
   |
   special_reg {$$ = $1; $$.mode = PRIVILEGE_KERNEL;}
;

gen_reg:
   TOKEN_REGISTER {$$.reg_or_mem = registers_get_register($1); free($1);}
   |
   TOKEN_SP {$$.reg_or_mem = registers_get_register("SP");}
   |
   TOKEN_BP {$$.reg_or_mem = registers_get_register("BP");}
;

special_reg:
   TOKEN_PORT  {$$.reg_or_mem = registers_get_register($1); free($1);}
   |
   TOKEN_PTBR  {$$.reg_or_mem = registers_get_register("PTBR");}
   |
   TOKEN_PTLR  {$$.reg_or_mem = registers_get_register("PTLR");}
   |
   TOKEN_EIP   {$$.reg_or_mem = registers_get_register("EIP");}
   |
   TOKEN_EC    {$$.reg_or_mem = registers_get_register("EC");}
   |
   TOKEN_EPN   {$$.reg_or_mem = registers_get_register("EPN");}
   |
   TOKEN_EMA   {$$.reg_or_mem = registers_get_register("EMA");}
;

dref_location:
   TOKEN_DREF_L register TOKEN_DREF_R {$$.reg_or_mem = $2; } //TODO
   |
   TOKEN_DREF_L TOKEN_NUMBER TOKEN_DREF_R{$$.reg_or_mem = memory_get_word($2);}
   |
   TOKEN_DREF_L TOKEN_NUMBER TOKEN_DREF_R TOKEN_NUMBER
   |
   TOKEN_DREF_L TOKEN_NUMBER TOKEN_DREF_R register 
;

opcode:
   TOKEN_INSTRUCTION {$$.instr.opcode = machine_get_opcode ($1); }
;
