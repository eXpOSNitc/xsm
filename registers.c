static
xsm_reg *_registers;

static
const
char *_register_names[] = {
   "R0",
   "R1",
   "R2",
   "R3",
   "R4",
   "R5",
   "R6",
   "R7",
   "R8",
   "R9",
   "R10",
   "R11",
   "R12",
   "R13",
   "R14",
   "R15",
   "R16",
   "R17",
   "R18",
   "R19",

   "P0",
   "P1",
   "P2",
   "P3",

   "BP",
   "SP",
   "IP",
   "PTBR",
   "PTLR",
   "EIP",
   "EC",
   "EPN",
   "EMA"
};

int
registers_init ()
{
   _registers = (xsm_reg *) malloc (sizeof(xsm_reg) * XSM_NUM_REG);
   
   if (!_registers)
      return XSM_FAILED;
      
   return XSM_SUCCESS;   
}

xsm_reg*
registers_get_register (const char *name)
{
   register int i;
   
   for (i = 0; i < XSM_NUM_REG; ++i)
   {
      if (!stricmp(name, _register_names[i]))
         return registers[i];
   }
   
   return NULL;
}

void
registers_destroy()
{
   free (_registers);
}
