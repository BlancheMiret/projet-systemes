#include "update_paths.c"


void execmv(char **argv)
{
    int pid = fork();
    if (pid < 0)
        erreur("ERREUR pid\n");
    else if(pid > 0)
    {
        execvp("mvt", argv+1);
        exit(EXIT_FAILURE);
    }
    pid = wait(NULL);
}
/*
typedef struct tabdyn
{
    int size;
    int capacity;
    char **tab;
}tabdyn;

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

void deletet(tabdyn t)
{
    for (int i = 0; i < t.size; i++)
        free(t.tab[i]);
    free(t.tab);
}

void affiche(tabdyn t)
{
    for (int i = 0; i < t.size; i++)
        printf("element %d : %s\n", i, t.tab[i]);
}

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

void lister(char *file, char *dest, tabdyn *t)
{
    char *remplace = traitement(file, dest);
    push(t, remplace);
    struct stat sb;
    char *pwd;

    if(stat(file, &sb)==-1)
        erreur("ERREUR stat\n");

    if(S_ISDIR(sb.st_mode)){
        pwd = getcwd(NULL, 0);
        chdir(file);
        
        DIR *dirp = opendir(".");
        struct dirent *entry;
        
        while((entry=readdir(dirp))){
            if(strcmp(entry->d_name, ".")!=0 && strcmp(entry->d_name, "..")!=0)
            {
                char *tmp = absolute_path(entry->d_name);
                lister(tmp, remplace, t); 
                free(tmp);
            }
        }
        free(remplace);
        closedir(dirp);
        chdir(pwd);
        free(pwd);
        free(remplace);
    }
}

void lister_supp(char *path, char *dest,tabdyn *t, tabdyn *t2)
{
    int is = 0;
    char *remplace = traitement(path, dest);
    char *abspath = absolute_path(path);
    if (find_path(abspath))
    {
        push(t, abspath);
        push(t2, remplace);
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
                lister_supp(entry->d_name, remplace, t, t2);
        }
        free(pwd);
        closedir(dirp);
    }
    free(abspath);
    free(remplace);
}
*/
int main(int argc, char *argv[])
{
    init_file_paths();
    char *pwd = getcwd(NULL, 200);
    int debut = -1;
    int backup = -1;
    int t = -1;
    tabdyn tremplace = create_table();
    tabdyn tdelete = create_table();
    
    for (int i = 2; i < argc; i++)
    {
        char *tmp = realpath(argv[i], NULL);
        if (strcmp(argv[i], "-t") == 0)
            t = i;
        else if (tmp != NULL && debut == -1)
            debut = i;
        free(tmp);
    }
    if (debut == -1)
        goto exe;
    
    int nbfile = argc-debut-1;
    if (t!=-1)
    {
        char *tmp = realpath(argv[t+1], NULL);
        if (tmp == NULL)
        {
            push(&tremplace, argv[t+1]);
            push(&tdelete, absolute_path(argv[t+2]));
        }
        else
        {
            for (int i = t+2; i < argc; i++)
            {
                char *arg1 = absolute_path(argv[i]);
                char *arg2 = absolute_path(argv[t+1]);
                lister_supp(arg1, arg2, &tdelete, &tremplace);
                free(arg1);
                free(arg2);
            }
        }
        free(tmp);
    }
    else
    {   
        char *tmp = realpath(argv[argc-1], NULL);
        if (tmp == NULL)
        {
            push(&tremplace, argv[argc-1]);
            push(&tdelete, absolute_path(argv[debut]));
            affiche(tdelete);
        }
        else
        {
            for (int i = debut; i < argc-1; i++)
            {
                char *arg1 = absolute_path(argv[i]);
                char *arg2 = absolute_path(argv[argc-1]); 
                lister_supp(arg1, arg2, &tdelete, &tremplace);
                free(arg1);
                free(arg2);
            }
        }
        free(tmp);
    }
    exe : chdir(pwd);
    execmv(argv);
    /*init_file_paths();
    printf("DEBUT\n");
    int debut = 0;
    int backup = 0;
    int t = 0;
    tabdyn tremplace = create_table();
    tabdyn tdelete = create_table();
    
    for (int i = 2; i < argc; i++)
    {
        printf("arg : %s\n", argv[i]);
        char *tmp = realpath(argv[i], NULL);
        printf("tmp : %s\n", tmp);
        if (strcmp(argv[i], "-t") == 0)
            t = i;
        else if (tmp != NULL && debut == 0)
            debut = i;
        free(tmp);
    }
    int nbfile = argc-debut-1;
    printf("NB : %d DEBUT : %d\n", nbfile, debut);
    if (t!=0)
    {
        char *tmp = realpath(argv[t+1], NULL);
        if (tmp == NULL)
        {
            push(&tremplace, argv[t+1]);
            push(&tdelete, absolute_path(argv[t+2]));
        }
        else
        {
            for (int i = t+2; i < argc; i++)
            {
                char *arg1 = absolute_path(argv[i]);
                char *arg2 = absolute_path(argv[t+1]);
                lister_supp(arg1, arg2, &tdelete, &tremplace);
                free(arg1);
                free(arg2);
                free(tmp);
            }
        }
    }
    else
    {   
        printf("PAS DE T\n");
        char *tmp = realpath(argv[argc-1], NULL);
        printf("temp  : %s\n", tmp);
        if (tmp == NULL)
        {
            printf("changename\n");
            push(&tremplace, argv[argc-1]);
            push(&tdelete, absolute_path(argv[debut]));
            affiche(tdelete);
        }
        else
        {
            for (int i = debut; i < argc-1; i++)
            {
                char *arg1 = absolute_path(argv[i]);
                char *arg2 = absolute_path(argv[argc-1]); 
                lister_supp(arg1, arg2, &tdelete, &tremplace);
                free(arg1);
                free(arg2);
            }
        }
    }*/
    
    char filedelete[150] = "";
    char fileremp[150] = "";
    char filedest[150] = "";
    strcat(filedelete, getenv("HOME"));
    strcat(fileremp, filedelete);
    strcat(filedest, filedelete);
    strcat(filedelete, "/.tag/stockdelete.txt");
    strcat(fileremp, "/.tag/remplace.txt");
    strcat(filedest, "/.tag/dest.txt");
    FILE *delete = fopen(filedelete, "w");
    FILE *remplace = fopen(fileremp, "w");
    FILE *dest = fopen(filedest, "w");
    if (delete == NULL || remplace == NULL || dest == NULL)
        erreur("ERREUR fopen\n");
    
    for (int i = 0; i < tdelete.size; i++)
        fprintf(delete, "%s\n",tdelete.tab[i]);
    for (int i = 0; i < tremplace.size; i++)
        fprintf(remplace, "%s\n",tremplace.tab[i]);
    if (t != -1)
    {
        char *arg = realpath(argv[t+1], NULL);
        if (arg != NULL)
            fprintf(dest, "%s\n", arg);
        else 
            fprintf(dest, "%s\n", argv[t+1]);
        free(arg);
    }
    else
    {
        char *arg = realpath(argv[argc-1], NULL);
        if (arg != NULL)
            fprintf(dest, "%s\n", arg);
        else
            fprintf(dest, "%s\n", argv[argc-1]);
        free(arg);
    }
    fclose(delete);
    fclose(remplace);

    affiche(tdelete);
    affiche(tremplace);
    deletetable(tdelete);
    deletetable(tremplace);
    free(pwd);
   
    return 0;
}
