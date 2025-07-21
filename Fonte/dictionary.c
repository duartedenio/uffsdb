#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

#ifndef FSQLCOMMANDS // garante que sqlcommands.h não seja reincluída
   #include "sqlcommands.h"
#endif

#ifndef FBUFFER // garante que buffer.h não seja reincluída
   #include "buffer.h"
#endif

/* ----------------------------------------------------------------------------------------------
    Objetivo:   Verificação de existência de um arquivo.
    Parametros: Nome do arquivo.
    Retorno:    INT 1 (existe) , 0 (não existe).
   ---------------------------------------------------------------------------------------------*/

int existeArquivo(const char* filename){

    char directory[LEN_DB_NAME_IO];
    strcpy(directory, connected.db_directory);
    strcat(directory, filename);
    FILE* fptr = fopen(directory, "r");

    if (fptr != NULL){
        fclose(fptr);

        return 1;
    }

    return 0;
}
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Verifica a existência do atributo antes de adicionar na tabela
    Parametros: Nome da tabela, coluna C.
    Retorno:    INT
                SUCCESS,
                ERRO_DE_PARAMETRO
   ---------------------------------------------------------------------------------------------*/

int existeAtributo(char *nomeTabela, column *c){
    int erro, x, count;
    struct fs_objects objeto;
    memset(&objeto, 0, sizeof(struct fs_objects));
    tp_table *tabela;
    tp_buffer *bufferpoll;
    column *aux = NULL;
    column *pagina = NULL;

    if(iniciaAtributos(&objeto, &tabela, &bufferpoll, nomeTabela) != SUCCESS)
        return ERRO_DE_PARAMETRO;

    erro = SUCCESS;
    for(x = 0; erro == SUCCESS; x++)
        erro = colocaTuplaBuffer(bufferpoll, x, tabela, objeto);

    pagina = getPage(bufferpoll, tabela, objeto, 0);

    if(pagina == NULL){
        pagina = getPage(bufferpoll, tabela, objeto, 1);
    }

    if(pagina != NULL){
        count = 0;
        for(x = 0; x < objeto.qtdCampos; x++){
            if (!pagina[x].nomeCampo) continue;
            for(aux = c; aux != NULL; aux=aux->next) {
                if (!aux->nomeCampo) continue;
                if(objcmp(pagina[x].nomeCampo, aux->nomeCampo) == 0)
                    count++;
            }
        }
        if(count != objeto.qtdCampos){
            free(pagina);
            free(bufferpoll);
            free(tabela);
            return ERRO_DE_PARAMETRO;
        }
    }

    free(pagina);
    free(bufferpoll);
    free(tabela);
    return SUCCESS;
}
//////
int verificaNomeTabela(char *nomeTabela) {
    if(strlen(nomeTabela) > TAMANHO_NOME_TABELA) return 0;

    FILE *dicionario;
    char *tupla = (char *)malloc(sizeof(char)*TAMANHO_NOME_TABELA);

    char directory[LEN_DB_NAME_IO];
    strcpy(directory, connected.db_directory);
    strcat(directory, "fs_object.dat");


    if((dicionario = fopen(directory,"a+b")) == NULL){
        free(tupla);
        return ERRO_ABRIR_ARQUIVO;
    }

    while(fgetc (dicionario) != EOF){
        fseek(dicionario, -1, 1);

        fread(tupla, sizeof(char), TAMANHO_NOME_TABELA, dicionario); //Lê somente o nome da tabela

        if(objcmp(tupla, nomeTabela) == 0){ // Verifica se o nome dado pelo usuario existe no dicionario de dados.
            free(tupla);
            fclose(dicionario);
            return 1;
        }

        fseek(dicionario, 32, 1);
    }

    fclose(dicionario);
    free(tupla);

    return 0;
}
////
int quantidadeTabelas(){

    FILE *dicionario;
    int codTbl = 0;

    char directory[LEN_DB_NAME_IO];
    strcpy(directory, connected.db_directory);
    strcat(directory, "fs_object.dat");

    if((dicionario = fopen(directory,"a+b")) == NULL)
        return ERRO_ABRIR_ARQUIVO;

    while(fgetc (dicionario) != EOF){
        fseek(dicionario, -1, 1);

        codTbl++; // Conta quantas vezes é lido uma tupla no dicionario.

        fseek(dicionario, 52, 1);
    }

    fclose(dicionario);

    return codTbl;
}

