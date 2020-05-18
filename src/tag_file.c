#include "tag_file.h"



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




//retourne 1 si new_tag existe dans tags, sinon 0

int check_tag_existence(char * tags, char * new_tag){

	char * pch;
  	pch = strstr (tags,new_tag);
  	
  	if(pch != NULL) return 1;
  	
  	return 0;

}

/**
flags:
	XATTR_CREATE = 0
	XATTR_REPLACE = 1

**/
int set_tag(char * path, char * usertag, char * all_tags, int flags){

    if(flags == 0){

    	if(setxattr(path,usertag,all_tags,strlen(all_tags),XATTR_CREATE) > -1){

    		add_path(path);

    	} 
    	else {

    		perror("setxattr error: ");
    		return 0;

    	}
    }

    if(flags){

    	if(setxattr(path,usertag,all_tags,strlen(all_tags),XATTR_REPLACE) > -1){

    		printf("fichier taggé! \n");			
    		add_path(path);

    	}   

    	else {

    		perror("setxattr error: ");
    		return 0;
    	}


    }
		
	return 1;

}


void concatenate_tags(char * tag_string, char * tags[], size_t tags_size){
    
	if(tags_size == 1){

		strcat(tag_string,tags[0]);
		strcat(tag_string,"/"); 

	}

	if(tags_size > 1){

		for(int i=0; i<tags_size; i++){

			strcat(tag_string,tags[i]); 
			strcat(tag_string,"/");
			strcat(tag_string,"\0"); 
		}

	}
}

//fonction qui lie des tags à un fichier grâce à xattr

/* *
* @param filename = nom du fichier
* @param tags[] = tableau qui contient tous les tags à ajouter
* @param subtags_size = taille de tags
* @return renvoie 1 si les tags ont bien été ajouté, sinon 0
* */

int link_tag(char *filename, char * tags[], size_t tags_size){

	
	int val;
	int res;
	char *path = realpath(filename, NULL);
	char buff_tag[1024];
	memset(buff_tag,'\0',1024);

	//on déclare new_tags qui va contenir tous les nouveaux tags concaténés
	//ATTENTION : avant de concaténer il faut vérifier si un des tags n'est pas déjà lié au fichier
	char new_tags[1024];
	memset(new_tags,'\0',1024);

	char *usertag = "user.tags";

	val = listxattr(filename, NULL, 0);

	if (val == -1) {
		perror("listxattr error");
		return 0;
	}

	//Cas où user.tags n'existe pas
	if (val == 0) {

		concatenate_tags(new_tags, tags, tags_size);
		res = set_tag(path, usertag, new_tags,0);

		if(res){
			printf("fichier taggé! \n");
			return 1;
		} 

	}

	if(val > 0){

		//buff_tag contient les tags concaténés
		//exemple: on a user.tags = bleu/bleu_ciel, buff_tag va contenir bleu/bleu_ciel

		//On appelle getxattr que dans le cas où user.tags existe déjà
		val = getxattr(filename,usertag, &buff_tag, sizeof(buff_tag));
		
		if(val == -1 ){
			perror("getxattr error: ");
			return 0;
		} 

		//Cas où user.tags ne contient rien
		if(val == 0){

			//printf("user.tags ne contient aucun tag\n");

			concatenate_tags(new_tags, tags, tags_size);
			res = set_tag(path, usertag, new_tags,1);

			if(res){

				printf("fichier taggé! \n");
				return 1;
			} 

		}

		//Cas où on a déjà un ou plusieurs tags dans user.tags, on ajoute les nouveaux tags à la fin
		if(val > 0){


			//Concaténation des tags à ajouter en vérifiant si ils sont liés au fichier
			if(tags_size == 1){

				if(check_tag_existence(buff_tag, tags[0]) == 0){

					strcat(new_tags,tags[0]);
					strcat(new_tags,"/"); 

				}
			}

			if(tags_size > 1){

				for(int i=0; i<tags_size; i++){

					if(check_tag_existence(buff_tag, tags[i]) == 0){
						strcat(new_tags,tags[i]); 
						strcat(new_tags,"/");
						strcat(new_tags,"\0"); 
					}
				}
			}

			//On ajouter les nouveaux tags à la fin de buff_tags
			strcat(buff_tag,new_tags);
			strcat(buff_tag,"\0"); 

			res = set_tag(path, usertag, buff_tag,1);

			if(res){

				printf("fichier taggé! \n");
				return 1;
			} 

		}

	}

	return 0;

}



int list_length(struct tag_node *tag_list) {
	struct tag_node *tag = tag_list;
	int count=0;
	while(tag != NULL) {
		count++;
		tag = tag->next;
	}
	return count;
}



//fonction qui supprime un tag du fichier

/* *
* @param filename = nom du fichier
* @param tags[] = tags qu'on va supprimer
* @param tags_size = taille de tags
* @return renvoie 0 si le ou les tags ont bien été supprimé, sinon 1
* */

