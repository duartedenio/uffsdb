#erro
Antes de liberar cada atributo de **[[types#inf_select|SELECT]]**, verifica-se se ele é `null`. Para liberar listas, utiliza-se a função **[[limparLista]]**. Após liberar cada atributo, o valor é atualizado para `null`.

```C
if(SELECT.tabela){
	free(SELECT.tabela);
	SELECT.tabela = NULL;
}

if(SELECT.tok) limparLista(SELECT.tok);
SELECT.tok = NULL;

if(SELECT.proj) limparLista(SELECT.proj);
SELECT.proj = NULL;
```