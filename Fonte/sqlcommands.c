#include <stdio.h>
#include <stdlib.h>
#include <string.h>
////
#ifndef FBTREE // includes only if this flag is not defined (preventing duplication)
   #include "btree.h"
#endif
////
#ifndef FMACROS // garante que macros.h não seja reincluída
   #include "macros.h"
#endif
///
#ifndef FTYPES // garante que types.h não seja reincluída
  #include "types.h"
#endif
////
#ifndef FMISC // garante que misc.h não seja reincluída
  #include "misc.h"
#endif

#ifndef FBUFFER // garante que buffer.h não seja reincluída
  #include "buffer.h"
#endif

#ifndef FDICTIONARY // the same
  #include "dictionary.h"
#endif

#ifndef FEXPRESSAO
  #include "Expressao.h"
#endif
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Recebe o nome de uma tabela e engloba as funções leObjeto() e leSchema().
    Parametros: Nome da Tabela, Objeto da Tabela e tabela.
    Retorno:    tp_table
   ---------------------------------------------------------------------------------------------*/
tp_table *abreTabela(char *nomeTabela, struct fs_objects *objeto, tp_table **tabela) {
    *objeto     = leObjeto(nomeTabela);
    *tabela     = leSchema(*objeto);

    return *tabela;
}
// Se foram especificadas colunas no *s_insert, verifica se elas existem no esquema.
int allColumnsExists(rc_insert *s_insert, table *tabela) {
	int i;
	if (!s_insert->columnName) return 0;

	for (i = 0; i < s_insert->N; i++)
		if (retornaTamanhoTipoDoCampo(s_insert->columnName[i], tabela) == 0) {
			printf("ERROR: column \"%s\" of relation \"%s\" does not exist.\n", s_insert->columnName[i], tabela->nome);
			return 0;
		}

	return 1;
}
////
int typesCompatible(char table_type, char insert_type) {
	return (table_type == 'D' && insert_type == 'I')
		|| (table_type == 'D' && insert_type == 'D')
		|| (table_type == 'I' && insert_type == 'I')
		|| (table_type == 'S' && insert_type == 'S')
		|| (table_type == 'S' && insert_type == 'C')
		|| (table_type == 'C' && insert_type == 'C')
		|| (table_type == 'C' && insert_type == 'S');
}
////
// Busca o tipo do valor na inserção *s_insert do valor que irá para *columnName
// Se não existe em *s_insert, assume o tipo do esquema já que não receberá nada.
char getInsertedType(rc_insert *s_insert, char *columnName, table *tabela) {
	int i;
	for (i = 0; i < s_insert->N; i++)
		if (objcmp(s_insert->columnName[i], columnName) == 0)
			return s_insert->type[i];
	return retornaTamanhoTipoDoCampo(columnName, tabela);;
}
// Busca o valor na inserção *s_insert designado à *columnName.
// Se não existe, retorna 0, 0.0 ou \0
char *getInsertedValue(rc_insert *s_insert, char *columnName, table *tabela) {
	int i;
	char tipo, *noValue;
	for (i = 0; i < s_insert->N; i++)
		if (objcmp(s_insert->columnName[i], columnName) == 0)
			return s_insert->values[i];

	tipo = retornaTamanhoTipoDoCampo(columnName, tabela);
	noValue = (char *)malloc(sizeof(char)*3);
	if (tipo == 'I') noValue = "0";
	else if (tipo == 'D') noValue = "0.0";
	else noValue[0] = '\0';
	return noValue;
}
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Inicializa os atributos necessários para a verificação de FK e PK.
    Parametros: Objeto da tabela, Tabela, Buffer e nome da tabela.
    Retorno:    INT
                SUCCESS,
                ERRO_DE_PARAMETRO,
   ---------------------------------------------------------------------------------------------*/

int iniciaAtributos(struct fs_objects *objeto, tp_table **tabela, tp_buffer **bufferpool, char *nomeT){
    *objeto     = leObjeto(nomeT);
    *tabela     = leSchema(*objeto);
    *bufferpool = initbuffer();
    if(*tabela == ERRO_ABRIR_ESQUEMA) return ERRO_DE_PARAMETRO;
    if(*bufferpool == ERRO_DE_ALOCACAO) return ERRO_DE_PARAMETRO;
    return SUCCESS;
}
////
int verifyFK(char *tableName, char *column){
  int r = 0;
  if(verificaNomeTabela(tableName) == 1){
    struct fs_objects objeto = leObjeto(tableName);
    tp_table *esquema = leSchema(objeto),*k;
    if(esquema == ERRO_ABRIR_ESQUEMA){
      printf("ERROR: cannot create schema.\n");
      return 0;
    }
    for(k = esquema; k && !r; k = k->next)
      r = (k->chave == PK && objcmp(k->nome, column) == 0);
    free(esquema);
  }
  return r;
}

////////
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Gera as verificações em relação a chave FK.
    Parametros: Nome da Tabela, Coluna C, Nome do Campo, Valor do Campo, Tabela Apontada e Atributo Apontado.
    Retorno:    INT
                SUCCESS,
                ERRO_DE_PARAMETRO,
                ERRO_CHAVE_ESTRANGEIRA
   ---------------------------------------------------------------------------------------------*/