////
int retornaTamanhoValorCampo(char *nomeCampo, table  *tab) {
  int tam = 0;
  tp_table *temp = tab->esquema;
  while(temp != NULL) {
    if (objcmp(nomeCampo,temp->nome) == 0) {
      tam = temp->tam;
      return tam;
    }
    temp = temp->next;
  }
  return 0;
}

////
char retornaTamanhoTipoDoCampo(char *nomeCampo, table  *tab) {
  char tipo = 0;
  tp_table *temp = tab->esquema;
  while(temp != NULL) {
    if (objcmp(nomeCampo,temp->nome) == 0) return temp->tipo;
    temp = temp->next;
  }
  return tipo;
}
////
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Retorna vetor de esquemas com todos os atributos chaves (PK, FK e NPK)
    Parametros: Objeto da tabela.
    Retorno:    Vetor de esquemas vetEsqm
   ---------------------------------------------------------------------------------------------*/

tp_table *procuraAtributoFK(struct fs_objects objeto){
    FILE *schema;
    int cod = 0, chave, i = 0;
    char *tupla = (char *)malloc(sizeof(char) * 109);
    tp_table *esquema = (tp_table *)malloc(sizeof(tp_table)*objeto.qtdCampos);
    tp_table *vetEsqm = (tp_table *)malloc(sizeof(tp_table)*objeto.qtdCampos);
    memset(vetEsqm, 0, sizeof(tp_table)*objeto.qtdCampos);
    memset(esquema, 0, sizeof(tp_table)*objeto.qtdCampos);

    char directory[LEN_DB_NAME_IO];
    strcpy(directory, connected.db_directory);
    strcat(directory, "fs_schema.dat");

    if((schema = fopen(directory, "a+b")) == NULL){
      printf("ERROR: could not read schema.\n");
      free(tupla);
		  free(esquema);
		  free(vetEsqm);
      return ERRO_ABRIR_ESQUEMA;
    }

    while((fgetc (schema) != EOF) && i < objeto.qtdCampos){ // Varre o arquivo ate encontrar todos os campos com o codigo da tabela.
        fseek(schema, -1, 1);

        if(fread(&cod, sizeof(int), 1, schema)){ // Le o codigo da tabela.
            if(cod == objeto.cod){
                fread(tupla, sizeof(char), TAMANHO_NOME_CAMPO, schema);
                strcpylower(esquema[i].nome,tupla);                  // Copia dados do campo para o esquema.

                fread(&esquema[i].tipo , sizeof(char), 1 , schema);
                fread(&esquema[i].tam  , sizeof(int) , 1 , schema);
                fread(&chave, sizeof(int) , 1 , schema);
                vetEsqm[i].tipo = esquema[i].tipo;
                vetEsqm[i].tam = esquema[i].tam;
                fread(tupla, sizeof(char), TAMANHO_NOME_TABELA, schema);
                strcpylower(esquema[i].tabelaApt,tupla);

                fread(tupla, sizeof(char), TAMANHO_NOME_CAMPO, schema);
                strcpylower(esquema[i].attApt,tupla);

                strcpylower(vetEsqm[i].tabelaApt, esquema[i].tabelaApt);
                strcpylower(vetEsqm[i].attApt, esquema[i].attApt);
                strcpylower(vetEsqm[i].nome, esquema[i].nome);
                vetEsqm[i].chave = chave;

                i++;
            } else {
                fseek(schema, 109, 1);
            }
        }
    }
    free(tupla);
	free(esquema);

	fclose(schema);

    return vetEsqm;
}

struct fs_objects leObjeto(char *nTabela){

    FILE *dicionario;
    char *tupla = (char *)malloc(sizeof(char)*TAMANHO_NOME_TABELA);
    memset(tupla, '\0', TAMANHO_NOME_TABELA);
    int cod;
    int i = 0;

    char directory[LEN_DB_NAME_IO];
    strcpy(directory, connected.db_directory);
    strcat(directory, "fs_object.dat");

    dicionario = fopen(directory, "a+b"); // Abre o dicionario de dados.

    struct fs_objects objeto;

    if(!verificaNomeTabela(nTabela)){
        printf("ERROR: relation \"%s\" was not found.\n", nTabela);
        if (dicionario)
            fclose(dicionario);
        free(tupla);

        return objeto;
    }

    if (dicionario == NULL) {
        printf("ERROR: data dictionary was not found.\n\n");
        free(tupla);
        return objeto;
    }


