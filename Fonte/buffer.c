#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef FMACROS // garante que macros.h não seja reincluída
   #include "macros.h"
#endif
///
#ifndef FTYPES // garante que types.h não seja reincluída
  #include "types.h"
#endif

#include "misc.h"
#include "dictionary.h"

static int isDeleted(char *linha);

// INICIALIZACAO DO BUFFER
tp_buffer * initbuffer(){

    tp_buffer *bp = (tp_buffer*)malloc(sizeof(tp_buffer)*PAGES);
    memset(bp, 0, sizeof(tp_buffer)*PAGES);

    int i;
    tp_buffer *temp = bp;

    if(bp == NULL)
        return ERRO_DE_ALOCACAO;
    for (i = 0;i < PAGES; i++){
        temp->db=0;
        temp->pc=0;
        temp->nrec=0;
        temp++;
    }

    return bp;
}
//// imprime os dados no buffer (deprecated?)
int printbufferpoll(tp_buffer *buffpoll, tp_table *s,struct fs_objects objeto, int num_page){

    int aux, i, num_reg = objeto.qtdCampos;

    if(buffpoll[num_page].nrec == 0){
        return ERRO_IMPRESSAO;
    }

    i = aux = 0;
    aux = cabecalho(s, num_reg);
    while(i < buffpoll[num_page].nrec){ // Enquanto i < numero de registros * tamanho de uma instancia da tabela
        drawline(buffpoll, s, objeto, i, num_page);
        i++;
    }
    return SUCCESS;
}

