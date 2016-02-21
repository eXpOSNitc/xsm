#include "memory.h"
#include "exception.h"
#include <stdlib.h>

static
xsm_word *_xsm_mem;

/* Turn the RAM on. */
int
memory_init ()
{
   _xsm_mem = (xsm_word *) malloc (sizeof(xsm_word) * XSM_MEMORY_SIZE);
   
   if (!_xsm_mem)
      return XSM_FAILURE;
      
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

/* Paging hardware functions. */
int
memory_translate_address (int ptbr, int address, int write)
{
   int page, offset;
   int target_page;

   page = address / XSM_PAGE_SIZE;
   offset = address % XSM_PAGE_SIZE;

   target_page = memory_translate_page(ptbr, page, write);
   return target_page + offset;
}

xsm_word*
memory_get_page (int page)
{
   return memory_get_word (page * XSM_PAGE_SIZE);
}

int
memory_translate_page (int ptbr, int page, int write)
{
   int page_entry, page_info;
   xsm_word *page_entry_w, *page_info_w;
   int entry;
   char *info;

   page_entry = page * 2 + ptbr;
   page_info = page_entry + 1;

   page_entry_w = memory_get_word (page_entry);
   page_info_w = memory_get_word(page_entry);

   entry = word_get_integer (page_entry_w);
   info = word_get_string (page_info_w);

   if (info[1] == '0')
      exception_raise (0, "Page fault.");

   if (write && info[2] == '0')
      exception_raise (0, "Memory fault.");

   return entry;
}

void 
memory_destroy()
{
   free (_xsm_mem);
}
