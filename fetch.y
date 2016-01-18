%{
#include <stdlib.h>

#include "machine.h"
%}

%union{
   char *str;
   xsm_instruction instr;
   xsm_operand opr;
}

%type <instr> instruction opcode
%type <opr> operand register string number dref_location special_reg gen_reg

%token TOKEN_NUMBER TOKEN_STRING TOKEN_DREF_L TOKEN_DREF_R TOKEN_SP TOKEN_BP
%token TOKEN_IP TOKEN_PTBR TOKEN_PTLR TOKEN_EIP TOKEN_EC TOKEN_EPN TOKEN_EMA
%token TOKEN_PORT TOKEN_REGISTER TOKEN_INSTRUCTION TOKEN_COMMA
%type <str> TOKEN_INSTRUCTION TOKEN_REGISTER TOKEN_PORT TOKEN_STRING TOKEN_NUMBER

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

register:
   gen_reg
   |
   special_reg
;

gen_reg:
   TOKEN_REGISTER
   |
   SP
   |
   BP
;

special_reg:
   TOKEN_PORT {$$.}
   |
   TOKEN_PTBR
   |
   TOKEN_PTLR
   |
   TOKEN_EIP
   |
   TOKEN_EC
   |
   TOKEN_EPN
   |
   TOKEN_EMA
;

dref_location:
   TOKEN_DREF_L register TOKEN_DREF_R {$$.reg_or_mem = register_from_name ($2);}
   |
   TOKEN_DREF_L number TOKEN_DREF_R{$$.reg_or_mem = memory_get_word(atoi(number));}
   |
   TOKEN_DREF_L number TOKEN_DREF_R number
   |
   TOKEN_DREF_L number TOKEN_DREF_R register 
;

opcode:
   TOKEN_INSTRUCTION {$$.instr.opcode = machine_get_opcode ($1); }
;
