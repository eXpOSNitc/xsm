#include "exception.h"

#include <stdio.h>

int
exception_raise (int exptype, const char *message)
{
	fprintf(stderr, "%s\n", message);
	return XSM_FAILED;
}