    while(fgetc (dicionario) != EOF){
        fseek(dicionario, -1, 1);

        fread(tupla, sizeof(char), TAMANHO_NOME_TABELA , dicionario); //Lê somente o nome da tabela

        if(objcmp(tupla, nTabela) == 0){ // Verifica se o nome dado pelo usuario existe no dicionario de dados.
            strcpylower(objeto.nome, tupla);
            fread(&cod,sizeof(int),1,dicionario);   // Copia valores referentes a tabela pesquisada para a estrutura.
            objeto.cod=cod;
            fread(tupla,sizeof(char),TAMANHO_NOME_TABELA,dicionario);
            strcpylower(objeto.nArquivo, tupla);
            fread(&cod,sizeof(int),1,dicionario);
            objeto.qtdCampos = cod;
      			fread(&i,sizeof(int),1,dicionario);
      			objeto.qtdIndice = i;

            free(tupla);
            fclose(dicionario);
            return objeto;
        }
        fseek(dicionario, 32, 1); // Pula a quantidade de caracteres para a proxima verificacao(4B do codigo, 20B do nome do arquivo e 4B da quantidade de campos).
    }
    free(tupla);
    fclose(dicionario);

    return objeto;
}
//
// LEITURA DE DICIONARIO E ESQUEMA
tp_table *leSchema (struct fs_objects objeto){
    FILE *schema;
    int i = 0, cod = 0;
    char *tupla = (char *)malloc(sizeof(char)*TAMANHO_NOME_CAMPO);
    memset(tupla, 0, TAMANHO_NOME_CAMPO);
    char *tuplaT = (char *)malloc(sizeof(char)*TAMANHO_NOME_TABELA+1);
    memset(tuplaT, 0, TAMANHO_NOME_TABELA+1);

    tp_table *esquema = (tp_table *)malloc(sizeof(tp_table)*(objeto.qtdCampos+1)); // Aloca esquema com a quantidade de campos necessarios.
    memset(esquema, 0, (objeto.qtdCampos+1)*sizeof(tp_table));
    for (i = 0; i < objeto.qtdCampos+1; i++)  esquema[i].next = NULL;

    i = 0;
    if(esquema == NULL){
        free(tupla);
        free(tuplaT);
        return ERRO_DE_ALOCACAO;
    }

    char directory[LEN_DB_NAME_IO];
    strcpy(directory, connected.db_directory);
    strcat(directory, "fs_schema.dat");

    schema = fopen(directory, "a+b"); // Abre o arquivo de esquemas de tabelas.

    if (schema == NULL){
        free(tupla);
        free(tuplaT);
        free(esquema);
        return ERRO_ABRIR_ESQUEMA;
    }

    while((fgetc (schema) != EOF) && (i < objeto.qtdCampos)){ // Varre o arquivo ate encontrar todos os campos com o codigo da tabela.
        fseek(schema, -1, 1);

        if(fread(&cod, sizeof(int), 1, schema)){ // Le o codigo da tabela.
            if(cod == objeto.cod){ // Verifica se o campo a ser copiado e da tabela que esta na estrutura fs_objects.
                fread(tupla, sizeof(char), TAMANHO_NOME_CAMPO, schema);
                strcpylower(esquema[i].nome,tupla);                  // Copia dados do campo para o esquema.

                fread(&esquema[i].tipo, sizeof(char),1,schema);
                fread(&esquema[i].tam, sizeof(int),1,schema);
                fread(&esquema[i].chave, sizeof(int),1,schema);

                fread(tuplaT, sizeof(char), TAMANHO_NOME_TABELA, schema);
                strcpylower(esquema[i].tabelaApt,tuplaT);

                fread(tupla, sizeof(char), TAMANHO_NOME_CAMPO, schema);
                strcpylower(esquema[i].attApt,tupla);

                if (i > 0) esquema[i-1].next = &esquema[i];
                i++;
            }
            else {
                fseek(schema, 109, 1); // Pula a quantidade de caracteres para a proxima verificacao (40B do nome, 1B do tipo e 4B do tamanho,4B da chave, 20B do nome da Tabela Apontada e 40B do atributo apontado).
            }
        }
    }
    esquema[i].next = NULL;

    free(tupla);
    free(tuplaT);
    fclose(schema);
    return esquema;
}
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Copia todas as informações menos a tabela com nome NomeTabela, que será removida.
    Parametros: Nome da tabela que será removida do object.dat.
    Retorno:    INT
                SUCCESS,
                ERRO_ABRIR_ARQUIVO
   ---------------------------------------------------------------------------------------------*/

