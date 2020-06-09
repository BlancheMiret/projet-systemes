#include "paths_manager.c"
#include "tabdyn.h"

/**
 * @brief Affiche un message d'erreur sur la sortie d'erreur standard et quitte
 * 
 * @param msg : message d'erreur
 */
void erreur(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

/**
 * @brief Création d'un tableau dynamique
 * 
 * @return tabdyn : tableau dynamique
 */
tabdyn create_table()
{
    tabdyn t;
    t.tab = malloc(100);
    if (t.tab == NULL)
        erreur("ERREUR malloc tabdyn\n");
    t.capacity = 100;
    t.size = 0;
    return t;
}

/**
 * @brief Ajoute une chaîne de charactères au tableau dynamique passé en paramètre
 * 
 * @param t : pointeur sur un tableau dynamique
 * @param path : chaîne de charactères à stocker
 */
void push(tabdyn *t, char *path)
{
    if (t->size >= t->capacity)
    {
        char **temp = (char **)realloc(t->tab, 2*t->capacity);
        if (temp == NULL)
            erreur("ERREUR realloc\n");
        t->tab = temp;
        t->capacity *=2;
    }
    size_t s = strlen(path);
    t->tab[t->size] = malloc(strlen(path)+1);
    if (t->tab[t->size] == NULL)
        erreur("ERREUR malloc push\n");
    memcpy(t->tab[t->size], path, strlen(path));
    t->tab[t->size][s] = '\0';
    t->size++;
}

/**
 * @brief Supprime un tableau dynamique
 * 
 * @param t : tableau dynamique à supprimer
 */
void deletetable(tabdyn t)
{
    for (int i = 0; i < t.size; i++)
        free(t.tab[i]);
    free(t.tab);
}

/**
 * @brief Affiche les éléments du tableau dynamique passé en paramètre
 * 
 * @param t : tableau dynamique
 */
void affiche(tabdyn t)
{
    for (int i = 0; i < t.size; i++)
        printf("element %d : %s\n", i, t.tab[i]);
}