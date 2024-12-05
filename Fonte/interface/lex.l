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

// Flags globais para rastrear erros e exibição de prompts
int error_shown = 0; 
int prompt_shown = 0;

// Declaração da função invalidCommand com tipo consistente
// Corrigida para `const char *` para evitar conflitos com parser.h
void invalidCommand(const char *input);
%}

%option noyywrap nounput

%%

// Regras para palavras reservadas e comandos SQL
(?i:INSERT)                     return INSERT;
(?i:INTO)                       return INTO;
(?i:VALUES)                     return VALUES;
(?i:SELECT)                     return SELECT;
(?i:FROM)                       return FROM;
(?i:WHERE)                      return WHERE;
(?i:CREATE)                     return CREATE;
(?i:TABLE)                      return TABLE;
(?i:ON)                         return ON;
(?i:INDEX)                      return INDEX;
(?i:INTEGER)                    return INTEGER;
(?i:AND)                        return LOGICO;
(?i:OR)                         return LOGICO;
(?i:INT)                        return INTEGER;
(?i:VARCHAR)                    return VARCHAR;
(?i:CHAR)                       return CHAR;
(?i:DOUBLE)                     return DOUBLE;
(?i:PRIMARY)                    return PRIMARY;
(?i:KEY)                        return KEY;
(?i:REFERENCES)                 return REFERENCES;
(?i:DATABASE)                   return DATABASE;
(?i:DROP)                       return DROP;
(?i:CONTRIBUTORS)               return CONTR;
help                            return HELP;
\\\\h                           return HELP;

// Regras para detectar comandos inválidos (corrigido escapamento)
\\\\c[^\\n]*                    { 
    if (!error_shown) {
        printf("ERROR: Comando inválido '\\c'. Use '\\c DBNAME' para conectar.\n");
        error_shown = 1;
    }
    return 0; 
}

\\\\t[^\\n]*                    { 
    if (!error_shown) {
        printf("ERROR: Comando inválido '\\t'. Use '\\t TABLENAME' para listar tabelas.\n");
        error_shown = 1;
    }
    return 0; 
}

\\\\q                           return QUIT;
\\\\q[^\\n]*                    { 
    if (!error_shown) {
        printf("ERROR: Comando inválido '\\q'.\n");
        error_shown = 1;
    }
}

\\\\d                           return LIST_TABLES;
\\\\d[^\\n]*                    { 
    if (!error_shown) {
        printf("ERROR: Comando inválido '\\d'.\n");
        error_shown = 1;
    }
}

// Regra genérica para capturar entradas inválidas
.* {
    if (!error_shown) {
        printf("ERROR: Entrada inválida detectada.\n");
        error_shown = 1;
    }
    return 0;
}

// Regras para operadores e símbolos
[,]                             return *yytext;
[;]                             return *yytext;
[-]                             return *yytext;
[+]                             return *yytext;

[)]                             {yylval.strval = yytext; return FECHA_P;}
[(]                             {yylval.strval = yytext; return ABRE_P;}
[*]                             {yylval.strval = yytext; return ASTERISCO;}
[a-zA-Z_][a-zA-Z0-9_]*          {yylval.strval = yytext; return OBJECT;}
[\']([^']|'')*[\']              {yylval.strval = yytext; return STRING;}
[0-9]+\.[0-9]+                  {yylval.strval = yytext; return VALUE;}
[0-9]+                          {yylval.strval = yytext; return NUMBER;}
[%/]                            {yylval.strval = yytext; return OPERADOR;}
([><]=|[!]=|[=><])              {yylval.strval = yytext; return RELACIONAL;}

// Prompt exibido no final da entrada
\n {
    if (!prompt_shown) {
        if (GLOBAL_PARSER.mode != 0) {
            if (GLOBAL_PARSER.parentesis)
                printf("%s(# ", connected.db_name);
            else
                printf("%s-# ", connected.db_name);
        } else if (connected.conn_active) {
            printf("%s=# ", connected.db_name);
        } else {
            printf(">");
        }
        prompt_shown = 1;
    }
}

// Ignorar espaços em branco
[ \t]+ /* ignora espaços em branco */

%%

// Definição da função invalidCommand
// Corrigida para ser consistente com a declaração em parser.h
void invalidCommand(const char *input) {
    if (!error_shown) {
        printf("ERROR: Comando não reconhecido '%s'. Digite \\h para ajuda.\n", input);
        error_shown = 1;
    }
}
