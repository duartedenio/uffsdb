/**
* estrutura do nodo da arvore b+
**/
typedef struct nodo{
	char **data;
	long int *endereco;
	struct nodo *pai, *prox, *ant;
	struct nodo **filhos;
	int quant_data;
} nodo;

/* Cria novo nodo vazio e retorna-o. */
nodo* criaNodo();

/* Recursão de impressão em níveis. Recebe um nodo da árvore e o nível
 * correspondente. */
void imprime_arvore(nodo* n, int nivel);

/* Base da recursão imprime_arvore. Recebe o nodo raiz da árvore. */
void imprime(nodo* n);

/* Recebe o nome de uma tabela, concatena a extensão .dat e retorna essa junção. */
char* concatena_extensao(char* nomeTabela);

/* Quando o usuário cria uma tabela, esta função deve ser chamada para criar o
 * arquivo de indice. Recebe uma string com o nome da tabela. retorna 1 em caso
 * de sucesso e 0 caso contrário.*/
int inicializa_indice(char* nomeTabela);

/* Insere os valores da chave (ind) e do endereço da tupla no arquivo de dados
 * (end) em um nodo (n). Retorna o mesmo nodo com a nova chave inclusa. */
nodo* insereChaveEmNodoFolha(char* ind, long int end, nodo *n);

/* Insere unicamente o valor da chave em um nodo interno (n). Retorna o mesmo
 * nodo com a nova chave inclusa. */
nodo* insereChaveEmNodoInterno(char* ind, nodo* n);

/* Busca a posição de inserção de uma chave (ind) e um endereço (end) na árvore.
 * Efetua a inserção e retorna um ponteiro para o início do índice denso
 * (lista de folhas). */
nodo* busca_insere(nodo* raiz, char* ind, int end);

/* Destruição recursiva dos nodos da árvore. */
void destroi_arvore(nodo* n);

/* Constrói uma b+ a partir do arquivo de indice, dado o nome da tabela.
 * Retorna o nodo raiz da árvore. */
nodo* constroi_bplus(char* nomeTabela);

/* Insere os dados do nodo folha no arquivo de indices da tabela. */
void insere_arquivo(nodo* inicio, char* nomeTabela);

/* Insere a chave (ind) e o endereço (end) no arquivo de indices. */
void insere_indice(nodo* raiz, char* ind, char* nomeTabela, long int end);

int buscaChaveBtree(nodo* raiz, char* ind);

int calculaOrdem (int nTuplas);

void decnTuplas();

//#ifndef FBTREE  // declare the variables only if this flag is not defined (preventing duplication)
//#define FBTREE 1  
//    int ordem;
//    int ntuplas;
//#endif



