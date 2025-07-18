O types tem todos os tipos definidos no sistema, sendo eles: 
- [[#db_connected]] usada para gerenciar informações da conexão;
- [[#data_base]] usada para gerenciar as databases.
- [[#rc_parser]] auxilia no funcionamento do parser.
- [[#inf_select]] registra informações do select.
- [[#rc_insert]] armazena dados dos tokens.


# Structs
## db_connected
#main

Utilizado no sistema para gerenciar informações da conexão ativa.
### Atributos
- **`db_directory`**: String com tamanho definido em [[Macros#Tamanhos]] que armazena o diretório.
- **`db_name`**: Ponteiro para uma string que armazena o nome do banco de dados.
- **`conn_active`**: Inteiro que indica se o usuário é superusuário (`#`) ou não (`<`).
 
```C
typedef struct db_connected {
	char db_directory[LEN_DB_NAME_IO];
	char *db_name;
	int conn_active;
}db_connected;
```

LEN_DB_NAME_IO é definido como [[Macros#Tamanhos|40]].
___
## data_base

Utilizada para trabalhar com database. 
### Atributos
* `valid`: char para saber se é valido 1 ou 0.
* `db_name`: string com tamanho definido em [[Macros#Tamanhos]] que armazena o diretório.
* **`db_directory`**: String com tamanho definido em [[Macros#Tamanhos]] que armazena o diretório.

```C
typedef struct data_base{
	char valid;
	char db_name[LEN_DB_NAME_IO];
	char db_directory[LEN_DB_NAME_IO];
}data_base;
```
___

## rc_insert

Guarda as informações obtidas nos tokens pelo parser.

```C
typedef struct rc_insert {
	char *objName; // Nome do objeto (tabela, banco de dados, etc...)
	char **columnName; // Colunas da tabela
	char **values; // Valores da inserção ou tamanho das strings na criação
	int N; // Número de colunas de valores
	char *type; // Tipo do dado da inserção ou criação de tabela
	int *attribute; // Utilizado na criação (NPK, PK,FK)
	char **fkTable; // Recebe o nome da tabela FK
	char **fkColumn; // Recebe o nome da coluna FK
}rc_insert;
```
## rc_parser

Auxilia o parser na verificação e validação do comando.

```c
typedef struct rc_parser {
	int mode; // Modo de operação (definido em /interface/parser.h)
	int parentesis; // Contador de parenteses abertos
	int step; // Passo atual (token)
	int noerror; // Nenhum erro encontrado na identificação dos tokens
	int col_count; // Contador de colunas
	int val_count; // Contador de valores
	int consoleFlag; // Auxiliar para não imprimir duas vezes nome=#
}rc_parser;
```

## inf_select

Para ajudar o select

### Atributos

* `tabela`: string com o nome da tabela.
* `tamTokens`: quantidade de tokens.
* `tok`: lista dos tokens.
* `proj`: lista da projeção.

```c
typedef struct inf_select{
char *tabela;
int tamTokens;
Lista *tok,*proj;
}inf_select;
```

