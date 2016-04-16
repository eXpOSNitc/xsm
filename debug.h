#ifndef _XSM_DEBUG_H

#define _XSM_DEBUG_H

#include "machine.h"
#include "memory.h"

#define OFF FALSE
#define ON TRUE

#define DEBUG_COMMAND_LEN 100

/* The machine that is debugged. */
xsm_cpu *db_machine;

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
debug_command(const char *command);

#endif