#include "update_paths.h"

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

/**
 * @brief Construit le chemin ~/.tag/stockdelete.txt
 * 
 * @return char* : ~/.tag/stockdelete.txt
 */
char *buildfiledelete()
{
    char *filedelete = calloc(200, 1);
    if (filedelete == NULL)
        erreur("ERREUR calloc filedelete\n");
    strcat(filedelete, getenv("HOME"));
    strcat(filedelete, "/.tag/stockdelete.txt");
    return filedelete;
}

/**
 * @brief Construit le chemin ~/.tag/dest.txt
 * 
 * @return char* : ~/.tag/dest.txt
 */
char *buildfiledest()
{
    char *filedest = calloc(200, 1);
    if (filedest == NULL)
        erreur("ERREUR calloc filedelete\n");
    strcat(filedest, getenv("HOME"));
    strcat(filedest, "/.tag/dest.txt");
    return filedest;
}

/**
 * @brief Construit le chemin ~/.tag/remplace.txt
 * 
 * @return char* : ~/.tag/remplace.txt
 */
char *buildfileremplace()
{
    char *fileremplace = calloc(200, 1);
    if (fileremplace == NULL)
        erreur("ERREUR calloc filedelete\n");
    strcat(fileremplace, getenv("HOME"));
    strcat(fileremplace, "/.tag/remplace.txt");
    return fileremplace;
}

/**
 * @brief Supprime de paths.txt les fichiers contenus dans stockdelete.txt si les conditions sont réunies
 * 
 */
void deletefromstock()
{
    char *filedelete = buildfiledelete();
    FILE *delete = fopen(filedelete, "r");
    if (delete == NULL)
        erreur("ERREUR fopen stockdelete.txt\n");
    
    char ligne[500] = "";
    while (fgets(ligne, 500, delete) != NULL)
    {
        char *tmp = realpath(ligne, NULL);
        if (tmp == NULL)
            delete_path2(tmp);
        free(tmp);
    }
    fclose(delete);
    free(filedelete);
    /*char *line_buff = NULL;
    size_t line_buff_size = 0;
    size_t line_size;
    line_size = getline(&line_buff, &line_buff_size, delete);

    while (line_size >= 0)
    {
        //line_buff[strcmp(line_buff, "\r\n")] = '\0';
        char *tmp = realpath(line_buff, NULL);
        if (tmp == NULL)
            delete_path2(line_buff);
        line_size = getline(&line_buff, &line_size, delete);
        free(tmp);
    }
    fclose(delete);
    free(delete);
    free(line_buff);*/
}

/**
 * @brief Ajoute dans paths.txt les fichiers de remplace.txt si les conditions sont remplies
 * 
 */
void addfromremplace()
{
    char *fileremplace = buildfileremplace();
    FILE *remplace = fopen(fileremplace, "r");
    if (remplace == NULL)
        erreur("ERREUR fopen remplace.txt\n");
    char line[500] = "";
    while (fgets(line, 500 ,remplace) != NULL)
    {
        line[strcspn(line, "\n")] = '\0';
        char *remp = realpath(line, NULL);
        if (remp != NULL && find_path(remp) == 0)
            add_path(line);
        free(remp);
    }
    fclose(remplace);
    free(fileremplace);
    /*char *line_buff = NULL;
    size_t line_buff_size = 0;
    size_t line_size;
    line_size = getline(&line_buff, &line_buff_size, remplace);
    while (line_size >= 0)
    {
        line_buff[strcspn(line_buff, "\n")] = '\0';
        printf("line : %s\n", line_buff);
        char *remp =realpath(line_buff, NULL);
        if (remp != NULL && find_path(remp) == 0)
            add_path(line_buff);
        line_size = getline(&line_buff, &line_size, remplace);
        free(remp);
    }
    fclose(remplace);
    free(fileremplace);
    free(line_buff);*/
}

/**
 * @brief Retourne le chemin de la destination stockée dans dest.txt
 * 
 * @return char* : destination
 */
char *getdestination()
{
    char *filedest = buildfiledest();
    FILE *dest = fopen(filedest, "r");
    if ( dest == NULL)
        erreur("ERREUR open\n");
    char *line_buff = NULL;
    size_t line_buf_size = 0;
	ssize_t line_size;
    line_size = getline(&line_buff, &line_buf_size, dest);
    line_buff[strcspn(line_buff, "\n")] = '\0';
    char *pathdest = realpath(line_buff, NULL);
    if (pathdest == NULL)
        exit(EXIT_SUCCESS);
    
    free(line_buff);
    fclose(dest);
    free(filedest);
    return pathdest;
}

void choose_dir(char *path)
{
    chdir(path);
       DIR *dir = opendir(path);
       struct dirent *entry;
       while ((entry=readdir(dir)))
       {
            char buff_tag[1024];
            ssize_t istag = getxattr(entry->d_name, "user.tags", buff_tag, sizeof(buff_tag));
            if (istag >= 0)
            {
                if (find_path(entry->d_name) == 0)
                    add_path(entry->d_name);
            }
            else
            {
                if (find_path(entry->d_name) == 1)
                    delete_path(entry->d_name);
            }   
        }
        free(dir); 
}

/**
 * @brief Ajoute ou supprime de paths.txt les fichiers de pathdest si c'est un repertoire ou ceux du repertoire courant sinon
 * 
 * @param pathdest : destination 
 */
void add_delete_in_dir(char *pathdest)
{
   struct stat sb;

   if (stat(pathdest, &sb) == -1)
        erreur("ERREUR stat\n");
   if (S_ISDIR(sb.st_mode))
       choose_dir(pathdest);      
   else
        choose_dir(".");
}