int procuraObjectArquivo(char *nomeTabela){

    int teste        = 0,
        cont         = 0,
        achou        = 0,
        tamanhoTotal = 52;

    char *table = (char *)malloc(sizeof(char) * tamanhoTotal);
    FILE *dicionario, *fp;

    char directory[LEN_DB_NAME_IO];
    strcpy(directory, connected.db_directory);
    strcat(directory, "fs_object.dat");

    if((dicionario = fopen(directory,"a+b")) == NULL) {
        free(table);
        return ERRO_ABRIR_ARQUIVO;
    }

    strcpy(directory, connected.db_directory);
    strcat(directory, "fs_nobject.dat");

    if((fp = fopen(directory, "a+b")) == NULL) {
        free(table);
        return ERRO_ABRIR_ARQUIVO;
    }

    fseek(dicionario, 0, SEEK_SET);
    fseek(fp, 0, SEEK_SET);

    while(cont < quantidadeTabelas()){
        fread(table, sizeof(char), tamanhoTotal, dicionario);
        teste = TrocaArquivosObj(nomeTabela, table);

        if(teste == 0){                                         //NÃO É IGUAL
            fseek(fp, 0, SEEK_END);
            fwrite(table, sizeof(char), tamanhoTotal, fp);
        } else if(achou != 1){                                    //É IGUAL E NÃO TINHA SIDO DESCOBERTO.
            achou = 1;
            fread(table, sizeof(char), 0, dicionario);
        }
        cont++;
    }

    fclose(fp);
    fclose(dicionario);

    char directoryex[LEN_DB_NAME_IO*2];
    strcpy(directoryex, connected.db_directory);
    strcat(directoryex, "fs_object.dat");

    remove(directoryex);

    strcpy(directoryex, "mv ");
    strcat(directoryex, connected.db_directory);
    strcat(directoryex, "fs_nobject.dat ");
    strcat(directoryex, connected.db_directory);
    strcat(directoryex, "fs_object.dat");

    system(directoryex);

    free(table);
    return SUCCESS;
}
//
int tamTupla(tp_table *esquema, struct fs_objects objeto) {// Retorna o tamanho total da tupla da tabela.

    int qtdCampos = objeto.qtdCampos, i, tamanhoGeral = 0;

    if(qtdCampos){ // Lê o primeiro inteiro que representa a quantidade de campos da tabela.
        for(i = 0; i < qtdCampos; i++)
            tamanhoGeral += esquema[i].tam; // Soma os tamanhos de cada campo da tabela.
    }

    return tamanhoGeral;
}
////


