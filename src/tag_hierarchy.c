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


// DÉCLARATION FONCTIONS INTERNES 
void write_tree(struct tag_t *tag, int fd); //
void *build_tree(); //
void print_tree(struct tag_t *tag, char * shift); //
void print_tree_children(struct tag_t *tag, char *shift); //
void print_tag(struct tag_t *tag); //

/*
Supprime toute la liste et hiérarchie de tags existante en demandant confirmation à l'utilisateur.
*/
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
// -------------------------------- OBTENIR LES ENFANTS D'UN TAG --------------------------------

void print_list(struct tag_node *tag_list) {
	struct tag_node *tag = tag_list;
	while(tag != NULL) {
		printf("%s\n", tag->name);
		tag = tag->next;
	}
}


void write_tag_list(struct tag_t *tag, struct tag_node **list) {
	while(tag != NULL) {
		struct tag_node *temp = *list; // temp est la liste
		*list = malloc(sizeof(struct tag_node));
		memcpy((*list)->name, tag->name, TAGNAME);
		(*list)->next = temp;

		if(tag->children != NULL) write_tag_list(tag->children, list);

		tag = tag->brother;
	}
}

void *get_tag_children(char *tag_name) {

	// --- CONSTRUCTION ARBRE ET HASHMAP ---

	struct hierarchy *h = build_tree(); 
	GHashTable *point_table = h->point_table;

	// 1. Avec la hashmap, trouver l'adresse du tag dans l'arbre
	struct tag_t *tag = g_hash_table_lookup(point_table, tag_name);

	if (tag == NULL) {
		perror("The tag you want the children from does not exist.\n");  // <---- OK VERIFICATION EXISTENCE TAG
		exit(1); //<------------------------------- ATTENTION RETOUR / GESTION D'ERREUR À PRÉCISER
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
ou si father est différent de NULL et que le nom n'est pas trouvé, une erreur est renvoyée
**/
int add_tag(char *father, char *tag_name) {

	// --- VÉRIFICATION LONGUEUR DU NOM DU TAG ---

	if(strlen(tag_name) > 18) {
		printf("This tag-name is too long. Try something with 18 letters or less.\n");
		exit(1); //<------------------------------- ATTENTION RETOUR / GESTION D'ERREUR À PRÉCISER
	}

	// --- VÉRIFICATION (NON)EXISTENCE FATHER ET TAG --- // <-- n'utilise pas la fonction de recherche puisque cherche père en même temps...

	int fd = open("tag_hierarchy", O_RDWR);
	int father_exists = FALSE; // défini dans glib library

	struct tag_t *tag = malloc(TAGSIZE);

	while(read(fd, tag, TAGSIZE) != 0) {
		//print_tag(tag);
		if (strcmp(tag->name, tag_name) == 0) {
			printf("This tag-name already exists.\n");
			exit(1); //<------------------------------- ATTENTION RETOUR / GESTION D'ERREUR À PRÉCISER
		}
		if (father != NULL && strcmp(tag->name, father) == 0) father_exists = TRUE;
	}

	if (father != NULL && !father_exists) {
		printf("The specified father does not exist in the tag hierarchy.\n");
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
	if (*precedent == tag_to_delete) {
		*precedent = tag_to_delete->brother;
	} else {
		while((*precedent)->brother != tag_to_delete) (*precedent) = (*precedent)->brother;
		(*precedent) -> brother = tag_to_delete->brother;
	}

	// --- RÉ-ÉCRIRE SUR LE FICHIER ---

	int fd = open("tag_hierarchy", O_WRONLY| O_TRUNC);
	write_tree(tree->children, fd); // PS : on ne veut pas écrire root
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

/*
Construit l'arbre de la hiérarchie des tags et l'affiche intégralement en utilisant print_tree()
*/
void print_hierarchy() {
	struct hierarchy *h = build_tree();
	print_tree(h->tree, "");
	g_hash_table_destroy(h->point_table); // <--- free tous les éléments de tree aussi !
	free(h);
}

/*
Prend l'adresse d'une structure tag_t, un décalage d'affichage, 
affiche de façon récursive toute l'arborescence à partir de cette structure en prenant en compte ses frères.
*/
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
