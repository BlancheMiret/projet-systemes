#include "paths_manager.c"

void erreur(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char const *argv[])
{
    char *HOME = getenv("HOME");
    if (HOME == NULL)
        erreur("ERREUR getenv\n");
    char pathfile1 [100] = "";
    strcat(pathfile1,HOME);
    strcat(pathfile1, "/.tag/paths.txt");

    if (argc != 3)
        erreur("ERREUR nombre d'arguments invalide\n");
    
    char *arg1 = absolute_path(argv[1]);
    char *arg2 = absolute_path(argv[2]);
    if (arg1 == NULL || arg2 == NULL)
        erreur("ERRUER arguments invalides\n");
    if (find_path(pathfile1, arg1) == 1)
    {
        printf("okay\n");
        delete_path(arg1);
        printf("ohh\n");
        add_path(arg2);
        printf("chemin remplacé\n");
    }

    execlp("mv", "mv", arg1, arg2, NULL);
    return 0;
}
