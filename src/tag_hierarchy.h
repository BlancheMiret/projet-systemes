#ifndef TAG_MANAGER_H
#define TAG_MANAGER_H 

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

struct tag_node {
	char				name[TAGNAME];
	struct tag_node		*next;
};

// Renvoie un pointeur vers un tag_node, liste chainée d'enfants de tag_name
void *get_tag_children(char *tag_name);

// Efface le fichier contenant la hiérarchie en demandant confirmation à l'utilisateur
void clean_hierarchy();

// Affiche la hiérarchie des tags créés
void print_hierarchy();
void print_list(struct tag_node *tag_list); // à SUPPRIMER ?

// Renvoie 1 si le tag de nom "tag_name" existe dans la hiérarchie, 0 sinon
int tag_exists(char *tag_name);

// Ajoute le tag "tag_name" à la hiérarchie comme enfant de "father"
// Si "father" est à NULL, alors le tag est ajouté par défaut à la racine
// Sort du programme si le tag existe déjà ou si le père spécifié n'existe pas
int add_tag(char *father, char *tag_name);

// Supprime le tag de nom "tag_name" de la hiérarchie en demandant confirmation à l'utilisateur
int delete_tag(char *tag_name);

#endif