int verificaChaveFK(char *nomeTabela,column *c, char *nomeCampo, char *valorCampo, char *tabelaApt, char *attApt){
    int x,j, erro, page;
    char str[20];
    char dat[5] = ".dat";
    struct fs_objects objeto;
    tp_table *tabela;
    tp_buffer *bufferpoll;
    column *pagina = NULL;

    strcpylower(str, tabelaApt);
    strcat(str, dat);              //Concatena e junta o nome com .dat

    erro = existeAtributo(nomeTabela, c);

    if(iniciaAtributos(&objeto, &tabela, &bufferpoll, tabelaApt) != SUCCESS) {
        free(bufferpoll);
        free(tabela);
        return ERRO_DE_PARAMETRO;
    }

    erro = SUCCESS;
    for(x = 0; erro == SUCCESS; x++)
        erro = colocaTuplaBuffer(bufferpoll, x, tabela, objeto);

    for (page = 0; page < PAGES; page++) {
        if (pagina) free(pagina);
        pagina = getPage(bufferpoll, tabela, objeto, page);
        if (!pagina) break;

        for(j = 0; j < objeto.qtdCampos * bufferpoll[page].nrec; j++){
            if (pagina[j].nomeCampo) {
                if(objcmp(pagina[j].nomeCampo, attApt) == 0){

                    if(pagina[j].tipoCampo == 'S'){
                        if(objcmp(pagina[j].valorCampo, valorCampo) == 0){
                            free(pagina);
                            free(bufferpoll);
                            free(tabela);
                            return SUCCESS;
                        }
                    }
                    else if(pagina[j].tipoCampo == 'I'){
                        int *n = (int *)&pagina[j].valorCampo[0];
                        if(*n == atoi(valorCampo)){
                            free(pagina);
                            free(bufferpoll);
                            free(tabela);
                            return SUCCESS;
                        }
                    }
                    else if(pagina[j].tipoCampo == 'D'){
                        double *nn = (double *)&pagina[j].valorCampo[0];

                        if(*nn == atof(valorCampo)){
                            free(pagina);
                            free(bufferpoll);
                            free(tabela);
                            return SUCCESS;
                        }
                    }
                    else if(pagina[j].tipoCampo == 'C'){
                        if(pagina[j].valorCampo == valorCampo){
                            free(pagina);
                            free(bufferpoll);
                            free(tabela);
                            return SUCCESS;
                        }
                    }
                    else {
                        free(pagina);
                        free(bufferpoll);
                        free(tabela);
                        return ERRO_CHAVE_ESTRANGEIRA;
                    }
                }
            }
        }
    }
    if (pagina) free(pagina);
    free(bufferpoll);
    free(tabela);
    return ERRO_CHAVE_ESTRANGEIRA;
}
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Gera as verificações em relação a chave PK.
    Parametros: Nome da Tabela, Coluna C, Nome do Campo, Valor do Campo
    Retorno:    INT
                SUCCESS,
                ERRO_DE_PARAMETRO,
                ERRO_CHAVE_PRIMARIA
   ---------------------------------------------------------------------------------------------*/
int verificaChavePK(char *nomeTabela, column *c, char *nomeCampo, char *valorCampo) {
    int j, x, erro, page;
    column *pagina = NULL;

    struct fs_objects objeto;
    tp_table *tabela;
    tp_buffer *bufferpoll;

    erro = existeAtributo(nomeTabela, c);
    if (erro != SUCCESS ) {
        free(bufferpoll);
        return ERRO_DE_PARAMETRO;
    }

    if (iniciaAtributos(&objeto, &tabela, &bufferpoll, nomeTabela) != SUCCESS) {
        free(bufferpoll);
        free(tabela);
        return ERRO_DE_PARAMETRO;
    }

    erro = SUCCESS;
    for(x = 0; erro == SUCCESS; x++)
        erro = colocaTuplaBuffer(bufferpoll, x, tabela, objeto);

    page = 0;
    for (page = 0; page < PAGES; page++) {
        if (pagina) free(pagina);
        pagina = getPage(bufferpoll, tabela, objeto, page);
        if (!pagina) break;

        for(j = 0; j < objeto.qtdCampos * bufferpoll[page].nrec; j++){
            if (pagina[j].nomeCampo) {
                if (objcmp(pagina[j].nomeCampo, nomeCampo) == 0) {
                    if (pagina[j].tipoCampo == 'S') {
                        if (objcmp(pagina[j].valorCampo, valorCampo) == 0){
                            free(pagina);
                            free(bufferpoll);
                            free(tabela);
                            return ERRO_CHAVE_PRIMARIA;
                        }
                    } else if (pagina[j].tipoCampo == 'I') {
                        int *n = (int *)&pagina[j].valorCampo[0];

                        if (*n == atoi(valorCampo)) {
                            free(pagina);
                            free(bufferpoll);
                            free(tabela);
                            return ERRO_CHAVE_PRIMARIA;
                        }
                    } else if (pagina[j].tipoCampo == 'D'){
                        double *nn = (double *)&pagina[j].valorCampo[0];

                        if (*nn == atof(valorCampo)){
                            free(pagina);
                            free(bufferpoll);
                            free(tabela);
                            return ERRO_CHAVE_PRIMARIA;
                        }
                    } else if (pagina[j].tipoCampo == 'C'){
                        if (pagina[j].valorCampo == valorCampo){
                            free(pagina);
                            free(bufferpoll);
                            free(tabela);
                            return ERRO_CHAVE_PRIMARIA;
                        }
                    }
                }
            }
        }
    }

    if (pagina) free(pagina);
    free(bufferpoll);
    free(tabela);
    return SUCCESS;
}

