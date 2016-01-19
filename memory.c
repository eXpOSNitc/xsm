#include "memory.h"
#include <stdlib.h>

static
xsm_word *_xsm_mem;

/* Turn the RAM on. */
int
memory_init ()
{
   _xsm_mem = (xsm_word *) malloc (sizeof(xsm_word) * XSM_MEMORY_SIZE);
   
   if (!_xsm_mem)
      return XSM_FAILED;
      
   return XSM_SUCCESS;
}

xsm_word*
memory_get_word (int address)
{
   if (!memory_is_address_valid (address))
      return NULL;
      
   return &_xsm_mem[address];
}

int
memory_is_address_valid (int address)
{
   if (address >= XSM_MEMORY_SIZE || address < 0)
      return FALSE;
   return TRUE;
}

void 
memory_destroy()
{
   free (_xsm_mem);
}
