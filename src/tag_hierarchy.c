 #define _GNU_SOURCE
#include <fcntl.h> 
#include <unistd.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include <glib.h>
#include <glib/gprintf.h>

#include "tag_hierarchy.h"

struct tag_t {
	char			father[TAGNAME];
	char			name[TAGNAME];
	struct tag_t	*brother;
	struct tag_t	*children;
};

struct hierarchy {
	GHashTable 		*point_table;
	struct tag_t 	*tree;
};

#define TAGSIZE sizeof(struct tag_t)

char hierarchy_file[1024];


// DÉCLARATION FONCTIONS INTERNES 
void write_tree(struct tag_t *tag, int fd); //
void *build_tree(); //
void print_tree(struct tag_t *tag, char * shift); //
void print_tree_children(struct tag_t *tag, char *shift); //
void print_tag(struct tag_t *tag); //


/*
Initialise variable globale nom du fichier
*/
void init_hierarchy() {
    memset(hierarchy_file, 0, 1024);
    const char *HOME = getenv("HOME");
    if (HOME == NULL) exit(1);
    strcat(hierarchy_file,HOME);
    strcat(hierarchy_file, "/.tag/tag_hierarchy");
}

/* 
Supprime toute la liste et hiérarchie de tags existante en demandant confirmation à l'utilisateur. 
*/
void reset_hierarchy() {

	printf("You're going to suppress all tags from all tagged files and completely reset your tagset."
        "This reset operation cannot be undone. Do you want to reset ? Enter Y(es) or N(o)\n");
	char answer;

	wait_answer:
	scanf("%c", &answer);
	switch(answer) {
		case 'y':
		case 'Y':
			printf("Reset operation validated.\n");
			break;
		case 'n':
		case 'N':
			printf("Reset operation canceled.\n");
			exit(0);
		default :
			printf("Please enter Y or y to validate the reset operation, N or n to cancel it.\n");
			while(answer != '\n') scanf("%c", &answer);
			goto wait_answer;
	}

	FILE *f = fopen(hierarchy_file, "w");
    if(f == NULL) {
        perror("fopen");
        exit(1);
    }
    
	fclose(f);
}

// ----------------------------------------------------------------------------------------------
// -------------------------------- OBTENIR LES ENFANTS D'UN TAG --------------------------------

/*
Affiche une liste chaînée de tag_node
*/
void print_list(struct tag_node *tag_list) {
	struct tag_node *tag = tag_list;
	while(tag != NULL) {
		printf("%s\n", tag->name);
		tag = tag->next;
	}
}

/*
Construit la liste chaînée de struct tag_node avec une tête de lecture
*/
void write_tag_list(struct tag_t *tag, struct tag_node **list) {
	while(tag != NULL) {
		struct tag_node *temp = *list; // temp est la liste
		*list = malloc(sizeof(struct tag_node));
		memset(*list, 0, sizeof(struct tag_node));
		memcpy((*list)->name, tag->name, TAGNAME);
		(*list)->next = temp;

		if(tag->children != NULL) write_tag_list(tag->children, list);

		tag = tag->brother;
	}
}

/* 
Renvoie une liste chaînée de struct tag_node des enfants de tag_name 
*/
void *get_tag_children(char *tag_name) {

	// --- CONSTRUCTION ARBRE ET HASHMAP ---

	struct hierarchy *h = build_tree(); 
	GHashTable *point_table = h->point_table;

	// 1. Avec la hashmap, trouver l'adresse du tag dans l'arbre
	struct tag_t *tag = g_hash_table_lookup(point_table, tag_name);

	if (tag == NULL) {
		perror("The tag you want the children from does not exist.\n");
		exit(1);
	}

	// -- PARCOURIR L'ARBRE POUR CONSTRUIRE LISTE CHAÎNÉE

	if (tag->children == NULL) return NULL;
	tag = tag->children;
	struct tag_node *list = NULL;

	write_tag_list(tag, &list);

	// -- LIBÉRER MÉMOIRE

	g_hash_table_destroy(point_table);
	free(h);

	// -- RENVOYER LA LISTE

	return list;
}


// ----------------------------------------------------------------------------------------------
// ------------------------------ EXISTENCE TAG DANS LA HIÉRARCHIE ------------------------------

/* 
Renvoie 1 si tag_name existe dans la liste et hiérarchie stockées, 0 sinon.
*/
int tag_exists(char *tag_name) {

	int fd = open(hierarchy_file, O_RDWR);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

	struct tag_t *tag = malloc(TAGSIZE);
	while(read(fd, tag, TAGSIZE) != 0) {
		if (strcmp(tag->name, tag_name) == 0) {
			free(tag);
			close(fd);
			return TRUE;
		}
	}
	free(tag);
	close(fd);
	return FALSE;
}

