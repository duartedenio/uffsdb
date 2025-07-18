O database é responsável pela criação, remoção de database e a função para conectar a database. Esse arquivo utliza o data/BD.dat; o DB.dat guarda as informações da database no DB informações estas contidas no [[types#Struct data_base|Struct data_base]].

---
# createDB

A função `createDB` cria uma nova database no arquivo `data/DB.dat`. 

---
## Assinatura

```C
void createDB(char *db_name)
```
### Parâmetro(s) / Retorno

db_name = uma string para criar o banco.

Retorna void.

----
## Fluxo

1. **Abertura ou criação do arquivo**: Garante que o arquivo `data/DB.dat` exista para leitura e escrita.
2. **Validação do nome da database**: Trunca nomes longos e verifica duplicidade.
3. **Limite de databases**: Garante que não ultrapasse o número máximo permitido.
4. **Escrita da nova database**: Salva as informações no arquivo e cria um diretório para a database.


---
## Dependências

- Função Dependente: [[#dbInit]].
- Funções Auxiliares:
	- [[misc#strcpylower|strcpylower]]
	- [[misc#objcmp|objcpm]]
- Types:
	 - [[types#data_base|data_base]]
---
## Implementação

A implementação está em [[createDB]].
 

---
# dbInit

Iniciar o ambiente, criando a pasta data e a database com o nome informado ou uffsdb.

---
## Assinatura
#erro
```C
void dbInit(char *db)
```

Obs: essa assinatura não existe no database.h
### Parâmetro(s) / Retorno

db = uma string para criar o banco.

Retorna void.

----
## Fluxo

1. **Cria o diretório `data`**;
2. **Chama a função [[#createDB]]:** Cria a database com o nome passado.

---
## Dependências

- Função Dependente: [[uffsdb|main]].
- Função Auxiliar:  [[#createDB]].

---
## Implementação

A implementação está em [[dbInit]].
 