/////
int finalizaInsert(char *nome, column *c){
    column *auxC, *temp;
    int i = 0, x = 0, t, erro, encontrou, j = 0, flag=0;
    FILE *dados;
    nodo *raiz = NULL;
    nodo *raizfk = NULL;

    struct fs_objects objeto,dicio; // Le dicionario
    tp_table *auxT ; // Le esquema
    auxT = abreTabela(nome, &dicio, &auxT);

    table *tab     = (table *)malloc(sizeof(table));
    tp_table *tab2 = (tp_table *)malloc(sizeof(struct tp_table));
    memset(tab2, 0, sizeof(tp_table));

    tab->esquema = abreTabela(nome, &objeto, &tab->esquema);
    tab2 = procuraAtributoFK(objeto);

    //-----------------------
    char *arquivoIndice = NULL;
    //------------------------

    for(j = 0, temp = c; j < objeto.qtdCampos && temp != NULL; j++, temp = temp->next){
        switch(tab2[j].chave){
            case NPK:
              erro = SUCCESS;
            break;

            case PK:
        		if(flag == 1) break;
                //monta o nome do arquivo de indice
                arquivoIndice = (char *)malloc(sizeof(char) *
                  (strlen(connected.db_directory) + strlen(nome) + strlen(tab2[j].nome)));
                strcpy(arquivoIndice, connected.db_directory); //diretorio
                strcat(arquivoIndice, nome); //nome da tabela
        				strcat(arquivoIndice, tab2[j].nome); //nome do atributo

        		// verificacao da chave primaria
        		raiz = constroi_bplus(arquivoIndice);
                free(arquivoIndice);
        		if(raiz != NULL) {
        			encontrou = buscaChaveBtree(raiz, temp->valorCampo);
        			if (encontrou) {
        				printf("ERROR: duplicate key value violates unique constraint \"%s_pkey\"\nDETAIL:  Key (%s)=(%s) already exists.\n",nome,temp->nomeCampo,temp->valorCampo);
        				free(auxT); // Libera a memoria da estrutura.
        				//free(temp); // Libera a memoria da estrutura.
        				free(tab); // Libera a memoria da estrutura.
        				free(tab2); // Libera a memoria da estrutura.
        				return ERRO_CHAVE_PRIMARIA;
        			}
        		}
        		flag = 1;
            break;

            case FK:
              //monta o nome do arquivo de indice da chave estrangeira
              arquivoIndice = (char *)malloc(sizeof(char) *
                (strlen(connected.db_directory) + strlen(tab2[j].tabelaApt) + strlen(tab2[j].attApt)));// caminho diretorio de arquivo de indice
              strcpy(arquivoIndice, connected.db_directory); //diretorio
      				strcat(arquivoIndice, tab2[j].tabelaApt);
      				strcat(arquivoIndice, tab2[j].attApt);

              raizfk = constroi_bplus(arquivoIndice); //verifica se o atributo referenciado pela FK possui indice B+
              free(arquivoIndice);
              if(raizfk == NULL) { //se não encontra faz a verificação sem indice b+
        				if (strlen(tab2[j].attApt) != 0 && strlen(tab2[j].tabelaApt) != 0){
        					  erro = verificaChaveFK(nome, temp, tab2[j].nome, temp->valorCampo,
                                            tab2[j].tabelaApt, tab2[j].attApt);
                    if (erro != SUCCESS){
                        printf("ERROR: invalid reference to \"%s.%s\". The value \"%s\" does not exist.\n", tab2[j].tabelaApt,tab2[j].attApt,temp->valorCampo);

            						free(auxT); // Libera a memoria da estrutura.
            						free(temp); // Libera a memoria da estrutura.
                        free(tab); // Libera a memoria da estrutura.
  					            free(tab2); // Libera a memoria da estrutura.
                        return ERRO_CHAVE_ESTRANGEIRA;
                    }
                }
              } else { //atributo referenciado possui indice B+
                  encontrou = buscaChaveBtree(raizfk, temp->valorCampo);
                  if (!encontrou) {
                    printf("ERROR: invalid reference to \"%s.%s\". The value \"%s\" does not exist.\n", tab2[j].tabelaApt,tab2[j].attApt,temp->valorCampo);

                    free(auxT); // Libera a memoria da estrutura.
                    free(temp); // Libera a memoria da estrutura.
                    free(tab); // Libera a memoria da estrutura.
                    free(tab2); // Libera a memoria da estrutura.
                    return ERRO_CHAVE_ESTRANGEIRA;
                  }
                  erro = SUCCESS;
              }
            break;
        }
    }
    flag = 0;
    char directory[LEN_DB_NAME_IO];
    strcpy(directory, connected.db_directory);
    strcat(directory, dicio.nArquivo);

    if((dados = fopen(directory,"a+b")) == NULL){
      printf("ERROR: cannot open file.\n");
  		free(auxT); // Libera a memoria da estrutura.
  		free(temp); // Libera a memoria da estrutura.
      free(tab); // Libera a memoria da estrutura.
  		free(tab2); // Libera a memoria da estrutura.
      return ERRO_ABRIR_ARQUIVO;
	   }
    long int offset = ftell(dados);
    for(auxC = c, t = 0; auxC != NULL; auxC = auxC->next, t++){
        if (t >= dicio.qtdCampos) t = 0;

        if (auxT[t].chave == PK && flag == 0) {
			char * nomeAtrib;
      		nomeAtrib = (char*)malloc((strlen(nome)+strlen(auxC->nomeCampo) + strlen(connected.db_directory))* sizeof(char));
      		strcpy(nomeAtrib, connected.db_directory);
      		strcat(nomeAtrib, nome);
      		strcat(nomeAtrib,auxC->nomeCampo);
            insere_indice(raiz, auxC->valorCampo, nomeAtrib, offset);
            free(nomeAtrib);
            flag = 1;
        }

		if (auxT[t].chave == BT) {
			char * nomeAtrib2;
            //ntuplas = ntuplas-1;
            decnTuplas();
      		nomeAtrib2 = (char*)malloc((strlen(nome)+strlen(auxC->nomeCampo) + strlen(connected.db_directory))* sizeof(char));
      		strcpy(nomeAtrib2, connected.db_directory);
      		strcat(nomeAtrib2, nome);
      		strcat(nomeAtrib2,auxC->nomeCampo);
      		nodo * raiz2 = NULL;
      		raiz2 = constroi_bplus(nomeAtrib2);
          insere_indice(raiz2, auxC->valorCampo, nomeAtrib2, offset);
          free(nomeAtrib2);
        }

        if (auxT[t].tipo == 'S'){ // Grava um dado do tipo string.
          if (sizeof(auxC->valorCampo) > auxT[t].tam && sizeof(auxC->valorCampo) != 8){
            printf("ERROR: invalid string length.\n");
			      free(tab); // Libera a memoria da estrutura.
			      free(tab2); // Libera a memoria da estrutura.
			      free(auxT); // Libera a memoria da estrutura.
			      free(temp); // Libera a memoria da estrutura.
			      fclose(dados);
            return ERRO_NO_TAMANHO_STRING;
          }

          if(objcmp(auxC->nomeCampo, auxT[t].nome) != 0){
            printf("ERROR: column name \"%s\" is not valid.\n", auxC->nomeCampo);
			      free(tab); // Libera a memoria da estrutura.
			      free(tab2); // Libera a memoria da estrutura.
			      free(auxT); // Libera a memoria da estrutura.
			      free(temp); // Libera a memoria da estrutura.
			      fclose(dados);
            return ERRO_NOME_CAMPO;
          }

          char valorCampo[auxT[t].tam];
          strncpy(valorCampo, auxC->valorCampo, auxT[t].tam);
          strcat(valorCampo, "\0");
          fwrite(&valorCampo,sizeof(valorCampo),1,dados);
          //------------- inserção B+ ------------

        }
        else if (auxT[t].tipo == 'I'){ // Grava um dado do tipo inteiro.
          i = 0;
          while (i < strlen(auxC->valorCampo)){
            if(auxC->valorCampo[i] < 48 || auxC->valorCampo[i] > 57){
              printf("ERROR: column \"%s\" expected integer.\n", auxC->nomeCampo);
  				    free(tab); // Libera a memoria da estrutura.
  				    free(tab2); // Libera a memoria da estrutura.
  				    free(auxT); // Libera a memoria da estrutura.
  				    //free(temp); // Libera a memoria da estrutura.
  				    fclose(dados);
              return ERRO_NO_TIPO_INTEIRO;
            }
            i++;
          }
          int valorInteiro = 0;
          sscanf(auxC->valorCampo,"%d",&valorInteiro);
          fwrite(&valorInteiro,sizeof(valorInteiro),1,dados);
        }
        else if (auxT[t].tipo == 'D'){ // Grava um dado do tipo double.
            x = 0;
            while (x < strlen(auxC->valorCampo)){
                if((auxC->valorCampo[x] < 48 || auxC->valorCampo[x] > 57) && (auxC->valorCampo[x] != 46)){
                    printf("ERROR: column \"%s\" expect double.\n", auxC->nomeCampo);
          					free(tab); // Libera a memoria da estrutura.
          					free(tab2); // Libera a memoria da estrutura.
          					free(auxT); // Libera a memoria da estrutura.
          					free(temp); // Libera a memoria da estrutura.
          					fclose(dados);
                    return ERRO_NO_TIPO_DOUBLE;
                }
                x++;
            }

            double valorDouble = convertD(auxC->valorCampo);
            fwrite(&valorDouble,sizeof(valorDouble),1,dados);
        }
        else if (auxT[t].tipo == 'C'){ // Grava um dado do tipo char.

            if (strlen(auxC->valorCampo) > (sizeof(char))) {
                printf("ERROR: column \"%s\" expect char.\n", auxC->nomeCampo);
        				free(tab); // Libera a memoria da estrutura.
        				free(tab2); // Libera a memoria da estrutura.
        				free(auxT); // Libera a memoria da estrutura.
        				free(temp); // Libera a memoria da estrutura.
        				fclose(dados);
                return ERRO_NO_TIPO_CHAR;
            }
            char valorChar = auxC->valorCampo[0];
            fwrite(&valorChar,sizeof(valorChar),1,dados);

        }

    }
  fclose(dados);
	free(tab); // Libera a memoria da estrutura.
	free(tab2); // Libera a memoria da estrutura.
	free(auxT); // Libera a memoria da estrutura.
	free(temp); // Libera a memoria da estrutura.
  return SUCCESS;
}

