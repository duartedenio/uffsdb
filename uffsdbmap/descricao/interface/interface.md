A função **interface** fornece a interface de comandos para o usuário. 

# Assinatura
```C
int interface();
```
___
# Fluxo
1. **Inicialização do Ambiente**:
	- Cria uma thread para executar `clearGlobalStructs` e aguarda sua finalização.
	- Conecta automaticamente ao banco padrão (`uffsdb`) com a função `connect("uffsdb")`.
    - Inicializa os campos da estrutura `SELECT` (`tok` e `proj`) com `NULL`.
2. **Exibição do Prompt**:
	- Se a conexão não estiver ativa, exibe `">"`.
	- Se a conexão estiver ativa, exibe o nome do banco seguido de `=#` (ex.: `mydb=#` ).
3. **Processamento do Comando**:
	- Cria uma thread para executar `yyparse`, responsável por analisar o comando do usuário, e aguarda sua conclusão.
	- Se a análise não apresentar erros (`GLOBAL_PARSER.noerror`):
	    - Se um comando válido foi identificado:
	        - Se não houver conexão ativa, chama a função `notConnected()`.
	        - Caso contrário, executa a operação correspondente, conforme o valor de `GLOBAL_PARSER.mode`:
	            - **OP_INSERT**: Chama `insert(&GLOBAL_DATA)` se houver dados para inserir; caso contrário, exibe um aviso.
	            - **OP_SELECT**: Obtém o resultado da consulta via `op_select(&SELECT)` e, se existir, exibe-o com `printConsulta`.
	            - **OP_CREATE_TABLE**: Executa `createTable(&GLOBAL_DATA)`.
	            - **OP_CREATE_DATABASE**: Executa `createDB(GLOBAL_DATA.objName)`.
	            - **OP_DROP_TABLE**: Executa `excluirTabela(GLOBAL_DATA.objName)`.
	            - **OP_DROP_DATABASE**: Executa `dropDatabase(GLOBAL_DATA.objName)`.
	            - **OP_CREATE_INDEX**: Executa `createIndex(&GLOBAL_DATA)`.

# Dependências 

- Função Dependente: **`não possui`**
- Função Auxiliar: [[misc#strcpylower|strcpylower]].