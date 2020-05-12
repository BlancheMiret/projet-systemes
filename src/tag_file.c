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


//printf("value of tag %s : %s\n", usertag ,buff_tag );

/**
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

}**/

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


	struct tag_node *tag =(struct tag_node *) malloc(sizeof(struct tag_node));
	memcpy(tag->name, "jaune", strlen("jaune"));
	tag->next = (struct tag_node *) malloc(sizeof(struct tag_node));
	memcpy(tag->next->name, "rouge", strlen("rouge"));
	tag->next->next = (struct tag_node *) malloc(sizeof(struct tag_node));
	memcpy(tag->next->next->name, "bleu", strlen("bleu"));
	tag->next->next->next = NULL;


	for (int i=0; i<tags_size;i++){

	  //children_list=get_tag_children(tags[i]);
		children_list=tag;
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


	}

     //Ajouter le cas: si on supprime le dernier tag on supprime le chemin dans paths.txt 
	   //Ajouter une fonction delete_path()


}




int main(int argc, char const *argv[])
{ char *tags[1];

	tags[0]="couleur";


//printf("%s\n", subtags[0]);
//link_tag("test1.txt", subtags, 1);

//unlink_tag("test12.txt", "bleu");
	//unlink_tag("test1.txt", tags,1);
	return 0;
}