/* insert: Recebe uma estrutura rc_insert e valida os tokens encontrados pela interface().
 *         Se os valores forem válidos, insere um novo valor.
 */
void insert(rc_insert *s_insert) {
	int i;
	table *tabela = (table *)malloc(sizeof(table));
	tabela->esquema = NULL;
	memset(tabela, 0, sizeof(table));
	column *colunas = NULL;
	tp_table *esquema = NULL;
	struct fs_objects objeto;
	memset(&objeto, 0, sizeof(struct fs_objects));
	char  flag=0;

	abreTabela(s_insert->objName, &objeto, &tabela->esquema); //retorna o esquema para a insere valor
	strcpylower(tabela->nome, s_insert->objName);

	if(s_insert->columnName != NULL){
		if (allColumnsExists(s_insert, tabela)){
			for (esquema = tabela->esquema; esquema != NULL; esquema = esquema->next){
				if(typesCompatible(esquema->tipo,getInsertedType(s_insert, esquema->nome, tabela))){
					colunas = insereValor(tabela, colunas, esquema->nome, getInsertedValue(s_insert, esquema->nome, tabela));
				}
        else {
					printf("ERROR: data type invalid to column '%s' of relation '%s' (expected: %c, received: %c).\n", esquema->nome, tabela->nome, esquema->tipo, getInsertedType(s_insert, esquema->nome, tabela));
					flag=1;
				}
			}
		}
    else {
			flag = 1;
		}
	}
  else {
		if (s_insert->N == objeto.qtdCampos) {
			for(i=0; i < objeto.qtdCampos; i++) {

				if(s_insert->type[i] == 'S' && tabela->esquema[i].tipo == 'C') {
					s_insert->values[i][1] = '\0';
					s_insert->type[i] = 'C';
				}

				if(s_insert->type[i] == 'I' && tabela->esquema[i].tipo == 'D') {
					s_insert->type[i] = 'D';
				}

				if(s_insert->type[i] == tabela->esquema[i].tipo)
					colunas = insereValor(tabela, colunas, tabela->esquema[i].nome, s_insert->values[i]);
				else {
					printf("ERROR: data type invalid to column '%s' of relation '%s' (expected: %c, received: %c).\n", tabela->esquema[i].nome, tabela->nome, tabela->esquema[i].tipo, s_insert->type[i]);
					flag=1;
				}
			}
		}
    else {
      printf("ERROR: INSERT has more expressions than target columns.\n");
		  flag = 1;
		}
	}

  if (!flag && finalizaInsert(s_insert->objName, colunas) == SUCCESS)
    printf("INSERT 0 1\n");

	//freeTp_table(&esquema, objeto.qtdCampos);
	free(esquema);
	freeColumn(colunas);
	freeTable(tabela);
}

