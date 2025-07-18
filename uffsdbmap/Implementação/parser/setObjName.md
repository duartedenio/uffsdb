O setObjName primeiro verifica que o mode é diferente de 0, ou seja, se é o modo não é nulo. Caso seja um modo não nulo, o nome da tabela é salvo em caixa baixa pelo [[misc#strcpylower|strcpylower]] no [[#Globais|GLOBAL_DATA]] e soma mais um passo "step".


```C
void setObjName(char **nome) {
	if (GLOBAL_PARSER.mode != 0) {
		GLOBAL_DATA.objName = malloc(sizeof(char)*((strlen(*nome)+1)));
		strcpylower(GLOBAL_DATA.objName, *nome);
		GLOBAL_DATA.objName[strlen(*nome)] = '\0';
		GLOBAL_PARSER.step++;
	}
}
``` 