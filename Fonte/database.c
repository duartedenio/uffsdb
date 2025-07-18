#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

char connectDB(char *db_name) {
	FILE *DB;
	int i;
	char vec_name 				[QTD_DB][LEN_DB_NAME_IO],
		    vec_directory 			[QTD_DB][LEN_DB_NAME_IO],
		    valid,
		    directory 				[LEN_DB_NAME_IO] = "data/";

  if((DB = fopen("data/DB.dat","rb")) == NULL)
     	return ERRO_ABRIR_ARQUIVO;

  for(i=0; fgetc(DB) != EOF; i++){
  	fseek(DB, -1, 1);

    fread(&valid,sizeof(char),1, DB);
    fread(vec_name[i],sizeof(char),LEN_DB_NAME_IO, DB);
    fread(vec_directory[i],sizeof(char),LEN_DB_NAME_IO, DB);

    if(valid && objcmp(vec_name[i], db_name) == 0){ //verifica se encontrou o banco
    	strcat(directory, vec_directory[i]); 			// atualiza o diretorio do banco que esta conectado
    	strcpylower(connected.db_directory, directory);
    	fclose(DB);
    	return SUCCESS;
    }
  }
  fclose(DB);

  return DB_NOT_EXISTS;
}

void createDB(char *db_name) {
	FILE *DB;
	int i, len;
	char vec_name[QTD_DB][LEN_DB_NAME_IO],
		    vec_directory[QTD_DB][LEN_DB_NAME_IO],
		    create[LEN_DB_NAME_IO] = "mkdir data/",
        *aux_name_tolower,valid;

  if((DB = fopen("data/DB.dat","a+b")) == NULL) {
    printf("ERROR: cannot open file\n");
	  return;
  }

  if(strlen(db_name) > LEN_DB_NAME) {
  	printf("WARNING: database name is too long, it will be truncated\n");
  	db_name[LEN_DB_NAME] = '\0';
  }

  for(i=0; fgetc (DB) != EOF; i++) {
  	fseek(DB, -1, 1);

  	fread(&valid			,sizeof(char), 			 1, DB);
    fread(vec_name[i]  		,sizeof(char), LEN_DB_NAME_IO, DB);
    fread(vec_directory[i] 	,sizeof(char), LEN_DB_NAME_IO, DB);

    if(valid && objcmp(vec_name[i], db_name) == 0) {
    	fclose(DB);
		  if(objcmp(db_name, "uffsdb") != 0) 			// banco de dados ja existe
        printf("ERROR: database already exists\n");
      return;
    }
  }

  if(i >= QTD_DB) {
  	fclose(DB);
  	printf("ERROR: The amount of databases in this machine exceeded the limit.\n");
  	return;
  }

  data_base *SGBD = (data_base*)malloc(sizeof(data_base));
	len = strlen(db_name);

	SGBD->valid = 1;
	strcpylower(SGBD->db_name, db_name);
	strcpylower(SGBD->db_directory, db_name);
	strcat(SGBD->db_directory	, "/");
	SGBD->db_name[len] 			= '\0';
	SGBD->db_directory[len+1] 	= '\0';
	fwrite(SGBD ,sizeof(data_base), 1, DB);

  aux_name_tolower = (char *)malloc(sizeof(char) * (strlen(db_name)+1));
  strcpylower(aux_name_tolower, db_name);
  strcat(create, aux_name_tolower);
  free(aux_name_tolower);

	if(system(create) == -1) {			//verifica se foi possivel criar o diretorio
		printf("ERROR: It was not possible to create the database\n");
	}
  else if(objcmp(db_name, "uffsdb") != 0) printf("CREATE DATABASE\n");
  free(SGBD);
  fclose(DB);
}

void dropDatabase(char *db_name){
	FILE *DB;
	int i;
	char vec_name[QTD_DB][LEN_DB_NAME_IO],
        vec_directory[QTD_DB][LEN_DB_NAME_IO],valid;

  if(strcmp(db_name, connected.db_name) == 0) {
    printf("ERROR: You can not delete the database that you are connected.\n");
    return;
  }

  if((DB = fopen("data/DB.dat","r+b")) == NULL) {
    printf("ERROR: cannot open file\n");
    return;
  }

  for(i=0; fgetc(DB) != EOF; i++) {
    fseek(DB, -1, 1);

    fread(&valid			,sizeof(char), 			 1, DB);
    fread(vec_name[i]  		,sizeof(char), LEN_DB_NAME_IO, DB);
    fread(vec_directory[i] 	,sizeof(char), LEN_DB_NAME_IO, DB);
    if(valid && objcmp(vec_name[i], db_name) == 0){
    	valid = 0;
    	fseek(DB, ((LEN_DB_NAME_IO*2+1)*i), SEEK_SET); 	// posiciona o cabecote sobre o byte de validade
    	fwrite(&valid ,sizeof(char), 1, DB);			// do banco e apaga ele

    	char directory[LEN_DB_NAME_IO] = "rm data/";
    	strcat(directory, vec_directory[i]);
    	strcat(directory, " -R\0");

    	system(directory);

      fclose(DB);
      printf("DROP DATABASE\n");
      return;
    }
  }
  fclose(DB);
  printf("ERROR: database does not exist\n");
}

void showDB() {

	FILE *DB;
	int i, qtdDB=0;
	char vec_name 				[QTD_DB][LEN_DB_NAME_IO],
		 vec_directory 			[QTD_DB][LEN_DB_NAME_IO],
		 valid;

    if((DB = fopen("data/DB.dat","r+b")) == NULL) {
       	printf("ERROR: cannot open file\n");
		return;
    }

    printf(" 					List of databases\n");
    printf("       Name  	    |  Owner   | Encoding |   Collate   |    Ctype    |   Access privileges  \n");
    printf("--------------------+----------+----------+-------------+-------------+-----------------------\n");
    for(i=0; fgetc (DB) != EOF; i++) {
    	fseek(DB, -1, 1);

    	fread(&valid			,sizeof(char), 			 1, DB);
        fread(vec_name[i]  		,sizeof(char), LEN_DB_NAME_IO, DB);
        fread(vec_directory[i] 	,sizeof(char), LEN_DB_NAME_IO, DB);

       	if(valid) {
       		printf("%-20s| uffsdb  | UTF8     | pt_BR.UTF-8 | pt_BR.UTF-8 | \n", vec_name[i]);
       		qtdDB++;
        }
    }

    printf("(%d %s)\n\n\n\n", qtdDB, (1 >= qtdDB)? "row": "rows");
    fclose(DB);
}
///
void dbInit(char *db) {
	char *name;
	if(system("mkdir data > /dev/null 2>&1") == -1)
		printf("ERROR: It was not possible to initialize uffsdb\n");
  if (db==NULL){
    name = malloc(sizeof(char)*7);
    name[0]='u';name[1]='f';name[2]='f';name[3]='s';
    name[4]='d';name[5]='b';name[6]=0;
  } else name=db;

	createDB(name);
}