//select * from t4;
int validaProj(Lista *proj,column *colunas,int qtdColunas,char *pert){
  if(proj->tam == 1 && ((char *)proj->prim->inf)[0] == '*'){
    free(rmvNodoPtr(proj,proj->prim));
    proj->prim = proj->ult = NULL;
    for(int j = 0;j < qtdColunas; j++){
      pert[j] = 1;
      char *str = malloc(sizeof(char)*strlen(colunas[j].nomeCampo));
      strcpy(str,colunas[j].nomeCampo);
      adcNodo(proj,proj->ult,str);
    }
    return 1;
  }
  char *validar = malloc(sizeof(char)*proj->tam);
  for(int j = 0; j < proj->tam; j++) validar[j] = 0;
  for(int j = 0; j < qtdColunas; j++){
    int k = 0;
    for(Nodo *it = proj->prim; it; it = it->prox,k++){
      pert[j] |= (strcmp((char *)it->inf,colunas[j].nomeCampo) == 0);
      validar[k] |= pert[j];
    }
  }
  int k = 0;
  for(Nodo *it = proj->prim; it; it = it->prox,k++){
    if(!validar[k]){
      free(validar);
      printf("The column from the projection %s does not belong to the table.\n",(char *)it->inf);
      return 0;
    }
  }
  free(validar);
  return 1;
}

inf_where *novoTokenWhere(char *str,int id){
  inf_where *novo = malloc(sizeof(inf_where));
  novo->id = id;
  char *tk = malloc(sizeof(char)*strlen(str));
  strcpy(tk,str);
  novo->token = (void *)tk;
  return novo;
}

inf_where *novoResWhere(void *tk,int id){
  if(id == STRING || id == ABRE_PARENT || id == FECHA_PARENT)
    return novoTokenWhere((char *)tk,id);
  inf_where *novo = malloc(sizeof(inf_where));
  novo->id = id;
  if(novo->id == VALUE_NUMBER){
    double *tok = malloc(sizeof(double));
    *tok = *((double *)tk);
    novo->token = (void *)tok;
  }
  else{//BOOLEANO
    char *tok = malloc(sizeof(char));
    *tok = *((char *)tk);
    novo->token = (void *)tok;
  }
  return novo;
}

int validaColsWhere(Lista *tok,column *colunas,int qtdColunas){
  if(!tok) return 1;
  for(Nodo *i = tok->prim; i; i = i->prox){
    inf_where *iw = (inf_where *)i->inf;
    if(iw->id == OBJETO){
      int achou = 0;
      char *str = (char *)iw->token;
      for(int j = 0; !achou && j < qtdColunas; j++)
        achou = (strcmp(str,colunas[j].nomeCampo) == 0);
      if(!achou){
        printf("The column %s does not belong to the table.\n",str);
        return 0;
      }
    }
  }
  return 1;
}

void printConsulta(Lista *p,Lista *l){
  if(!l->tam){
    printf("\n 0 line.\n");
    return;
  }
  //cabecalho
  for(Nodo *j = ((Lista *)(l->prim->inf))->prim,
           *i = p->prim; j; j = j->prox,i = i->prox){
    inf_where *jw = (inf_where *)(j->inf);
    if(jw->id == (int)'S') printf(" %-20s ", (char *)(i->inf));
    else printf(" %-10s ", (char *)(i->inf));
    if(j->prox) printf("|");
  }
  printf("\n");
  for(Nodo *j = ((Lista *)(l->prim->inf))->prim; j; j = j->prox){
    inf_where *jw = (inf_where *)(j->inf);
    printf("%s",(jw->id == (int)'S') ? "----------------------"
                                     : "------------");
    if(j->prox) printf("+");
  }
  printf("\n");//fim do cabecalho
  for(Nodo *i = l->prim; i; i = i->prox){
    Lista *li = (Lista *)(i->inf);
    for(Nodo *j = li->prim; j; j = j->prox){
      inf_where *ij = (inf_where *)(j->inf);
      if(ij->id == (int)'S')
        printf(" %-20s ", (char *)ij->token);
      else if(ij->id == (int)'I'){
        int *n = (int *)(ij->token);
        printf(" %-10d ", *n);
      }
      else if(ij->id == (int)'C')
        printf(" %-10c ", *(char *)ij->token);
      else if(ij->id == (int)'D'){
        double *n = (double *)(ij->token);
        printf(" %-10f ", *n);
      }
      if(j->prox) printf("|");
    }
    printf("\n");
  }
  printf("\n %d Line%s.\n",l->tam,l->tam == 1 ? "" : "s");
}

void adcResultado(Lista *resultado,Lista *tupla,char *pertence){
  adcNodo(resultado,resultado->ult,(void *)novaLista(NULL));
  Lista *tuplaRes = (Lista *)(resultado->ult->inf);
  int ci = 0;
  for(Nodo *n1 = tupla->prim; n1; n1 = n1->prox,ci++){
    column *c = (column *)(n1->inf);
    if(pertence[ci]){//adcionar em resultado.
      inf_where *nw = malloc(sizeof(inf_where));
      nw->id = c->tipoCampo;
      if(c->tipoCampo == 'S'){
        char *str = malloc(sizeof(char)*strlen(c->valorCampo));
        str[0] = '\0';
        strcpy(str,c->valorCampo);
        nw->token = (void *)str;
      }
      else if(c->tipoCampo == 'I'){
        int *n = malloc(sizeof(int));
        n = (int *)(c->valorCampo);
        nw->token = ( void *)n;
      }
      else if(c->tipoCampo == 'C'){
        char *n = malloc(sizeof(char));
        n = (char *)(c->valorCampo);
        nw->token = ( void *)n;
      }
      else if(c->tipoCampo == 'D'){
        double *n = malloc(sizeof(double));
        n = (double *)(c->valorCampo);
        nw->token = ( void *)n;
      }
      adcNodo(tuplaRes,tuplaRes->ult,(void *)nw);
    }
  }
}

