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

#define FLSE 0
#define TRE 1
#define TAGSIZE 20

// Pour créer l'arbre de la hiérarchie au moment de l'affichage ? 
struct tag_node {
	char		name[TAGSIZE];
	struct tag	*father;
	struct tag	*brother;
};

// Pour parcourir les fichiers <------------------------ dans le fichier tagh 
struct tag_t {
	char 			father[TAGSIZE];
	char 			name[TAGSIZE];
	int 			tag_id;
	struct tag_t 	*next;
};


/*
Pour ajouter un tag :
- ouvrir le fichier list en mode lecture
- le parcourir pour regarder si le tag existe déjà (provoquer une erreur) (et si le père existe bien)
- ouvrir le fichier list en mode écriture
- ajouter le tag à la fin du fichier list 
- ouvrir le fichier hierarchy en mode écriture 
- 
*/
/*
Comportement :
	- fils existe déjà -> erreur
	- père non spécifié (NULL) -> considère root
	- père spécifié n'existe pas -> erreur <------------- à faire : le créer, ayant lui même root comme père
*/

int add_tag(char *father, char *tag);
void print_table(struct tag_t *tag_tab[], int id);
void print_hierarchy();
void print_tag(struct tag_t *tag);


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
	*/
	print_hierarchy();
	

}


/*
Vendredi 17 avril :
- Puisque séparateur % dans la liste de tag, possibilité d'utiliser la fonction getdelim. MAIS fonctionne avec un stream
Ici : explique que pour fonctionner avec un délimiteur à partir d'un fd il faudrait demander à l'OS un caractère par un caractère -> terriblement non efficace.
https://stackoverflow.com/questions/34064049/getline-with-a-file-descriptor-instead-of-a-file-pointer
- Globalement, vaut-il mieux utiliser les descripteurs ou les streams ? Une création de stream fait un appel système en interne, donc moins rapide...

*/

int add_tag(char *father, char *tag_name) {

	// ------------------- VÉRIFICATION PAS CARACTÈRE SPÉCIAL ---------------------------



	// ------------------- VÉRIFICATION (NON)EXISTENCE FATHER ET TAG --------------------

	int fdr = open("tagl", O_RDONLY); //<--- le fichier doit exister ! Créé par Louis avec programme 0

	int father_exists = FLSE;

	char buf[TAGSIZE] = {0}; //<--- nécessaire pour strcmp !! //Problèmatique du nom qui fait plus que nécessaire... D'où structure... !!! Sauf si écrit directement
	while(read(fdr, buf, TAGSIZE) != 0) {
		printf("name read : %s\n", buf);
		printf("result of strcmp :  %d\n", strcmp(buf, tag_name));
		if (strcmp(buf, tag_name) == 0) {
			perror("This tag-name exists already.");
			exit(1); //<---- ou return -1 ?
		}
		if (father != NULL && strcmp(buf, father) == 0) father_exists = TRE;
		memset(buf, 0, TAGSIZE);
	}

	if (father != NULL && !father_exists) {
		perror("The specified father does not already exist. ");
		exit(1); 
	}

	close(fdr);

	// ---------------------------- AJOUT DU TAG DANS LA LISTE ---------------------------

	// - tag n'existe pas
	// - le père existe, ou n'est pas spécifié (NULL -> root)

	int fdw = open("tagl", O_WRONLY | O_APPEND);
	memset(buf, 0, TAGSIZE);
	buf[TAGSIZE - 1] = '%';
	memcpy(buf, tag_name, strlen(tag_name));
	write(fdw, buf, TAGSIZE);
	printf("I have written : %s\n", buf);
	close(fdw);

	// -------------------------- AJOUT DU TAG DANS LA HIERARCHY -------------------------

	fdw = open("tagh", O_WRONLY | O_APPEND);

	struct tag_t tag;
	memset(&tag, 0, sizeof(struct tag_t));
	memcpy(tag.name, buf, TAGSIZE);
	if (father == NULL) memcpy(tag.father, "root", strlen("root"));
	else memcpy(tag.father, father, strlen(father));
	tag.father[TAGSIZE - 1] = '-';
	tag.tag_id = 0;
	write(fdw, &tag, sizeof(struct tag_t));
	close(fdw);

	return 0;
}

