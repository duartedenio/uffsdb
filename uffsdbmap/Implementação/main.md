#Implementação #main

```C
db_connected connected;

int main(){
	dbInit(NULL);
	printf("uffsdb (16.2).\nType \"help\" for help.\n\n");
	interface();
	return 0;
}
```

# Estratégias

- Inicia a variável connected do tipo [[types#Struct db_connected|db_connected]] 
- Chama a dbInit passando como parâmetro `NULL` para criar a database UFFS;
- Exibe a versão do uffsbd e o comando para ajuda;
- Interface para disponibilizar todos os comandos do yacc.