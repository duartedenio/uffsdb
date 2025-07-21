#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef FMACROS
   #include "macros.h"
#endif

#ifndef FTYPES
  #include "types.h"
#endif

#ifndef FEXPRESSAO
  #include "Expressao.h"
#endif

#ifndef FTYPES
  #include "types.h"
#endif

#ifndef FUTILITY
  #include "Utility.h"
#endif

#ifndef FSQLCOMMANDS
  #include "sqlcommands.h"
#endif

int operador(int id){
  return (id == SINAL) || (id == ARITIMETICO);
}

char precedArit(int id){
  return (id == ABRE_PARENT) ? 0 : id;
}

void limparExpNodos(Lista *l){
  for(Nodo *k = l->prim,*j; k; k = j){
    j = k->prox;
    rmvNodoPtr(l,k);
  }
  l->prim = l->ult = NULL;
}

void limparExpNodosInf(Lista *l){
  for(Nodo *k = l->prim,*j; k; k = j){
    inf_where *kw = (inf_where *)(k->inf);
    free(kw->token); kw->token = NULL;
    free(kw); kw = NULL;
    j = k->prox;
    rmvNodoPtr(l,k);
  }
  l->prim = l->ult = NULL;
}

char buscaTipoObjecto(inf_where *iw,Lista *t){
  char tipo = iw->id;
  if(iw->id == OBJETO){
    char c = buscaColuna(t,(char *)iw->token)->tipoCampo;
    tipo = (c == 'S' || c == 'C') ? STRING : VALUE_NUMBER;
  }
  return tipo;
}

char validaExp(Lista *l,Lista *t){
  for(Nodo *i = l->prim; i; i = i->prox){
    inf_where *iw = (inf_where *)(i->inf);
    char tipo1 = buscaTipoObjecto(iw,t);
    if(tipo1 != STRING && tipo1 != VALUE_NUMBER) continue;
    for(Nodo *j = i->prox; j; j = j->prox){
      char tipo2 = buscaTipoObjecto((inf_where *)(j->inf),t);
      if(tipo2 == LOGICO) break;
      if((tipo2 == STRING && tipo1 == VALUE_NUMBER) ||
          (tipo2 == VALUE_NUMBER && tipo1 == STRING) ) return 0;
    }
  }
  return 1;
}

Lista *resArit(Lista *l,Lista *t){
  if(!validaExp(l,t)){
    printf("Operação entre tipos diferentes inválida.\n");
    return NULL;
  }
  Lista *op = novaLista(NULL),
        *novaExp = novaLista(NULL);
  for(Nodo *i = l->prim; i; i = i->prox){
    inf_where *iw = (inf_where *)(i->inf);
    if(iw->id == LOGICO || iw->id == RELACIONAL){
      aritPosfixa(op,t,novaExp);
      limparExpNodos(op);
      adcNodo(novaExp,novaExp->ult,
                      (void *)novoTokenWhere(iw->token,iw->id));
    }
    else adcNodo(op,op->ult,i->inf);
  }
  aritPosfixa(op,t,novaExp);
  limparExpNodos(op);
  free(op); op = NULL;
  return novaExp;
}

void aritPosfixa(Lista *l,Lista *t,Lista *novaExp){
  int fechap = 0;
  Pilha *p = novaPilha();
  Lista *pos = novaLista(NULL);
  for(Nodo *i = l->prim; i; i = i->prox){
    inf_where *iw = (inf_where *)(i->inf);
    if(iw->id == STRING || iw->id == OBJETO || iw->id == VALUE_NUMBER )
      adcNodo(pos,pos->ult,(void *)novoTokenWhere(iw->token,iw->id));
    else if(iw->id == ABRE_PARENT){
      push(p,i->inf);
      adcNodo(novaExp,novaExp->ult,(void *)novoTokenWhere(iw->token,iw->id));
    }
    else if(operador(iw->id)){
      while(p->tam && precedArit(iw->id) <= precedArit(((inf_where *)topo(p))->id)){
        inf_where *k = (inf_where *)pop(p);
        adcNodo(pos,pos->ult,(void *)novoTokenWhere(k->token,k->id));
      }
      push(p,i->inf);
    }
    else{//caso de entrada: ')'
      fechap++;
      while(p->tam && (iw = ((inf_where *)pop(p)))->id != ABRE_PARENT)
        adcNodo(pos,pos->ult,(void *)novoTokenWhere(iw->token,iw->id));
    }
  }
  while(p->tam){
    inf_where *iw = (inf_where *)pop(p);
    if(iw->id != ABRE_PARENT) adcNodo(pos,pos->ult,
                                      (void *)novoTokenWhere(iw->token,iw->id));
  }
  inf_where *iw = opArit(pos,t);
  adcNodo(novaExp,novaExp->ult,(void *)novoResWhere(iw->token,iw->id));

  while(fechap--) adcNodo(novaExp,novaExp->ult,
                          (void *)novoTokenWhere(")",FECHA_PARENT));
  free(p); p = NULL;
  limparExpNodosInf(pos);
  free(pos); pos = NULL;
}