// CRIA TABELA
table *iniciaTabela(char *nome){
    if(verificaNomeTabela(nome)){   // Se o nome já existir no dicionario, retorna erro.
        return ERRO_NOME_TABELA_INVALIDO;
    }

    table *t = (table *)malloc(sizeof(table)*1);
    memset(t,0,sizeof(table));
    strcpylower(t->nome,nome); // Inicia a estrutura de tabela com o nome da tabela.
    t->esquema = NULL; // Inicia o esquema da tabela com NULL.
    return t; // Retorna estrutura para criação de uma tabela.
}
////
////
table *adicionaCampo(table *t,char *nomeCampo, char tipoCampo, int tamanhoCampo, int tChave, char *tabelaApt, char *attApt){
    tp_table *e = NULL;
     // Se a estrutura passada for nula, retorna erro.
    if(t == NULL) return ERRO_ESTRUTURA_TABELA_NULA;
    tp_table *aux;
    if(t->esquema == NULL){ // Se o campo for o primeiro a ser adicionado, adiciona campo no esquema.
      e = (tp_table *)malloc(sizeof(tp_table));
      memset(e, 0, sizeof(tp_table));
      if (e == NULL) return ERRO_DE_ALOCACAO;

      e->next = NULL;
      int n = strlen(nomeCampo)+1;
      if (n > TAMANHO_NOME_CAMPO) n = TAMANHO_NOME_CAMPO;

      strncpylower(e->nome, nomeCampo,n); // Copia nome do campo passado para o esquema
      e->tipo = tipoCampo; // Copia tipo do campo passado para o esquema
      e->tam = tamanhoCampo; // Copia tamanho do campo passado para o esquema
      e->chave = tChave; // Copia tipo de chave passado para o esquema

      if(strlen(tabelaApt) >= 1)
          strcpylower(e->tabelaApt, tabelaApt); //Copia a Tabela Refenciada da FK de chave passado para o esquema;

      if(strlen(attApt) >= 1)
          strcpylower(e->attApt, attApt); //Copia o Atributo Refenciado da FK de chave passado para o esquema

      t->esquema = e;
      return t; // Retorna a estrutura
    }
    else {
        for(aux = t->esquema; aux != NULL; aux = aux->next){ // Anda até o final da estrutura de campos.
            if(aux->next == NULL){ // Adiciona um campo no final.
                e = (tp_table *)malloc(sizeof(tp_table));
                memset(e, 0, sizeof(*e));
                if (e == NULL) return ERRO_DE_ALOCACAO;
                e->next = NULL;
                int n = strlen(nomeCampo)+1;
                if (n > TAMANHO_NOME_CAMPO) n = TAMANHO_NOME_CAMPO;

                strncpylower(e->nome, nomeCampo,n); // Copia nome do campo passado para o esquema
                e->tipo = tipoCampo; // Copia tipo do campo passado para o esquema
                e->tam = tamanhoCampo; // Copia tamanho do campo passado para o esquema
                e->chave = tChave; // Copia tipo de chave passado para o esquema

                strcpylower(e->tabelaApt, tabelaApt); //Copia a Tabela Refenciada da FK de chave passado para o esquema;

                strcpylower(e->attApt, attApt); //Copia o Atributo Refenciado da FK de chave passado para o esquema

                aux->next = e; // Faz o campo anterior apontar para o campo inserido.
                return t;
            }
        }
    }

    return t; //Retorna estrutura atualizada.
}
///
int finalizaTabela(table *t){
    if(t == NULL)
        return ERRO_DE_PARAMETRO;

    FILE *esquema, *dicionario;
    tp_table *aux;
    int qtdIndice = 0;
    int codTbl = quantidadeTabelas() + 1, qtdCampos = 0; // Conta a quantidade de tabelas já no dicionario e soma 1 no codigo dessa nova tabela.
    char nomeArquivo[TAMANHO_NOME_ARQUIVO];
    memset(nomeArquivo, 0, TAMANHO_NOME_ARQUIVO);

    char directory[LEN_DB_NAME_IO];
    strcpy(directory, connected.db_directory);
    strcat(directory, "fs_schema.dat");

    if((esquema = fopen(directory,"a+b")) == NULL)
        return ERRO_ABRIR_ARQUIVO;

    for(aux = t->esquema; aux != NULL; aux = aux->next){ // Salva novos campos no esquema da tabela, fs_schema.dat

        fwrite(&codTbl         ,sizeof(codTbl)         ,1,esquema);  //Código Tabela
        fwrite(&aux->nome      ,sizeof(aux->nome)      ,1,esquema);  //Nome campo
        fwrite(&aux->tipo      ,sizeof(aux->tipo)      ,1,esquema);  //Tipo campo
        fwrite(&aux->tam       ,sizeof(aux->tam)       ,1,esquema);  //Tamanho campo
        fwrite(&aux->chave     ,sizeof(aux->chave)     ,1,esquema);  //Chave do campo
        fwrite(&aux->tabelaApt ,sizeof(aux->tabelaApt) ,1,esquema);  //Tabela Apontada
        fwrite(&aux->attApt    ,sizeof(aux->attApt)    ,1,esquema);  //Atributo apontado.

    		if(aux->chave == PK && !qtdIndice) {
    			qtdIndice++;
    		}

        qtdCampos++; // Soma quantidade total de campos inseridos.
    }

    fclose(esquema);

    strcpy(directory, connected.db_directory);
    strcat(directory, "fs_object.dat");

    if((dicionario = fopen(directory,"a+b")) == NULL)
        return ERRO_ABRIR_ARQUIVO;

    strcpylower(nomeArquivo, t->nome);
    strcat(nomeArquivo, ".dat\0");
    strcat(t->nome, "\0");
    // Salva dados sobre a tabela no dicionario.
    fwrite(&t->nome,sizeof(t->nome),1,dicionario);
    fwrite(&codTbl,sizeof(codTbl),1,dicionario);
    fwrite(&nomeArquivo,sizeof(nomeArquivo),1,dicionario);
    fwrite(&qtdCampos,sizeof(qtdCampos),1,dicionario);
    fwrite(&qtdIndice,sizeof(int),1,dicionario);

    fclose(dicionario);
    return SUCCESS;
}
////
// INSERE NA TABELA
column *insereValor(table  *tab, column *c, char *nomeCampo, char *valorCampo) {
    int i;
    column *aux;
    column *e = NULL;
    if(c == NULL){ // Se o valor a ser inserido é o primeiro, adiciona primeiro campo.
        e = (column *)malloc(sizeof(column));
        if (e == NULL) return ERRO_DE_ALOCACAO;
        memset(e, 0, sizeof(column));
        int tam = retornaTamanhoValorCampo(nomeCampo, tab);
        char tipo = retornaTamanhoTipoDoCampo(nomeCampo,tab);
        int nTam = strlen(valorCampo);
        if (tipo == 'S') {
            nTam = tam;
        }
        e->valorCampo = (char *)malloc(sizeof(char) * (nTam+1));
        if (e->valorCampo == NULL) {
            free(e);
            return ERRO_DE_ALOCACAO;
        }
        int n = strlen(nomeCampo)+1;

        /**
         * Verifica se o nome ultrapassa o limite, se sim trunca
         */
        if (n > TAMANHO_NOME_CAMPO) {
           n = TAMANHO_NOME_CAMPO;
           printf("WARNING: field name exceeded the size limit and was truncated.\n");
        }
        strncpylower(e->nomeCampo, nomeCampo, n-1);
        n = strlen(valorCampo);
        if (n > tam && tipo == 'S') {
            n = tam;
            printf("WARNING: value of column \"%s\" exceeded the size limit and was truncated.\n", nomeCampo);
        }

        for(i=0; i < n; i++) e->valorCampo[i] = valorCampo[i];
            e->valorCampo[i] = '\0';

        //strncpy(e->valorCampo, valorCampo,n);
        e->next = NULL;
        c = e;
        return c;
    } else {
        for(aux = c; aux != NULL; aux = aux->next) { // Anda até o final da lista de valores a serem inseridos e adiciona um novo valor.
            if(aux->next == NULL){

                e = (column *)malloc(sizeof(column));

                if (e == NULL) {
                    return ERRO_DE_ALOCACAO;
                }

                memset(e, 0, sizeof(column));

                int tam = retornaTamanhoValorCampo(nomeCampo, tab);
                char tipo = retornaTamanhoTipoDoCampo(nomeCampo,tab);

                int nTam = strlen(valorCampo);

                if (tipo == 'S') {
                    nTam = tam;
                }

                e->valorCampo = (char *) malloc (sizeof(char) * (nTam+1));

                if (e->valorCampo == NULL) {
                    free(e);
                    return ERRO_DE_ALOCACAO;
                }

                e->next = NULL;

                int n = strlen(nomeCampo)+1;

                /**
                 * Verifica se o nome do campo ultrapassa o limite, se sim trunca
                 */
                if (n > TAMANHO_NOME_CAMPO) {
                   n = TAMANHO_NOME_CAMPO;
                   printf("WARNING: field name exceeded the size limit and was truncated.\n");
                }

                strncpylower(e->nomeCampo, nomeCampo, n-1);

                //strncpy(e->nomeCampo, nomeCampo,n);

                n = strlen(valorCampo);

                if (n > tam && tipo == 'S') {
                    n = tam;
                    printf("WARNING: value of column \"%s\"exceeded the size limit and was truncated.\n", nomeCampo);
                }

                for(i=0; i < n; i++) e->valorCampo[i] = valorCampo[i];
                e->valorCampo[i] = '\0';

                //strncpy(e->valorCampo, valorCampo,n);
                aux->next = e;
                return c;
            }
        }
    }

    if (e) free(e);
    return ERRO_INSERIR_VALOR;
}
//////
/*------------------------------------------------------------------------------------------
Objetivo: Mostrar as tabelas do banco de dados ou, em específico, os atributos de uma tabela
------------------------------------------------------------------------------------------*/

