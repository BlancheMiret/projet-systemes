#include "tabdyn.c"
#include <dirent.h>
#include <sys/wait.h>

char* traitement(char *path, char *dest);

void lister_supp(char *path, char *dest,tabdyn *remove, tabdyn *add);

char *buildfiledelete();

char *buildfiledest();

char *buildfileremplace();

void choose_dir(char *path);

void add_delete_in_dir(char *pathdest);