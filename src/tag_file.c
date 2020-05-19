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

	val = listxattr(path, NULL, 0);

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
		val = getxattr(path,usertag, &buff_tag, sizeof(buff_tag));
		
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
	char * subtags[100];
	char subtags_concat[1024];
	char *path = realpath(filename, NULL);
	char * usertag = "user.tags";
	char buff_tag[1024];
	struct tag_node * children_list=NULL;
	char *all_tags =malloc(1024*sizeof(char));
	memset(all_tags,'\0',1024);
	char *ptr = NULL;
	int count=0;
	int j = 0;

    
	val = listxattr(path, NULL, 0);

	if (val == -1) {
		perror("listxattr error");
		return 0;
	}

	//Cas où user.tags n'existe pas
	if (val == 0) {

		printf("Le fichier ne contient aucun tag!\n");
		return 0;

	}

	for (int i=0; i<tags_size;i++){

		val = getxattr(filename,usertag, &buff_tag, sizeof(buff_tag));
		
		if(val == -1 ){
			perror("getxattr error: ");
			return 0;
		} 

		//Cas où user.tags existe mais ne contient aucun tag
		if(val == 0){
			printf("Le fichier ne contient aucun tag!\n");
			return 0;
		}

		//Suppression de tags[i] 

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
		val = set_tag(path, usertag, all_tags,1);
		if(val == 0) return 0;	

	    children_list=get_tag_children(tags[i]);
		
        
		buff_tag[val] = '\0';
		//printf("VAL  %d\n ",(int) val);
		//printf("FIRST buff_tag %s\n ", buff_tag);

        //Si le tag a des enfants dans la hierarchie
		if(children_list != NULL){
   
            //On vérifie si un ou plusieurs tags de children_list sont liés au fichier
            //On ajoute les tags de children_list qui sont liés dans subtags
			while(children_list != NULL) {

				if(check_tag_existence(buff_tag,  children_list->name) == 1){
				
					subtags[j] = children_list->name;
					j++;
				
				}
				
				children_list = children_list->next;
			}

			if(j == 0){

				memset(buff_tag,'\0',1024);
				val = getxattr(filename,usertag, &buff_tag, sizeof(buff_tag));
	
				if(val == 0){
					printf("Le fichier ne contient plus de tags!, il va être supprimé de paths.txt\n");
					delete_path(filename);
				}
				return 1;

			} 

			if(j == 1){ 
				
				printf("Le fichier concerné possède également le sous-tag suivant hériant de %s :\n", tags[i]);
				printf("- %s\n", subtags[0]);
				printf("Souhaitez vous supprimer également ce sous-tag? Répondez par 'oui' ou 'non'\n");
			}

			if(j > 1){

				printf("Le fichier concerné possède également les sous-tags suivant hériant de %s :\n", tags[i]);
				for(int i=0; i < j; i++) printf("- %s\n", subtags[i]);
				printf("Souhaitez vous supprimer également ces sous-tags? Répondez par 'oui' ou 'non'\n");
			}
			
			//print_list(children_list);
			while(1){
				scanf("%s", reply);
				if(strcmp(reply,"oui") == 0 || strcmp(reply,"non") ==0) break;
				printf("Répondez par 'oui' ou 'non'\n");
			}
			

			if(strcmp(reply,"oui") == 0){

				printf("vous avez répondu oui\n");
				//printf("buff_tag  %s\n", buff_tag);

				concatenate_tags(subtags_concat, subtags, j);

				char delim[] = "/";

				char *ptr = strtok(buff_tag, delim);
				while(ptr != NULL)
				{
					//printf("'%s'\n", ptr);

					if(strcmp(ptr,tags[i]) != 0 && check_tag_existence(subtags_concat, ptr) == 0){

						strcat(all_tags,ptr); 
						strcat(all_tags,"/"); 

					}

					ptr = strtok(NULL, delim);
				}

				//all_tags = contient tous les tags sauf ceux qu'on a supprimé
				strcat(all_tags,"\0");
				//printf("all_tags  %s\n",all_tags);

				//On attribut les tags après supression au fichier
				val = set_tag(path, usertag, all_tags, 1);

				if(val == 0) return 0;
				printf("Le tag %s ainsi que ses sous-tags ont été supprimé !\n", tags[i]);

		
			}

			if(strcmp(reply,"non")==0){

				printf("Seul le tag %s a été supprimé !\n", tags[i]);


			}
		}


	}

	memset(buff_tag,'\0',1024);

    
	val = getxattr(filename,usertag, &buff_tag, sizeof(buff_tag));
		if(val == 0){
			printf("Le fichier ne contient plus de tags!, il va être supprimé de paths.txt\n");
			delete_path(filename);

		}

	return 1;

}

