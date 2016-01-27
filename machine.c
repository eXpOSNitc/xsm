#include <stdlib.h>
#include <string.h>
#include "machine.h"
#include "tokenize.h"

static
xsm_cpu _thecpu;

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
   YYSTYPE token_info;

   while (TRUE){
      token = tokenize_next_token (&token_info);

      if (token != TOKEN_INSTRUCTION)
      {
         machine_raise_exception ("The simulator has encountered an illegal instruction.");
      }

      opcode = machine_get_opcode(yylval.str);
      machine_execute_instruction (opcode);

      /* TODO: Post executing instruction. */
   }

   return TRUE;
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

   return TRUE;
}

int
machine_execute_mov ()
{
   int token;
   xsm_word *l_address, *r_address;
   YYSTYPE token_info;

   token = tokenize_peek (&token_info);

   switch (token)
   {
      case TOKEN_DREF_L:
         l_address = machine_get_address ();
         break;

      case TOKEN_REGISTER:
         l_address = registers_get_register (yylval.str);
         token = tokenize_next_token (&token_info);
         break;
   }

   if (!address)
   {
      machine_raise_exception("Error in calculating the target address.");
      return;
   }

   token = tokenize_next_token(&token_info);

   if (token != TOKEN_COMMA)
   {
      machine_raise_exception ("Malformed instruction.");
      return XSM_FAILURE;
   }

   token = tokenize_peek (&token_info);

   switch (token)
   {
      case TOKEN_DREF_L:
         r_address = machine_get_address ();
         word_copy (l_adress, r_address);
         break;

      case TOKEN_REGISTER:
         r_address = registers_get_register(yylval.str);
         word_copy (l_address, r_address);
         tokenize_next_token(&token_info);
         break;

      case TOKEN_NUMBER:
         word_store_integer (l_address, atoi(yylval.val));
         tokenize_next_token(&token_info);
         break;

      case TOKEN_STRING:
         word_store_string (l_address, yylval.str);
         tokenize_next_token(&token_info);
         break;

      default:
         /* Nothing to do. */
   }

   return XSM_SUCCESS;
}

xsm_word*
machine_get_address ()
{
   int token, address;
   YYSTYPE token_info;

   /* Skip the opening square bracket. */
   tokenize_next_token(&token_info);
   token = tokenize_next_token(&token_info);

   switch (token)
   {
      case TOKEN_REGISTER:
      {
         xsm_reg *reg = registers_get_register (token_info.str);
         address = word_get_integer (reg);
      }
      break;

      case TOKEN_NUMBER:
         address = token_info.val;
         break;

      default:
         /* Mark him. */
         machine_raise_exception ("Invalid memory derefence.");
   }

   /* Next one is a bracket, neglect. */
   tokenize_next_token(&token_info);

   /* What is the next one ? A comma ?*/

   token = tokenize_peek (&token_info);

   switch (token)
   {
      case TOKEN_REGISTER:
      {
         xsm_reg *reg = registers_get_register (token_info.str);
         address = address + word_get_integer(reg);
      }
      break;

      case TOKEN_NUMBER:
         address = address + token_info.val;
         break;
   }

   /* Ask the MMU to translate the address for us. */
   address = machine_translate_address (address);

   return memory_get_word(address);
}

int
machine_translate_address (int address)
{
   int ptbr;

   if (_thecpu->mode == XSM_MODE_KERNEL)
      return address;

   /* User mode, ask the MMU to translate. */
   ptbr = word_get_integer (registers_get_register("PTBR"));
   return memory_translate_address (ptbr, address);
}

int
machine_execute_arith (int opcode)
{
   int result;
   xsm_reg *l_operand, *r_operand;
   YYSTYPE token_info;
   int l_value, r_value;

   token = tokenize_next_token(&token_info);

   if (token != TOKEN_REGISTER)
      machine_raise_exception ("Wrong operand.");

   l_operand = registers_get_register(token_info.str);
   l_value = word_get_integer(l_operand);

   /* Next one is a comma, neglect. */
   tokenize_next_token(&token_info);

   token = tokenize_next_token(&token_info);

   if (token == TOKEN_NUMBER)
   {
      r_value = token_info.val;
   }
   else
   {
      r_operand = registers_get_register(token_info.str);
      r_value = word_get_integer (r_operand);
   }

   switch (opcode)
   {
      case ADD:
         result = r_value + l_value;
         break;

      case SUB:
         result = l_value - r_value;
         break;

      case MUL:
         result = l_value * r_value;
         break;

      case DIV:
         /* Integer division by zero !*/
         result = l_value / r_value;
         break;

      case MOD:
         result = l_value / r_value;
         break;
   }

   word_store_integer (l_operand, result);
   return XSM_SUCCESS;
}

int
machine_execute_jump (int opcode)
{
   int test, target, token;
   YYSTYPE token_info;

   token = tokenize_next_token(&token_info);

   if (token == TOKEN_NUMBER)
   {
      test = TRUE; /* Take the branch, the jump is unconditional. */

      target = token_info.val;
   }
   else 
   {
      test = word_get_integer(registers_get_register(token_info.str));
      /* Skip the comma. */
      tokenize_next_token(&token_info);
      token = tokenize_next_token(&token_info);
      target = token_info.val;
   }

   if (JZ == opcode)
      test = !test;

   target = machine_translate_address (target);

   if (test)
   {
      /* Take the branch. */
      word_store_integer (registers_get_register("IP"), target);
   }
   else
   {
      /* Nothing to do. */
   }

   return XSM_SUCCESS;
}