#include "paths_manager.c"

void erreur(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
        erreur("ERREUR nombre d'arguments invalide\n");
    
    char *arg1 = absolute_path(argv[1]);
    char *arg2 = absolute_path(argv[2]);
    if (arg1 == NULL || arg2 == NULL)
        erreur("ERRUER arguments invalides\n");
    if (find_path(arg1) == 1)
    {
        printf("okay\n");
        delete_path(arg1);
        printf("ohh\n");
        add_path(arg2);
        printf("chemin remplacé\n");
    }

    execlp("mv", "mv", (const char *)arg1, (const char *)arg2, NULL);
    return 0;
}
