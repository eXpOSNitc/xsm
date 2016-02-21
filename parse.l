%{
   /* Include files. */
   #include <stdlib.h>
   #include "machine.h"
   #include "lexer.h"

   YYSTYPE yylval;
%}
%%

-?[0-9]+ {
   yylval.val = atoi(yytext);
   return TOKEN_NUMBER;
}

\"[^"]* {
   yylval.str = yytext;
   return TOKEN_STRING;
}

, {
   return TOKEN_COMMA;
}

\[ {
   return TOKEN_DREF_L;
}

\] {
   return TOKEN_DREF_R;
}

SP|BP|IP|PTBR|PTLR|EIP|EC|EPN|EMA {
   yylval.str = yytext;
   return TOKEN_REGISTER;
}

P[0-3] {
   yylval.str = yytext;
   return TOKEN_REGISTER;
}

R[0-9]+ {
   yylval.str = yytext;
   return TOKEN_REGISTER;
}

[a-zA-Z] {
   yylval.str = yytext;
   return TOKEN_INSTRUCTION;
}
