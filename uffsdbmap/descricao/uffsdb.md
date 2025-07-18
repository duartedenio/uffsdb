#main

O `uffsdb` é a raiz do programa. Contém a função `main` e inicia o tipo [[types#Struct db_connected|db_connected]] responsável por gerenciar conexões.

---
# Fluxo

1. Verifica se a pasta `data` existe. Caso contrário, cria a pasta e a database uffs.
2.  Exibe a mensagem de inicio:
```Textplain
uffsdb (16.2).
Type "help" for help.
```
3. Inicia a interface de comandos.

---
# Dependências

- Função Dependente: `Não possui`.
- Funções Auxiliares:  
	-  [[database#dbInit|dbinit]];
	- [[Parser#Interface|interface]].
- Types:
	- [[types#db_connected|db_connected]]

---
# Implementação

A implementação está em [[main]].
 