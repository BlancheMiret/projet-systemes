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
    execrm(argv);
    return 0;
}
