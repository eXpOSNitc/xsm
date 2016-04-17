#include "debug.h"

#include <stdio.h>

debug_status
_db_status;

const
char *_db_commands_lh[] = {
	"step",
	"continue",
	"reg",
	"mem",
	"pcb",
	"pagetable",
	"filetable",
	"diskfreelist",
	"inodetable",
	"usertable",
	"location",
	"watch",
	"watchclear",
	"exit",
	"help"
};

const
char *_db_commands_sh[] = {
	"s",
	"c",
	"r",
	"m",
	"p",
	"pt",
	"ft",
	"mf",
	"df",
	"it",
	"ut",
	"l",
	"w",
	"wc",
	"e",
	"h"
};

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
		return debug_show_interface ();
	}

	return TRUE;
}

int
debug_show_interface ()
{
	char command[DEBUG_COMMAND_LEN];
	int done = FALSE;
	char next_instr[DEBUG_STRING_LEN];

	memory_retrieve_raw_instr (next_instr, _db_status.ip);

	printf ("Next instruction to execute: %s\n", next_instr);

	while (!done)
	{
		fgets (command, DEBUG_COMMAND_LEN, stdin);

		if (!strcmp(command, "exit") || !strcmp(command, "e"))
			return FALSE;

		done = debug_command (command);
	}

	return TRUE;
}

int
debug_command(const char *restrict command)
{
	char *arg1, *cmd, *arg2;
	int code;

	const char *delim = " \t";

	cmd = strtok(command, delim);

	code = debug_command_code (cmd);

	switch (code):
	{
		case DEBUG_STEP:
			return TRUE;

		case DEBUG_CONTINUE:
			_db_status.state = OFF;
			return TRUE;

		case DEBUG_REG:
			arg1 = strtok(NULL, delim);
			if (!arg1)
			{
				debug_display_registers();
				break;
			}
	}
}

int
debug_command_code (const char *restrict cmd)
{
	int i;

	for (i = 0; i <= DEBUG_HELP; ++i)
	{
		if (!strcmp(cmd, _db_commands_lh[i]))
			return i;
	}

	for (i = 0; i <= DEBUG_HELP; ++i)
	{
		if (!strcmp(cmd, _db_commands_sh[i]))
			return i;
	}

	return -1;
}