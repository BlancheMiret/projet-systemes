
typedef struct tabdyn
{
    int size;
    int capacity;
    char **tab;
}tabdyn;

tabdyn create_table();

void push(tabdyn *t, char *path);

void deletetable(tabdyn t);

void affiche(tabdyn t);