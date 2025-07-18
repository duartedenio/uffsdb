# Criando o diretório `data`
#erro

Primeiramente, a função tenta criar o diretório `data`. O mkdir é redirecionado para o `dev/null` para que mensagens do linux não sejam exibidas. Se o mkdir, a função exibe a mensagem:

```
ERROR: It was not possible to initialize uffsdb
```

```c
if(system("mkdir data > /dev/null 2>&1") == -1)
	printf("ERROR: It was not possible to initialize uffsdb\n");
```

Obs: acredito que a função deveria encerrar quando o mkdir não funciona.

---
# Criando a pasta para a database
#erro
A função verifica o parâmetro recebido:

- **Se o parâmetro for `NULL`:** Um nome padrão `uffsdb` é criado dinamicamente para ser usado como nome do banco de dados.
- **Se o parâmetro não for `NULL`:** O nome passado como argumento é usado diretamente.

A função chama [[#createDB]] para criar o banco.
```C
if (db==NULL){
	name = malloc(sizeof(char)*7);
	name[0]='u';name[1]='f';name[2]='f';name[3]='s';
	name[4]='d';name[5]='b';name[6]=0;
} else name=db;

createDB(name);
```
Obs: Deveria liberar essa memória alocada.