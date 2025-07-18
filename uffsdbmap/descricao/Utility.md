Arquivo responsável por implementar:

- **Pilha**;
- **[[#Lista]]** duplamente encadeada.

---

# Lista

A **Lista** é uma estrutura duplamente encadeada que contém:

- **Tamanho** (`int`).
- Ponteiros para o **primeiro** e **último nodo** (**[[#Nodo]]**).
- Ponteiro para uma função de **comparação** (`int (*cmp)(void *inf1, void *inf2)`).

```C
typedef struct Lista {
	int tam; 
	Nodo *prim,
	*ult; int (*cmp)(void *inf1, void *inf2);
} Lista;
```
---

## **rmvNodoPtr**

Função para remover um [[#Nodo|nó]] de uma [[Utility#Lista|Lista]] duplamente encadeada.  
Recebe como parâmetros a lista e o nó a ser removido, retornando o ponteiro para a informação armazenada no nó.

### **Lógica da Função:**

1. Verifica se o nó é o **primeiro** ou **último** para ajustar os ponteiros da lista.
2. Caso contrário, ajusta os ponteiros do nó anterior e do próximo.
3. Libera o nó da memória e reduz o tamanho da lista.

```C
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
```
---
# Nodo

Estrutura base da **Lista** e da **Pilha**. Cada `Nodo` contém:

- Um ponteiro `void *inf` para armazenar a informação.
- Ponteiros para o **nodo anterior** e o **próximo**.

```C
typedef struct Nodo {
    void *inf;
    struct Nodo *ant, *prox;
} Nodo;
```