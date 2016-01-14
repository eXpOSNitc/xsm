#ifndef XSM_MACHINE_H

#define XSM_MACHINE_H

#include "types.h"

#define XSM_DIRECT_ADDR 0
#define XSM_INDIRECT_ADDR 1
#define XSM_CONSTANT 2

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
   /* Addressing mode for left and right operands.*/
   int src_mode, dest_mode;
   /* Left and right operands. */
   xsm_word *dest;
   xsm_word *src;
   /* Additional information for the left and right operands. */
   xsm_word *dest_address;
   xsm_word *src_address;
}
xsm_instruction;

#endif
