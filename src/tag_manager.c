#include "tag_manager_test.h"

#define TAGNAME 20 // TAILLE EFFECTIVE : 18 À CAUSE DU CARACTÈRE DE DÉLIMITATION AJOUTÉ. ESPACE NÉCESSAIRE.

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


// DÉCLARATION FONCTIONS INTERNES 
void write_tree(struct tag_t *tag, int fd); //
void *build_tree(); //
void print_tree(struct tag_t *tag, char * shift); //
void print_tree_children(struct tag_t *tag, char *shift); //
void print_tag(struct tag_t *tag); //


void clean_hierarchy() {

	printf("La supression de la hiérarchie est définitive, voulez-vous supprimer ? Entrez Y(es) ou N(o)\n");
	char answer;

	wait_answer:
	scanf("%c", &answer);
	switch(answer) {
		case 'y':
		case 'Y':
			printf("Ok pour suppression.\n");
			break;
		case 'n':
		case 'N':
			printf("Suppression annulée.\n");
			exit(0); //<------------------------------- ATTENTION RETOUR / GESTION D'ERREUR À PRÉCISER
		default :
			printf("Veuillez entrez Y ou y pour valider la suppression, N ou n pour l'annuler.\n");
			while(answer != '\n') scanf("%c", &answer);
			goto wait_answer;
	}

	FILE *f = fopen("tag_hierarchy", "w");
	fclose(f);
}


// ----------------------------------------------------------------------------------------------
// ------------------------------ EXISTENCE TAG DANS LA HIÉRARCHIE ------------------------------


