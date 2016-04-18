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
			arg1 = strtok (NULL, delim);

			if (!arg1)
				debug_display_pcb();
			else
				debug_display_pcb_pid (atoi(arg1));
			break;

		case DEBUG_PAGETABLE:

			break;
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

int
debug_display_pcb_pid (int pid)
{
	int i, ptr;
	xsm_word *word;
	const char *fields[] = {"Tick", "PID", "PPID", "UserID", "State", "Swap Flag", "Inode Index",
	"Input Buffer", "Mode Flag", "User Area Swap Status", "User Area Page Number",
	"Kernel Stack Pointer", "User Stack Pointer", "PTBR", "Unused"
	};
	const int fields_len[] = {1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2};
	int fields = 14;

	ptr = DEBUG_LOC_PT + pid * PT_ENTRY_SIZE;

	for (i = 0; i < fields; ++i)
	{
		printf ("%s: ", fields[i]);

		/* Display the corresponding number of words. */
		int j;

		for (j = 0; j < fields_len[i]; ++j)
		{
			word = memory_get_word(ptr);
			printf ("%s ", word_get_string(word));
			ptr = ptr + 1;

		}

		printf ("\n");
	}

	return TRUE;
}

int
debug_display_pcb ()
{
	int pid;

	pid = debug_active_process();

	if (pid > -1)
	{
		debug_display_pcb_pid(pid);
		return TRUE;
	}
	
	printf ("No active processes.\n");
	return FALSE;
}

/* Returns the PID of the active process.*/
int
debug_active_process ()
{
	int ptr, pid, i;
	int pid_base, state_base;
	xsm_word *w_pid_base, *w_state_base;

	ptr = DEBUG_LOC_PT;
	pid = -1;

	/* Determine the active process and display it. */
	for (i = 0; i < MAX_PROC_NUM; ++i)
	{
		pid_base = ptr + 1;
		state_base = ptr + 4;

		w_pid_base = memory_get_word (pid_base);
		w_state_base = memory_get_word(state_base);

		if (DEBUG_PROC_RUNNING == word_get_integer(w_state_base))
		{
			pid = word_get_integer(w_pid_base);
			break;
		}

		ptr = ptr + PT_ENTRY_SIZE;
	}

	return pid;
}

int
debug_pcb_base (int pid)
{
	int ptr, pid_base, i;
	xsm_word *w_pid_base;
	int result = -1;

	ptr = DEBUG_LOC_PT;

	for (i = 0; i < MAX_PROC_NUM; ++i)
	{
		pid_base = ptr + 1;

		w_pid_base = memory_get_word(pid_base);

		if (pid == word_get_integer(w_pid_base))
		{
			result = ptr;
			break;
		}

		ptr = ptr + PT_ENTRY_SIZE;
	}

	return result;
}

int
debug_display_pt_ptbr ()
{
	int addr;
	xsm_word *reg_ptbr;

	reg_ptr = registers_get_register("PTBR");
	addr = word_get_integer(reg_ptbr);

	return debug_display_pt_at (addr);
}

int
debug_display_pt_at (int addr)
{
	int i, ptr;
	xsm_word *word;

	ptr = addr;

	for (i = 1; i <= MAX_NUM_PAGES; ++i)
	{
		printf ("VIRT %d\t", i);
		word = memory_get_word(ptr++);
		printf ("PHY %s\t", word_get_string(word));

		word = memory_get_word(ptr++);
		printf ("REF %s\t", word_get_string(word));

		word = memory_get_word(ptr++);
		printf ("VAL %s\t", word_get_string(word));

		word = memory_get_word(ptr++);
		printf ("WRITE %s\t", word_get_string(word));
	}

	return TRUE;
}

int
debug_display_pt_pid (int pid)
{
	int pcb_base, ptbr_addr, addr;
	xsm_word *word;

	pcb_base = debug_pcb_base (pid);
	ptbr_addr = pcb_base + PTBR_PCB_OFFSET;

	word = memory_get_word(ptbr_addr);

	addr = word_get_integer (word);
	return debug_display_pt_at(addr);
}