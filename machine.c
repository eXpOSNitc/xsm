#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "machine.h"
#include "tokenize.h"

static
xsm_cpu _thecpu;

static
xsm_options _theoptions;

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
machine_init (xsm_options *options)
{
   xsm_word *ipreg;

   _theoptions = *options;

   /* Set up the registers. */
   if (!registers_init ())
      return XSM_FAILURE;

   if (!memory_init())
      return XSM_FAILURE;

   /* Load the boot code onto the memory.. */
   disk_read_block (0, 0);

   /* Set up IP.. */
   ipreg = machine_get_ipreg ();
   word_store_integer(ipreg, 0);

   return XSM_SUCCESS;
}

int
machine_get_opcode (const char* instr)
{
   int i;
   
   for (i = 0; i < XSM_INSTRUCTION_COUNT; ++i)
   {
      if (!strcasecmp(instr, instructions[i]))
         return i;
   }
   
   return XSM_ILLINSTR;
}

/* When the lexer calls, serve him with the instruction to execute. */
int
machine_serve_instruction (char *buffer, int *read_bytes, int max)
{
   int ip_val, i;
   xsm_word *ip_reg;
   int bytes_to_read;
   xsm_word *instr_mem;

   bytes_to_read = XSM_INSTRUCTION_SIZE * XSM_WORD_SIZE;

   ip_reg = machine_get_ipreg();
   ip_val = word_get_integer(ip_reg);

   ip_val = machine_translate_address(ip_val);
   instr_mem = memory_get_word(ip_val);

   memcpy (buffer, instr_mem, bytes_to_read);

   /* Trim. */
   for (i = 0; i < bytes_to_read; ++i)
   {
      if (buffer[i] == '\0')
         buffer[i] = ' ';
   }

   buffer[bytes_to_read - 1] = '\0';
   *read_bytes = bytes_to_read;

   return TRUE;
}

xsm_word *
machine_get_ipreg ()
{
   return registers_get_register("IP");
}

xsm_word *
machine_get_spreg ()
{
   return registers_get_register("SP");
}

int
machine_run ()
{
   int token, opcode;
   YYSTYPE token_info;
   xsm_word *ipreg;
   int ipval;

   ipreg = machine_get_ipreg ();

   while (TRUE){
      /* Flush the instruction stream. */
      tokenize_clear_stream ();

      token = tokenize_next_token (&token_info);

      if (token != TOKEN_INSTRUCTION)
      {
         machine_raise_exception ("The simulator has encountered an illegal instruction.");
      }

      /* IP = IP + instruction length. */
      ipval = word_get_integer(ipreg);
      ipval = ipval + XSM_INSTRUCTION_SIZE;
      word_store_integer (ipreg, ipval);

      opcode = machine_get_opcode(yylval.str);
      machine_execute_instruction (opcode);

      /* TODO: Post executing instruction. */
      machine_post_execute ();
   }

   return TRUE;
}

void
machine_post_execute ()
{
   /* Tick the timers. */
   _thecpu.timer--;

   if (_thecpu.timer == 0)
   {
      machine_execute_interrupt(XSM_INTERRUPT_TIMER);
      _thecpu.timer = _theoptions.timer;
   }
   /* Handle the disk interrupt. */
   if (_thecpu.disk_state == XSM_DISK_BUSY)
   {
      _thecpu.disk_wait--;

      if (_thecpu.disk_wait == 0)
      {
         if (XSM_DISKOP_LOAD == _thecpu.disk_op.operation)
         {
            machine_execute_load_do (_thecpu.disk_op.dest_page, _thecpu.disk_op.src_block);
            machine_execute_interrupt_do(XSM_INTERRUPT_DISK);
         }
         else if (XSM_DISKOP_STORE == _thecpu.disk_op.operation)
         {
            machine_execute_store_do (_thecpu.disk_op.dest_page, _thecpu.disk_op.src_block);
            machine_execute_interrupt_do(XSM_INTERRUPT_DISK);
         }

         _thecpu.disk_state = XSM_DISK_IDLE;
      }
   }

   if (XSM_CONSOLE_BUSY == _thecpu.console_state)
   {
      _thecpu.console_wait--;
      if (_thecpu.console_wait == 0)
      {
         if (XSM_CONSOLE_PRINT == _thecpu.console_op.operation)
         {
            machine_execute_print_do(&_thecpu.console_op.word);
            machine_execute_interrupt_do(XSM_INTERRUPT_CONSOLE);
         }
         else if (XSM_CONSOLE_READ == _thecpu.console_op.operation)
         {
            xsm_word *dest_port;
            machine_execute_in_do(&_thecpu.console_op.word);

            dest_port = registers_get_register ("P0");
            word_copy (dest_port, &_thecpu.console_op.operation);
            machine_execute_interrupt_do(XSM_INTERRUPT_CONSOLE);
         }

         _thecpu.console_state = XSM_CONSOLE_IDLE;
      }
   }

   return;
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

      case BRKP:
         machine_execute_brkp ();
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
         machine_execute_store (FALSE);
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

      case HALT:
         printf ("System halted.\n");
         return TRUE;
   }

   return TRUE;
}