// ----------------------------------------------------------------------------------------------
// --------------------------------------- AJOUTER UN TAG ---------------------------------------

/* Version prend plusieurs tags*/
/*
Ajoute un tag dans le système de tags.
Si "father" est NULL, le tag prend "root" comme père.
Si tag_name existe déjà dans la liste des tags, 
ou si father est différent de NULL et que le nom n'est pas trouvé, une erreur est renvoyée
**/
int create_tag(char *father, char* tags[], int nb_tags) {

    // -- VÉRIFICATION LONGUEURS DES TAGS --
    for(int i = 0; i < nb_tags; i++) {
        if(strlen(tags[i]) > 18) {
            printf("%s is too long for a tag_name. Try a name with 18 caracters or less.\n", tags[i]);
            exit(1);
        }
    }

    // -- VÉRIFICATION (NON)EXISTENCE FATHER ET TAG --
    int fd = open(hierarchy_file, O_RDWR);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    int father_exists = FALSE;

    struct tag_t *tag = malloc(TAGSIZE);

    while(read(fd, tag, TAGSIZE) != 0) {
        for (int i = 0; i < nb_tags; i++) {
            if(strcmp(tag->name, tags[i]) == 0) {
                printf("%s already exists in your hierarchy. Two tags cannot have the same name.\n", tags[i]);
                exit(1);
            }
        }
        if (father != NULL && strcmp(tag->name, father) == 0) father_exists = TRUE;
    }

    if (father != NULL && !father_exists) {
    	printf("%s\n", father);
        printf("The specified father does not exist in the tag hierarchy. See --help\n");
        exit(1);
    }

    for(int i = 0; i < nb_tags; i++) {
        memset(tag, 0, TAGSIZE);
        memcpy(tag->name, tags[i], strlen(tags[i]) + 1);
        tag->name[TAGNAME - 1] = '%';

        if (father == NULL) memcpy(tag->father, "root", strlen("root") + 1);
        else memcpy(tag->father, father, strlen(father) + 1);
        tag->father[TAGNAME-1] = '-';
        write(fd, tag, TAGSIZE);
    }


    close(fd);
    free(tag);

    return 0;
}

// ----------------------------------------------------------------------------------------------
// -------------------------------------- SUPPRIMER UN TAG --------------------------------------

/*
Supprime tag_name de la liste et la hiérarchie stockée.
Si tag_name a des enfants dans la hiérarchie, ils sont automatiquement supprimée.
L'arborescence en demande de suppression est affichée dans le terminal avant de demander confirmation à l'utilisateur.
*/
int delete_tag(char *tag_name) {

	// --- CONSTRUCTION ARBRE ET HASHMAP ---

	struct hierarchy *h = build_tree(); 
	GHashTable *point_table = h->point_table;
	struct tag_t *tree = h->tree;

	// --- DEMANDER CONFIRMATION SUPPRESSION ARBORESCENCE ---

	// 1. Avec la hashmap, trouver l'adresse du tag dans l'arbre
	struct tag_t *tag_to_delete = g_hash_table_lookup(point_table, tag_name);

	if (tag_to_delete == NULL) {
		perror("The tag you want to delete does not exist in your tagset.\n");
		exit(1); 
	}

	printf("You're going to delete all this tag hierarchy from your tagset and from the files tagged with them :\n");
	print_tree_children(tag_to_delete, "");
	printf("The removal cannot be undone, do you want to proceed ? Enter Y(es) or N(o)\n");
	char answer;

	wait_answer:
	scanf("%c", &answer);
	switch(answer) {
		case 'y':
		case 'Y':
			printf("Delete operation validated.\n");
			break;
		case 'n':
		case 'N':
			printf("Delete operation canceled.\n");
			exit(0);
		default :
			printf("Plesae enter Y or y to validate the delete operaton, N or n to cancel it.\n");
			while(answer != '\n') scanf("%c", &answer);
			goto wait_answer;
	}

	// --- SUPPRIMER DE L'ARBORESCENCE ---

	// 2. Trouver l'adresse du père
	struct tag_t *father = g_hash_table_lookup(point_table, tag_to_delete->father);
	struct tag_t **precedent = &(father->children);

	// 3. Introduire les frères de tag_to_delete en tête de liste des enfants du père
	if (*precedent == tag_to_delete) {
		*precedent = tag_to_delete->brother;
	} else {
		while((*precedent)->brother != tag_to_delete) precedent = &(*precedent)->brother;
		(*precedent) -> brother = tag_to_delete->brother;
	}

	// --- RÉ-ÉCRIRE SUR LE FICHIER ---

	int fd = open(hierarchy_file, O_WRONLY| O_TRUNC);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

	write_tree(tree->children, fd);
	close(fd);

	g_hash_table_destroy (point_table);
	free(h);

	return 0;
}

