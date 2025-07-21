%{
#include <stdio.h>
#ifndef FMACROS
   #include "../macros.h"
#endif
#ifndef FTYPES
   #include "../types.h"
#endif
#ifndef FPARSER
   #include "parser.h"
#endif
#include "y.tab.h"
#define YY_NO_INPUT
%}

%option nounput

%%

(?i:INSERT)						return INSERT;
(?i:INTO)						return INTO;
(?i:VALUES)						return VALUES;
(?i:SELECT)						return SELECT;
(?i:FROM)						return FROM;
(?i:WHERE)						return WHERE;
(?i:CREATE)						return CREATE;
(?i:TABLE)						return TABLE;
(?i:ON)						    return ON;
(?i:INDEX)						return INDEX;
(?i:INTEGER)					return INTEGER;
(?i:AND)					return LOGICO;
(?i:OR)					return LOGICO;
(?i:INT)						return INTEGER;
(?i:VARCHAR)					return VARCHAR;
(?i:CHAR)						return CHAR;
(?i:DOUBLE)						return DOUBLE;
(?i:PRIMARY)					return PRIMARY;
(?i:KEY)						return KEY;
(?i:REFERENCES)					return REFERENCES;
(?i:DATABASE)					return DATABASE;
(?i:DROP)						return DROP;
(?i:CONTRIBUTORS)               return CONTR;
help							return HELP;
[\\][h]							return HELP;
[\\][h]+[^\n]*					{invalidCommand(yytext);};
[\\][\?]						return HELP;
(?i:CLEAR)						return CLEAR;

[,]								return *yytext;
[;]								return *yytext;
[-]								return *yytext;
[+]								return *yytext;


[)]                     {yylval.strval = yytext; return FECHA_P;};
[(]                     {yylval.strval = yytext; return ABRE_P;};
[*]                     {yylval.strval = yytext; return ASTERISCO;};
([a-zA-Z_]+[0-9_]*)+		{yylval.strval = yytext; return OBJECT;};
([\']([^']|'')*[\'])		{yylval.strval = yytext; return STRING;};
[0-9]*\.[0-9]*					{yylval.strval = yytext; return VALUE;};
[0-9]+							    {yylval.strval = yytext; return NUMBER;};
([%]|[/])       {yylval.strval = yytext; return OPERADOR;}

([>][=]|[<][=]|[=]|[!][=]|[>]|[<]) {yylval.strval = yytext; return RELACIONAL;};

\n								{
									if (GLOBAL_PARSER.mode != 0) {
										if (GLOBAL_PARSER.parentesis)
											printf("%s(# ", connected.db_name);
										else
											printf("%s-# ", connected.db_name);
									} else if (connected.conn_active) {
								 		if (GLOBAL_PARSER.consoleFlag)
							                GLOBAL_PARSER.consoleFlag = 0;
							            else
							                printf("%s=# ", connected.db_name);
									} else
								 		printf(">");
								};

[\\][c]							return CONNECT;
[\\][c]+[^ ^\n]*		{invalidCommand(yytext);};
[\\][c]+[ ]*[\n]		{invalidCommand("\\c <enter>'. Expecting '\\c DBNAME"); return 0;};
[\\][l]							return LIST_DBASES;
[\\][l]+[^\n]*			{invalidCommand(yytext);};
[\\][q]							return QUIT;
[\\][q]+[^\n]*			{invalidCommand(yytext);};
[\\][d]+						return LIST_TABLES;
[\\][d]+[^\n]*			{invalidCommand(yytext);};
[\\][t]+						return LIST_TABLE;
[\\][t]+[^ ^\n]*		{invalidCommand(yytext);};
[\\][t]+[ ]*[\n]		{invalidCommand("\\t <enter>'. Expecting '\\t TABLENAME"); return 0;};
[ \t]+							/* ignore whitespace */;
%%
