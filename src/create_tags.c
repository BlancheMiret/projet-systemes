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


/**
   int i;
   char * line ="This is line\n";
   // open the file for writing
   int fp = open ("paths.txt",O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
 
   // write 10 lines of text into the file stream
   for(i = 0; i < 10;i++){
       write (fp, line,strlen(line));
   }
 
   // close the file  
   close (fp);**/


}


//prendre un tableau de tags
void link_tag_to_file( char * filename,  char *tagname[], int argc){

    char buff_tag[1024];
    memset(buff_tag,'\0',1024);

    //Pour avoir "user.nom_du_tag"
    char usertag[20];
    char *user = "user.";
    memset(usertag, '\0', 20);
    memcpy(usertag,user,strlen(user));  
    memcpy(usertag+strlen(user),tagname[2] ,strlen(tagname[2]));
    //printf("%s\n",usertag);    

   //ajouter "/" à chaque sous-tag

    char *arr=malloc(100*sizeof(char));

    for(int i=2; i<argc-1; i++){
        strcat(arr,tagname[i+1]); 
        strcat(arr,"/"); 
 

        strcat(arr,"\0"); 
    }

    //printf("%s\n",arr);
   // printf("%d",(int) strlen(arr));
    int fd = open(filename, O_RDWR);
    
    //buff_tag contient la valeur de l'attributs étendus, 
    //exemple: on a user.couleur = bleu/bleu_ciel, buff_tag va contenir bleu/bleu_ciel

    if(getxattr(filename,usertag, &buff_tag, sizeof(buff_tag)) == -1){

        printf("le tag n'existe pas!!");

        if(fsetxattr(fd,usertag,arr,strlen(arr),XATTR_CREATE) > -1){

          printf("fichier taggé! \n");

          if(find_path == 0){
            add_path(filename);
          }
          

        }   
   
        else {
          perror("error set: ");
        }
   
    }

    else{

        printf("value of tag %s : %s\n", usertag ,buff_tag );
    }
    
    //Si les tags existent déjà, on les remplace

    if(strcmp(buff_tag, arr) == 0){
         
         if(fsetxattr(fd,usertag,arr,strlen(arr),XATTR_REPLACE) > -1){

             printf("tag set!\n");

             if(find_path == 0){
                 add_path(filename);
             }
        }   
   
        else {
          perror("error set: ");
        }

    }

        
}









/**************** TEST ****************/

int main(int argc, char *argv[])
{
     char *buffer;
    //commande: tag filename tag_name sous_tag1 sous_tag2 sous_tag3 ...etc
     //if(argc > 2) link_tag_to_file(argv[1], argv,argc);


      //find_path("paths.txt","test10.txt");

	return 0;
}
 
