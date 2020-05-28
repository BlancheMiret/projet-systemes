#include "paths_manager.c"

void erreur(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    char *HOME = getenv("HOME");
    if (HOME == NULL)
        erreur("ERREUR getenv\n");
    char pathfile[100] = "";
    strcat(pathfile, HOME);
    strcat(pathfile, "/.tag/paths.txt");
    printf("%s\n", pathfile);
    if (argc != 2)
        erreur("ERREUR nombre d'arguments invalide\n");
    
    char *arg = absolute_path(argv[1]);
    if (arg == NULL)
        erreur("ERREUR argument invalide\n");
    
    if (find_path(pathfile, arg) == 1)
        delete_path(arg);
    execlp("rm", "rm", (const char *)arg, NULL);
    
    return 0;
}
