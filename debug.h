#ifndef _XSM_DEBUG_H

#define _XSM_DEBUG_H

#include "machine.h"
#include "memory.h"

#define OFF FALSE
#define ON TRUE

#define DEBUG_COMMAND_LEN 100
#define DEBUG_STRING_LEN 100

#define DEBUG_STEP			0
#define DEBUG_CONTINUE		1
#define DEBUG_REG			2
#define DEBUG_MEM			3
#define DEBUG_PCB			4
#define DEBUG_PAGETABLE		5
#define DEBUG_FILETABLE		6
#define DEBUG_DISKFREELIST	7
#define DEBUG_INODETABLE	8
#define DEBUG_USERTABLE		9
#define DEBUG_LOCATION		10
#define DEBUG_WATCH 		11
#define DEBUG_WATCHCLEAR	12
#define DEBUG_EXIT			13
#define DEBUG_HELP			14

struct
_xsm_cpu;

typedef
struct _xsm_cpu
xsm_cpu;

/* The machine that is debugged. */

typedef
struct _debug_status
{
	int state;
	int ip;
}
debug_status;

/* Function prototypes. */
int
debug_init (xsm_cpu *machine);

int
debug_next_step (int curr_ip);

int
debug_show_interface ();

int
debug_command(char *command);

int
debug_command_code (const char *cmd);

int
debug_display_all_registers();

#endif