int unlink_tag(char * filename, char * tags[], size_t tags_size){

	int val;
	char reply[100];
	char *path = realpath(filename, NULL);
	char * usertag = "user.tag";
	char buff_tag[1024];
	struct tag_node * children_list=NULL;
	char *all_tags =malloc(1024*sizeof(char));
	memset(all_tags,'\0',1024);
	char *ptr = NULL;
	int count=0;

/**
struct tag_node *tag =(struct tag_node *) malloc(sizeof(struct tag_node));
memcpy(tag->name, "jaune", strlen("jaune"));
tag->next = (struct tag_node *) malloc(sizeof(struct tag_node));
memcpy(tag->next->name, "rouge", strlen("rouge"));
tag->next->next = (struct tag_node *) malloc(sizeof(struct tag_node));
memcpy(tag->next->next->name, "bleu", strlen("bleu"));
tag->next->next->next = NULL; **/


	for (int i=0; i<tags_size;i++){

		children_list=get_tag_children(tags[i]);
//children_list=tag;
		val = getxattr(filename,usertag, &buff_tag, sizeof(buff_tag)) ;
		
		buff_tag[val] = '\0';
		printf("FIRST buff_tag %s\n ", buff_tag);

/********** OK ***********/
		if(val > 0){

//int init_size = strlen(buff_tag);
			char delim[] = "/";

			ptr = strtok(buff_tag, delim);

			while(ptr != NULL)
			{
				printf("'%s'\n", ptr);

				if(strcmp(ptr,tags[i]) != 0){

					strcat(all_tags,ptr); 
					strcat(all_tags,"/"); 

				}

				ptr = strtok(NULL, delim);
			}

//all_tags = contient tous les tags sauf tags[i]
			strcat(all_tags,"\0");
//printf("all_tags  %s\n",all_tags);

//On attribut les sous-tags sans tags[i]
			if(setxattr(path,usertag,all_tags,strlen(all_tags),XATTR_REPLACE) > -1){
				printf("all_tags %s ", all_tags);
				printf("tag set!\n");
				add_path(filename);
//return 0;

			}   

			else {
				perror("error set: ");
				return 1;
			}


		}

		memset(all_tags,'\0',1024);
//printf("all_tags %s ", all_tags);

//Si le tag a des enfants dans la hierarchie
		if(children_list != NULL){

			printf("Le fichier concerné possède également le ou les sous-tags suivant hériant de %s :\n", tags[i]);

			print_list(children_list);

			printf("souhaitez vous supprimer également le ou les sous-tags? Répondez par 'oui' ou 'non'\n");
			scanf("%s", reply);

			if(strcmp(reply,"oui")==0){

				printf("vous avez répondu oui\n");

				val = getxattr(filename,usertag, &buff_tag, sizeof(buff_tag)) ;
				buff_tag[val] = '\0';
				printf("buff_tag  %s\n", buff_tag);

				if(val > 0){

					char delim[] = "/";

					char *ptr = strtok(buff_tag, delim);

					while(ptr != NULL)
					{
//printf("'%s'\n", ptr);

						while(children_list != NULL) {
							printf("ptr  %s\n", ptr);
//printf("children list  %s\n", children_list->name);

//On vérifie si le tag contenu dans ptr n'est pas le même que les sous-tags dans children_list
//Si count =
							if(strcmp(ptr,children_list->name) != 0){
								count++;
// printf("ON EST DANS STRCMP\n");
//count ++ puis on compare count avec size de children_list, si ils sont égaux
//alors ça veut dire qu'on peut laisser ce tag lié au fichier

//break; 

							}

							children_list = children_list->next;
						}
						printf("count   %d\n", count);
						printf("list_length(tag)    %d\n", list_length(tag) );

//Si count = list_length(tag), alors on peut laissé le tag contenu dans ptr
//car il est différent de tous les tags contenu dans children_list
						if(count == list_length(tag) && strcmp(ptr,tags[i]) != 0){
// printf("count   %d", count);
							strcat(all_tags,ptr); 
							strcat(all_tags,"/");

						}
						count =0;
						children_list = tag;  
						ptr = strtok(NULL, delim);
					}

//all_tags = contient tous les tags sauf ceux qu'on a supprimé
					strcat(all_tags,"\0");
//printf("all_tags  %s\n",all_tags);

//On attribut les tags après supression au fichier
					if(setxattr(path,usertag,all_tags,strlen(all_tags),XATTR_REPLACE) > -1){

						printf("Le tag %s ainsi que ses enfants ont été supprimé !\n", tags[i]);
						add_path(filename);

					}   

					else {
						perror("error set: ");
						return 1;
					}


				}

			}

			if(strcmp(reply,"non")==0){
				printf("Seul le tag %s a été supprimé !\n", tags[i]);
/** 
if(val > 0){

//int init_size = strlen(buff_tag);
char delim[] = "/";

ptr = strtok(buff_tag, delim);

while(ptr != NULL)
{
printf("'%s'\n", ptr);

if(strcmp(ptr,tags[i]) != 0){

strcat(all_tags,ptr); 
strcat(all_tags,"/"); 

}


ptr = strtok(NULL, delim);
}

//all_subtags = contient tous les sous-tags sauf tagname
strcat(all_tags,"\0");
//printf("all_subtags  %s\n",all_subtags);

//On attribut les sous-tags sans tagname
if(setxattr(path,usertag,all_tags,strlen(all_tags),XATTR_REPLACE) > -1){

printf("tag set!\n");
add_path(filename);
return 0;

}   

else {
perror("error set: ");
return 1;
}


}

**/
			}




		}

//Ajouter le cas: si on supprime le dernier tag on supprime le chemin dans paths.txt 
//Ajouter une fonction delete_path()




		memset(buff_tag,'\0',1024);
	}

	memset(buff_tag,'\0',1024);
	val = getxattr(filename,usertag, &buff_tag, sizeof(buff_tag)) ;
	buff_tag[val] = '\0';
	if (val == -1) perror("getxattr error: ");

	if (val == 0){

		printf("Il n'y a aucun dans ce fichier\n");
		delete_path(filename);
	}




}










