#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tag_hierarchy.h"
#include "research.h"
#include "tag_file.h"

#define DEBUG 1

/* Utilisation ? 

NOUVEAU MANUEL

%tag print [filename]                    		-> affiche liste tags de filename si précisé, sinon la hiérarchie des tags

%tag create -n|<father> <tag1> [tag2] ...   	-> crée tag1 tag2... avec pour père father si précisé, root si -n
%tag delete <tag>                         		-> supprime tag + enfants de la hiérarchie - ATTENTION SUPPRIMER CES TAGS DES FICHIERS + éventuellement fichiers de la liste des fichiers

%tag link <filename> <tag1> [tag2] ...        	-> ajoute tag1, tag2 ... à filname - ATTENTION doit ajouter filename à la liste fichiers si pas déjà présent
%tag unlink <filename> [--all]<tag1> [tag2] ... -> supprime tag1, tag2 ... de filename. Tous les tags si --all - ATTENTION doit supprimer fichier de la liste des fichiers si nbtags tombe à 0

%tag search tag1 [-not] tag2 ...        		-> renvoie chemin absolu des fichiers dont les tags correspondent à la combinaison

%tag reset                              		-> efface toute la hiérarchie des tags - ATTENTION DOIT SUPPRIMER LES TAGS DE TOUS LES FICHIERS

*/

void exit_with_man_page() {
	printf("NAME\n");
	printf("	tag - the manager of you tagging-file system.\n");
	printf("\n");
	printf("SYNOPSIS\n");
	printf("	tag <command> [<args>]\n");
	printf("\n");
	printf("DESCRIPTION\n");
	printf("	Tag is a tagging-file system manager...\n");
	printf("\n");
	printf("TAG COMMANDS\n");
	printf("	tag create -n|<father> <tag1> [tag2] ...\n");
	printf("	tag delete <tag>\n");
	printf("	tag print [filename]\n");
	printf("	tag link <filename> <tag1> [tag2] ...\n");
	printf("	tag unlink <filename> [--all] <tag> [tag]\n");
	printf("	tag search tag1 [-not] [tag2] ...\n");
	printf("	tag reset\n");
	printf("\n");
	exit(0);
}

void exit_with_help_page() {
    printf("usage : tag [--version] [--help] <command> [--all] [<args>]");
    printf("These are common Tag commands used in various situations:\n");
    printf("\n");
    printf("manage the tagset\n");
    printf("    create      Add tag to your tagset\n");
    printf("    delete      Supress tag from your tagset, also supressing its children if needed\n")
    printf("    print       Display the tagset\n");
    printf("\n");
    printf("add tags to file\n");
    printf("    link        Add tags to a file\n");
    printf("    unlink      Delete tags from a file\n");
    printf("\n");
    printf("find files\n");
    printf("    search      Display the absolute path of files associated with a tag combination");
    printf("\n");
    exit(0);
}

enum error { CREATE, DELETE, PRINT, LINK, UNLINK, SEARCH, RESET };

void exit_with_syntax_error(enum error e) {
	printf("Arguments are missing\n");
	switch(e) {
		case CREATE : printf("usage : tag create -n|<father> <tag1> [tag2] ...\n"); break;
		case DELETE : printf("usage : tag delete <tag>\n"); break;
		case PRINT : printf("usage : tag print [filename]\n"); break;
		case LINK : printf("usage : tag link <filename> <tag1> [tag2] ...\n"); break;
		case UNLINK : printf("usage : tag unlink <filename> [--all] <tag> [tag]\n"); break;
		case SEARCH : printf("usage : tag search tag1 [-not] [tag2] ...\n"); break;
		case RESET : printf("usage : tag reset\n"); break;
	}
	printf("See --help to get more informations\n");
	exit(0);
}

int main(int argc, char **argv) {

	init_hierarchy();

	if (argc < 2) exit_with_man_page(); // PS : git fait la même chose que git --help. La page de manuel c'est différent

    if (strcmp(argv[1], "--help") == 0) exit_with_help_page();

	char command;

	if      (strcmp(argv[1], "create") == 0) command = 'c';
	else if (strcmp(argv[1], "delete") == 0) command = 'd';
	else if (strcmp(argv[1], "print") == 0) command = 'p';
	else if (strcmp(argv[1], "link") == 0) command = 'l';
	else if (strcmp(argv[1], "unlink") == 0) command = 'u';
	else if (strcmp(argv[1], "search") == 0) command = 's';
	else if (strcmp(argv[1], "reset") == 0) command = 'r';
	else {
		printf("%s is not a tag command. See --help\n", argv[1]);
		exit(1);
	}

	switch(command) {
		case 'c' :
			if (argc < 4) exit_with_syntax_error(CREATE);
			char *father = (strcmp(argv[2], "-n") != 0 ) ? argv[2] : NULL;
			create_tag(father, argv + 3, argc - 3);
			break;

		case 'd' :
			if (argc != 3) exit_with_syntax_error(DELETE); 
			char *tag_to_delete = argv[2];
			struct tag_node *children = get_tag_children(tag_to_delete);
			// for_all_files_delete(tag_to_delete, children); // <----------------------------------
			delete_tag(tag_to_delete);
			break;

		case 'p' :

			if (argc == 2) print_hierarchy();
			else if (argc == 3) printf("Yop\n"); // afficher contenu d'un fichier // <-------------------------------------
			else exit_with_syntax_error(PRINT); 
			break;

		case 'l' :
			if (argc < 4) exit_with_syntax_error(LINK); 
			for (int i = 3; i < argc; i++) {
				if (!tag_exists(argv[i])) {
					printf("%s does not exist in your tag hierarchy yet. Create it first with tag create\n", argv[i]);
					exit(1);
				}
			}
			link_tag(argv[2], argv + 3, argc - 3);
			break;

		case 'u' :
			if (argc < 4) exit_with_syntax_error(UNLINK); 
			char *filename = argv[2];
			if (strcmp(argv[3], "--all") == 0) { 
				if (argc == 4) printf("Must add call of function reset_all_files()\n"); // <----------reset_all_files()
				else exit_with_syntax_error(UNLINK);
			}
			else unlink_tag(filename, argv + 3, argc - 3);
			break;

		case 's' :
			if (argc < 3) exit_with_syntax_error(SEARCH);
			research(argc, argv);
			break;

		case 'r' :
			if (argc != 2) exit_with_syntax_error(RESET);
			clean_hierarchy();
			delete_all_tags("paths.txt");
			break;

		default :
			fprintf(stderr, "Wrong parsing of arguments.\n");
			exit(-1);
	}
}
