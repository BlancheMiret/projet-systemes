#include <fcntl.h> //open
#include <unistd.h> //write
#include <stdio.h> //printf
#include <stdlib.h> //malloc
#include <string.h> //strcmp

// pour read
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

//pour glib
#include <glib.h>
#include <glib/gprintf.h>

#define TAGNAME 20 // TAILLE EFFECTIVE : 18 À CAUSE DU CARACTÈRE DE DÉLIMITATION AJOUTÉ. ESPACE NÉCESSAIRE.

struct tag_t {
	char			father[TAGNAME];
	char			name[TAGNAME];
	struct tag_t	*brother;
	struct tag_t	*children;
};

#define TAGSIZE sizeof(struct tag_t)

GHashTable *point_table = NULL;
struct tag_t *tree = NULL;

int add_tag(char *father, char *tag_name);
void print_tag(struct tag_t *tag);
void *build_tree();
void print_tree(struct tag_t *tag, char* shift);
int delete_tag(char *tag_name);
void print_tree_children(struct tag_t *tag, char *shift);


int main() {
	/*
	printf("ADDING COULEUR\n");
	add_tag(NULL, "Couleur");
	printf("ADDING ROUGE\n");
	add_tag("Couleur", "Rouge");
	printf("ADDING JAUNE\n");
	add_tag("Couleur", "Jaune"); 
	printf("ADDING GENRE\n");
	add_tag(NULL, "Genre");
	printf("ADDING WESTER\n");
	add_tag("Genre", "Western");
	printf("ADDING DRAME\n");
	add_tag("Genre", "Drame");
	printf("ADDING JAUNE FLUO\n");
	add_tag("Jaune", "Jaune fluo");

	add_tag("Couleur", "Lila");
	*/
	build_tree();
	print_tree(tree, "");
	delete_tag("Jaune");

}

void write_tree(struct tag_t *tag, int fd) { 
	while(tag != NULL) {
		struct tag_t tag_bis = *tag;
		tag_bis.children = NULL;
		tag_bis.brother = NULL;
		write(fd, &tag_bis, TAGSIZE);
		if(tag->children != NULL) write_tree(tag->children, fd);
		tag = tag->brother;
	}
}

int delete_tag(char *tag_name) {

	// ------------------- CONSTRUCTION ARBRE ET HASHMAP ---------------------------

	build_tree(); 

	// ------------- DEMANDER CONFIRMATION SUPPRESSION ARBORESCENCE ----------------

	// 1. Avec la hashmap, trouver l'adresse du tag dans l'arbre
	struct tag_t *tag_to_delete = g_hash_table_lookup(point_table, tag_name);

	if (tag_to_delete == NULL) {
		perror("The tag you want to delete does not exist.\n");
		exit(1);
	}

	printf("Vous allez supprimer toute cette arborescence de tag.\n");
	print_tree_children(tag_to_delete, "");

	// On considère que oui

	// ------------------- SUPPRIMER DE L'ARBORESCENCE -------------------------

	// 2. Trouver l'adresse du père
	struct tag_t *father = g_hash_table_lookup(point_table, tag_to_delete->father);
	struct tag_t **precedent = &(father->children);

	// 3. Introduire les frères de tag_to_delete en tête de liste des enfants du père
	while((*precedent)->brother != tag_to_delete) (*precedent) = (*precedent)->brother;
	(*precedent) -> brother = tag_to_delete->brother;

	printf("A-t-il été supprimé ?\n");
	print_tree(tree, "");

	// ------------------- RÉ-ÉCRIRE SUR LE FICHIER -------------------------

	int fd = open("tag_hierarchy", O_WRONLY| O_TRUNC);
	write_tree(tree->children, fd); // PS : on ne veut pas écrire root

	return 0;
}

