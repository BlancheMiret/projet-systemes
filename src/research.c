
#include <string.h>
#include <stdio.h>
#include "tag_manager.h"

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


/*
Construit le tableau de listes 
*/
int research(int argc, char **argv) {

	// 0. Compter nombre de termes "utiles" ( -> #listes )
	int c = 0;
	for (int i = 2; i < argc; i++) { // On commence à 2 puisque que commence par ./tag -s
		if(!strcmp(argv[i], "-not")) c ++;
	}

	// 1. construire tableau
	struct tag_l *tab = malloc(sizeof(struct tag_l) * c);

	// 2. Parcourir les termes 1 par 1 et créer une liste par terme.
	c = 0;
	for (int i = 2; i < argc; i++) {
		int must_be = TRUE;

		if(strcmp(argv[i], "-not")) {
			must_be = FALSE;
			i++;
		}

		if (i >= argc) { 
			perror("Wrong argumentation\n");
			exit(1);
		}

		tab[c] = build_list(argv[i], must_be);
		c++;
	}

	// 3. Filtrer la liste de documents taggés

	// ouvir le fichier de la liste des paths des fichiers taggés
	// le parcourir 

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

