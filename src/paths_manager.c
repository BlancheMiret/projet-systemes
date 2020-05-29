#define _GNU_SOURCE
//#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/xattr.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

#include "paths_manager.h"




char file_paths[1024];

void init_file_paths() {
    memset(file_paths, 0, 1024);
    const char *HOME = getenv("HOME");
    if (HOME == NULL) exit(1);
    strcat(file_paths,HOME);
    strcat(file_paths, "/.tag/paths.txt");
}




//fonction qui renvoie le chemin abosolu d'un fichier

/* *
* @param filename = nom du fichier 
* @return renvoie 1 si le fichier, sinon 0
* */

int check_file_existence(char * filename){

	if( access( filename, F_OK ) == -1 ) {

		printf("access error: %s\n", strerror(errno));
		return 0;
	}  

//printf("le fichier existe!!!\n");
	return 1;


}



//fonction qui renvoie le chemin abosolu d'un fichier

/* *
* @param filename = nom du fichier 
* @return renvoie le chemin absolu du fichier sinon NULL
* */

char * absolute_path(char * filename){

	char *path = realpath(filename, NULL);

	if(path == NULL){
		printf("real path error: %s\n", strerror(errno));
		return NULL;
// printf("cannot find file with name[%s]\n", filename);
	} 

	else{
		//printf("path --> %s \n", path);
		return path;
	}

}


//fonction pour vérifier si le chemin existe déjà

/* *
* @param file_paths = nom du fichier qui contient les chemins des fichiers taggés
* @param filename = nom du fichier qu'on veut tagger
* @return renvoie 1 si le chemin existe sinon 0
* */

int find_path(char * filename){

	char *path = realpath(filename, NULL);
	char *line_buf = NULL;
	size_t line_buf_size = 0;
	ssize_t line_size;
	int line_count = 0;
	FILE *file = fopen(file_paths, "r");
	char lol[100];
	memset(lol,0, 100); 

//retourne le nombre de caractères de la première ligne
	line_size = getline(&line_buf, &line_buf_size, file);
	
	
	line_buf[strcspn(line_buf, "\r\n")] = 0;
	while (line_size>= 0) {

		

		if (strcmp(path, line_buf) == 0) {
	
			line_count++;
		}

//ligne suivante:
		line_size = getline(&line_buf, &line_buf_size, file);
	}

//printf("nb of lines equal to path = %d\n",line_count);

	if(line_count == 0) return 0;

	return 1;
}


//Fonction qui ajoute le chemin du fichier taggé dans un fichier paths.txt

/* *
* @param filename = nom du fichier qu'on veut tagger
* @return renvoie 1 si le chemin a été ajouté dans le fichier paths.txt sinon 0
* */

int add_path(char * filename){


	if(check_file_existence(filename) == 0){

		printf("The file doesn't exist!\n");
		return 0;
	}
//On récupère le chemin d'accès absolu du fichier
	char *path = realpath(filename, NULL);

	if(path == NULL){
		printf("cannot find file with name [%s]\n", filename);
	}

	else{
		//printf("path --> %s \n", path);
		//free(path);
	}
	//char *path_to_add = malloc(strlen(path) + strlen("\n") + 1); // +1 for the null-terminator
	// in real code you would check for errors in malloc here
   //strcpy(path_to_add, path);
	//strcat(path_to_add, "\n");

	FILE  *file;

	//fichier où on va stocker les chemins des fichiers taggés

	file = fopen(file_paths, "a");

	if (file == NULL)
	{
		fprintf(stderr, "cannot open %s for appending\n", file_paths);
		return 0;
	}


	if(find_path(filename) == 1){
		
		//printf("Le chemin existe déjà!\n");
		return 0;
	}

	else {
		
		fprintf(file, "%s\n", path);

	}

	fclose(file);

	return 1;


}


//fonction qui supprime un chemin absolu d'un fichier

/* *
* @param filename = nom du fichier
* @return renvoie 1 si le chemin a bien été supprimé, sinon 0
* */

int delete_path(char * filename){

  
	if(check_file_existence(filename) == 0){

		printf("le fichier n'existe pas !\n");
		return 0;
	}

	if(find_path(filename)==0){

		printf("This path doesn't exist in paths.txt!\n");
		return 0;
	}

	char *path = absolute_path(filename);

	char *line_buf = NULL;
	size_t line_buf_size = 0;
	ssize_t line_size;
	char *temp_file = "temp.txt";
	FILE *file = fopen(file_paths, "r");
	FILE *file2 = fopen(temp_file, "a+");
//retourne le nombre de caractères de la première ligne
	line_size = getline(&line_buf, &line_buf_size, file);

	while (line_size>= 0) {

//printf("LINE %s",line_buf );

		if (strncmp(path, line_buf,strlen(line_buf)-1) != 0) {

			fprintf(file2, "%s", line_buf);
		}

//ligne suivante:
		line_size = getline(&line_buf, &line_buf_size, file);
	}



	fclose(file);
	fclose(file2);
	remove(file_paths);
	rename(temp_file, file_paths);


	return 1;
  
}


void * init_iterator(){

	FILE *file = fopen(file_paths, "r"); 
	if (file == NULL) {
		perror("fopen");
		exit(1);
	}
	return file;
}


void * next_path(FILE * file){

	char *line_buf = NULL;
	size_t line_buf_size = 0;
	ssize_t line_size;
    size_t ln;
    //retourne le nombre de caractères de la première ligne
    //getline : return -1 on failure to read a line (including end-of-file condition).
	line_size = getline(&line_buf, &line_buf_size, file);
    ln = line_size-1;
    
    if(line_buf[ln] == '\n') line_buf[ln] = '\0';
	
	//printf("next_path %s",line_buf);
	
	if(line_size < 0) return NULL;
	return line_buf;

}
