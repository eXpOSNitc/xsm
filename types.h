#ifndef XSM_TYPES_H

#define XSM_TYPES_H

#define R0     0
#define R1     1
#define R2     2
#define R3     3
#define R4     4
#define R5     5
#define R6     6
#define R7     7
#define R8     8
#define R9     9
#define R10    10
#define R11    11
#define R12    12
#define R13    13
#define R14    14
#define R15    15
#define R16    16
#define R17    17
#define R18    18
#define R19    19

#define P0     20
#define P1     21
#define P2     22
#define P3     23

#define BP_REG    24
#define SP_REG    25
#define IP_REG    26
#define PTBR_REG  27
#define PTLR_REG  28
#define EIP_REG   29
#define EC_REG    30
#define EPN_REG   31
#define EMA_REG   32

typedef
struct _xsm_reg
{
   char val[XSM_REGSIZE];
}
xsm_reg;

#endif