Lista *op_select(inf_select *select) {
  //inicio das validações.
  tp_table *esquema;
  tp_buffer *bufferpoll;
  struct fs_objects objeto;
  if(!verificaNomeTabela(select->tabela)){
    printf("\nERROR: relation \"%s\" was not found.\n\n\n", select->tabela);
    return NULL;
  }
  objeto = leObjeto(select->tabela);
  esquema = leSchema(objeto);
  if(esquema == ERRO_ABRIR_ESQUEMA){
    printf("ERROR: schema cannot be created.\n");
    free(esquema);
    return NULL;
  }
  bufferpoll = initbuffer();
  if(bufferpoll == ERRO_DE_ALOCACAO){
    free(bufferpoll);
    free(esquema);
    printf("ERROR: no memory available to allocate buffer.\n");
    return NULL;
  }
  int erro = SUCCESS,x;
  for(x = 0; erro == SUCCESS; x++)
    erro = colocaTuplaBuffer(bufferpoll, x, esquema, objeto);
  x--;
  column *pagina = getPage(bufferpoll, esquema, objeto, 0);
  if(!pagina){
    printf("Empty Table.\n");
    free(bufferpoll);
    free(esquema);
    return NULL;
  }
  char *pertence = malloc(sizeof(char)*objeto.qtdCampos);
  if(!validaProj(select->proj,pagina,objeto.qtdCampos,pertence)){
    free(bufferpoll);
    free(pertence);
    free(esquema);
    return NULL;
  }
  if(!validaColsWhere(select->tok,pagina,objeto.qtdCampos)){
    free(bufferpoll);
    free(pertence);
    free(esquema);
    return NULL;
  }
  int i,j,k;
  char abortar = 0;
  Lista *tupla = novaLista(NULL),
        *resultado = novaLista(NULL);
  for(int p = 0; !abortar && x; x -= bufferpoll[p++].nrec){
    pagina = getPage(bufferpoll, esquema, objeto, p);
    if(pagina == ERRO_PARAMETRO){
      printf("ERROR: could not open the table.\n");
      free(bufferpoll);
      free(esquema);
      return NULL;
    }
    for(k = j = i = 0; !abortar && k < bufferpoll[p].nrec; k++){
      for(i = 0; i < objeto.qtdCampos; i++,j++)
        adcNodo(tupla,tupla->ult,(void *)(&pagina[j]));
      char sat = 0;
      if(select->tok){
        Lista *l = resArit(select->tok,tupla);
        if(l){
          Lista *l2 = relacoes(l);
          sat = (logPosfixa(l2) != 0);
        }
        else abortar = 1;
      }
      else sat = 1;
      if(!abortar && sat) adcResultado(resultado,tupla,pertence);
      for(Nodo *n1 = tupla->prim,*n2; n1; n1 = n2){
        n2 = n1->prox;
        rmvNodoPtr(tupla,n1);
        //não precisa dar free na informação
        //pois é ponteiro pra pagina[j]
      }
      tupla->prim = tupla->ult = NULL;
    }
  }
  if(abortar){
    resultado = NULL;
  }
  free(tupla); tupla = NULL;
  free(esquema); esquema = NULL;
  free(pertence); pertence = NULL;
  free(bufferpoll); bufferpoll = NULL;
  return resultado;
}
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Copia todas as informações menos a tabela do objeto, que será removida.
    Parametros: Objeto que será removido do schema.
    Retorno:    INT
                SUCCESS,
                ERRO_REMOVER_ARQUIVO_SCHEMA
   ---------------------------------------------------------------------------------------------*/

int procuraSchemaArquivo(struct fs_objects objeto){
    FILE *schema, *newSchema;
    int cod = 0;
    char *tupla = (char *)malloc(sizeof(char) * 109);
    memset(tupla, '\0', 109);

    tp_table *esquema = (tp_table *)malloc(sizeof(tp_table)*objeto.qtdCampos);
    memset(esquema, 0, sizeof(tp_table)*objeto.qtdCampos);

    char directory[LEN_DB_NAME_IO];
    memset(&directory, '\0', LEN_DB_NAME_IO);

    strcpy(directory, connected.db_directory);
    strcat(directory, "fs_schema.dat");

    if((schema = fopen(directory, "a+b")) == NULL) {
        free(tupla);
        return ERRO_REMOVER_ARQUIVO_SCHEMA;
    }

    strcpy(directory, connected.db_directory);
    strcat(directory, "fs_nschema.dat");

    if((newSchema = fopen(directory, "a+b")) == NULL) {
        free(tupla);
        return ERRO_REMOVER_ARQUIVO_SCHEMA;
    }

    fseek(newSchema, 0, SEEK_SET);

    while((fgetc (schema) != EOF)){ // Varre o arquivo ate encontrar todos os campos com o codigo da tabela.
        fseek(schema, -1, 1);
        fseek(newSchema, 0, SEEK_END);

        if(fread(&cod, sizeof(int), 1, schema)){ // Le o codigo da tabela.
            if(cod != objeto.cod){
                fwrite(&cod, sizeof(int), 1, newSchema);

                fread(tupla, sizeof(char), TAMANHO_NOME_CAMPO, schema);
                strcpylower(esquema[0].nome,tupla);                  // Copia dados do campo para o esquema.
                fwrite(tupla, sizeof(char), TAMANHO_NOME_CAMPO, newSchema);

                fread(&esquema[0].tipo , sizeof(char), 1 , schema);
                fread(&esquema[0].tam  , sizeof(int) , 1 , schema);
                fread(&esquema[0].chave, sizeof(int) , 1 , schema);

                fwrite(&esquema[0].tipo , sizeof(char), 1, newSchema);
                fwrite(&esquema[0].tam  , sizeof(int) , 1, newSchema);
                fwrite(&esquema[0].chave, sizeof(int) , 1, newSchema);

                fread(tupla, sizeof(char), TAMANHO_NOME_TABELA, schema);
                strcpylower(esquema[0].tabelaApt,tupla);
                fwrite(&esquema[0].tabelaApt, sizeof(char), TAMANHO_NOME_TABELA, newSchema);

                fread(tupla, sizeof(char), TAMANHO_NOME_CAMPO, schema);
                strcpylower(esquema[0].attApt,tupla);
                fwrite(&esquema[0].attApt, sizeof(char), TAMANHO_NOME_CAMPO, newSchema);

            } else {
                fseek(schema, 109, 1);
            }
        }
    }

    fclose(newSchema);
    fclose(schema);

    char directoryex[LEN_DB_NAME_IO*2];
    memset(&directoryex, '\0', LEN_DB_NAME_IO*2);
    strcpy(directoryex, connected.db_directory);
    strcat(directoryex, "fs_schema.dat");

    remove(directoryex);

    strcpy(directoryex, "mv ");
    strcat(directoryex, connected.db_directory);
    strcat(directoryex, "fs_nschema.dat ");
    strcat(directoryex, connected.db_directory);
    strcat(directoryex, "fs_schema.dat");

    system(directoryex);

    free(tupla);
    return SUCCESS;
}