/*
Prend un arbre de structures tag_t et écrit chaque structure dans le fichier précédemment ouvert avec le descripteur fd.
*/
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


// ----------------------------------------------------------------------------------------------
// ------------------------ CONSTRUCTION HIERARCHIE PAR LECTURE FICHIER -------------------------

/*
Ouvre le fichier stockant la liste et la hiérarchie des tags 
et l'interprète pour construire sa représentation par un arbre de structures tag_t.
Construit en même temps la HashMap qui associe un nom de tag l'adresse de la structure tag_t le représentant dans l'arbre.
*/
void *build_tree() {

	// --- INITIALISATION ---

	GHashTable *point_table = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, &free); 

	// insert root;
	struct tag_t *tree = malloc(TAGSIZE);
	struct tag_t *tag = tree;
	memset(tag, 0, TAGSIZE);
	memcpy(tag->name, "root", strlen("root") + 1);

	g_hash_table_insert(point_table, tag->name, tag);

	// ---- VARIABLES DE PARCOURS ----

	int fd = open(hierarchy_file, O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

	char *buf = malloc(TAGSIZE);
	memset(buf, 0, TAGSIZE);

	// --- LIRE LE FICHIER, CONSTRUIRE LE TABLEAU DE HIÉRARCHIE EN MÉMOIRE  ---

	while(read(fd, buf, TAGSIZE) != 0) {
		tag = (struct tag_t *)buf;

		// 0. Ajouter l'adresse de ce tag à la hashmap
		g_hash_table_insert(point_table, tag->name, tag) ; 

		// 1. Trouver l'adresse du père
		struct tag_t *inter = g_hash_table_lookup(point_table, tag->father);
		struct tag_t **father = &inter;

		// 2. Lier père à son fils (introduire entre père et fils)
		tag->brother = (*father)->children;
		(*father)->children = tag;

		// 3. Pour la prochaine itération
		buf = malloc(TAGSIZE);
		memset(buf, 0, TAGSIZE);
	}

	close(fd);

	struct hierarchy *h = malloc(sizeof(struct hierarchy));
	memset(h, 0, sizeof(struct hierarchy));
	h->point_table = point_table;
	h->tree = g_hash_table_lookup(point_table, "root");
	return h;
}


// ----------------------------------------------------------------------------------------------
// ----------------------------------- FONCTIONS D'AFFICHAGES -----------------------------------

/*
Construit l'arbre de la hiérarchie des tags et l'affiche intégralement en utilisant print_tree()
*/
void print_hierarchy() {
	struct hierarchy *h = build_tree();
	print_tree(h->tree, "");
	g_hash_table_destroy(h->point_table); 
	free(h);
}


/*
Prend l'adresse d'une structure tag_t, un décalage d'affichage, 
affiche de façon récursive toute l'arborescence à partir de cette structure en prenant en compte ses frères.
*/
void print_tree(struct tag_t *tag, char * shift) { 
	while(tag != NULL) {
		printf("%s%s\n", shift, tag->name);
		if(tag->children != NULL) {
			char *new_shift = shift;
			asprintf(&new_shift, "  %s", shift);
			print_tree(tag->children, new_shift);
			free(new_shift);
		}
		tag = tag->brother;
	}
}


/*
Prend l'adresse d'une structure tag_t dans l'arbre de la hiérarchie préalablement construit,
affiche l'arborescende à partire de cette structure sans prendre en compte ses frères.
*/
void print_tree_children(struct tag_t *tag, char *shift) {
	printf("%s%s\n", shift, tag->name);
	if(tag->children != NULL) {
		char *new_shift = shift;
		asprintf(&new_shift, "  %s", shift);
		print_tree(tag->children, new_shift);
		free(new_shift);
	}
}


/*
Prend l'adresse d'une structure tag_t et la présente de manière lisible sur le terminal.
*/
void print_tag(struct tag_t *tag) {
	printf("--PRINTING TAG--\n");
	printf("Father = %s\n", tag->father);
	printf("Name = %s\n", tag->name);
	printf("Brother = %p\n", tag->brother);
	printf("Child = %p\n", tag->children);
}