int
machine_execute_brkp ()
{
   /* TODO: Initiate debugger. */
   return XSM_SUCCESS;
}

int
machine_execute_unary (int opcode)
{
   int token;
   YYSTYPE token_info;
   xsm_word *arg_reg;
   int val;

   token = tokenize_next_token(&token_info);
   arg_reg = registers_get_register(token_info.str);

   val = word_get_integer(arg_reg);

   switch (opcode)
   {
      case INR:
         val = val + 1;
         break;

      case DCR:
         val = val - 1;
         break;
   }

   word_store_integer (arg_reg, val);
   return XSM_SUCCESS;
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

int
machine_execute_stack (int opcode)
{
   YYSTYPE token_info;
   int token;
   xsm_word *reg;

   token = tokenize_next_token(&token_info);

   if (token == TOKEN_REGISTER)
   {
      reg = registers_get_register(token_info.str);
   }
   else
   {
      /* TODO Invalid argument, raise exception. */

   }

   switch (opcode)
   {
      case PUSH:
         return machine_push_do(reg);

      case POP:
         return machine_pop_do (reg);
   }
}

int
machine_push_do (xsm_word *reg)
{
   xsm_word *xw_stack_top;
   xsm_word *sp_reg;
   int stack_top;

   xw_stack_top = machine_stack_pointer ();
   sp_reg = registers_get_register("SP");
   stack_top = word_get_integer(sp_reg);

   word_copy (xw_stack_top, reg);
   word_store_integer(sp_reg, stack_top + 1);
   return XSM_SUCCESS;
}

int
machine_pop_do (xsm_word *dest)
{
   xsm_word *xw_stack_top;
   xsm_word *sp_reg;
   int stack_top;

   xw_stack_top = machine_stack_pointer();
   sp_reg = registers_get_register("SP");
   stack_top = word_get_integer(sp_reg);

   word_copy (reg, xw_stack_top);
   word_store_integer(sp_reg, stack_top - 1);
   return XSM_SUCCESS;
}

xsm_word*
machine_stack_pointer ()
{
   xsm_word *sp_reg;
   int stack_top;

   sp_reg = registers_get_register ("SP");
   stack_top = word_get_integer(sp_reg);

   stack_top = machine_translate_address (stack_top);

   return memory_get_word(stack_top);
}

int
machine_execute_call_do (int target)
{
   int target, curr_ip, curr_sp;
   xsm_word *ipreg;
   xsm_word *stack_pointer;
   xsm_word *spreg;

   ipreg = registers_get_register("IP");
   curr_ip = word_get_integer(ipreg);

   stack_pointer = machine_stack_pointer ();
   word_store_integer(stack_pointer, curr_ip + XSM_INSTRUCTION_SIZE);

   spreg = registers_get_register("SP");
   curr_sp = word_get_integer(spreg);
   word_store_integer(spreg, curr_sp + 1);

   word_store_integer (ipreg, target);
   return XSM_SUCCESS;
}

int
machine_execute_backup()
{
   xsm_word *reg;
   int ireg;
   char str_reg[5];

   reg = registers_get_register("BP");
   machine_execute_push_do (reg);

   reg = registers_get_register("PTBR");
   machine_execute_push_do (reg);

   reg = registers_get_register("PTLR");
   machine_execute_push_do (reg);

   for (ireg = 0; ireg < 19; ++ireg)
   {
      sprintf (str_reg, "R%d", ireg);
      reg = registers_get_register(str_reg);
      machine_execute_push_do(reg);
   }

   return XSM_SUCCESS;
}

int
machine_execute_restore ()
{
   xsm_word *reg;
   int ireg;
   char str_reg[5];

   for (ireg = 19; ireg >= 0; ireg--)
   {
      sprintf (str_reg, "R%d", ireg);
      reg = registers_get_register(str_reg);
      machine_execute_pop_do (reg);
   }

   reg = registers_get_register ("PTLR");
   machine_execute_pop_do (reg);

   reg = registers_get_register ("PTBR");
   machine_execute_pop_do (reg);

   reg = registers_get_register ("BP");
   machine_execute_pop_do (reg);

   return XSM_SUCCESS;
}

int
machine_execute_call ()
{
   int token, target;
   YYSTYPE token_info;

   token = tokenize_next_token(&token_info);
   target = token_info.val;

   machine_execute_call_do (target);
}

int
machine_execute_ret ()
{
   int target;
   xsm_word *spreg, *ipreg;
   xsm_word *stack_pointer;
   int curr_sp;

   spreg = registers_get_register ("SP");
   stack_pointer = machine_stack_pointer ();
   target = word_get_integer(stack_pointer);

   curr_sp = word_get_integer (spreg);
   curr_sp = curr_sp - 1;
   word_store_integer (spreg, curr_sp);

   ipreg = registers_get_register("IP");
   word_store_integer (ipreg, target);

   return XSM_SUCCESS;
}

int
machine_execute_interrupt()
{
   int token;
   YYSTYPE token_info;
   int interrupt_num, target;

   token = tokenize_next_token(&token_info);

   interrupt_num = token_info.val;

   return machine_execute_interrupt_do(interrupt_num);
}

int 
machine_execute_interrupt_do (int interrupt)
{
   int target;

   target = machine_interrupt_address (interrupt_num);

   machine_execute_call_do (target);

   /* Change the mode now, that will do. */
   machine_set_mode (XSM_MODE_KERNEL);
   return XSM_SUCCESS;
}

int
machine_interrupt_address (int int_num)
{
   if (int_num < 0 || int_num > 18)
      return -1; /* Not supposed to happen. */

   return (int_num * 2 + 4) * XSM_PAGE_SIZE;
}

void
machine_set_mode (int mode)
{
   _thecpu.mode = mode;
}

int
machine_execute_disk (int operation, int immediate)
{
   int token;
   YYSTYPE token_info;
   int page_num;
   int block_num;
   xsm_word *page_base;

   token = tokenize_next_token(&token_info);
   page_num = token_info.val;

   token = tokenize_next_token(&token_info);
   block_num = token_info.val;

   if (immediate)
   {
      if (operation == XSM_DISKOP_LOAD)
         machine_execute_load_do(page_num, block_num);
      else if (XSM_DISKOP_STORE == operation)
         machine_execute_store_do(page_num, block_num);
   }
   else
      return machine_schedule_disk (page_num, block_num, _theoptions.disk, operation);
}

int
machine_execute_store_do (int page_num, int block_num)
{
   xsm_word *page_base;

   page_base = memory_get_page(page_num);
   return disk_write_page (page_num, block_num);
}

int
machine_schedule_disk (int page_num, int block_num, int firetime, int operation)
{
   /* If the disk is busy, just ignore the request. */
   if (_thecpu.disk_state == XSM_DISK_BUSY)
      return XSM_SUCCESS;

   _thecpu.disk_state = XSM_DISK_BUSY;
   _thecpu.disk_wait = firetime;
   _thecpu.disk_op.src_block = block_num;
   _thecpu.disk_op.dest_page = page_num;
   _thecpu.disk_op.operation = operation;

   return XSM_SUCCESS;
}

int
machine_execute_load_do (int page_num, int block_num)
{
   xsm_word *page_base;
   
   page_base = memory_get_page (page_num);

   return disk_read_block (page_base, block_num);
}

int
machine_execute_encrypt ()
{
   int token;
   YYSTYPE token_info;
   xsm_word *reg;
   xsm_word encrypted;

   token = tokenize_next_token(&token_info);
   reg = registers_get_register(token_info.str);

   /* Some very easy encryption. */
   word_encrypt (&encrypted, reg);

   return XSM_SUCCESS;
}

int
machine_execute_print_do (xsm_word *word)
{
   int type;
   char *str;
   int val;

   type = word_get_unix_type (word);

   if (type == XSM_TYPE_STRING)
   {
      str = word_get_string(word);
      fprintf (stdout, "%s\n", str);
   }
   else
   {
      val = word_get_integer(word);
      fprintf(stdout, "%d\n", val);
   }

   return XSM_SUCCESS;
}

int
machine_execute_print ()
{
   int val;
   int token;
   YYSTYPE token_info;
   xsm_word *reg;

   token = tokenize_next_token(&token_info);
   reg = registers_get_register (token_info.str);

   return machine_execute_print_do(reg);
}

int
machine_execute_in_do (xsm_word *word)
{
   char input[XSM_WORD_SIZE];

   /*TODO: Be a bit careful here. */
   fgets (input, XSM_WORD_SIZE, stdin);
   return word_store_string(word, input);
}

int
machine_execute_iret ()
{
   xsm_word target;
   xsm_word *ipreg;

   machine_set_mode (XSM_MODE_USER);
   machine_pop_do (&target);
   
   ipreg = registers_get_register("IP");
   word_store_integer(ipreg, target);
   return XSM_SUCCESS;
}

void
machine_destroy ()
{
   /* Demolish. */
   memory_destroy ();
   registers_destroy();
}