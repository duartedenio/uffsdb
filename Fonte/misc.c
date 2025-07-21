#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
////
#ifndef FMACROS // garante que macros.h não seja reincluída
   #include "macros.h"
#endif
///
#ifndef FTYPES // garante que types.h não seja reincluída
  #include "types.h"
#endif

#ifndef FBUFFER // garante que buffer.h não seja reincluída
  #include "buffer.h"
#endif

#ifndef FDICTIONARY // the same
  #include "dictionary.h"
#endif

int cabecalho(tp_table *s, int num_reg) {
    int count, aux=0;

    for(count = 0; count < num_reg; count++) {
        cria_campo(s[count].tam, 1, s[count].nome, 0); // O segundo parâmetro significa se = 1 Cabecalho se = 0 é valor daquele campo
        aux += s[count].tam;
    }
    printf("\n");
    return aux;
}
///////
int drawline(tp_buffer *buffpoll, tp_table *s, struct fs_objects objeto, int p, int num_page) {

    if (num_page > PAGES || p > SIZE) {
        return ERRO_DE_PARAMETRO;
    }
    int *pos_ini, aux = (p * tamTupla(s,objeto)) , num_reg = objeto.qtdCampos;
    pos_ini = &aux;
    int count, pos_aux, bit_pos;
    union c_double cd;
    union c_int ci;
    int x = 0;

    count = pos_aux = bit_pos = 0;

    for(count = 0; count < num_reg; count++) {
        pos_aux = *(pos_ini);
        bit_pos = 0;

        switch(s[count].tipo) {
            case 'S':
                x = 0;
                while(buffpoll[num_page].data[pos_aux] != '\0'){

                    printf("%c", buffpoll[num_page].data[pos_aux]);
                    if ((buffpoll[num_page].data[pos_aux++] & 0xc0) != 0x80) bit_pos++; //Conta apenas bits que possam ser impressos (UTF8)
                x++;
                }

                cria_campo((TAMANHO_NOME_CAMPO - (bit_pos)), 0, (char*)' ', (30 - x));
                break;

            case 'I':
                while(pos_aux < *(pos_ini) + s[count].tam){
                    ci.cnum[bit_pos++] = buffpoll[num_page].data[pos_aux++];
                }
                printf("%d", ci.num); //Controla o número de casas até a centena
                cria_campo((TAMANHO_NOME_CAMPO - (bit_pos)), 0, (char*)' ', 28);
                break;

            case 'D':
                while(pos_aux < *(pos_ini) + s[count].tam){
                    cd.double_cnum[bit_pos++] = buffpoll[num_page].data[pos_aux++]; // Cópias os bytes do double para área de memória da union
                }
                printf("%.3lf", cd.dnum);
                cria_campo((TAMANHO_NOME_CAMPO - (bit_pos)), 0, (char*)' ', 24);
                break;

            case 'C':
                printf("%c", buffpoll[num_page].data[pos_aux]);
                if(s[count].tam < strlen(s[count].nome)){
                    bit_pos = strlen(s[count].nome);
                }
                else{
                    bit_pos = s[count].tam;
                }
                cria_campo((bit_pos - 1), 0, (char*)' ', 29);
                break;

            default:
                return ERRO_IMPRESSAO;
                break;
        }
        *(pos_ini) += s[count].tam;
    }
    printf("\n");
    return SUCCESS;
}
////
void contr() {
	printf("Who is helping to developing uffsdb:\n");
	printf("\t Gabrielle Almeida de Souza e Lais Borin (14-1)\n");
	printf("\t Natan J. Mai, Ricardo Zanuzzo, Rogerio T. Schmidt (14-2)\n");
	printf("\t Igor Beilner, Eliton Traverssini, Régis T. Feyh (15-1)\n");
    printf("\t Matheus A. V. Dall'Rosa (16-1)\n");
	printf("\t Jefferson Coppini, Jonathan Rauber, Nicholas Brutti, Ricardo Müller (16-2)\n\n");
}
////
void help() {
	printf("Você está utilizando o DBMS-Interface\n");
	printf("-------------------------- Comandos gerais --------------------------\n");
	printf("Digite:");
	printf("\t\\c NOME\t\t- Para conectar-se no banco NAME\n");
	printf("\t\\q\t\t- Para sair do programa\n");
	printf("----------------- Comandos válidos quando conectado -----------------\n");
	printf("Digite:");
	printf("\t\\d\t\t- Para listar as tabelas do banco\n");
	printf("\t\\l\t\t- Para listar os bancos de dados existentes\n");
	printf("\t\\t NOME\t\t- Para listar os atributos de uma tabela\n\n");
	printf("------------------------------------------------ Sintaxe SQL ------------------------------------------------\n");
	printf("CREATE DATABASE NOME;\t\t\t\t- Cria um novo banco de dados\n");
	printf("\nDROP DATABASE NOME;\t\t\t\t- Exclui um banco de dados\n");
	printf("\nCREATE TABLE NOME (\t\t\t\t- Cria uma nova tabela. Onde 'type' deve\n");
	printf("\tcolumn1 type [attribute][,\t\t  pertencer à {int, integer, double, varchar(SIZE), char}.\n");
	printf("\t...][,\t\t\t\t\t  E 'attribute' pode ser 'PRIMARY KEY', caso seja PK,\n");
	printf("\tcolumnN type [attribute]]\t\t  ou 'REFERENCES NOME(COLUMN)', indicando a tabela\n");
	printf(");\t\t\t\t\t\t  e a coluna que está referenciando (FK).\n");
	printf("\nDROP TABLE NOME;\t\t\t\t- Exclui uma tabela do banco de dados\n");
	printf("\nINSERT INTO NOME [(column1, ..., columnN)]\t- Insere um novo registro na tabela, padrão SQL mas só\n");
	printf("VALUES (1, 'text value', 1.11);\t\t\t  aceita um conjunto de valores. Se as colunas não forem\n");
	printf("\t\t\t\t\t\t  especificadas, a quantidade de valores deve\n");
	printf("\t\t\t\t\t\t  corresponder à quantidade de colunas da tabela.\n");
	printf("\nSELECT * FROM NOME;\t\t\t\t- Não é a implementação padrão do SELECT mas serve para\n");
	printf("\t\t\t\t\t\t  imprmir todos os valores de uma tabela\n");
  printf("\nCREATE INDEX ON tabela (atributo);\t\t- Cria um índice B+ para o atributo especificado.\n");
}
////
int objcmp(char *obj, char *str) {
	int i;
  char a,b;
  for(i = 0; obj[i] && str[i]; i++){
    a = tolower(obj[i]), b = tolower(str[i]);
    if(a != b) return a-b;
  }
	return tolower(obj[i])-tolower(str[i]);
}

