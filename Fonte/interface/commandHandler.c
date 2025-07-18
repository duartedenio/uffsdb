#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

#ifndef FCOMMANDHANDLER
   #include "commandHandler.h"
#endif

char *comandos[] = {"select", "from", "where", "insert into", "values", "drop", "create", "table", "database", "index on", "int", "varchar", "char", NULL};


// Função que retorna uma palavra que casa com o que foi digitado
char *complete(const char *text, int state) {
    static int i;
    char *nome;

    if (state == 0) i = 0;  // Reinicia a busca

    while ((nome = comandos[i++])) {
        if (strncmp(nome, text, strlen(text)) == 0)
            return strdup(nome);
    }

    return NULL;
}

// Função que retorna uma lista de possíveis completações
char **autocomplete(const char *text, int start, int end) {
    (void)start; (void)end; // Evita warnings

    return rl_completion_matches(text, complete);
}

void getComando(char * input){
    add_history(input);
    
    size_t len = strlen(input);
    char *buffer = malloc(len + 2);  // +2 para '\n' e '\0'
    
    strcpy(buffer, input);
    buffer[len] = '\n';  // Adiciona quebra de linha
    buffer[len + 1] = '\0';  // Null-terminator

    yyin = fmemopen(buffer, len + 1, "r");
    yyparse();

    free(buffer);
    fclose(yyin);
}

void historyInit(){
    rl_attempted_completion_function = autocomplete; 
    using_history();
    
    FILE *f = fopen("data/history.txt", "r");
    if (!f) return ;

    fclose(f);
    read_history("data/history.txt"); 
}