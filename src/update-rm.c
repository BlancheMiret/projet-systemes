#include "update_paths.c"

void execrm(char **argv)
{
    int pid = fork();
    if (pid < 0)
        erreur("ERREUR pid\n");
    else if(pid > 0)
    {
        execvp("rmta", argv+1);
        exit(EXIT_FAILURE);
    }
    pid = wait(NULL);
}

int main(int argc, char *argv[])
{
    init_file_paths();
    char *pwd = getcwd(NULL, 200);
    int debut = -1;
    tabdyn tremplace = create_table();
    tabdyn tdelete = create_table();
    
    for (int i = 2; i < argc; i++)
    {
        char *tmp = realpath(argv[i], NULL);
        if (tmp != NULL && debut == -1)
            debut = i;
        free(tmp);
    }
    for (int i = debut; i < argc; i++)
    {
        char *arg1 = absolute_path(argv[i]);
        char *arg2 = absolute_path(argv[argc-1]); 
        lister_supp(arg1, arg2, &tdelete, &tremplace);
        free(arg1);
        free(arg2);
    }
    chdir(pwd);
    execrm(argv);

     if (debut == -1)
        return 0;
    char *filedelete = buildfiledelete();
    FILE *delete = fopen(filedelete, "w");
    if (delete == NULL)
        erreur("ERREUR fopen rm delete\n");
     for (int i = 0; i < tdelete.size; i++)
        fprintf(delete, "%s\n",tdelete.tab[i]);
    
    fclose(delete);
    deletetable(tdelete);
    deletetable(tremplace);
    free(pwd);
    free(filedelete);
    return 0;
}
