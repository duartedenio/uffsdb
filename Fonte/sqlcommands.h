#define FSQLCOMMANDS 1   // flag para garantir a não reinclusão

#ifndef FMACROS // garante que macros.h não seja reincluída
   #include "macros.h"
#endif
///
#ifndef FTYPES // garante que types.h não seja reincluída
  #include "types.h"
#endif
/* insert: Recebe uma estrutura rc_insert e valida os tokens encontrados pela interface().
 *         Se os valores forem válidos, insere um novo valor.
 */
void insert(rc_insert *);
int allColumnsExists(rc_insert *, table *);
char getInsertedType(rc_insert *, char *c, table *);
char *getInsertedValue(rc_insert *, char *, table *);
int typesCompatible(char , char);
/*
    Esta função finaliza a inserção de valores em uma tabela. Assume que o usuário entrou com todos
    os campos de uma tupla completa.
    Retorna:
        -> ERRO_ABRIR_ARQUIVO, quando ocorreu um erro ao abrir o arquivo fs_object.dat ou fs_schema.dat;
        -> ERRO_NO_TAMANHO_STRING, quando ocorreu um erro no tamanho da string inserida;
        -> ERRO_NOME_CAMPO, quando o nome do campo passado na estrutura;
        -> ERRO_NO_TIPO_INTEIRO, quando o valor passado não é um inteiro;
        -> ERRO_NO_TIPO_DOUBLE, quando o valor passado não é um double;
        -> ERRO_NO_TIPO_CHAR, quando o valor passado é maior que 1byte;
        -> SUCCESS, quando a função teve sucesso em sua operação de inserção na tabela.
    *nome - Nome da tabela que vai ser inserido os valores da estrutura *c.
    *c - Estrutura com o valores que vão ser inseridos na tabela *nome.
*/
int finalizaInsert(char *, column *);
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Utilizada para impressão de tabelas.
    Parametros: Nome da tabela (char).
    Retorno:    void.
   ---------------------------------------------------------------------------------------------*/
Lista *op_select(inf_select *);
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Função para exclusão de tabelas.
    Parametros: Nome da tabela (char).
    Retorno:    INT
                SUCCESS,
                ERRO_REMOVER_ARQUIVO_OBJECT,
                ERRO_REMOVER_ARQUIVO_SCHEMA,
                ERRO_LEITURA_DADOS.
   ---------------------------------------------------------------------------------------------*/
int excluirTabela(char *);
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Função para criação de índices para qualquer atributo de uma tabela.
    Parametros: Estrutura do tipo rc_insert.
    Retorno:    INT
                SUCCESS,
                ERRO_DE_PARAMETRO.
   ---------------------------------------------------------------------------------------------*/
int createIndex(rc_insert *);
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Copia todas as informações menos a tabela do objeto, que será removida.
    Parametros: Objeto que será removido do schema.
    Retorno:    INT
                SUCCESS,
                ERRO_REMOVER_ARQUIVO_SCHEMA
   ---------------------------------------------------------------------------------------------*/
int procuraSchemaArquivo(struct fs_objects);
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Gera as verificações em relação a chave FK.
    Parametros: Nome da Tabela, Coluna C, Nome do Campo, Valor do Campo, Tabela Apontada e Atributo Apontado.
    Retorno:    INT
                SUCCESS,
                ERRO_DE_PARAMETRO,
                ERRO_CHAVE_ESTRANGEIRA
   ---------------------------------------------------------------------------------------------*/
int verificaChaveFK(char *, column *, char *, char *, char *, char *);

/* ----------------------------------------------------------------------------------------------
    Objetivo:   Gera as verificações em relação a chave pK.
    Parametros: Nome da Tabela, Coluna C, Nome do Campo, Valor do Campo
    Retorno:    INT
                SUCCESS,
                ERRO_DE_PARAMETRO,
                ERRO_CHAVE_PRIMARIA
   ---------------------------------------------------------------------------------------------*/
int verificaChavePK(char *, column *, char *, char *);
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Inicializa os atributos necessários para a verificação de FK e PK.
    Parametros: Objeto da tabela, Tabela, Buffer e nome da tabela.
    Retorno:    INT
                SUCCESS,
                ERRO_DE_PARAMETRO,
   ---------------------------------------------------------------------------------------------*/
int iniciaAtributos(struct fs_objects *, tp_table **, tp_buffer **, char *);
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Recebe o nome de uma tabela e engloba as funções leObjeto() e leSchema().
    Parametros: Nome da Tabela, Objeto da Tabela e tabela.
    Retorno:    tp_table
   ---------------------------------------------------------------------------------------------*/
tp_table *abreTabela(char *, struct fs_objects *, tp_table **);
/* createTable: Recebe uma estrutura contendo os parâmetros do CREATE TABLE
 *              e cria uma tabela no banco corrente
 */
void createTable(rc_insert *);


/*
  Objetivo: Verificar se a projeção feita no insert é válida
  Parametros: Uma lista com a projeção, um ponteiro
              para as colunas da tabela e um inteiro
              dizendo a quantidade de colunas.
  Retorno: retorna 1 se a projecao é válida
          returna 0 caso contrário.
          no vetor char *, cada posicao i
          possui 0 se a coluna i pertence a projecao
          possui 1 caso contrário.
*/
int validaProj(Lista *,column *,int,char *);

/*
  Verifica se na clausula where esta sendo utilizado
  alguma coluna que não é da tabela.
*/
int validaColsWhere(Lista *tok,column *colunas,int qtdColunas);

/*
  Memsma coisa que novoResWhere somente para string.
*/
inf_where *novoTokenWhere(char *str,int id);

void printConsulta(Lista *p,Lista *l);

/*
  Retorna: ponteiro struct inf_where,
  recebe: void *tk,int id
  onde tk é uma regiao qualquer da memória,
  id é um identificador de tipo de dado qualquer presente em Expressao.h
*/
inf_where *novoResWhere(void *tk,int id);

////
int verifyFieldName(char **, int );
////
int verifyFK(char *, char *);
