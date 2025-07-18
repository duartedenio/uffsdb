O Yacc.y possui todas as produções possiveis a partir da produção start.


# INSERT

O insert tem a seguinte estrutura:

```c
insert: INSERT INTO {setMode(OP_INSERT);} table opt_column_list VALUES
	parentesis_open value_list parentesis_close semicolon {
	
if (GLOBAL_PARSER.col_count == GLOBAL_PARSER.val_count || GLOBAL_DATA.columnName == NULL)
	GLOBAL_DATA.N = GLOBAL_PARSER.val_count;
else {
	printf("ERROR: The column counter doesn't match the value counter.\n");
	GLOBAL_PARSER.noerror=0;
}
return 0;
};

table: OBJECT {setObjName(yytext);};

opt_column_list: /*optional*/ | parentesis_open column_list parentesis_close;

column_list: column | column ',' column_list;

column: OBJECT {setColumnInsert(yytext);};

value_list: value | value ',' value_list;

value: VALUE {setValueInsert(yylval.strval, 'D');}
	| NUMBER {setValueInsert(yylval.strval, 'I');}
	| STRING {setValueInsert(yylval.strval, 'S');};
	
semicolon: ';';
```

podemos ver que inicia na [[#Produção Insert]] e ela deriva as produções: [[#Table|table]], 
[[#opt_column_list]] e [[#value_list]] . 

### Produção Insert

 A produção **insert**, quando reconhece o "INSERT INTO", chama o [[Parser#setMode]] para configurar o modo como **insert**. Em seguida deriva as produções [[#Table]] e [[#opt_column_list]]. Somente após reconhecer o token VALUES, inicia a definição definir os valores inseridos por meio do [[#value_list]]. 

Após finalizar o **value_list** e reconhecer o **;**, o código realiza a comparação se a quantidade de colunas definidas (**GLOBAL_PARSER.col_count**) é igual a quantidade de valores (**GLOBAL_PARSER.val_count**) ou se as colunas não foram definidas. 

- **Caso verdadeiro**:  Salva a quantidade de valores no GLOBAL_PARSER.N; 
- **Se falso:** Imprime uma mensagem de erro e definir a flag **noerror** do GLOBAL_PARSER como 0.

```C
insert: INSERT INTO {setMode(OP_INSERT);} table opt_column_list VALUES
	parentesis_open value_list parentesis_close semicolon {
	
if (GLOBAL_PARSER.col_count == GLOBAL_PARSER.val_count || GLOBAL_DATA.columnName == NULL)
	GLOBAL_DATA.N = GLOBAL_PARSER.val_count;
else {
	printf("ERROR: The column counter doesn't match the value counter.\n");
	GLOBAL_PARSER.noerror=0;
}
```

### Table
A produção table espera um token "OBJECT" e chama a setObjName com o valor desse OBJECT

```C
table: OBJECT {setObjName(yytext);};
```
### opt_column_list

A produção opt_column_list define um nada (vazio) ou uma lista de colunas entre parentes. Essa lista de valores tem que ter pelo menos uma coluna, e a cada coluna inserida ele chama a função [[Parser#setColumnInsert]] com o nome da coluna.

```C
opt_column_list: /*optional*/ | parentesis_open column_list parentesis_close;

column_list: column | column ',' column_list;

column: OBJECT {setColumnInsert(yytext);};
```

### value_list
A produção value_list define como pelo menos uma produção value obrigatória (não tem como inserir numa tabela sem valor), e os valores serão separados com virgulas. 

A value vai reconhecer o tipo que está sendo inserido e chamar a função [[Parser#setValueInsert|setValueInsert]]
com o valor e o tipo inserido.

```C

value_list: value | value ',' value_list;

value: VALUE {setValueInsert(yylval.strval, 'D');}
	| NUMBER {setValueInsert(yylval.strval, 'I');}
	| STRING {setValueInsert(yylval.strval, 'S');};
	
```