void printTable(char *tbl){
	if(tbl == NULL){     //mostra todas as tabelas do banco
		FILE *dicionario;
		printf("		List of Relations\n");
		char *tupla = (char *)malloc(sizeof(char)*TAMANHO_NOME_TABELA);

		char directory[LEN_DB_NAME_IO];
    	strcpy(directory, connected.db_directory);
    	strcat(directory, "fs_object.dat");

		if((dicionario = fopen(directory,"a+b")) == NULL){
			free(tupla);
			printf("ERROR: cannot open file\n");
			return;
		}

		printf(" %-10s | %-15s | %-10s | %-10s\n", "Schema", "Name", "Type", "Owner");
		printf("------------+-----------------+------------+-------\n");
		int i=0;
		while(fgetc (dicionario) != EOF){
			fseek(dicionario, -1, 1);
			fread(tupla, sizeof(char), TAMANHO_NOME_TABELA, dicionario);
			printf(" %-10s | %-15s | %-10s | %-10s \n", "public", tupla, "tuple", connected.db_name);
			fseek(dicionario, 32, 1);
			i++;
		}
		fclose(dicionario);
		free(tupla);
		printf("(%d %s)\n\n", i, (i<=1)? "row": "rows");
	} else{               //mostra todos atributos da tabela *tbl

		if(!verificaNomeTabela(tbl)) {
			printf("Did not find any relation named \"%s\".\n", tbl);
			return;
		}

		printf("	  Table \"public.%s\"\n", tbl);
		printf(" %-18s | %-12s | %-10s\n", "Column", "Type", "Modifiers");
		printf("--------------------+--------------+-----------\n");

		struct fs_objects objeto1;
		tp_table *esquema1;

		abreTabela(tbl, &objeto1, &esquema1);

		tp_table *tab3 = (tp_table *)malloc(sizeof(struct tp_table));
		tab3 = procuraAtributoFK(objeto1); //retorna tp_table
		int l, ipk=0, ifk=0, ibt=0;

		char **pk 			= (char**)malloc(objeto1.qtdCampos*sizeof(char**));
		char **fkTable		= (char**)malloc(objeto1.qtdCampos*sizeof(char**));
		char **fkColumn 	= (char**)malloc(objeto1.qtdCampos*sizeof(char**));
		char **refColumn 	= (char**)malloc(objeto1.qtdCampos*sizeof(char**));
    char **btIndex		= (char**)malloc(objeto1.qtdIndice*sizeof(char*));

		memset(pk 		, 0, objeto1.qtdCampos);
		memset(fkTable 	, 0, objeto1.qtdCampos);
		memset(fkColumn , 0, objeto1.qtdCampos);
		memset(refColumn, 0, objeto1.qtdCampos);
    memset(btIndex, 0, objeto1.qtdIndice);

		int i;
		for(i=0; i<objeto1.qtdCampos; i++) {
			pk[i] 			= (char*)malloc(TAMANHO_NOME_CAMPO*sizeof(char));
			fkTable[i] 		= (char*)malloc(TAMANHO_NOME_CAMPO*sizeof(char));
			fkColumn[i] 	= (char*)malloc(TAMANHO_NOME_CAMPO*sizeof(char));
			refColumn[i] 	= (char*)malloc(TAMANHO_NOME_CAMPO*sizeof(char));

			memset(pk[i] 		, '\0', TAMANHO_NOME_CAMPO);
			memset(fkTable[i] 	, '\0', TAMANHO_NOME_CAMPO);
			memset(fkColumn[i]  , '\0', TAMANHO_NOME_CAMPO);
			memset(refColumn[i] , '\0', TAMANHO_NOME_CAMPO);

		}

    for(i=0; i<objeto1.qtdIndice; i++) {
      btIndex[i] = (char*)malloc (TAMANHO_NOME_CAMPO*sizeof(char));
    }

		for(l=0; l<objeto1.qtdCampos; l++) {

			if(tab3[l].chave == PK){
				strcpylower(pk[ipk++], tab3[l].nome);
			}
			else if(tab3[l].chave == FK){
				strcpylower(fkTable[ifk]	, tab3[l].tabelaApt);
				strcpylower(fkColumn[ifk]	, tab3[l].attApt);
				strcpylower(refColumn[ifk++], tab3[l].nome);
			}
      else if(tab3[l].chave == BT){
        strcpylower(btIndex[ibt++], tab3[l].nome);
      }

			printf("  %-17s |", tab3[l].nome);

			if(tab3[l].tipo == 'S')
				printf(" %-8s(%d) |", " varchar", tab3[l].tam);
			else if(tab3[l].tipo == 'I')
				printf(" %-10s   |", " integer");
			else if(tab3[l].tipo == 'C')
				printf(" %-10s   |", " char");
			else if(tab3[l].tipo == 'D')
				printf(" %-10s   |", " double");

			printf(" %-10s ", (tab3[l].chave == PK || tab3[l].chave == FK)? "not null": "null");

			printf("\n");
		}
    if(ipk || ibt)
      printf("Indexes:\n");
		if(ipk){	//printf PK's
			for(l = 0; l < ipk; l++){
				printf("\t\"%s_pkey\" PRIMARY KEY (%s)\n", tbl, pk[l]);
			}
		}
    if(ibt){
      for(l = 0; l < ibt; l++){
        printf("\t\"%s\" BTREE INDEX\n", btIndex[l]);
      }
    }
		if(ifk){	//printf FK's
			printf("Foreign-key constrains:\n");
			for(l = 0; l < ifk; l++){
				printf("\t\"%s_%s_fkey\" FOREIGN KEY (%s) REFERENCES %s(%s)\n",tbl, refColumn[l], refColumn[l], fkTable[l], fkColumn[l]);
			}
		}
		
		for(i=0; i < objeto1.qtdIndice; i++)	free(btIndex[i]);
		
		free(pk);
		free(btIndex);
		free(fkTable);
		free(fkColumn);
		free(refColumn);
		free(tab3);
		printf("\n");
	}
}

