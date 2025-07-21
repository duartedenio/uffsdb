#ifndef FFUTILITY
  #include "Utility.h"
#endif

#define FTYPES 1 // flag para identificar se types.h já foi incluída

struct fs_objects { // Estrutura usada para carregar fs_objects.dat
    char nome[TAMANHO_NOME_TABELA];      // Nome da tabela.
    int cod;                             // Código da tabela.
    char nArquivo[TAMANHO_NOME_ARQUIVO]; // Nome do arquivo onde estão armazenados os dados da tabela.
    int qtdCampos;                       // Quantidade de campos da tabela.
    int qtdIndice;						           // Quantidade de índices da tabela.
};

typedef struct tp_table{ // Estrutura usada para carregar fs_schema.dat
    char nome[TAMANHO_NOME_CAMPO];  // Nome do Campo.                    40bytes
    char tipo;                      // Tipo do Campo.                     1bytes
    int tam;                        // Tamanho do Campo.                  4bytes
    int chave;                      // Tipo da chave                      4bytes
    char tabelaApt[TAMANHO_NOME_TABELA]; //Nome da Tabela Apontada        20bytes
    char attApt[TAMANHO_NOME_CAMPO];    //Nome do Atributo Apontado       40bytes
    struct tp_table *next;          // Encadeamento para o próximo campo.
}tp_table;

typedef struct column{ // Estrutura utilizada para inserir em uma tabela, excluir uma tupla e retornar valores de uma página.
    char tipoCampo;                     // Tipo do Campo.
    char nomeCampo[TAMANHO_NOME_CAMPO]; //Nome do Campo.
    char *valorCampo;                   // Valor do Campo.
    struct column *next;                // Encadeamento para o próximo campo.
}column;

typedef struct table{ // Estrutura utilizada para criar uma tabela.
    char nome[TAMANHO_NOME_TABELA]; // Nome da tabela.
    tp_table *esquema;              // Esquema de campos da tabela.
}table;

typedef struct tp_buffer{ // Estrutura utilizada para armazenar o buffer.
   unsigned char db;        //Dirty bit
   unsigned char pc;        //Pin counter
   unsigned int nrec;       //Número de registros armazenados na página.
   char data[SIZE];         // Dados
   unsigned int position;   // Próxima posição válida na página.
}tp_buffer;

typedef struct rc_insert {
    char    *objName;           // Nome do objeto (tabela, banco de dados, etc...)
    char   **columnName;        // Colunas da tabela
    char   **values;            // Valores da inserção ou tamanho das strings na criação
    int      N;                 // Número de colunas de valores
    char    *type;              // Tipo do dado da inserção ou criação de tabela
    int     *attribute;         // Utilizado na criação (NPK, PK,FK)
    char   **fkTable;           // Recebe o nome da tabela FK
    char   **fkColumn;          // Recebe o nome da coluna FK
}rc_insert;

typedef struct inf_where{
  int id;
  void *token;
}inf_where;

typedef struct inf_select{
  char *tabela;
  int tamTokens;
  Lista *tok,*proj;
}inf_select;

typedef struct rc_parser {
    int         mode;           // Modo de operação (definido em /interface/parser.h)
    int         parentesis;     // Contador de parenteses abertos
    int         step;           // Passo atual (token)
    int         noerror;        // Nenhum erro encontrado na identificação dos tokens
    int         col_count;      // Contador de colunas
    int         val_count;      // Contador de valores
    int         consoleFlag;   // Auxiliar para não imprimir duas vezes nome=#
}rc_parser;

typedef struct data_base{
	char 		valid;
	char 		db_name[LEN_DB_NAME_IO];
	char 		db_directory[LEN_DB_NAME_IO];
}data_base;

typedef struct db_connected {
	char db_directory[LEN_DB_NAME_IO];
    char *db_name;
    int conn_active;
}db_connected;

// Union's utilizados na conversão de variáveis do tipo inteiro e double.

union c_double{

    double dnum;
    char double_cnum[sizeof(double)];
};

union c_int{

    int  num;
    char cnum[sizeof(int)];
};

/************************************************************************************************
**************************************  VARIAVEIS GLOBAIS  **************************************/

extern db_connected connected;

/************************************************************************************************
 ************************************************************************************************/
