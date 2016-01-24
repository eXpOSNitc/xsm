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
machine_init ()
{
   /* TODO :Add initialization code here. */
   
}

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

int
machine_run ()
{
   int token, opcode;

   while (TRUE){
      token = yylex();

      if (token != TOKEN_INSTRUCTION)
      {
         machine_raise_exception ("The simulator has encountered an illegal instruction.");
      }

      opcode = machine_get_opcode(yylval.str);
      machine_execute_instruction (opcode);

      /* TODO: Post executing instruction. */
   }
}

int
machine_execute_instruction (int opcode)
{
   switch (opcode)
   {
      case MOV:
      case PORT:
         machine_execute_mov ();
         break;

      case ADD:
      case SUB:
      case MUL:
      case DIV:
      case MOD:
         machine_execute_arith(opcode);
         break;

      case INR:
      case DCR:
         machine_execute_unary(opcode);
         break;

      case LT:
      case GT:
      case EQ:
      case NE:
      case GE:
      case LE:
         machine_execute_logical(opcode);
         break;

      case JZ:
      case JNZ:
      case JMP:
         machine_execute_jump (opcode);
         break;

      case PUSH:
      case POP:
         machine_execute_stack (opcode);
         break;

      case CALL:
         machine_execute_call ();
         break;

      case RET:
         machine_execute_ret ();
         break;

      case BKRP:
         machine_execute_bkrp ();
         break;

      case INT:
         machine_execute_interrupt ();
         break;

      case LOAD:
         machine_execute_load ();
         break;

      case LOADI:
         machine_execute_loadi ();
         break;

      case STORE:
         machine_execute_store ();
         break;

      case ENCRYPT:
         machine_execute_encrypt();
         break;

      case BACKUP:
         machine_execute_backup ();
         break;

      case RESTORE:
         machine_execute_restore ();
         break;
   }
}