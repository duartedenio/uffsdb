#define FDICTIONARY 1 // para testar se já foi incluída

int existeArquivo(const char* );
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Verifica a existência do atributo antes de adicionar na tabela
    Parametros: Nome da tabela, coluna C.
    Retorno:    INT
                SUCCESS,
                ERRO_DE_PARAMETRO
   ---------------------------------------------------------------------------------------------*/
int existeAtributo(char *, column *);
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Retorna vetor de esquemas com todos os atributos chaves (PK, FK e NPK)
    Parametros: Objeto da tabela.
    Retorno:    Vetor de esquemas vetEsqm
   ---------------------------------------------------------------------------------------------*/
tp_table *procuraAtributoFK(struct fs_objects);
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Copia todas as informações menos a tabela com nome NomeTabela, que será removida.
    Parametros: Nome da tabela que será removida do object.dat.
    Retorno:    INT
                SUCCESS,
                ERRO_ABRIR_ARQUIVO
   ---------------------------------------------------------------------------------------------*/
int procuraObjectArquivo(char *);
/*
    Esta função busca, no arquivo fs_object.dat, pelo nome da tabela retornando as informações que
    estão no dicionário em uma estrutura fs_objects. Caso o nome da tabela não exista, o programa
    aborta.

    *nTabela - Nome da tabela a ser buscado no dicionário de dados
*/
struct fs_objects leObjeto(char *);
/*
    Esta função busca, no arquivo fs_schema.dat, pelas informações do objeto, carregando o esquema
    da tabela que é retornadado em tp_table.
    Caso o nome da tabela não exista, o programa aborta
    *objeto - Objeto, já previamente inicializado em leObjeto(nTabela), que contém as informações
              sobre uma determinada tabela.
*/
tp_table *leSchema (struct fs_objects );
/*
    Esta função calcula, usando o esquema e o dicionário de dados, o tamanho da tupla de uma
    tabela, retornando o mesmo.

    *esquema - Estrutura que contém o esquema da tabela (nome de campo, tipo de campo, etc)
    *objeto  - Estrutura que contém informações sobre a tabela (nome da tabela, nome do arquivo da
               tabela, etc)

*/
int tamTupla(tp_table *, struct fs_objects);
/*
    Esta função conta quantas tabelas já estão inseridas dentro do dicionario, para poder colocar
    um código válido para a próxima tabela. Retorna a quantidade exata de tabelas.
*/
int quantidadeTabelas();
/*
    Esta função verifica se um nome de tabela já está inserido no dicionario.
    Retorna:
        -> 1 se o nome existe no dicionario;
        -> 0 se existe no dicionário.
    *nomeTabela - Nome de uma tabela,  a qual deseja-se saber se existe no dicionario.
*/
int verificaNomeTabela(char *);
/*
    Esta função inicia um estrutura do tipo table, como nome de tabela passado.
    Retorna:
        -> a estrutura do tipo table iniciada;
        -> ERRO_NOME_TABELA_INVALIDO se o nome passado já existir no dicionário.
    *nomeTabela - Nome de uma tabela, a qual deseja-se inserir.
*/
table *iniciaTabela(char *);
/*
    Esta função encadeia a lista de campos na estrutura de uma tabela que vai ser criada.
    Retorna:
        -> a estrutura com a coluna inserida na lista.
    *t - Estrutura da tabela à ser criada.
    *nomeCampo - Nome do campo que irá ser inserido na lista de campos.
    tipoCampo - Tipo do campo que irá ser inserido na lista de campos.
    tamanhoCampo - Tamanho do campo que irá ser inserido na lista de campos.
*/
table *adicionaCampo(table *,char *, char , int , int , char *, char *);
/*
    Esta função finaliza a tabela preveamente estrutura pelas funcoes iniciaTabela() e adicionaCampo().
    Escreve nos arquivos fs_object.dat e fs_schema.dat, a estrutura passada.
    Retorna:
        -> SUCCESS quando teve sucesso na sua operaçãoç;
        -> ERRO_ABRIR_ARQUIVO quando teve problemas ao abrir os arquivos fs_object.dat e fs_schema.dat;
        -> ERRO_PARAMETRO quando a estrutura passada é inválida.
    *t - Estrutura da tabela à ser criada.
*/
int finalizaTabela(table *);
/*
    Esta função inicia e aloca dinâmicamente uma lista de valores que vão ser inseridos em uma tabela.
    Retorna:
        -> estrutura iniciada e alocad com o valor passado por parâmetro.
        -> ERRO_DE_PARAMETRO, quando a estrutura enviada for inválida.
    *c - Estrutura de valores que vão ser inseridos em uma tabela.
    *nomeCampo - Nome do campo que o usuário vai inserir um valor.
    *valorCampo - Valor do campo que vai ser inserido.
*/
column *insereValor(table  *, column *, char *, char *);
////
int retornaTamanhoValorCampo(char *nomeCampo, table  *tab);
///////
char retornaTamanhoTipoDoCampo(char *nomeCampo, table  *tab);
////
/* printTable: Imprime a list de tabelas quando o usuário enviar o comando \d
 *             ou imprime a list de atributos da tabela quando o usuário informar
 *             \dt <Nome da tabela>
 */
void printTable(char *);
///
void freeTp_table(tp_table **, int );
void freeTable(table *);
void freeColumn(column *);

void incrementaQtdIndice(char *nTabela);

void adicionaBT(char *nomeTabela, char *nomeAtrib);
