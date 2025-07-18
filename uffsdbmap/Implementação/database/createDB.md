# Abrindo o arquivo data/DB.dat

O arquivo `data/DB.dat` é aberto no modo **`a+b`** (leitura e escrita binária, com criação se não existir). Em seguida, a função verifica se o nome da database (`db_name`) excede o tamanho máximo de [[Macros#Tamanhos|20]] caracteres. Caso exceda, o nome será truncado, e um aviso será exibido.

```C
void createDB(char *db_name) {
	FILE *DB;
	int i, len;
	char vec_name[QTD_DB][LEN_DB_NAME_IO],
		vec_directory[QTD_DB][LEN_DB_NAME_IO],
		create[LEN_DB_NAME_IO] = "mkdir data/",
		*aux_name_tolower,valid;
	
	if((DB = fopen("data/DB.dat","a+b")) == NULL) {
		printf("ERROR: cannot open file\n");
		return;
	}
	if(strlen(db_name) > LEN_DB_NAME) {
		printf("WARNING: database name is too long, it will be truncated\n");
		db_name[LEN_DB_NAME] = '\0';
	}
```

# Verificando duplicidade e limite de databases 

Procura por todo o arquivo se existe uma dabase válida com o nome passado no parâmetro. Se existir a função é interrompida com uma mensagem de erro.

Também verifica se a quantidade máxima de databases foi atingida ([[Macros#Tamanhos|100]]).
```C
  
for(i=0; fgetc (DB) != EOF; i++) {
	fseek(DB, -1, 1);
	fread(&valid ,sizeof(char), 1, DB);
	fread(vec_name[i] ,sizeof(char), LEN_DB_NAME_IO, DB);
	fread(vec_directory[i] ,sizeof(char), LEN_DB_NAME_IO, DB);

	if(valid && objcmp(vec_name[i], db_name) == 0) {
		fclose(DB);
		if(objcmp(db_name, "uffsdb") != 0) // banco de dados ja existe
			printf("ERROR: database already exists\n");
		return;
	}
}

if(i >= QTD_DB) {
	fclose(DB);
	printf("ERROR: The amount of databases in this machine exceeded the limit.\n");
	return;
}
```

> **Nota**: A comparação dos nomes é feita com a função [[misc#objcmp|`objcmp`]], que realiza uma verificação **case-insensitive**.
# Salvando a database

1. Inicializa uma variável `SGBD` do tipo [[types#data_base|data_base]].
2. Define a database como válida (`1`) e converte os campos `db_name` e `db_directory` para minúsculas utilizando a função [[misc#strcpylower|strcpylower]].
3. Escreve a estrutura no arquivo `data/DB.dat`.

```C
data_base *SGBD = (data_base*)malloc(sizeof(data_base));
len = strlen(db_name);
  
SGBD->valid = 1;
strcpylower(SGBD->db_name, db_name);
strcpylower(SGBD->db_directory, db_name);
strcat(SGBD->db_directory , "/");
SGBD->db_name[len] = '\0';
SGBD->db_directory[len+1] = '\0';
fwrite(SGBD ,sizeof(data_base), 1, DB);

```
# Criando o diretório da database

- Aloca memória para uma string auxiliar (`aux_name_tolower`) e copia o nome da database para ela, convertendo-o para minúsculas com a função [[misc#strcpylower|strcpylowew]].
- Concatena o nome da database ao comando **`mkdir data/`**.
- Executa o comando utilizando a função `system`. Caso a criação do diretório falhe, uma mensagem de erro é exibida.
- Libera a memória alocada e fecha o arquivo.

```C
aux_name_tolower = (char *)malloc(sizeof(char) * (strlen(db_name)+1));
strcpylower(aux_name_tolower, db_name);
strcat(create, aux_name_tolower);
free(aux_name_tolower);

if(system(create) == -1) { //verifica se foi possivel criar o diretorio
	printf("ERROR: It was not possible to create the database\n");
}
else if(objcmp(db_name, "uffsdb") != 0) printf("CREATE DATABASE\n");
free(SGBD);
fclose(DB);
```