int tag_exists(char *tag_name) {

	int fd = open("tag_hierarchy", O_RDWR);
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


/*
Ajoute un tag dans le système de tags.
Si "father" est NULL, le tag prend "root" comme père.
Si tag_name existe déjà dans la liste des tags, 
Si father est différent de NULL et que le nom n'est pas trouvé, une erreur est renvoyée
**/
int add_tag(char *father, char *tag_name) {

	// --- VÉRIFICATION LONGUEUR DU NOM DU TAG ---

	if(strlen(tag_name) > 18) {
		perror("This tag-name is too long. Try something with 18 letters or less.\n");
		exit(1); //<------------------------------- ATTENTION RETOUR / GESTION D'ERREUR À PRÉCISER
	}

	// --- VÉRIFICATION (NON)EXISTENCE FATHER ET TAG --- // <-- n'utilise pas la fonction de recherche puisque cherche père en même temps...

	int fd = open("tag_hierarchy", O_RDWR);
	int father_exists = FALSE; // défini dans glib library

	struct tag_t *tag = malloc(TAGSIZE);

	while(read(fd, tag, TAGSIZE) != 0) {
		//print_tag(tag);
		if (strcmp(tag->name, tag_name) == 0) {
			perror("This tag-name already exists.");
			exit(1); //<------------------------------- ATTENTION RETOUR / GESTION D'ERREUR À PRÉCISER
		}
		if (father != NULL && strcmp(tag->name, father) == 0) father_exists = TRUE;
	}

	if (father != NULL && !father_exists) {
		perror("The specified father does not already exist. ");
		exit(1); //<------------------------------- ATTENTION RETOUR / GESTION D'ERREUR À PRÉCISER
	}
	
	// --- ÉCRITURE DU TAG DANS LA HIERARCHIE ---

	memset(tag, 0, TAGSIZE);
	memcpy(tag->name, tag_name, strlen(tag_name) + 1);
	tag->name[TAGNAME - 1] = '%';

	if (father == NULL) memcpy(tag->father, "root", strlen("root") + 1);
	else memcpy(tag->father, father, strlen(father) + 1);
	tag->father[TAGNAME-1] = '-';
	write(fd, tag, TAGSIZE);
	close(fd);
	free(tag);

	return 0;
}


// ----------------------------------------------------------------------------------------------
// -------------------------------------- SUPPRIMER UN TAG --------------------------------------


int delete_tag(char *tag_name) {

	// --- CONSTRUCTION ARBRE ET HASHMAP ---

	struct hierarchy *h = build_tree(); 
	GHashTable *point_table = h->point_table;
	struct tag_t *tree = h->tree;

	// --- DEMANDER CONFIRMATION SUPPRESSION ARBORESCENCE ---

	// 1. Avec la hashmap, trouver l'adresse du tag dans l'arbre
	struct tag_t *tag_to_delete = g_hash_table_lookup(point_table, tag_name);

	if (tag_to_delete == NULL) {
		perror("The tag you want to delete does not exist.\n");  // <---- OK VERIFICATION EXISTENCE TAG
		exit(1); //<------------------------------- ATTENTION RETOUR / GESTION D'ERREUR À PRÉCISER
	}

	printf("Vous allez supprimer cette arborescence de tag :\n");
	print_tree_children(tag_to_delete, "");
	printf("La supression sera définitive, voulez-vous supprimer ? Entrez Y(es) ou N(o)\n");
	char answer;

	wait_answer:
	scanf("%c", &answer);
	switch(answer) {
		case 'y':
		case 'Y':
			printf("Ok pour suppression.\n");
			break;
		case 'n':
		case 'N':
			printf("Suppression annulée.\n");
			exit(0); //<------------------------------- ATTENTION RETOUR / GESTION D'ERREUR À PRÉCISER
		default :
			printf("Veuillez entrez Y ou y pour valider la suppression, N ou n pour l'annuler.\n");
			while(answer != '\n') scanf("%c", &answer);
			goto wait_answer;
	}

	// --- SUPPRIMER DE L'ARBORESCENCE ---

	// 2. Trouver l'adresse du père
	struct tag_t *father = g_hash_table_lookup(point_table, tag_to_delete->father);
	struct tag_t **precedent = &(father->children);

	// 3. Introduire les frères de tag_to_delete en tête de liste des enfants du père
	while((*precedent)->brother != tag_to_delete) (*precedent) = (*precedent)->brother;
	(*precedent) -> brother = tag_to_delete->brother;

	// --- RÉ-ÉCRIRE SUR LE FICHIER ---

	int fd = open("tag_hierarchy", O_WRONLY| O_TRUNC);
	write_tree(tree->children, fd); // PS : on ne veut pas écrire root
	close(fd);

	g_hash_table_destroy (point_table);
	free(h);

	return 0;
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


// ----------------------------------------------------------------------------------------------
// ------------------------ CONSTRUCTION HIERARCHIE PAR LECTURE FICHIER -------------------------


void *build_tree() {

	// --- INITIALISATION ---

	GHashTable *point_table = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, &free); 

	// insert root;
	struct tag_t *tree = malloc(TAGSIZE);
	struct tag_t *tag = tree;
	memset(tag, 0, TAGSIZE);
	memcpy(tag->name, "root", strlen("root") + 1);

	g_hash_table_insert(point_table, tag->name, tag); // <--- les VALUES SONT DES POINTEURS --> TO FREE WITH FREE();
														// <---- les KEYS SONT DES STRING EXISTANT DANS LE TAG ! DONC NO NEED TO FREE

	// ---- VARIABLES DE PARCOURS ----

	int fd = open("tag_hierarchy", O_RDONLY);

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

	struct hierarchy *h = malloc(sizeof(struct hierarchy));
	memset(h, 0, sizeof(struct hierarchy));
	h->point_table = point_table;
	h->tree = g_hash_table_lookup(point_table, "root");
	return h;
}


// ----------------------------------------------------------------------------------------------
// ----------------------------------- FONCTIONS D'AFFICHAGES -----------------------------------


void print_hierarchy() {
	struct hierarchy *h = build_tree();
	print_tree(h->tree, "");
	g_hash_table_destroy(h->point_table); // <--- free tous les éléments de tree aussi !
	free(h);
}


void print_tree(struct tag_t *tag, char * shift) { // <---- Code à factoriser ?
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


void print_tree_children(struct tag_t *tag, char *shift) {
	printf("%s%s\n", shift, tag->name);
	if(tag->children != NULL) {
		char *new_shift = shift;
		asprintf(&new_shift, "  %s", shift);
		print_tree(tag->children, new_shift);
		free(new_shift);
	}
}


void print_tag(struct tag_t *tag) {
	printf("--PRINTING TAG--\n");
	printf("Father = %s\n", tag->father);
	printf("Name = %s\n", tag->name);
	printf("Brother = %p\n", tag->brother);
	printf("Child = %p\n", tag->children);
}