/* ----------------------------------------------------------------------------------------------
    Objetivo:   Função para exclusão de tabelas.
    Parametros: Nome da tabela (char).
    Retorno:    INT
                SUCCESS,
                ERRO_REMOVER_ARQUIVO_OBJECT,
                ERRO_REMOVER_ARQUIVO_SCHEMA,
                ERRO_LEITURA_DADOS.
   ---------------------------------------------------------------------------------------------*/

int excluirTabela(char *nomeTabela) {
    struct fs_objects objeto, objeto1;
    tp_table *esquema, *esquema1;
    int x,erro, i, j, k, l, qtTable;
	  char str[20];
    char dat[5] = ".dat";
    FILE *f = NULL;
    memset(str, '\0', 20);

    if (!verificaNomeTabela(nomeTabela)) {
        printf("ERROR: table \"%s\" does not exist.\n", nomeTabela);
        return ERRO_NOME_TABELA;
    }

    strcpylower(str, nomeTabela);
    strcat(str, dat);              //Concatena e junta o nome com .dat

    abreTabela(nomeTabela, &objeto, &esquema);
    qtTable = quantidadeTabelas();

    char **tupla = (char **)malloc(sizeof(char **)*qtTable);

    memset(tupla, 0, qtTable);

    for (i=0; i < qtTable; i++) {
        tupla[i] = (char *)malloc(sizeof(char)*TAMANHO_NOME_TABELA);
        memset(tupla[i], '\0', TAMANHO_NOME_TABELA);
    }
    tp_table *tab2 = (tp_table *)malloc(sizeof(struct tp_table));
    tab2 = procuraAtributoFK(objeto);   //retorna o tipo de chave que e cada campo
    FILE *dicionario;
    char directory[LEN_DB_NAME_IO*2];
    memset(directory, '\0', LEN_DB_NAME_IO*2);
    strcpy(directory, connected.db_directory);
    strcat(directory, "fs_object.dat");

    if((dicionario = fopen(directory,"a+b")) == NULL)
        return ERRO_ABRIR_ARQUIVO;
    k=0;
    while(fgetc (dicionario) != EOF){
        fseek(dicionario, -1, 1);
        //coloca o nome de todas as tabelas em tupla
        fread(tupla[k], sizeof(char), TAMANHO_NOME_TABELA , dicionario);
        k++;
        fseek(dicionario, 32, 1);
    }
    fclose(dicionario);
    for(i = 0; i < objeto.qtdCampos; i++){
        if(tab2[i].chave == PK){
            for(j=0; j<qtTable; j++) {                      //se tiver chave primaria verifica se ela e chave
                if(objcmp(tupla[j], nomeTabela) != 0) {     //estrangeira em outra tabela

                    abreTabela(tupla[j], &objeto1, &esquema1);

                    tp_table *tab3 = (tp_table *)malloc(sizeof(struct tp_table));
                    tab3 = procuraAtributoFK(objeto1);

                    for(l=0; l<objeto1.qtdCampos; l++) {
                        if(tab3[l].chave == FK) { //verifica se a outra tabela possui chave estrangeira. se sim, verifica se e da tabela anterior.
                            if(objcmp(nomeTabela, tab3[l].tabelaApt) == 0) {
                                printf("ERROR: cannot drop table \"%s\" because other objects depend on it.\n", nomeTabela);
                                return ERRO_CHAVE_ESTRANGEIRA;
                            }
                        }
                    }
                    free(tab3);
                }
            }
        }
    }

    for (i = 0; objeto.qtdIndice != 0; i++) {
      if(tab2[i].chave == PK || tab2[i].chave == BT) {
        strcpy(directory, connected.db_directory);
        strcat(directory, nomeTabela);
        strcat(directory, tab2[i].nome);
        strcat(directory, dat);
        if ((f = fopen(directory,"r")) != NULL){
    			remove(directory);
          fclose(f);
    		}
        objeto.qtdIndice--;
      }
    }

    free(tab2);
    tp_buffer *bufferpoll = initbuffer();
    if(bufferpoll == ERRO_DE_ALOCACAO){
        printf("ERROR: no memory available to allocate buffer.\n");
        return ERRO_LEITURA_DADOS;
    }

    erro = SUCCESS;
    for(x = 0; erro == SUCCESS; x++)
        erro = colocaTuplaBuffer(bufferpoll, x, esquema, objeto);

    if(procuraSchemaArquivo(objeto) != 0) {
        free(bufferpoll);
        return ERRO_REMOVER_ARQUIVO_SCHEMA;
    }

    if(procuraObjectArquivo(nomeTabela) != 0) {
        free(bufferpoll);
        return ERRO_REMOVER_ARQUIVO_OBJECT;
    }
   	strcpy(directory, connected.db_directory);
    strcat(directory, str);
    remove(directory);
    free(bufferpoll);

    printf("DROP TABLE\n");
    return SUCCESS;
}