inf_where *opArit(Lista *l,Lista *t){
  substitui(l,t);
  if(l->tam == 1) return (inf_where *)(l->prim->inf);
  Pilha *p = novaPilha();
  for(Nodo *i = l->prim; i; i = i->prox){
    inf_where *iw = (inf_where *)(i->inf);
    if(iw->id == VALUE_NUMBER) push(p,(void *)iw);
    else{
      inf_where *op1 = (inf_where *)pop(p);
      char *oper = (char *)(iw->token);
      if(!p->tam){
          if(oper[0] == '-') *((double *)(op1->token)) *= -1;
          push(p,(void *)op1);
          continue;
      }
      inf_where *op2 = (inf_where *)topo(p);
      if(oper[0] == '*')
        *((double *)(op2->token)) *= *((double *)(op1->token));
      else if(oper[0] == '/')
        *((double *)(op2->token)) /= *((double *)(op1->token));
      else if(oper[0] == '-')
        *((double *)(op2->token)) -= *((double *)(op1->token));
      else if(oper[0] == '+')
        *((double *)(op2->token)) += *((double *)(op1->token));
      else if(oper[0] == '%'){
        int a = (int)(*((double *)(op2->token)));
        int b = (int)(*((double *)(op1->token)));
        *((double *)(op2->token)) = a % b;
      }
    }
  }
  inf_where *iw = pop(p);
  free(p); p = NULL;
  return iw;
}

void substitui(Lista *l,Lista *t){
  for(Nodo *i = l->prim; i; i = i->prox){
    inf_where *iw = (inf_where *)(i->inf);
    //substituindo as colunas do where pelo seu valor na tupla
    if(iw->id == OBJETO){
      column *c = buscaColuna(t,(char *)(iw->token));
      free(iw->token);
      iw->token = converter(c->tipoCampo,c->valorCampo);
      if(c->tipoCampo == 'S' || c->tipoCampo == 'C') iw->id = STRING;
      else if(c->tipoCampo == 'I' || c->tipoCampo == 'D') iw->id = VALUE_NUMBER;
    }
    else if(iw->id == VALUE_NUMBER){//todo inteiro vai pra double.
      void *tk = iw->token;
      double *num = malloc(sizeof(double));
      *num = atof((char *)(iw->token));
      iw->token = (void *)num;
      free(tk); tk = NULL;
    }
  }
}

column *buscaColuna(Lista *t,char *str){
  for(Nodo *i = t->prim; i; i = i->prox){
    column *ic = (column *)(i->inf);
    if(strcmp(ic->nomeCampo,str) == 0) return ic;
  }
  return NULL;//nao pode chegar aqui pq a query já foi validada.
}

void *converter(char tipo,char valor[]){
  if(tipo == 'S' || tipo == 'C'){
    char *str = malloc(sizeof(char)*strlen(valor)+3);
    str[0] = '\0'; //não retire.
    strcat(str,"'");
    strcat(str,valor);
    strcat(str,"'");
    return (void *)str;
  }
  double *num = malloc(sizeof(double));
  if(tipo == 'I') *num = (*((int *)valor))*1.0;
  else num = (double *)valor;
  return (void *)num;
}

