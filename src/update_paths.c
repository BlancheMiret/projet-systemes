#include "tabdyn.c"
#include <dirent.h>
#include <sys/wait.h>

/**
 * @brief Renvoie le chemin du fichier path dans sa nouvelle destination dest
 * 
 * @param path : fichier à deplacer
 * @param dest : destination
 * @return char* : le nouveau chemin de path
 */
char* traitement(char *path, char *dest)
{
    char *arg1 = absolute_path(path);
    char arg2[500] = "";
    strcat(arg2, dest);
    if (arg1 == NULL)
        erreur("ERREUR arguments invalides\n");
    int debut = -1;
    for (int j = 0; j < strlen(arg1); j++)
    {
        if (arg1[j] == '/')
            debut = j;
    }
   char namefile[100] = "";
    strcat(namefile, arg1+debut+1);
    char *remplace = calloc(600, 1);
    if (remplace == NULL)
        erreur("ERREUR malloc\n");
    strcat(remplace, arg2);
    if (arg2[strlen(arg2)] != '/')
        strcat(remplace, "/");
    strcat(remplace, namefile);

    free(arg1);
    return remplace;
}


/**
 * @brief Traite un élément (fichier ou repertoire) pour trouver son nouveau chemin et celui de ses sous-éléments si il en a
 * 
 * @param path : fichier à deplacer (fichier simple ou repertoire)
 * @param dest : destination
 * @param remove : tableau dynamique où sont stockés les chemins à supprimer
 * @param add : tableau dynamique où sont stockés les nouveaux chemins 
 */
void lister_supp(char *path, char *dest,tabdyn *remove, tabdyn *add)
{
    int is = 0;
    char *remplace = traitement(path, dest);
    char *abspath = absolute_path(path);
    if (find_path(abspath))
    {
        push(remove, abspath);
        push(add, remplace);
    }
    else
        is = 1;
    
    struct stat sb;
    char *pwd;

    if (stat(path, &sb) == -1)
        erreur("ERREUR stat list_sup\n");
    
    if (S_ISDIR(sb.st_mode))
    {
        pwd = getcwd(NULL, 0);
        chdir(path);

        DIR *dirp = opendir(".");
        struct dirent *entry;

        while ((entry=readdir(dirp)))
        {
            if (strcmp(entry->d_name, ".")!= 0 && strcmp(entry->d_name, "..") != 0)
                lister_supp(entry->d_name, remplace, remove, add);
        }
        free(pwd);
        closedir(dirp);
    }
    free(abspath);
    free(remplace);
}
