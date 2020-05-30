#include "paths_manager.c"
#include <dirent.h>

void erreur(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int add_path2(char * filename)
{
	FILE  *file;
	file = fopen(file_paths, "a");

	if (file == NULL)
	{
		fprintf(stderr, "cannot open %s for appending\n", file_paths);
		return 0;
	}
    fprintf(file, "%s\n", filename);
	fclose(file);
	return 1;
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
    char *remplace = malloc(600);
    if (remplace == NULL)
        erreur("ERREUR malloc\n");
    
    strcat(remplace, arg2);
    if (arg2[strlen(arg2)] != '/')
        strcat(remplace, "/");
    strcat(remplace, namefile);
    if (find_path(arg1) == 1)
    {
        delete_path(arg1);
        add_path2(remplace);
    }
    return remplace;
}



void lister(char *file, char *dest)
{
    char *remplace = traitement(file, dest);
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
                lister(absolute_path(entry->d_name), remplace); 
        }
        free(remplace);
        closedir(dirp);
        chdir(pwd);
        free(pwd);
    }
}

int main(int argc, char *argv[])
{

    if (argc < 3)
        erreur("ERREUR nombre d'arguments invalide\n");
    
    int debfile = 0;
    int nbfile = 0;
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] != '-' && debfile == 0)
            debfile = i;
        if (argv[i][0] == '-' && debfile > 0)
            erreur("ERREUR arguments invalides\n");
        if (argv[i][0] != '-')
            nbfile++;     
    }
    if (nbfile < 2)
        erreur("ERREUR nombre d'arguments invalide\n");

    if (nbfile == 2)
    {
         struct stat sb, sb2;
        if (stat(argv[debfile], &sb) == -1 || stat(argv[argc-1], &sb2) == -1)
            erreur("ERREUR stat\n");
        
        if(S_ISDIR(sb.st_mode) && !S_ISDIR(sb2.st_mode))
            erreur("ERREUR dernier argument n'est pas un répertoire\n");
    }
    
    if (argc > 3 && nbfile > 2)
    {
        struct stat sb;
        if (stat(argv[argc-1], &sb) == -1)
            erreur("ERREUR stat\n");
        if(!S_ISDIR(sb.st_mode))
            erreur("ERREUR dernier argument n'est pas un répertoire\n");
    }

    init_file_paths();
    for (int i = debfile; i < argc-1; i++)
    {
        lister(argv[i], absolute_path(argv[argc-1]));
    }
    execvp("mv", argv);
    return 0;
}