/*
int test_function(const void *s1, const void *s2) {
	if (strcmp(s1, s2) == 0) return TRUE;
	return FALSE;
}
*/

void print_hierarchy() {

	// ---------------------------- INITIALISATION -------------------------

	GHashTable *id_table = g_hash_table_new(g_str_hash, g_str_equal);
	int *id_count = malloc(sizeof(int));
	*id_count = 0;

	g_hash_table_insert(id_table, "root", id_count);
	struct tag_t *tag_tab[1024] = {0};
	int fdr = open("tagh", O_RDONLY);

	// ---------------------------- VARIABLES DE PARCOURS -------------------------

	int SIZE = sizeof(struct tag_t);
	struct tag_t *tag = NULL; 
	char *buf = malloc(SIZE);
	memset(buf, 0, SIZE);
	int *father_id = malloc(sizeof(int));


	// ---------------------------- LIRE LE FICHIER, CONSTRUIRE LE TABLEAU DE HIÉRARCHIE EN MÉMOIRE  -------------------------

	
	while(read(fdr, buf, SIZE) != 0) { //lire structure par structure
		
		tag = (struct tag_t*)buf;

		//printf("Father is : %s\n", tag->father);
		//printf("Sizeof father is : %lu\n", strlen(tag->father));

		// 1. Trouver le numéro du père, Attribuer id et Hacher le nom
		father_id = g_hash_table_lookup(id_table, tag->father); //retrouve l'id du père
		//printf("father id : %d\n", *father_id);
		int i = *id_count;
		id_count = malloc(sizeof(int));
		*id_count = i + 1;

		tag->tag_id = *id_count; // attribue un id

		//print_tag(tag); // <-- ok
		g_hash_table_insert(id_table, tag->name, id_count); // ajoute le nom dans la table des id

		// 2. Ajouter au tableau : avoir une variable tag2 qui parcourt la liste des tag dans une case donnée du tableau

		struct tag_t **tagbis = &tag_tab[*father_id];
		//print_tag(tag);

		while((*tagbis) != NULL) tagbis = &((*tagbis)->next);
 
		*tagbis = tag;

		// 3. Pour la prochaine itération
		buf = malloc(SIZE);
		memset(buf, 0, SIZE);

	}


	close(fdr);

	// ---------------------------- AFFICHER LE TABLEAU DE HIÉRARCHIE -------------------------

	print_table(tag_tab, 0);

}

void print_tag(struct tag_t *tag) {
	printf("--PRINTING TAG--\n");
	printf("Father = %s\n", tag->father);
	printf("Name = %s\n", tag->name);
	printf("Id = %d\n", tag->tag_id);
	printf("Next = %p\n", tag->next);
}

void print_table(struct tag_t *tag_tab[], int id) {

	
	struct tag_t *tag = tag_tab[id];

	while(tag != NULL) {
		printf("%s\n", tag->name);
		id = tag->tag_id;
		if(tag_tab[id] != NULL) print_table(tag_tab, id);
		tag = tag->next;
	}
	

}


/*

-------------  TROIS BESOINS 
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

-----------  FONCTIONS DE LECTURE

read : écrit dans un buffer jusqu'à taille donnée, depuis fd * <------- sans doute la meilleure solution !!! Avec une écriture de structure donc. 
fread : écrit dans à l'adresse d'un pointeur un nb d'objets d'une taille donnée, depuis stream

getdelim : écrit dans un buffer jusqu'à délimiteur
ATTENTION, f = FILE pas un descripteur
char *linep = NULL;
size_t linecapp = 0;
getdelim(&linep, &linccapp, '%', f) // allocate memory accordingly. 

memmem : renvoie pointeur vers 1ère occurence d'un délimiteur dans un CHAR * <---- implique d'abord de lire tous le fichier dans un buffer char*...


-------------  QUESTION
	- est-ce un arbre, qu'il faut recréer à chaque fois
	- ou une façon de lire plus "brutale". 

*/






