#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "tag_hierarchy.h"
#include "paths_manager.h"
#include "tag_file.h"

#define DEBUG 0

struct tag_l {
	int 			must_be; // <-- à 1 si oui (dans ce cas disjonction), 0 sinon (dans ce cas conjonction)
	struct tag_node *list;
};

//SYNTAXE : $tag -s couleur film -not bleu -not jaune 

/*
Crée la liste des enfants + tag avec must_be correctement "positionné"
*/
struct tag_l build_list(char *tag_name, int must_be) {
	struct tag_l tag_list;
	tag_list.must_be = must_be;
	tag_list.list = get_tag_children(tag_name);

	// ATTENTION : ajouter le tag lui-même à la liste !
	struct tag_node *temp = tag_list.list;
	tag_list.list = malloc(sizeof(struct tag_node));
	memcpy(tag_list.list->name, tag_name, TAGNAME);
	tag_list.list->next = temp;

	return tag_list;
}

int belong_to_list(char *tag_name, struct tag_l tag_list) {
	struct tag_node *tag = tag_list.list;
	while(tag != NULL) {
		if(strcmp(tag_name, tag->name) == 0) return 1;
		tag = tag->next;
	}
	return 0;
}


/*
Construit le tableau de listes 
*/
int research(int argc, char **argv) {

	// 0. Compter nombre de termes "utiles" ( -> #listes )
	int c = 0;
	for (int i = 2; i < argc; i++) { // On commence à 2 puisque que commence par ./tag -s
		if(strcmp(argv[i], "-not") != 0) c ++;
	}
	
	// 1. construire tableau
	struct tag_l *tab = malloc(sizeof(struct tag_l) * c);

	// 2. Parcourir les termes 1 par 1 et créer une liste par terme.
	c = 0;
	for (int i = 2; i < argc; i++) {
		int must_be = 1;

		if(strcmp(argv[i], "-not") == 0) {
			must_be = 0;
			i++;
		}

		if (i >= argc) {
			printf("argc %d\n",argc);
			printf("i %d\n",i);
			perror("Wrong argumentation\n");
			exit(1);
		}

		tab[c] = build_list(argv[i], must_be);
		c++;
	}

	
	

	// 3. Filtrer la liste de documents taggés
	FILE *path_file = init_iterator();
	char *path = next_path(path_file);
	while(path != NULL) { // parcours de path

		if (DEBUG) printf("--------------------\n");
		if (DEBUG) printf("Examining path %s\n", path);

		struct tag_node *tag_list = get_file_tag_list(path);

		if (DEBUG) printf("The file contains the following tags :\n");
		if (DEBUG) print_list(tag_list);

		for(int i = 0; i < c; i++) { // parcours de liste

			if (DEBUG) printf("-----\n");
			if (DEBUG) printf("Examining list :\n");
			if (DEBUG) printf("Must be ? %d\n", tab[i].must_be);
			if (DEBUG) print_list(tab[i].list);

			struct tag_node *tag = tag_list;
			int tag_found = 0;

			if(DEBUG) printf("---\n");

			while(tag != NULL) {
				if(DEBUG) printf("- Examining tag %s\n", tag->name);

				int belong = belong_to_list(tag->name, tab[i]);
				if(tab[i].must_be) {
					if (belong) {
						if(DEBUG) printf("%s belongs to list, list is checked\n", tag->name);
						goto  next_list;
					}
				} else {
					if (belong) {
						if(DEBUG) printf("%s belongs to list, file is wrong\n", tag->name);
						goto next_file;
					}
				}
				tag = tag->next;
			}

			if(DEBUG) printf("--\n");

			if (tab[i].must_be && !tag_found) goto next_file;

			next_list :
			;

			if (DEBUG) printf("Passing on to next list\n");
			if (DEBUG) printf("-----\n");


		}

		if (DEBUG) printf("FILE WINS \n");
		printf("%s\n", path);

		next_file :
		path = next_path(path_file);

		if (DEBUG) printf("Passing on to next file\n");
		if (DEBUG) printf("--------------------\n");
	}


	/*

	pour chaque path {

		pour chaque liste de tab {
	
			pour chaque tag du fichier {
				if (liste->must_be) {
					si tag appartient à liste :
					passer à la LISTE suivante (le fichier peut convenir, on inspecte la liste suivante)
				} else {
					si tag appartient à la liste :
					passer au FICHIER suivant (le fichier ne convient pas)
				}
			}
		}

		Arrivé ici, le fichier a passé tous les tests : afficher path sur terminal comme résultat de la recherche
		Et passer au fichier suivant

	}

	*/

	return 0;
}

/* -------------------------------------------------------------------------

NOTES SUR L'INTEPRÉTATION

But est d'obtenir un tableau de liste de tags qu'on va rechercher dans les attributs, selon si "must be" ou pas
Une liste par "terme" de la recherche
- Pour chaque liste dans le tableau : si "must be" --> fichier est retenu ssi possède AU MOINS UN DES TAGS de la liste chaînée
- 									  sinon 	   --> fichier est retenu ssi ne possède AUCUN des tags de la liste chaînée

EXEMPLE 
HIÉRARCHIE :
root
	Couleur
		rouge
		bleu
			cobalt
			turquoise
		vert
			vert clair
	Film


Si c'est couleur -not bleu film

1. 1er terme : Recherche enfants de "couleur"
rouge, bleu, cobalt, turquoise, vert, vert clair

2. 2e terme : Recherche enfants de "bleu"
bleu, cobalt, turquoise

3. 3e terme : Recherche enfants de "film"

Et s'il y a par exemple un fichier taggé user.tag = rouge/bleu/film
Donc à l'interprétation :
Si taggé "must_be" --> il suffit qu'un des tags de la liste soit présent pour ok
Si taggé "must_not_be" --> il faut qu'aucun des tags de la liste ne soit présent

*/