//daqui pra baixo relações
Lista *relacoes(Lista *l){
  Pilha *p = novaPilha();
  Lista *r = novaLista(NULL);
  for(Nodo *i = l->prim; i; i = i->prox){
    inf_where *iw = (inf_where *)(i->inf);
    if(iw->id == ABRE_PARENT || iw->id == LOGICO)
      adcNodo(r,r->ult,(void *)novoResWhere(iw->token,iw->id));
    else if(iw->id == FECHA_PARENT){
      if(((inf_where *)(r->ult->inf))->id != ABRE_PARENT)
        adcNodo(r,r->ult,(void *)novoResWhere(iw->token,iw->id));
      else{
        inf_where *rw = (inf_where *)r->ult;
        free(rw->token); rw->token = NULL;
        free(rw); rw = NULL;
        rmvNodoPtr(r,r->ult);
      }
    }
    else if((iw->id == VALUE_NUMBER || iw->id == STRING)
            && p->tam == 2){
      inf_where *oper = pop(p);
      inf_where *op = pop(p);
      char val = 0;
      char *oper2 = (char *)(oper->token);
      if(!strcmp(oper2,">")){
        if(op->id == STRING)
          val = strcmp((char *)(op->token), (char *)(iw->token)) > 0;
        else val = (*(double *)(op->token) > *(double *)(iw->token));
      }
      else if(!strcmp(oper2,">=")){
        if(op->id == STRING)
          val = strcmp((char *)(op->token), (char *)(iw->token)) >= 0;
        else val = (*(double *)(op->token) >= *(double *)(iw->token));
      }
      else if(!strcmp(oper2,"<")){
        if(op->id == STRING)
          val = strcmp((char *)(op->token), (char *)(iw->token)) < 0;
        else val = (*(double *)(op->token) < *(double *)(iw->token));
      }
      else if(!strcmp(oper2,"<=")){
        if(op->id == STRING)
          val = strcmp((char *)(op->token), (char *)(iw->token)) <= 0;
        else val = (*(double *)(op->token) <= *(double *)(iw->token));
      }
      else if(!strcmp(oper2,"=")){
        if(op->id == STRING)
          val = !strcmp((char *)(op->token), (char *)(iw->token));
        else val = (*(double *)(op->token) == *(double *)(iw->token));
      }
      else if(!strcmp(oper2,"!=")){
        if(op->id == STRING)
            val = strcmp((char *)(op->token), (char *)(iw->token));
        else val = (*(double *)(op->token) != *(double *)(iw->token));
      }
      adcNodo(r,r->ult,(void *)novoResWhere(&val,BOOLEANO));
    }
    else push(p,(void *)iw);
  }
  free(p); p = NULL;
  return r;
}

//daqui pra baixo operações lógicas
char precedLog(char *op){
  return (tolower(op[0]) == 'a');
}

char logPosfixa(Lista *l){
  Pilha *p = novaPilha();
  Lista *pos = novaLista(NULL);
  for(Nodo *i = l->prim; i; i = i->prox){
    inf_where *iw = (inf_where *)(i->inf);
    if(iw->id == BOOLEANO) adcNodo(pos,pos->ult,i->inf);
    else if(iw->id == ABRE_PARENT) push(p,i->inf);
    else if(iw->id == LOGICO){
      inf_where *iw2;
      while(p->tam && (iw2 = (inf_where *)topo(p))->id != ABRE_PARENT
            && precedLog(iw2->token) >= precedLog(iw->token))
        adcNodo(pos,pos->ult,pop(p));
      push(p,i->inf);
    }
    else//se iw->id == FECHA_PARENT
      while(p->tam && (iw = (inf_where *)pop(p))->id != ABRE_PARENT)
        adcNodo(pos,pos->ult,(void *)iw);
  }
  while(p->tam) adcNodo(pos,pos->ult,pop(p));
  free(p); p = NULL;
  char r = opLog(pos);
  limparExpNodos(pos);
  free(pos); pos = NULL;
  return r;
}

char opLog(Lista *l){
  Pilha *p = novaPilha();
  for(Nodo *i = l->prim; i; i = i->prox){
    inf_where *iw = (inf_where *)(i->inf);
    if(iw->id == BOOLEANO) push(p,i->inf);
    else{
      inf_where *v1 = (inf_where *)pop(p);
      char *dest = (char *)((inf_where *)topo(p))->token;
      if(precedLog(iw->token)) *dest &= *((char *)(v1->token));
      else *dest |= *((char *)(v1->token));
    }
  }
  char r = *((char *)(((inf_where *)pop(p))->token ));
  free(p); p = NULL;
  return r;
}
