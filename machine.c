#include <stdlib.h>
#include <string.h>
#include "machine.h"

const char *instructions[]=
{
   "MOV",
   "ADD",
   "SUB",
   "MUL", 
   "DIV",
   "MOD",
   "INR",
   "DCR",
   "LT",
   "GT", 
   "EQ",
   "NE", 
   "GE", 
   "LE",
   "JZ",
   "JNZ",
   "JMP",
   "PUSH",
   "POP",
   "CALL",
   "RET",
   "BRKP",
   "INT",
   
   "LOADI",
   "LOAD",
   "STORE",
   "ENCRYPT",
   "BACKUP",
   "RESTORE",
   "PORT",
   "IN",
   "INI",
   "OUT",
   "IRET",
   "HALT"
};

int
machine_get_opcode (const char* instr)
{
   int i;
   
   for (i = 0; i < XSM_INSTRUCTION_COUNT; ++i)
   {
      if (!stricmp(instr, instructions[i])
         return i;
   }
   
   return XSM_ILLINSTR;
}

void
machine_fetch_instruction (xsm_instruction *instr)
{
   int token;
   
   token = yylex ();
   
   /* The first token must be a string, an operation, to be precise. */
   if (token != TOKEN_INSTRUCTION)
   {
      machine_raise_exception ("The XSM simulator has encountered an illegal instruction.");
   }
   
   instr->opcode = machine_get_opcode (yylval);
   free (yylval);
   
   token = yylex ();
   
   if (token == TOKEN_DREF_L)
}
