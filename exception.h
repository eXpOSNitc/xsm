#ifndef _XSM_EXCEPTION_H

#define _XSM_EXCEPTION_H

#include "types.h"

int
exception_raise (int exptype, const char *message);

#endif