#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "tag_hierarchy.h"
#include "paths_manager.h"
#include "tag_file.h"

#define DEBUG 0

struct tag_l {
	int 			must_be;
	struct tag_node *list;
};


/*
Renvoie une struct tag_l contenant tag_name et ses enfants
Et positionne son champ must_be
*/
struct tag_l build_list(char *tag_name, int must_be) {
	struct tag_l tag_list;
	tag_list.must_be = must_be;
	tag_list.list = get_tag_children(tag_name);
	struct tag_node *temp = tag_list.list;
	tag_list.list = malloc(sizeof(struct tag_node));
	memcpy(tag_list.list->name, tag_name, TAGNAME);
	tag_list.list->next = temp;

	return tag_list;
}


/*
Renvoie 1 si tag_name appartient à la liste tag_list, 0 sinon
*/
int belong_to_list(char *tag_name, struct tag_l tag_list) {
	struct tag_node *tag = tag_list.list;
	while(tag != NULL) {
		if(strcmp(tag_name, tag->name) == 0) return 1;
		tag = tag->next;
	}
	return 0;
}


/*
Affiche le nom d'un fichier ou son chemin complet selon l'agument ABS
*/
void print_filename(char *path, int ABS) {
	if(ABS) printf("%s\n", path);
	else printf("%s\n", strrchr(path, '/') + 1);
}


/*
Affiche les chemins des fichiers correspondant à la combinaison de tags passée en paramètres
*/
void research(int ABS, char **terms, int nb_terms) {

	// 0. Compter nombre de termes "utiles" ( -> #listes )
	int c = 0;
	for (int i = 0; i < nb_terms; i++) {
		if(strcmp(terms[i], "-not") != 0) {
			if (!tag_exists(terms[i])) {
				printf("%s does not exist in your tagset yet. Create it first with tag create\n", terms[i]);
				exit(0);
			}
			c ++;
		}
	}
	
	// 1. construire tableau
	struct tag_l *tab = malloc(sizeof(struct tag_l) * c);

	// 2. Parcourir les termes 1 par 1 et créer une liste par terme.
	c = 0;
	for (int i = 0; i < nb_terms; i++) {
		int must_be = 1;

		if(strcmp(terms[i], "-not") == 0) {
			must_be = 0;
			i++;
		}

		if (i >= nb_terms) {
			printf("Tagname missing after -not : stoping\n");
			exit(EXIT_FAILURE);
		}

		tab[c] = build_list(terms[i], must_be);
		c++;
	}

	// 3. Filtrer la liste de documents taggés
	FILE *path_file = init_iterator();
	char *path = next_path(path_file);

	int nb_file = 0;

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

			while(tag != NULL) { // parcours des tags du fichier
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
		if (nb_file == 0) printf("Here are the files corresponding to your research : \n");
		print_filename(path, ABS);
		nb_file += 1;

		next_file :
		path = next_path(path_file);

		if (DEBUG) printf("Passing on to next file\n");
		if (DEBUG) printf("--------------------\n");

		free_tag_list(tag_list);
	}

	if(nb_file == 0) printf("No file correspond to your research.\n");

	for(int i = 0; i < c; i++) {
		free_tag_list(tab[i].list);
	}
	free(tab);

}
