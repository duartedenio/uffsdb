
Faz uma chamada para [[resetSelect]] limpando o SELECT
```c
int i;
resetSelect();
```


Verifica cada atributo da GLOBAL_DATA. Se forem diferentes de nulos, libera a memória e "seta" o valor para null

```C
if (GLOBAL_DATA.objName) {
	free(GLOBAL_DATA.objName);
	GLOBAL_DATA.objName = NULL;
}
 
for (i = 0; i < GLOBAL_DATA.N; i++ ) {
	if (GLOBAL_DATA.columnName)
		free(GLOBAL_DATA.columnName[i]);
	if (GLOBAL_DATA.values)
		free(GLOBAL_DATA.values[i]);
	if (GLOBAL_DATA.fkTable)
		free(GLOBAL_DATA.fkTable[i]);
	if (GLOBAL_DATA.fkColumn)
		free(GLOBAL_DATA.fkColumn[i]);
}

free(GLOBAL_DATA.columnName);
GLOBAL_DATA.columnName = NULL;

free(GLOBAL_DATA.values);
GLOBAL_DATA.values = NULL;

free(GLOBAL_DATA.fkTable);
GLOBAL_DATA.fkTable = NULL;

free(GLOBAL_DATA.fkColumn);
GLOBAL_DATA.fkColumn = NULL;

free(GLOBAL_DATA.type);
GLOBAL_DATA.type = (char *)malloc(sizeof(char));

free(GLOBAL_DATA.attribute);
GLOBAL_DATA.attribute = (int *)malloc(sizeof(int));
```

Destrói o lex do parser e zera todos os contadores da GLOBAL_PARSER  e GLOBAL_DATA.
```C
yylex_destroy();


GLOBAL_DATA.N = 0;
GLOBAL_PARSER.mode = 0;
GLOBAL_PARSER.parentesis = 0;
GLOBAL_PARSER.noerror = 1;
GLOBAL_PARSER.col_count = 0;
GLOBAL_PARSER.val_count = 0;
GLOBAL_PARSER.step = 0;

}
```
