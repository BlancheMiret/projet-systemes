#include <fcntl.h> //open
#include <unistd.h> //write
#include <stdio.h> //printf
#include <stdlib.h> //malloc

// pour read
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

// Pour créer l'arbre de la hiérarchie au moment de l'affichage ? 
struct tag_node {
	char		name[20];
	struct tag	*father;
	struct tag	*brother;
}

// Pour parcourir les fichiers 
struct tag_pair {
	char 	father[20];
	char 	son[20]
}

#define FALSE 0
#define TRUE 1

/*
read : écrit dans un buffer jusqu'à taille donnée, depuis fd
fread : écrit dans à l'adresse d'un pointeur un nb d'objets d'une taille donnée, depuis stream
*/

/* 
getdelim : écrit dans un buffer jusqu'à délimiteur

ATTENTION, f = FILE pas un descripteur
char *linep = NULL;
size_t linecapp = 0;
getdelim(&linep, &linccapp, '%', f) // allocate memory accordingly. 
*/

/* 
memmem : renvoie pointeur vers 1ère occurence d'un délimiteur dans un CHAR *
*/

int add_tag(char *name) {
	fd_read = read_tag_list();
	fd_write = write_tag_list();
	return 0;
}

int tag_exists(char *name); //renvoie 1 si le tag existe, 0 sinon


int read_tag_list() {
	return open("tag_list", O_RDONLY);
}

int write_tag_list() {
	return open("tag_list", O_WRONLY); //en cas d'erreur gérer plus haut !
}


/*

TROIS BESOINS 
- AJOUTER UN TAG À LA LISTE ET DANS LA HIÉRARCHIE / SUPPRIMER
	- Vérifier qu'il n'existe pas déjà (?? )


- VÉRIFIER QU'UN TAG APPARTIENT À LA LISTE (au moment de l'ajouter à un inoeud);
	Donc lire le fichier de façon à avoir accès rapidemment à la liste des tags
		- Avec écriture/lecture brut : 
		aller de % en %, mot précédent, dès qu'on trouve : ok

- RECHERHCE DE FICHIER ASSOCIÉS À UNE COMBINAISON
	- parser la combinaison

	- pouvoir chercher tous les enfants des tags désirés
		- Avec écriture/lecture brut :
		aller de % en %, mot suivant, si nom : 

		- La recherche dans un arbre c'est pénible...

		- PS : pour chaque mot (exemple combinaison = rouge & vert) 
		refaire tout le parcourt de fichier... 
		Si on pouvait combiner...
		Faire une fonction avec arguments multiples ???

	- faire une recherche dans la liste des chemins de fichiers taggés, avec xattr


- AFFICHER
	reconstruire l'arbre et l'afficher ? 



QUESTION
	- est-ce un arbre, qu'il faut recréer à chaque fois
	- ou une façon de lire plus "brutale". 

*/






