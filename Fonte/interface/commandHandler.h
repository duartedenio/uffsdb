#include <stdio.h>
#include <stdlib.h>

#define FCOMMANDHANDLER
 
int yyparse();
int yylex();
int yylex_destroy();
extern int  yylineno;
extern FILE *yyin;

void historyInit();

void getComando(char * input);