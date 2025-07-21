#define FEXPRESSAO 1

#define LOGICO 1
#define RELACIONAL 2
#define SINAL 3
#define ARITIMETICO 4
#define ABRE_PARENT 5
#define FECHA_PARENT 6
#define STRING 7
#define OBJETO 8
#define VALUE_NUMBER 9
#define BOOLEANO 10

char precedArit(int id);
int operador(int id);
void *converter(char tipo,char valor[]);
void substitui(Lista *l,Lista *t);
Lista *resArit(Lista *l,Lista *t);
inf_where *opArit(Lista *l,Lista *t);
void aritPosfixa(Lista *l,Lista *t,Lista *novaExp);
column *buscaColuna(Lista *t,char *str);
Lista *relacoes(Lista *l);
char logPosfixa(Lista *l);
char opLog(Lista *l);
char precedLog(char *);
