#include "create_tags.h"


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
else fprintf(file, "\n%s", path);
fclose(file);



}


//fonction qui lie des tags à un fichier grâce à xattr

/* *
 * @param filename = nom du fichier
 * @param maintag = nom du tag
 * @param subtags[] = tableau qui contient tous les sous-tags de maintag
 * @param subtags_size = taille de subtags
 * @return renvoie 0 si les tags ont bien été ajouté, sinon 1
 * */

int link_tag(char *filename, char * maintag, char * subtags[], size_t subtags_size){

  char *path = realpath(filename, NULL);
  char buff_tag[1024];
  memset(buff_tag,'\0',1024);

  //Pour avoir "user.nom_du_tag"
  char usertag[100];
  char *user = "user.";
  memset(usertag, '\0', 100);
  memcpy(usertag,user,strlen(user));  
  memcpy(usertag+strlen(user),maintag ,strlen(maintag));
  printf("%s\n",usertag);    


   //on déclare all_subtags qui va contenir tous les sous tags concaténés
  char *all_subtags =malloc(100*sizeof(char));
  memset(all_subtags,'\0',100);

    //Si on a un seul élément dans le tableau subtags
  if(subtags_size == 1){

    memcpy(all_subtags,subtags[0],strlen(subtags[0]));

  } 


  if(subtags_size > 1){

    for(int i=0; i<subtags_size; i++){

      strcat(all_subtags,subtags[i]); 
      strcat(all_subtags,"/"); 


      strcat(all_subtags,"\0"); 
    }
  }
  

  printf("ALL SUBTAGS %s\n",all_subtags);
   // printf("%d",(int) strlen(arr));
  int fd = open(filename, O_RDWR);

    //buff_tag contient la valeur de l'attributs étendus, 
    //exemple: on a user.couleur = bleu/bleu_ciel, buff_tag va contenir bleu/bleu_ciel

  if(getxattr(filename,usertag, &buff_tag, sizeof(buff_tag)) == -1){

    printf("le tag n'existe pas!!");

    if(setxattr(path,usertag,all_subtags,strlen(all_subtags),XATTR_CREATE) > -1){

      printf("fichier taggé! \n");

      add_path(filename);
      

    }   

    else {
      perror("error set: ");
      return 1;
    }

  }

  else{

    printf("value of tag %s : %s\n", usertag ,buff_tag );
  }

    //Si les tags existent déjà, on les remplace

  if(strcmp(buff_tag, all_subtags) == 0){

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

/**
int main(int argc, char const *argv[])
{ char *subtags[3];

  subtags[0]="rouge";
  subtags[1]="bleu";
  subtags[2]="jaune";

 //printf("%s\n", subtags[0]);
  //link_tag("test12.txt", "couleur", subtags, 3);
  unlink_tag("test12.txt", "bleu");
  return 0;
}
**/







