#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
////
#ifndef FBTREE // includes only if this flag is not defined (preventing duplication)
   #include "btree.h"
#endif
int ordem;
int ntuplas;
//cria novo nodo vazio
void decnTuplas()
{
	ntuplas--;
}

nodo* criaNodo() {
	nodo * novo = NULL;
	novo = (nodo*)malloc(sizeof(nodo));
	novo->filhos = NULL;
	novo->pai = novo->prox = novo->ant = NULL;
	novo->data = (char**)malloc(ordem * sizeof(char*));
	novo->endereco = (long int*)malloc(ordem * sizeof(long int));
	novo->quant_data = 0;
	return novo;
}

//recursão de impressão em níveis
void imprime_arvore(nodo* n, int nivel) {
	int i;
	if (!n) return;
	for (i = 0; i < nivel; i++) printf("*");
	for (i = 0; i < n->quant_data; i++) {
		if (i) printf(" | ");
		printf("%s", n->data[i]);
	}
	printf("\n");
	if (n->filhos) {
		for (i = 0; i <= n->quant_data; i++)
			imprime_arvore(n->filhos[i], nivel+1);
	}
}
//base da recursão imprime_arvore
void imprime(nodo* n) {
	imprime_arvore(n, 1);
	printf("---\n");
}

//recebe o nome de uma tabela, concatena a extensão .dat e retorna essa junção.
char* concatena_extensao(char* nomeTabela) {
	char *concatena;
	concatena = (char*)malloc(sizeof(char)*(strlen(nomeTabela) + strlen(".dat")+1));
	strcpy(concatena,nomeTabela);
	strcat(concatena,".dat");
	return concatena;
}

//quando o usuário cria uma tabela, está função deve ser chamada para criar o arquivo de indice.
int inicializa_indice(char* nomeTabela){
	char arroba = '@';
	FILE * new_indice = NULL;
	char* nomeArquivo = concatena_extensao(nomeTabela);
	new_indice = fopen(nomeArquivo, "a");
	fwrite(&arroba, sizeof(char), 1, new_indice);
	free(nomeArquivo);
	if(new_indice == NULL) return 0;
	fclose(new_indice);
	return 1;
}

/* Função comparação entre strings para uso na função Qsort */
int cmpstr (const void *a, const void *b){
	const char **ia = (const char **)a;
	const char **ib = (const char **)b;
	return strcmp(*ia, *ib);
}

/* Insere os valores da chave (ind) e do endereço da tupla no arquivo de dados
 * (end) em um nodo (n) */
nodo* insereChaveEmNodoFolha(char* ind, long int end, nodo *n){
	n->data[n->quant_data] = (char *)malloc((strlen(ind)+1) * sizeof(char));
	strcpy(n->data[n->quant_data], ind);
	n->endereco[n->quant_data] = end;
	n->quant_data++;
	// Ordenação de índices primários
	qsort(n->data, n->quant_data, sizeof(char *), cmpstr);
	return n;
}

/* Insere unicamente o valor da chave em um nodo interno (n) */
nodo* insereChaveEmNodoInterno(char* ind, nodo* n) {
	n->data[n->quant_data] = (char *)malloc((strlen(ind)+1) * sizeof(char));
	strcpy(n->data[n->quant_data], ind);
	n->quant_data++;
	n->filhos = (nodo**)realloc(n->filhos, sizeof(nodo**) * (n->quant_data+1)); //aumenta o numero de ponteiros para filhos
	return n;
}

/* Recebe a raiz para a árvore e o valor de uma chave. Tenta encontrar a chave
 * na árvore. Se encontrar retorna 1, caso contrário retorna 0. */
int buscaChaveBtree(nodo* raiz, char* ind) {
	int i;
	nodo* aux = raiz;
	if (!raiz) return 0; //arvore vazia
	while (aux->filhos != NULL) { //desce na arvore até chegar na folha
		for (i = 0; i < aux->quant_data; i++) { //anda dentro do nodo
			if (strcmp(ind, aux->data[i]) < 0) { //se é menor que a chave na posição i
				aux = aux->filhos[i]; //desce para a esquerda
				break;
			}
		}
		//se não é menor que nenhum valor do nodo, desce pelo ponteiro mais à direita
		if (i == aux->quant_data) {
			aux = aux->filhos[aux->quant_data];
		}
	}
	for (i = 0; i < aux->quant_data; i++) {
		if (strcmp(aux->data[i], ind) == 0) return 1;
	}
	return 0;
}


