#include "debug.h"

#include <stdio.h>

debug_status
_db_status;

int
debug_init (xsm_cpu *machine)
{
	db_machine = machine;
	_db_status.state = OFF;

	return TRUE;
}

/* The instruction fetch-exec cycle will repeatedly call this function before each
 * instruction exec phase.
 * This function will invoke the debug mode, if necessary.
 * @param curr_ip The current [physical] address of IP.
 */
int
debug_next_step (int curr_ip)
{
	_db_status.ip = curr_ip;

	if (_db_status.state == ON)
	{
		debug_show_interface ();
	}

	return TRUE;
}

int
debug_show_interface ()
{
	char command[DEBUG_COMMAND_LEN];
	int done = FALSE;

	printf ("Next instruction to execute: %s\n", memory_display_instruction(_db_status.ip));

	while (!done)
	{
		fgets (command, DEBUG_COMMAND_LEN, stdin);
		done = debug_command (command);
	}

	return TRUE;
}

int
debug_command(const char *command)
{
	if (!strmcp (command, "exit"))
		return TRUE;
}