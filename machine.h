#ifndef XSM_MACHINE_H

#define XSM_MACHINE_H

#include "types.h"

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

#endif
