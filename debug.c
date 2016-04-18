#include "debug.h"

#include <stdio.h>
#include <string.h>

static
debug_status
_db_status;

static
xsm_cpu
*db_machine;

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
debug_command(char *command)
{
	char *arg1, *cmd, *arg2;
	int code;

	const char *delim = " \t";

	cmd = strtok(command, delim);

	code = debug_command_code (cmd);

	switch (code)
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
				debug_display_all_registers();
			}
			else{
				arg2 = strtok(NULL, delim);
				if (!arg2)
				{
					debug_display_register(arg1);
				}
				else
				{
					debug_display_range_reg(arg1, arg2);
				}
			}
			break;

		case DEBUG_MEM:
			arg1 = strtok (NULL, delim);
			arg2 = strtok (NULL, delim);

			if (arg2)
			{
				debug_display_mem_range(atoi(arg1), atoi(arg2));
			}
			else
			{
				debug_display_mem(atoi(arg1));
			}
			break;

		case DEBUG_PCB:
			//TODO

	}

	return FALSE;
}

int
debug_command_code (const char *cmd)
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

int
debug_display_all_registers()
{
	const char **reg_names = registers_names ();
	int num_regs = registers_len ();
	int i;
	char *content;
	xsm_word *reg;

	for (i = 0; i < num_regs; ++i)
	{
		reg = registers_get_register (reg_names[i]);
		content = word_get_string (reg);
		printf ("%s: %s\n", reg_names[i], content);
	}

	return TRUE;
}

int
debug_display_register (const char *regname)
{
	xsm_word *reg;
	char *content;

	reg = registers_get_register (regname);
	content = word_get_string (reg);

	if (!reg)
	{
		printf ("No such register.\n");
		return FALSE;
	}

	printf ("%s: %s\n", regname, content);
	return TRUE;
}

int
debug_display_range_reg (const char *reg_b_name, const char *reg_e_name)
{
	const char **reg_names = registers_names ();
	int num_regs = registers_len ();
	int i;
	xsm_word *reg;
	char *content;

	for (i = 0; i <  num_regs; ++i)
	{
		if (!strcmp (reg_b_name, reg_names[i]))
			break;
	}

	for (; i < num_regs; ++i)
	{
		reg = registers_get_register (reg_names[i]);
		content = word_get_string(reg);

		printf ("%s: %s\n", reg_names[i], content);

		if (!strcmp (reg_e_name, reg_names[i]))
			break;
	}

	return TRUE;
}

int
debug_display_mem(int page)
{
	xsm_word *page;
	int i;
	char *content;

	page = memory_get_page(page);

	if (!page)
	{
		printf ("No such page.\n");
		return FALSE;
	}

	for (i = 0; i < XSM_PAGE_SIZE; i++)
	{
		content = word_get_string(page);
		printf("+%d: %s\n", i, content);
		page++; /* ! */
	}

	return TRUE;
}

int
debug_display_mem_range (int page_l, page_h)
{
	int i;

	for (i = page_l; i <= page_h; ++i)
	{
		printf ("Page: %d\n", i);
		debug_display_mem(i);
	}

	return TRUE;
}