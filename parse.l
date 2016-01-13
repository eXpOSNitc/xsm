-?[0-9]+ {
   yylval = strdup(yytext);
   return TOKEN_NUMBER;
}

\"[^"]* {
   yylval = strdup(yytext);
   return TOKEN_STRING;
}

\[ {
   return TOKEN_DREF_L;
}

\] {
   return TOKEN_DREF_R;
}

SP {
   return TOKEN_SP;
}

BP {
   return TOKEN_BP;
}

IP {
   return TOKEN_IP;
}

PTBR {
   return TOKEN_PTBR;
}

PTLR {
   return TOKEN_PTLR;
}

EIP {
   return TOKEN_EIP;
}

EC {
   return TOKEN_EC;
}

EPN {
   return TOKEN_EPN;
}

EMA {
   return TOKEN_EMA;
}

P[0-3] {
   yylval = strdup(yytext);
   return TOKEN_PORT;
}

R[0-9]+ {
   yylval = strdup(yytext);
   return TOKEN_REGISTER;
}

[a-zA-Z] {
   yylval = strdup(yytext);
   return TOKEN_INSTRUCTION;
}
