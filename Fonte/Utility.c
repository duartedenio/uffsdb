#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef FUTILITY
   #include "Utility.h"
#endif

Nodo *novoNodo(void *inf){
  Nodo *novo = malloc(sizeof(Nodo));
  novo->inf = inf;
  novo->ant = novo->prox = NULL;
  return novo;
}

Lista *novaLista(int (*cmp)(void *inf1,void *inf2)){
  Lista *l = malloc(sizeof(Lista));
  if(!l){
    printf("Impossivel alocar lista!\n");
    return NULL;
  }
  l->tam = 0;
  l->cmp = cmp;
  l->prim = l->ult = NULL;
  return l;
}

void adcNodo(Lista *l,Nodo *i,void *inf){
  Nodo *novo = novoNodo(inf);
  if(i){
    if(i == l->ult){
      novo->ant = l->ult;
      l->ult->prox = novo;
      l->ult = novo;
    }
    else{
      novo->ant = i;
      novo->prox = i->prox;
      novo->prox->ant = novo;
      i->prox = novo;
    }
  }
  else if(l->prim){
    novo->prox = l->prim;
    if(l->prim) l->prim->ant = novo;
    l->prim = novo;
  }
  else l->prim = l->ult = novo;
  l->tam++;
}

void *rmvNodoPtr(Lista *l,Nodo *i){
  if(i == l->prim){
    l->prim = i->prox;
    if(l->prim) l->prim->ant = NULL;
  }
  else if(i == l->ult){
    l->ult = i->ant;
    if(l->ult) l->ult->prox = NULL;
  }
  else{
    i->ant->prox = i->prox;
    i->prox->ant = i->ant;
  }
  void *rinf = i->inf;
  free(i); i = NULL;
  l->tam--;
  return rinf;
}

Nodo *busca(Lista *l,void *inf){
  Nodo *i;
  for(i = l->prim; i; i = i->prox)
    if(!l->cmp(i->inf,inf)) return i;
  return NULL;
}

Pilha *novaPilha(){
  Pilha *p = malloc(sizeof(Pilha));
  p->tam = 0;
  p->topo = NULL;
  return p;
}

void push(Pilha *p,void *inf){
  Nodo *novo = novoNodo(inf);
  if(p->topo){
    p->topo->ant = novo;
    novo->prox = p->topo;
  }
  p->topo = novo;
  p->tam++;
}

void *pop(Pilha *p){
  Nodo *k = p->topo;
  void *inf = k->inf;
  p->topo = k->prox;
  if(p->topo) p->topo->ant = NULL;
  p->tam--;
  free(k);
  return inf;
}

void *topo(Pilha *p){
  return p->topo->inf;
}
