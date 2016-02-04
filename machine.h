#ifndef XSM_MACHINE_H

#define XSM_MACHINE_H

#include "types.h"
#include "registers.h"
#include "memory.h"
#include "disk.h"

#define XSM_ADDR_DREF 0
#define XSM_ADDR_NODREF 1

#define PRIVILEGE_USER 0
#define PRIVILEGE_KERNEL 1

/* Operation codes. */
#define MOV       0
#define ADD       1
#define SUB       2
#define MUL       3
#define DIV       4
#define MOD       5
#define INR       6
#define DCR       7
#define LT        8
#define GT        9
#define EQ        10
#define NE        11
#define GE        12
#define LE        13
#define JZ        14
#define JNZ       15
#define JMP       16
#define PUSH      17
#define POP       18
#define CALL      19
#define RET       20
#define BRKP      21
#define INT       22
#define LOADI     23
#define LOAD      24
#define STORE     25
#define ENCRYPT   26
#define BACKUP    27
#define RESTORE   28
#define PORT      29
#define IN        30
#define INI       31
#define OUT       32
#define IRET      33
#define HALT      34

#define XSM_INSTRUCTION_COUNT 34

#define XSM_DISKOP_LOAD 0
#define XSM_DISKOP_STORE 1

#define XSM_CONSOLE_PRINT 0
#define XSM_CONSOLE_READ 1

typedef
struct _disk_operation
{
   int src_block;
   int dest_page;
   int operation;
} disk_operation;

typedef
struct _console_operation
{
   xsm_word word;
   int operation;
} console_operation;

typedef 
struct _xsm_cpu
{
   xsm_reg *regs;
   int timer;
   int mode;
   int disk_state, disk_wait;
   int console_state, console_wait;

   disk_operation disk_op;
   console_operation console_op;
}
xsm_cpu;

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
   /* Operation code. */
   int opcode;
   /* The previlege that is required for executing this instruction. */
   int previlege;
   /* Addressing mode for left and right operands.*/
   int src_mode, dest_mode;
   /* Left and right operands. */
   xsm_word *dest;
   xsm_word *src;
   /* Additional information for the left and right operands. */
   xsm_word dest_info;
   xsm_word src_info;
}
xsm_instruction;

typedef
struct _xsm_operand
{
   int mode;
   int previlege;
   xsm_word *reg_or_mem;
   xsm_word info;
}
xsm_operand;

int
machine_init (xsm_options *options);

int
machine_get_opcode (const char* instr);

int
machine_serve_instruction (char *buffer, int *read_bytes, int max);

xsm_word *
machine_get_ipreg ();

xsm_word *
machine_get_spreg ();

int
machine_run ();

void
machine_post_execute ();

int
machine_execute_instruction (int opcode);

int
machine_execute_unary (int opcode);

int
machine_execute_mov ();

xsm_word*
machine_get_address ();

int
machine_translate_address (int address);

int
machine_execute_arith (int opcode);

int
machine_execute_jump (int opcode);

int
machine_execute_stack (int opcode);

int
machine_push_do (xsm_word *reg);

int
machine_pop_do (xsm_word *dest);

xsm_word*
machine_stack_pointer ();

int
machine_execute_call_do (int target);

int
machine_execute_backup();

int
machine_execute_restore ();

int
machine_execute_call ();

int
machine_execute_ret ();

int
machine_execute_interrupt();

int 
machine_execute_interrupt_do (int interrupt);

int
machine_interrupt_address (int int_num);

void
machine_set_mode (int mode);

int
machine_execute_disk (int operation, int immediate);

int
machine_execute_store_do (int page_num, int block_num);

int
machine_schedule_disk (int page_num, int block_num, int firetime, int operation);

int
machine_execute_load_do (int page_num, int block_num);

int
machine_execute_encrypt ();

int
machine_execute_print_do (xsm_word *word);

int
machine_execute_print ();

int
machine_execute_in_do (xsm_word *word);

int
machine_execute_iret ();

void
machine_destroy ();

#endif