// RETORNA PAGINA DO BUFFER
tupla *getPage(tp_buffer *buffer, tp_table *campos, struct fs_objects objeto, int page){

    if(page >= PAGES) return ERRO_PAGINA_INVALIDA;

    if(buffer[page].nrec == 0) //Essa página não possui registros
        return ERRO_PARAMETRO;

    tupla *tuplas = (tupla *)malloc(sizeof(tupla) * (buffer[page].nrec)); //Aloca a quantidade de tuplas necessária

    if(!tuplas)
        return ERRO_DE_ALOCACAO;

    int  indiceTupla=0, i=0;

    if (!buffer[page].position)
        return tuplas;

    char* nullos =(char *)malloc(objeto.qtdCampos * sizeof(char));

    while(i < buffer[page].position){
        
        if(isDeleted(buffer[page].data + i)) {
            i+=tamTupla(campos, objeto);
            continue;
        }
        tuplas[indiceTupla].endereco = i; //para o 

        i++; //para o byte de deleted
        memcpy(nullos, buffer[page].data + i, objeto.qtdCampos);
        i += objeto.qtdCampos;


        tuplas[indiceTupla].column = (column *)malloc(sizeof(column) * objeto.qtdCampos);
        for (int ic = 0; ic < objeto.qtdCampos; ic++){
            column *c = &tuplas[indiceTupla].column[ic];

            c->tipoCampo = campos[ic].tipo;
            strcpy(c->nomeCampo, campos[ic].nome); //Guarda nome do campo
            if(nullos[ic]) c->valorCampo = COLUNA_NULL;
            else {
                c->valorCampo = (char *)malloc(sizeof(char) * campos[ic].tam + 1);
                memcpy(c->valorCampo, buffer[page].data + i, campos[ic].tam);
                c->valorCampo[campos[ic].tam] = '\0';
            }
            i += campos[ic].tam;
        }
    
        indiceTupla++;
    }
    return tuplas; //Retorna a 'page' do buffer
}
// EXCLUIR TUPLA BUFFER
column * excluirTuplaBuffer(tp_buffer *buffer, tp_table *campos, struct fs_objects objeto, int page, int nTupla){
    column *tuplas = (column *)malloc(sizeof(column)*objeto.qtdCampos);

    if(tuplas == NULL)
        return ERRO_DE_ALOCACAO;

    if(buffer[page].nrec == 0) //Essa página não possui registros
        return ERRO_PARAMETRO;

    int i, tamTpl = tamTupla(campos, objeto), j=0, t=0;
    i = tamTpl*nTupla; //Calcula onde começa o registro

    while(i < tamTpl*nTupla+tamTpl){
        t=0;

        tuplas[j].valorCampo = (char *)malloc(sizeof(char)*campos[j].tam); //Aloca a quantidade necessária para cada campo
        tuplas[j].tipoCampo = campos[j].tipo;  // Guarda o tipo do campo
        strcpylower(tuplas[j].nomeCampo, campos[j].nome);   //Guarda o nome do campo

        while(t < campos[j].tam){
            tuplas[j].valorCampo[t] = buffer[page].data[i];    //Copia os dados
            t++;
            i++;
        }
        j++;
    }
    j = i;
    i = tamTpl*nTupla;
    for(; i < buffer[page].position; i++, j++) //Desloca os bytes do buffer sobre a tupla excluida
        buffer[page].data[i] = buffer[page].data[j];

    buffer[page].position -= tamTpl;
    buffer[page].nrec--;

    return tuplas; //Retorna a tupla excluida do buffer
}
// INSERE UMA TUPLA NO BUFFER!
char *getTupla(tp_table *campos,struct fs_objects objeto, int from){ //Pega uma tupla do disco a partir do valor de from
    // + qtdCampos para os bytes de coluna null e +1 para o byte de tupla valida
    int tamTpl = tamTupla(campos, objeto); 
    char *linha=(char *)malloc(sizeof(char)*tamTpl);

    FILE *dados;
    from = from * tamTpl;
	char directory[LEN_DB_NAME_IO];
    strcpy(directory, connected.db_directory);
    strcat(directory, objeto.nArquivo);

    dados = fopen(directory, "r");

    if (dados == NULL) {
        free(linha);
        return ERRO_DE_LEITURA;
    }

    fseek(dados, from, SEEK_CUR);
    if(fgetc (dados) == EOF){
        fclose(dados);
        free(linha);
        return ERRO_DE_LEITURA;
    }
    
    fseek(dados, -1, SEEK_CUR);
    fread(linha, sizeof(char), tamTpl, dados); //Traz a tupla inteira do arquivo


  

    fclose(dados);
    return linha;
}
/////
void setTupla(tp_buffer *buffer,char *tupla, int tam, int pos) { //Coloca uma tupla de tamanho "tam" no buffer e na página "pos"
  int i = buffer[pos].position;
  for (; i < buffer[pos].position + tam; i++)
    buffer[pos].data[i] = *(tupla++);
}
//// insere uma tupla no buffer
int colocaTuplaBuffer(tp_buffer *buffer, int from, tp_table *campos, struct fs_objects objeto){//Define a página que será incluida uma nova tupla
    int i, found;
    char *tupla = getTupla(campos, objeto, from);
    if(tupla == ERRO_DE_LEITURA)  return ERRO_LEITURA_DADOS;

    int tam = tamTupla(campos, objeto);

    for(i = found = 0; !found && i < PAGES; i++) {//Procura pagina com espaço para a tupla.
        if(SIZE - buffer[i].position > tam) {// Se na pagina i do buffer tiver espaço para a tupla, coloca tupla.
            setTupla(buffer, tupla, tam, i);
            found = 1;
            buffer[i].position += tam; // Atualiza proxima posição vaga dentro da pagina.
            if(isDeleted(tupla)) {
                free(tupla);
                return ERRO_LEITURA_DADOS_DELETADOS;
            }
             buffer[i].nrec++;
        }
    }
    free(tupla);
    return found ? SUCCESS : ERRO_BUFFER_CHEIO;
}
////////

void cria_campo(int tam, int header, char *val, int x) {
  int i;
  char aux[30];
  if(header){
    for(i = 0; i <= 30 && val[i] != '\0'; i++) aux[i] = val[i];
    for(;i < 30;i++) aux[i] = ' ';
    aux[i] ='\0';
    printf("%s", aux);
    return;
  }
  for(i = 0; i < x; i++) printf(" ");
}

static int isDeleted(char *linha){
    return linha[0]; //byte se foi deletado
}

void addColumn(column **colList, column *c){
    c->next = NULL;
    if(*colList == NULL) {
        *colList = c;
        return;
    }
    column *t = *colList;
    while(t->next != NULL) t = t->next;
    
    t->next = c;
}