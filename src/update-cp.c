#include "update_paths.c"

void execcp(char **argv)
{
    int pid = fork();
    if (pid < 0)
        erreur("ERREUR pid\n");
    else if(pid > 0)
    {
        execvp("cpt", argv+1);
        exit(EXIT_FAILURE);
    }
    pid = wait(NULL);
}

int main(int argc, char const *argv[])
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
    struct stat sb;
    if (t!=-1)
    {
        char *tmp = realpath(argv[t+1], NULL);
        if (tmp != NULL)
        {
            if (stat(tmp, &sb) == NULL)
                erreur("ERREUR stat\n");
            if (!S_ISDIR(sb.st_mode))
            {
                char *tmp2 = absolute_path(argv[t+2]);
                push(&tremplace, argv[t+1]);
            }
            
        }
        else if (tmp == NULL)
        {
            char *tmp = absolute_path(argv[t+2]);
            push(&tremplace, argv[t+1]);
            push(&tdelete, tmp);
            free(tmp);
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
            char *tmp2 = absolute_path(argv[debut]);
            push(&tremplace, argv[argc-1]);
            push(&tdelete, tmp2);
            free(tmp2);
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
    execcp(argv);
    return 0;
}
