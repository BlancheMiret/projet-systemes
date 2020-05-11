 
#include "paths_manager.h"


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
   printf("path --> %s \n", path);
   return path;
 }

}


//fonction pour vérifier si le chemin existe déjà

/* *
 * @param file_paths = nom du fichier qui contient les chemins des fichiers taggés
 * @param file_to_tag = nom du fichier qu'on veut tagger
 * @return renvoie 1 si le chemin existe sinon 0
 * */

int find_path(char * file_paths, char * file_to_tag ){

  char *path = realpath(file_to_tag, NULL);
  char *line_buf = NULL;
  size_t line_buf_size = 0;
  ssize_t line_size;
  int line_count = 0;
  FILE *file = fopen(file_paths, "r");

    //retourne le nombre de caractères de la première ligne
  line_size = getline(&line_buf, &line_buf_size, file);
  
  while (line_size>= 0) {

    printf("LINE %s",line_buf );
    
    if (strncmp(path, line_buf,strlen(line_buf)-1) == 0) {
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

    //On récupère le chemin d'accès absolu du fichier
  char *path = realpath(filename, NULL);

  if(path == NULL){
   printf("cannot find file with name[%s]\n", filename);
 } 
 else{
   printf("path --> %s \n", path);
         //free(path);
 }

 FILE  *file;

    //fichier où on va stocker les chemins des fichiers taggés
 const char *filename_paths = "paths.txt";
 file = fopen(filename_paths, "a");

 if (file == NULL)
 {
  fprintf(stderr, "cannot open %s for appending\n", filename_paths);
  return 0;
}

    //fseek: positionne le "curseur de fichier" à une distance comptée en octets à partir du début, de la fin ou de la position courante 
    //on positionne ici à la fin du fichier
fseek (file, 0, SEEK_END);

    //ftell renvoie la position courante du « curseur de fichier », 
int size = ftell(file);

if (size == 0) {
  printf("file is empty !\n");
  fprintf(file, "%s", path);
}

else {

  if(find_path("paths.txt", filename) == 1){

   printf("Le chemin existe déjà!\n");
   return 0;
 }

 else {
  fprintf(file, "\n%s", path);
}

}

fclose(file);

return 1;


}