void incrementaQtdIndice(char *nTabela){
  FILE *dicionario = NULL;
  char tupla[TAMANHO_NOME_TABELA];
  memset(tupla, '\0', TAMANHO_NOME_TABELA);
  int qt = 0, offset = 0;

  char directory[LEN_DB_NAME_IO];
  strcpy(directory, connected.db_directory);
  strcat(directory, "fs_object.dat");

  if((dicionario = fopen(directory,"r+b")) == NULL){
    printf("Erro ao abrir dicionário de dados.\n");
    return;
  }

  while(fgetc (dicionario) != EOF){
      fseek(dicionario, -1, 1);

      fread(tupla, sizeof(char), TAMANHO_NOME_TABELA , dicionario); //Lê somente o nome da tabela
      if(strcmp(tupla, nTabela) == 0){
          fseek(dicionario,sizeof(int),SEEK_CUR);
          fseek(dicionario,sizeof(char)*TAMANHO_NOME_ARQUIVO,SEEK_CUR);
          fseek(dicionario,sizeof(int),SEEK_CUR);
          offset = ftell(dicionario);
          fread(&qt, sizeof(int), 1, dicionario);
          qt++;
          fseek(dicionario,offset,SEEK_SET);
          fwrite(&qt,sizeof(int),1,dicionario);
          fclose(dicionario);
          return;
      }
      fseek(dicionario, 32, 1); // Pula a quantidade de caracteres para a proxima verificacao(4B do codigo, 20B do nome do arquivo e 4B da quantidade de campos).
  }
  printf("Erro ao atualizar dicionário de dados.\n");
}


