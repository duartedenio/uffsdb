#define FUTILITY 1

typedef struct Nodo{
  void *inf;
  struct Nodo *ant,*prox;
}Nodo;

/*
  Retorna o ponteiro para um nodo com void *
*/
Nodo *novoNodo(void *);

typedef struct Lista{
  int tam;
  Nodo *prim,*ult;
  int (*cmp)(void *inf1,void *inf2);
}Lista;

/*
    Cria uma nova lista com a função de comparação cmp,
  que vai ser utilizada na função rmvNode.
    Quando for remover ele precisa se utilizar da função
  rmvf para dar free nas informações do ponteiro void *inf
  da estrutura Nodo
*/
Lista *novaLista(int (*cmp)(void *,void *));

/*
  Adciona um novo nodo a Lista *l
  com a informação void *inf, a frente do nodo Nodo *i.
  se i = NULL a inserçao será feita no inicio.
  se i = l->ult a inserção será feita no fim.
*/
void adcNodo(Lista *l,Nodo *i,void *inf);

/*
  Remove o primeiro nodo com informação void *inf
  da lista List *l em O(N)
*/
void *rmvNodoInf(Lista *l,void *inf);

/*
  Remove o Nodo *i da Lista *l.
*/
void *rmvNodoPtr(Lista *l,Nodo *i);

/*
  Encontra e retorna o elemento da lista com
  informação void *inf e retorna seu ponteiro em O(N)
*/
Nodo *busca(Lista *,void *inf);

typedef struct Pilha{
  int tam;
  Nodo *topo;
}Pilha;

/*
  Retorna o ponteiro para uma nova pilha.
*/
Pilha *novaPilha();

/*
  Recebe uma pilha e um void *, cria um novo
  nodo e o coloca no topo da pilha.
*/
void push(Pilha *,void *);

/*
  Recebe uma pilha e retira seu nodo do topo.
*/
void *pop(Pilha *);

/*
  Retorna a informação do nodo no topo da pilha.
*/
void *topo(Pilha *);
