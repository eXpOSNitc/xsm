#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "simulator.h"

static
xsm_options _options;

static
const char *XSM_DEFAULT_DISK = "../xfs-interface/disk.xfs";

int
simulator_run ()
{
	disk_init (XSM_DEFAULT_DISK);

	/* On your marks! */
	if (!machine_init (&_options))
		return XSM_FAILURE;

	/* Fire! */
	if (!machine_run ())
		return XSM_FAILURE;

	printf ("Machine is halting.\n");

	/* A little housekeeping. */
	machine_destroy ();
	disk_close ();
	return XSM_SUCCESS;
}

int
simulator_parse_args(int argc, char **argv)
{
	int val;

	argv++;
	argc --;

	/* TODO: Initialize with default values. */

	while (argc > 0)
	{
		if (!strcmp (*argv, "--debug"))
		{
			_options.debug = TRUE;

			argv ++;
			argc --;
		}
		else if (!strcmp(*argv, "--timer"))
		{
			argv ++;
			argc --;

			val = atoi(*argv);
			_options.timer = val;

			argv++;
			argc --;
		}
		else if (!strcmp(*argv, "--console"))
		{
			argv ++;
			argc --;

			val = atoi(*argv);
			_options.console = val;

			argv++;
			argc--;
		}
		else if (!strcmp(*argv, "--disk"))
		{
			argv++;
			argc --;

			val = atoi(*argv);
			_options.disk = val;

			argv++;
			argc--;
		}
		else
		{
			/* Unrecognised option. */
			return XSM_FAILURE;
		}
	}

	return XSM_SUCCESS;
}