void adicionaBT(char *nomeTabela, char *nomeAtrib) {
  int cod;
  char directory[LEN_DB_NAME_IO];
  strcpy(directory, connected.db_directory);
  strcat(directory, "fs_schema.dat");

  char atrib[TAMANHO_NOME_CAMPO];
  memset(atrib, '\0', TAMANHO_NOME_CAMPO);


  FILE *schema = fopen(directory, "r+b"); // Abre o arquivo de esquemas de tabelas.

  if (schema == NULL){
      printf("Erro ao abrir esquema da tabela.\n");
      return;
  }
  struct fs_objects objeto = leObjeto(nomeTabela);

  while(fgetc (schema) != EOF){ // Varre o arquivo ate encontrar todos os campos com o codigo da tabela.
      fseek(schema, -1, 1);

      if(fread(&cod, sizeof(int), 1, schema)){ // Le o codigo da tabela.
          if(cod == objeto.cod){ // Verifica se o campo a ser copiado e da tabela que esta na estrutura fs_objects.
              fread(atrib, sizeof(char), TAMANHO_NOME_CAMPO, schema);
              if(strcmp(atrib, nomeAtrib) == 0) {
                //Pula até a posição de tp_table.chave (1B do tipo e 4B do tam)
                fseek(schema, 5, SEEK_CUR);
                int chave = BT;
                fwrite(&chave,sizeof(int),1,schema);
                fclose(schema);
                return;
              } else {
                // pula 1B do tipo, 4B do tamanho,4B da chave, 20B do nome da Tabela Apontada e 40B do atributo apontado
                fseek(schema, 69, SEEK_CUR);
              }

          }
          else {
              fseek(schema, 109, 1); // Pula a quantidade de caracteres para a proxima verificacao (40B do nome, 1B do tipo e 4B do tamanho,4B da chave, 20B do nome da Tabela Apontada e 40B do atributo apontado).
          }
      }
  }

  printf("Erro ao abrir o esquema da tabela \"%s\".\n", nomeTabela);
}
/* NÃO IMPLEMENTADO: a ideia era dar o free de maneira correta nas listas alocadas ao longo
 * da execução do programa, já que os grupos anteriores não se preocuparam com isso. No entanto,
 * não tivemos tempo ágil para implementar uma solução livre de erros e preferimos deixar
 * dessa maneira.
 */

void freeTp_table(tp_table **tabela, int n) {
	free(tabela);
}


void freeTable(table *tabela) {
	if (tabela != NULL) {
		free(tabela->esquema);
		free(tabela);
	}
}


void freeColumn(column *colunas) {
	if (colunas != NULL) {
		free(colunas);
	}
}
