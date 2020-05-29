#include "paths_manager.c"

void erreur(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}


int add_path2(char * filename){


	/*if(check_file_existence(filename) == 0){

		printf("le fichier n'existe pas !\n");
		return 0;
	}*/
//On récupère le chemin d'accès absolu du fichier

	FILE  *file;

	file = fopen(file_paths, "a");

	if (file == NULL)
	{
		fprintf(stderr, "cannot open %s for appending\n", file_paths);
		return 0;
	}


	if(find_path(filename) == 1){

		return 0;
	}

	else {
    	fprintf(file, "%s\n", filename);
	}

	fclose(file);

	return 1;


}

int main(int argc, char *argv[])
{


    if (argc != 3)
        erreur("ERREUR nombre d'arguments invalide\n");
    
    init_file_paths();
    char *arg1 = absolute_path(argv[1]);
    
    char arg2[500] = "";
    strcat(arg2, absolute_path(argv[2]));
    if (arg1 == NULL || arg2 == NULL)
        erreur("ERRUER arguments invalides\n");
    

    int debut = -1;
    for (int i = 0; i < strlen(arg1); i++)
    {
        if (arg1[i] == '/')
            debut = i;
    }
    char namefile[100] = "";
    strcat(namefile, arg1+debut+1);
    char remplace[600] = "";
    strcat(remplace, arg2);
    if (arg2[strlen(arg2)] != '/')
        strcat(remplace, "/");
    strcat(remplace, namefile);
    


    if (find_path(arg1) == 1)
    {
        delete_path(arg1);
        add_path2(remplace);
    }

    execlp("mv", "mv", (const char *)arg1, (const char *)arg2, NULL);
    return 0;
}
