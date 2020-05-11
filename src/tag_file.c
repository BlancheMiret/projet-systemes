#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/xattr.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


//Associer un tag à un fichier grâce à xattr
//Ajouter le chemin dans un fichier (le créer s'il n'existe pas encore)
//Ajouter les tags dans la hiérarchie
//Recherche de tags (dans un autre fichier)


/**
        impossible de créer: user.tag = bleu, user.tag = bleu_ciel ...etc
        faut-il utiliser user.couleur = bleu/bleu_ciel 
        ou user.couleur = bleu & user.bleu = bleu_ciel ?


     **/  


//Commandes qu'on va utiliser : 
//tag -a -n couleur  ---> user.root = couleur
//tag -a couleur bleu rouge jaune ---> user.couleur = bleu/rouge/jaune
//donc la méthode link_tag prend en argument un tableau qui contient des sous tags ?
// link_tag(char *filename, char * maintag, char * subtags)


//mettre dans un autre fichier = paths_manager.c (à revoir)

 //fonction pour vérifier si le chemin existe déjà
//renvoie 1 si le chemin existe sinon 0
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


//Fonction qui ajoute le chemin du fichier taggé dans un fichier .txt
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
  return -1;
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
 }

 else {
  fprintf(file, "\n%s", path);
 }

}
fclose(file);



}



//fonction qui lie des tags à un fichier grâce à xattr

/* *
 * @param filename = nom du fichier
 * @param tags[] = tableau qui contient tous les tags à ajouter
 * @param subtags_size = taille de tags
 * @return renvoie 1 si les tags ont bien été ajouté, sinon 0
 * */

int link_tag(char *filename, char * tags[], size_t tags_size){

  //voir le cas où le tag existe déjà
  //vérifier si le chemin existe déjà dans paths.txt
  //si le fichier il a aucun tag 

  int val;
  char *path = realpath(filename, NULL);
  char buff_tag[1024];
  memset(buff_tag,'\0',1024);

  //on déclare all_tags qui va contenir tous les tags concaténés ou non
  char *all_tags =malloc(1024*sizeof(char));
  memset(all_tags,'\0',1024);

  //Pour avoir "user.nom_du_tag"
  char *usertag = "user.tag";

  if(tags_size == 1){

    strcat(all_tags,tags[0]);
    strcat(all_tags,"/"); 
  
  }

  if(tags_size > 1){

    for(int i=0; i<tags_size; i++){

      strcat(all_tags,tags[i]); 
      strcat(all_tags,"/");
      strcat(all_tags,"\0"); 

    }

  }
  
  //printf("ALL TAGS %s\n",all_tags);

  int fd = open(filename, O_RDWR);

  //buff_tag contient la valeur de l'attributs étendus, 
  //exemple: on a user.tag = bleu/bleu_ciel, buff_tag va contenir bleu/bleu_ciel
  
  val = getxattr(filename,usertag, &buff_tag, sizeof(buff_tag));
  
  //Cas où user.tag n'existe pas ou user.tag ne contient rien
  if(val == -1 || val == 0){

    printf("user.tag n'existe pas!!");

    if(setxattr(path,usertag,all_tags,strlen(all_tags),XATTR_CREATE) > -1){

      printf("fichier taggé! \n");

      //Vérifier dans cette fonction si le chemin du fichier existe déjà
      add_path(filename);

      return 1;
      

    }   

    else {
      perror("error set: ");
      return 0;
    }

  }

  //Cas où on a déjà un ou plusieurs tags, on ajoute le nouveau tag à la fin
  if(val > 0){

    strcat(buff_tag,all_tags);
    strcat(buff_tag,"\0"); 

    if(setxattr(path,usertag,buff_tag,strlen(buff_tag),XATTR_REPLACE) > -1){

      printf("fichier taggé! \n");

      //Vérifier dans cette fonction si le chemin du fichier existe déjà
      add_path(filename);

      return 1;
      

    }   

    else {
      perror("error set: ");
      return 0;
    }

  }


  return 0;




}



//fonction qui supprime un tag du fichier

/* *
 * @param filename = nom du fichier
 * @param tag = nom du tag
 * @param subtags[] = tableau qui contient tous les sous-tags de maintag
 * @param subtags_size = taille de subtags
 * @return renvoie 0 si le tag a bien été supprimé, sinon 1
 * */


//Dans le cas où on 
//user.root = couleur/roman/film

//On cherche si user.couleur existe, si oui on supprime tout
//user.couleur=bleu/rouge/vert
/**
int unlink_tag(char * filename, char * tags[]){

   //si on supprime le dernier tag on supprime le chemin dans 
  // user.tag = couleur

}


**/
/**

int unlink_tag(char * filename, char * tagname){

  int val;
  char *path = realpath(filename, NULL);
  char *father = get_father_name(tagname);
  char *name_value= NULL;

  char *all_subtags =malloc(100*sizeof(char));
  memset(all_subtags,'\0',100);

  //Pour avoir "user.nom_du_tag"
  char usertag[100];
  char *user = "user.";

  //Pour récupérer value de extended attributes
  char buff_tag[1024];

  //Cas où tagname n'as pas de père (donc c'est lui le père)
  if(father == NULL){

   memset(usertag, '\0', 100);
   memcpy(usertag,user,strlen(user));  
   memcpy(usertag+strlen(user),tagname ,strlen(tagname));
   
   //On supprime le tag et ses sous-tags
   val=removexattr(path, usertag);

 }

  //Cas où tagname a un père


   //concaténation de "user."" avec le nom du père, pour récupérer les fils avec getxattr
 memset(usertag, '\0', 100);
 memcpy(usertag,user,strlen(user));  
 memcpy(usertag+strlen(user), father ,strlen(father));

 val = getxattr(filename,usertag, &buff_tag, sizeof(buff_tag)) ;

    //Dans le cas où le tag a des sous-tags
 if(val > 0){

  int init_size = strlen(buff_tag);
  char delim[] = "/";

  char *ptr = strtok(buff_tag, delim);

  while(ptr != NULL)
  {
    printf("'%s'\n", ptr);

    if(strcmp(ptr,tagname) != 0){

      strcat(all_subtags,ptr); 
      strcat(all_subtags,"/"); 

    }
    

    ptr = strtok(NULL, delim);
  }

    //all_subtags = contient tous les sous-tags sauf tagname
  strcat(all_subtags,"\0");
    //printf("all_subtags  %s\n",all_subtags);

    //On attribut les sous-tags sans tagname
  if(setxattr(path,usertag,all_subtags,strlen(all_subtags),XATTR_REPLACE) > -1){

   printf("tag set!\n");
   add_path(filename);

 }   

 else {
  perror("error set: ");
  return 1;
}


}


return 0;



}

**/
int main(int argc, char const *argv[])
{ char *subtags[3];

  subtags[0]="film";


 //printf("%s\n", subtags[0]);
  link_tag("test1.txt", subtags, 1);

  //unlink_tag("test12.txt", "bleu");
  return 0;
}