void strcpylower(char *dest, char *src) {
	int i;
  for(i = 0; src[i]; i++) dest[i] = tolower(src[i]);
  dest[i] = '\0';
}

void strncpylower(char *dest, char *src, int length) {
	int i;
	for(i = 0; i < length && src[i]; i++)
    dest[i] = tolower(src[i]);
  dest[i] = '\0';
}
///
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Verifica se o nome da tabela 'nomeTabela' está nos primeiros bytes de 'linha'
    Parametros: Nome da tabela, char linha.
    Retorno:    INT(1 - Está contido, 0 - Não está)
   ---------------------------------------------------------------------------------------------*/

int TrocaArquivosObj(char *nomeTabela, char *linha){
    int x = 0,r = 0;
    char *tabela = malloc(sizeof(char) * TAMANHO_NOME_TABELA);

    while(x < TAMANHO_NOME_TABELA){
        tabela[x] = linha[x];
        x++;
    }

    if(objcmp(tabela, nomeTabela) == 0) r = 1;

    free(tabela);
    tabela = NULL;
    return r;
}
/////
int pot10(int n) {
    if(n == 0)
        return 1;
    return 10 * pot10(n-1);
}

int strtam(char n[]) {
    if(n[0]==0)
        return 0;
    return 1+strtam(n+1);
}

int convertI(char u[]) {
    if(strtam(u) == 0)
        return 0;
    return (u[0]-48)*pot10(strtam(u)-1) + convertI(u+1);
}

double get_decimal(char u[]) {
    double decimal=0;
    int i,tamanho = strtam(u);
    for(i=0;i<tamanho && u[i]!='.';i++); // posiciona o indice i encima do ponto
    if (u[i] != '.') return 0;
    decimal=convertI(u+i+1);///(pot10(tamanho-i-1));
    decimal=(double)convertI(u+i+1)/(double)(pot10(tamanho-i-1));
    return decimal;
}

double get_inteiro(char v[]) {
    double inteiro=0;
    int i,tamanho = strtam(v);
    char u[10];
    strcpy(u,v); //copia o vetor para uma vaiável auxiliar para preservar sua originalidade
    for(i=0;i<tamanho && u[i]!='.';i++); // posiciona o indice i encima do ponto
    u[i]=0; // separa a parte inteira da parte decimal, inserindo um null no lugar do ponto
    inteiro=convertI(u);
    return inteiro;
}

double convertD(char u[]) {
    return get_inteiro(u)+get_decimal(u);
    //Soma inteiro com decimal.ss
}
///
void clear() {
	system("clear");
}
