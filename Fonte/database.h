/* createDB: Recebe um nome para criar um banco de dados
 *           cria o banco, caso ainda não exista
 */
void createDB(char *);

/* connectDB: Recebe o nome de um banco de dadados
 *            conecta-se com o banco, caso exista
 */
char connectDB(char *);

/* dbInit: cria o banco de dados padrão
 *         com o nome "ibetres", caso não exista
 */
void dbInit();

/* dropDatabase: Recebe o nome de um banco de dados
 *               deleta o banco caso o banco exista
 *               e o usuário esteja conectado em outro banco
 */
void dropDatabase(char *);

/* showDB: lista todos os bancos do SGBD "\l"
 *
 */
void showDB();
/* interface: Parte gerada pelo yacc, presente no arquivo yacc.y
 *            É responsável por fazer a conexão ao banco e chamar yyparse()
 *            para fazer a validação dos comandos enviados pelo usuário.
 */
int interface();
