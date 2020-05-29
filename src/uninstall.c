#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "tag_hierarchy.h"
#include "research.h"
#include "tag_file.h"
#include "paths_manager.h"

void erreur( char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}


int main(int argc, char const *argv[])
{    
    char pathfile[100] = "";
    char *HOME = getenv("HOME");
    if (HOME == NULL)
        erreur("ERREUR getenv");
    strcat(pathfile, HOME);
    strcat(pathfile, "/.tag");
    char dir[100] = "";
    strcpy(dir, pathfile);
    strcat(pathfile, "/paths.txt");
    reset_all_files();
    execlp("rm", "rm", "-RF", dir, NULL);
    return 0;
}