/* Busca a posição de inserção de uma chave (ind) e um endereço (end) na árvore.
 * Retorna um ponteiro para o índice denso (lista de folhas). */
nodo* busca_insere(nodo* raiz, char* ind, int end) {
	int i;
	nodo* aux = raiz;
	if (!raiz) return NULL; //arvore vazia
	while (aux->filhos != NULL) { //desce na arvore até chegar na folha
		for (i = 0; i < aux->quant_data; i++) { //anda dentro do nodo
			if (strcmp(ind, aux->data[i]) < 0) { //se é menor que a chave na posição i
				aux = aux->filhos[i]; //desce para a esquerda
				break;
			}
		}
		//se não é menor que nenhum valor do nodo, desce pelo ponteiro mais à direita
		if (i == aux->quant_data) {
			aux = aux->filhos[aux->quant_data];
		}
	}
	//OBS: abordagem ignora se realmente há espaço no nodo para a inserção. Motivos:
	//1. A árvore é destruída após a operação (SELECT, INSERT...)
	//2. O retorno desta função é um ponteiro para o indice denso, e não para a árvore de maneira organizada.
	aux = insereChaveEmNodoFolha(ind, end, aux);
	while (aux->ant) aux = aux->ant; //retorna ao inicio do indice denso
	return aux;
}

//destruição recursiva dos nodos da árvore
void destroi_arvore(nodo* n) {
	int i;
	if(n->filhos) {//desce em profundidade
		for(i = 0; i < n->quant_data; i++)
			destroi_arvore(n->filhos[i]);
	}
	free(n->data);
	if(n->prox) { //se é nodo folha
		n->prox->ant = NULL;
		free(n->endereco);
	} else { //se é nodo interno
		free(n->filhos);
	}
	free(n);
}

//constrói uma b+ dado o nome da tabela; retorna o nodo raiz
nodo* constroi_bplus(char* nomeTabela){
	FILE * new = NULL;
	char le;
	int le2 = 0, cont = 0, flag = 0;
	char* palavra;
	nodo *aux = NULL;
	nodo *raiz = NULL;
	nodo *aux2 = NULL;
	nodo *aux3 = NULL;
	nodo *aux4 = NULL;
	nodo *aux5 = NULL;

	char* nomeArquivo = concatena_extensao(nomeTabela);
	new = fopen(nomeArquivo,"r");
	free(nomeArquivo);
	if(!new){
		printf("Erro de abertura de arquivo\n");
		fclose(new);
		return NULL;
	}
	fread(&le, sizeof(char),1,new);
	if(le == '@'){
		fclose(new);
		ntuplas = 0;
		ordem = 0;
		return NULL;
	}
	//CALCULO DA ORDEM
	fseek(new,0,SEEK_END);
	fread(&ntuplas, sizeof(int),1,new);
	ordem = calculaOrdem(ntuplas);

	fseek(new,0,SEEK_SET);

	while(1){
		palavra = (char*)malloc(sizeof(char));
		while(le != '$'){
			fread(&le, sizeof(char),1,new);
			if(le != '$') {
				palavra = (char *) realloc(palavra, (++cont) * sizeof(char));
				palavra[cont-1] = le;
			}
		}
		palavra = (char *) realloc(palavra, (++cont) * sizeof(char));
		palavra[cont-1] = '\0';
		cont = 0;
		fread(&le2, sizeof(long int), 1, new); //lê o endereço
		fread(&le, sizeof(char), 1, new); //lê o caractere especial '#'
		if(aux == NULL){ //árvore está vazia
			aux = criaNodo();
			aux = insereChaveEmNodoFolha(palavra,le2,aux);
			aux3 = aux;
		}
 		else if(aux->quant_data < ordem-1){ //há espaço no nodo atual
			aux = insereChaveEmNodoFolha(palavra,le2,aux);
		}
		else { //nodo folha atual estourou a capacidade
			aux->prox = criaNodo();
			aux->prox = insereChaveEmNodoFolha(palavra,le2,aux->prox);
			aux->prox->ant = aux;
			if(aux->pai == NULL) { //se não há mais que um nível na árvore
				aux->prox->pai = criaNodo();
				aux->prox->pai = insereChaveEmNodoInterno(palavra,aux->prox->pai);
				aux->prox->pai->filhos[aux->prox->pai->quant_data - 1] = aux; //reposiciona os ponteiros para o filho a esquerda
				aux->prox->pai->filhos[aux->prox->pai->quant_data] = aux->prox; //filho a direita
				aux->pai = aux->prox->pai;
			}
			else{
				flag=0;
				aux4 = aux;
				aux2 = aux->prox;
				while(aux->pai){
					if(aux->pai->quant_data < ordem - 1){ //há espaço no pai para a colocação da chave do novo nodo
						aux->pai = insereChaveEmNodoInterno(palavra,aux->pai);
						aux2->pai = aux->pai;
						aux->pai->filhos[aux->pai->quant_data] = aux2;
						flag= 1;
						break;
					}
					else { //estourou a capacidade do nodo interno pai
						aux2->pai = criaNodo();
						aux2->pai = insereChaveEmNodoInterno(palavra,aux2->pai);
						aux2->pai->filhos[aux2->pai->quant_data-1] = aux;
						aux2->pai->filhos[aux2->pai->quant_data] = aux2;
						aux5 = aux;
						aux = aux->pai;
						aux5->pai = aux2->pai;
						aux2 = aux2->pai;
						palavra = (char *)realloc(palavra, sizeof(char)*strlen(aux->data[aux->quant_data-1]));
						strcpy(palavra, aux->data[aux->quant_data-1]);
						aux->quant_data--;
					}
				}
				if(flag == 0){
					aux2->pai = criaNodo();
					aux2->pai = insereChaveEmNodoInterno(palavra,aux2->pai);
					aux2->pai->filhos[aux2->pai->quant_data-1] = aux;
					aux2->pai->filhos[aux2->pai->quant_data] = aux2;
					aux->pai = aux2->pai;
				}
				flag = 0;
			}
		}
		free(palavra);
		if(aux4)aux = aux4;
		while (aux->prox) aux = aux->prox;
		fread(&le, sizeof(char), 1, new);// tenta ler o fim do arquivo
		if(le == '&') break;
		fseek(new,-1,SEEK_CUR);
	}
	raiz = aux3;
	while(raiz->pai){
		raiz = raiz->pai;
	}
	return raiz;
}

