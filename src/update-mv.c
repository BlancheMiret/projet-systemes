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

char* traitement2(char *path, char *dest)
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
        if (find_path(arg2) != 1)
                add_path2(absolute_path(dest));
    }
    else
    {
        if (find_path(arg2))
            delete_path(arg2);
    }
    return remplace;
}

char* traitement(char *path, char *dest)
{
    char *arg1 = absolute_path(path);
    printf("arg1 : %s\n", arg1);
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
        printf("da\n");
        delete_path(arg1);
        add_path2(remplace);
    }
    return remplace;
}



void lister(char *file, char *dest)
{
    char *remplace = traitement(file, dest);
    printf("remplace : %s\n", remplace);
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
    int backup = 0;
    int force = 0;
    int interactive = 0;
    int update = 0;
    int verbose = 0;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--backup") == 0)
            backup = 1;
        else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--force") == 0)
            force = 1;
        else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--interactive") == 0)
            interactive ++;
        else if (strcmp(argv[i], "-u") == 0 || strcmp(argv[i], "--update") == 0)
            update = 1;
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0)
            verbose = 1;
    
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
       if(!S_ISDIR(sb.st_mode) && !S_ISDIR(sb2.st_mode))
        {
            init_file_paths();
            traitement2(absolute_path(argv[argc-2]), absolute_path(argv[argc-1]));
             if (backup)
            {
                printf("ouais\n");
                char tmp[500] = "";
                strcat(tmp, absolute_path(argv[argc-1]));
                printf("tmp : %s\n", tmp);
                strcat(tmp, "~");
                add_path2(tmp);
            }
            execvp("mv", argv);
            return 0;
        }    
    }
    printf("ok\n");
    if (argc > 3 && nbfile > 2)
    {
        struct stat sb;
        if (stat(argv[argc-1], &sb) == -1)
            erreur("ERREUR stat\n");
        if(!S_ISDIR(sb.st_mode))
            erreur("ERREUR dernier argument n'est pas un répertoire\n");
    }    

    if (interactive > 0)
    {
        char buff[100];
        printf("écraser tout fichier existant ?");
        scanf("%s", buff);
        if (buff[0] != 'Y' || buff[0] != 'y')
            exit(EXIT_SUCCESS);

        char **bis;
        bis = malloc(argc - interactive);
        if (bis == NULL)
            erreur("ERREUR malloc\n");
        int j = 0;
        for (int i = 0; i < argc; i++)
        {
            if (strcmp(argv[i], "-i") != 0 && strcmp(argv[i], "--interactive") != 0)
            {
                bis[j] = malloc(strlen(argv[i]));
                if (bis[j] == NULL)
                    erreur("ERREUR malloc\n");
                bis[j][0] = '\0';
                strcat(bis[j], argv[i]);
                j++;
            }
        }
        init_file_paths();
        for (int i = debfile-interactive; i < argc-interactive; i++)
        {
            lister(bis[i], absolute_path(argv[argc -1]));
        }
        if (backup)
        {
            printf("ouais\n");
            char tmp[500] = "";
            strcat(tmp, absolute_path(argv[argc-1]));
            printf("tmp : %s\n", tmp);
            strcat(tmp, "~");
            add_path2(tmp);
        }
        execvp("mv", bis);
        return 0;
    }
    printf("okkk\n");
    init_file_paths();
    for (int i = debfile; i < argc-1; i++)
    {
        printf("%s\n", argv[debfile]);
        lister(argv[i], absolute_path(argv[argc-1]));
    }
    if (backup)
    {
        char tmp[500] = "";
        strcat(tmp, absolute_path(argv[argc-1]));
        strcat(tmp, "~");
        add_path2(tmp);
    }

    
    execvp("mv", argv);
    return 0;
}