/*
Ajoute un tag dans le système de tags.
Si "father" est NULL, le tag prend "root" comme père.
Si tag_name existe déjà dans la liste des tags, 
Si father est différent de NULL et que le nom n'est pas trouvé, une erreur est renvoyée
**/
int add_tag(char *father, char *tag_name) {

	// ------------------- VÉRIFICATION LONGUEUR DU NOM DU TAG ---------------------------

	if(strlen(tag_name) > 18) {
		perror("This tag-name is too long. Try something with 18 letters or less.\n");
		exit(1);
	}

	// ------------------- VÉRIFICATION (NON)EXISTENCE FATHER ET TAG --------------------

	int fd = open("tag_hierarchy", O_RDWR);
	int father_exists = FALSE; // défini dans glib library

	char *buf = malloc(TAGSIZE);
	struct tag_t *tag;
	while(read(fd, buf, TAGSIZE) != 0) {
		tag = (struct tag_t *)buf;
		//print_tag(tag);
		if (strcmp(tag->name, tag_name) == 0) {
			perror("This tag-name already exists.");
			exit(1);
		}
		if (father != NULL && strcmp(tag->name, father) == 0) father_exists = TRUE;
	}

	if (father != NULL && !father_exists) {
		perror("The specified father does not already exist. ");
		exit(1); 
	}
	
	// ---------------------------- AJOUT DU TAG DANS LA LISTE ---------------------------
	
	// -------------------------- ÉCRITURE DU TAG DANS LA HIERARCHY -------------------------

	memset(tag, 0, TAGSIZE);
	memcpy(tag->name, tag_name, strlen(tag_name) + 1);
	tag->name[TAGNAME - 1] = '%';

	if (father == NULL) memcpy(tag->father, "root", strlen("root") + 1);
	else memcpy(tag->father, father, strlen(father) + 1);
	tag->father[TAGNAME-1] = '-';
	write(fd, tag, TAGSIZE);
	close(fd);

	return 0;
}

void *build_tree() {

	// ---------------------------- INITIALISATION -------------------------

	point_table = g_hash_table_new(g_str_hash, g_str_equal);
	// insert root;

	tree = malloc(TAGSIZE);
	struct tag_t *tag = tree;
	memset(tag, 0, TAGSIZE);
	memcpy(tag->name, "root", strlen("root") + 1);

	g_hash_table_insert(point_table, tag->name, tag);

	// ---------------------------- VARIABLES DE PARCOURS -------------------------

	int fd = open("tag_hierarchy", O_RDONLY);

	char *buf = malloc(TAGSIZE);
	memset(buf, 0, TAGSIZE);

	// ---------------------------- LIRE LE FICHIER, CONSTRUIRE LE TABLEAU DE HIÉRARCHIE EN MÉMOIRE  -------------------------

	while(read(fd, buf, TAGSIZE) != 0) {
		tag = (struct tag_t *)buf;

		// 0. Ajouter l'adresse de ce tag à la hashmap
		g_hash_table_insert(point_table, tag->name, tag) ;

		// 1. Trouver l'adresse du père
		struct tag_t *inter = g_hash_table_lookup(point_table, tag->father);
		struct tag_t **father = &inter;
		// On obtient un pointeur vers l'adresse du père

		// 2. Lier père à son fils (introduire entre père et fils)
		tag->brother = (*father)->children;
		//print_tag(tag);
		(*father)->children = tag;

		// 3. Pour la prochaine itération
		buf = malloc(TAGSIZE);
		memset(buf, 0, TAGSIZE);
	}

	close(fd);
	return g_hash_table_lookup(point_table, "root");
}


void print_tree_children(struct tag_t *tag, char *shift) {
	printf("%s%s\n", shift, tag->name);
	if(tag->children != NULL) {
		char *new_shift = malloc(strlen(shift) + 1);
		memset(new_shift, 0, strlen(shift) + 1);
		new_shift[0] = ' ';
		new_shift[1] = ' ';
		strcat(new_shift, shift);
		print_tree(tag->children, new_shift);
	}
}


void print_tree(struct tag_t *tag, char * shift) { // <---- Code à factoriser

	while(tag != NULL) {
		printf("%s%s\n", shift, tag->name);
		if(tag->children != NULL) {
			char *new_shift = malloc(strlen(shift) + 1);
			memset(new_shift, 0, strlen(shift) + 1);
			new_shift[0] = ' ';
			new_shift[1] = ' ';
			strcat(new_shift, shift);
			print_tree(tag->children, new_shift);
		}
		tag = tag->brother;
	}
}


void print_tag(struct tag_t *tag) {
	printf("--PRINTING TAG--\n");
	printf("Father = %s\n", tag->father);
	printf("Name = %s\n", tag->name);
	printf("Brother = %p\n", tag->brother);
	printf("Child = %p\n", tag->children);
}