// insere os dados do nodo folha no arquivo de indices Da tabela
void insere_arquivo(nodo* inicio, char* nomeTabela){
	char char1 = '$';
	char char2 = '#';
	int i;
	nodo *aux = NULL;
	aux = inicio;
	FILE * new = NULL;
	char* nomeArquivo = concatena_extensao(nomeTabela);
	new = fopen(nomeArquivo,"w");
	free(nomeArquivo);
	if(!new){
		printf("Erro de abertura de arquivo\n");
		return;
	}
	fseek(new,0,SEEK_SET);
	while(aux){
		i = 0;
		while(i < aux->quant_data){
			fwrite(aux->data[i],sizeof(char),strlen(aux->data[i]),new);
			fwrite(&char1,sizeof(char),1,new);
			fwrite(&aux->endereco[i],sizeof(long int),1,new);
			fwrite(&char2,sizeof(char),1,new);
			i++;
		}
		aux = aux->prox;
	}
	char2 = '&';
	fwrite(&char2,sizeof(char),1,new);
	ntuplas++;
	fwrite(&ntuplas,sizeof(int),1,new);
	fclose(new);
}

//insere o indice e o endereço no arquivo de indices
void insere_indice(nodo* raiz, char* ind, char* nomeTabela, long int end){
	nodo * aux;
	aux = busca_insere(raiz,ind,end); // retorna inicio da lista dos folhas
	if(aux == NULL){
		 aux = criaNodo();//arvore vazia
		 aux = insereChaveEmNodoFolha(ind,end,aux);
	}
	insere_arquivo(aux,nomeTabela);
	destroi_arvore(aux);
}

/* Calculo da Ordem da B+ a partir do número de Tuplas */
/* Formula para o cálculo: Log ordem (nTuplas) <= 5 */
int calculaOrdem (int nTuplas) {
 int ordem;
 float aux, aux2;
 aux = (pow(nTuplas, 0.2));   //Raiz quinta de nTuplas (onde 5 corresponde à altura máxima da arvore)
 /*Calculo do teto do valor gerado em aux */
 aux2 = aux - (int) aux;
 if(aux2 > 0) ordem = aux+1;
 else ordem = aux;
 return ordem;
}