/////
int verifyFieldName(char **fieldName, int N){
    int i, j;
    if(N<=1) return 1;
    for(i=0; i < N; i++){
        for(j=i+1; j < N; j++){
            if(objcmp(fieldName[i], fieldName[j]) == 0){
                printf("ERROR: column \"%s\" specified more than once\n", fieldName[i]);
                return 0;
            }
        }
    }
    return 1;
}

//////
void createTable(rc_insert *t) {
  if(strlen(t->objName) > TAMANHO_NOME_TABELA){
      printf("A table name must have no more than %d caracteres.\n",TAMANHO_NOME_TABELA);
      return;
  }
  int size;
  strcpylower(t->objName, t->objName);        //muda pra minúsculo
  char *tableName = (char*) malloc(sizeof(char)*(TAMANHO_NOME_TABELA+10)),
                    fkTable[TAMANHO_NOME_TABELA], fkColumn[TAMANHO_NOME_CAMPO];

  strncpylower(tableName, t->objName, TAMANHO_NOME_TABELA);
  strcat(tableName, ".dat\0");                  //tableName.dat
  if(existeArquivo(tableName)){
    printf("ERROR: table already exist\n");
    free(tableName);
    return;
  }

  table *tab = NULL;
  tab = iniciaTabela(t->objName);    //cria a tabela
  if(0 == verifyFieldName(t->columnName, t->N)){
    free(tableName);
    freeTable(tab);
    return;
  }
  int i;
  for(i = 0; i < t->N; i++){
    if(t->type[i] == 'S')
  		size = atoi(t->values[i]);
  	else if(t->type[i] == 'I')
  		size = sizeof(int);
  	else if(t->type[i] == 'D')
  		size = sizeof(double);
    else if(t->type[i] == 'C')
  		size = sizeof(char);
  	if(t->attribute[i] == FK){
  		strncpylower(fkTable, t->fkTable[i], TAMANHO_NOME_TABELA);
  		strncpylower(fkColumn,t->fkColumn[i], TAMANHO_NOME_CAMPO);
  	}
    else{
  		strcpy(fkTable, "");
  		strcpy(fkColumn, "");
  	}
    tab = adicionaCampo(tab, t->columnName[i], t->type[i], size, t->attribute[i], fkTable, fkColumn);
    if((objcmp(fkTable, "") != 0) || (objcmp(fkColumn, "") != 0)){
      if(verifyFK(fkTable, fkColumn) == 0){
  		  printf("ERROR: attribute FK cannot be referenced\n");
        free(tableName);
        freeTable(tab);
        return;
      }
    }
  }

  //Se não existe tabela com esse nome
  if(finalizaTabela(tab) == SUCCESS) {
  	for(int i = 0; i < t->N; i++) {
  		if(t->attribute[i] == PK) { //procura o atributo PK e cria o arquivo de índice
        char *aux_nome_index = NULL;
  		  aux_nome_index = (char *)malloc(strlen(connected.db_directory) + strlen(t->objName) + strlen(t->columnName[i]));
        strcpy(aux_nome_index, connected.db_directory);
        strcat(aux_nome_index, t->objName);
  		  strcat(aux_nome_index, t->columnName[i]);
        inicializa_indice(aux_nome_index);
        break;
  		}
  	}
  	printf("CREATE TABLE\n");
  } else { //Tabela já existe, então não é preciso criar o índice b+.
	  printf("ERROR: table already exist\n");
  }

  free(tableName);
  if(tab != NULL) freeTable(tab);
}

void createIndex(rc_insert *t) {
  struct fs_objects obj;
  struct tp_table   *tb;
  char dir[TAMANHO_NOME_TABELA + TAMANHO_NOME_ARQUIVO + TAMANHO_NOME_CAMPO + TAMANHO_NOME_INDICE];
  int flag = 0;
  FILE *f = NULL;

  if (!verificaNomeTabela(t->objName)) {
    printf("ERROR: table \"%s\" does not exist.\n", t->objName);
    return;
  }

  obj = leObjeto(t->objName);
  tb  = leSchema(obj);

  for(tp_table *aux = tb; aux != NULL && !flag; aux = aux->next) {
    if(strcmp(aux->nome, t->columnName[0]) == 0) { //Procura o atributo na tabela
      if(aux->chave == PK) {// Se o atributo é PK já possui índice criado automaticamente
        printf("ERROR: attribute \"%s\" already has a b+ index.\n", t->columnName[0]);
        return;
      }
      flag = 1;
    }
  }

  if (!flag) {
    printf("ERROR: attribute \"%s\" does not exist on table %s.\n", t->columnName[0], t->objName);
    return;
  }

  strcpy(dir, connected.db_directory);
  strcat(dir, t->objName);
  strcat(dir, t->columnName[0]);
  strcat(dir, ".dat");

  if ((f = fopen(dir,"r")) != NULL){
    printf("ERROR: B+ index file already exists.\n");
    return;
  }

  strcpy(dir, connected.db_directory);
  strcat(dir, t->objName);
  strcat(dir, t->columnName[0]);

  inicializa_indice(dir);
  incrementaQtdIndice(t->objName);
  adicionaBT(t->objName, t->columnName[0]);
  printf("CREATE INDEX\n");
}

///////
