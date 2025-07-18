#erro

Percorre todos os nós, removendo-os com [[Utility#rmvNodoPtr|rmvNodoPtr]], e libera a lista.

"Seta" o prim, e ult como null antes de liberar.

```C
void limparLista(Lista *l){
	Nodo *k = l->prim,*j;
	while(k){
		j = k->prox;
		free( rmvNodoPtr(l,k) );
		k = j;
	}
	l->prim = l->ult = NULL;
	free(l);
}
```

Erro: deveria estar em utility.c