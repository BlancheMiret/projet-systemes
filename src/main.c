#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tag_hierarchy.h"
#include "research.h"
#include "tag_file.h"

/* Utilisation ? 

NOUVEAU MANUEL

%tag print                              -> affiche hiérarchie des tags
%tag print filename                     -> affiche liste tags d'un fichier

%tag create -n/nompère tag1 tag2 ...    -> crée tag1 tag2... avec pour père nompère si précisé, root si -n
%tag delete tag                         -> supprime tag + enfants de la hiérarchie - ATTENTION SUPPRIMER CES TAGS DES FICHIERS + éventuellement fichiers de la liste des fichiers
%tag link filename tag1 tag2 ...        -> ajoute tag1, tag2 ... à filname - ATTENTION doit ajouter filename à la liste fichiers si pas déjà présent
%tag unlink filename tag1 tag2 ...      -> supprime tag1, tag2 ... à filename - ATTENTION doit supprimer fichier de la liste des fichiers si nbtags tombe à 0

%tag search tag1 [-not] tag2 ...        -> renvoie chemin absolu des fichiers dont les tags correspondent à la combinaison

%tag reset                              -> efface toute la hiérarchie des tags - ATTENTION DOIT SUPPRIMER LES TAGS DE TOUS LES FICHIERS
%tag reset filename                     -> efface tous les tags d'un fichier

*/

void print_error_message() {
	printf("Wrong use of option or arguments. Enter 'tag' to see manuel page.\n");
}

void print_manual_page() {
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
	printf("	git create [-n]/[father] <tag1> [tag2] ...\n");
	printf("	git delete <tag>\n");
	printf("	git print [filename]\n");
	printf("	git link <filename> <tag1> [tag2] ...\n");
	printf("	git unlink <filename> <tag> \n");
	printf("	git search tag1 [[-not] tag2] ...\n");
	printf("	git reset [filename]\n");
	printf("\n");
}

void print_command_use() {
    printf("Welcome to TAGGER, manager of your tagging file system.\n");
    printf("usage: tag [-apdlusr] [<args>]\n");
    printf("Options are:\n");
    printf("    -a father_name tag_name :   create a tag named tag_name, having father_name as father in the hierarchy.\n"
           "                                father_name should already exist in the hierarchy.\n"
           "                                If father_name is not defined, the tag will be added at the root of the tag hierarchy.\n");
    printf("    -p :                        display the existing tag hierarchy.\n");
    printf("    -d tag_name :               ask to delete the tag tag_name. If the tag has children, they will be deleted as well.\n");
    printf("    -l file_path tag_name :     link tag_name to the file designated by file_path.\n"
           "                                tag_name should already exist in the hierarchy.\n");
    printf("    -u file_path tag_name :     unlink tag_name from the designated file.\n");
    printf("    -s <tag_combinaison> :      return path of file corresponding to the research.\n"
           "                                See below for tag combinaison syntax.\n");
    printf("    -r :                        reset tag-system definitively and delete all tags associated with files.\n");
}

int main (int argc, char **argv) {

	if (argc == 1) {
		print_command_use();
		return 0;
	}

	if (strlen(argv[1]) != 2 ||
		argv[1][0] != '-' ||
		(argv[1][1] != 'a' &&
		argv[1][1] != 'p' &&
		argv[1][1] != 'd' &&
		argv[1][1] != 'l' &&
		argv[1][1] != 'u' &&
		argv[1][1] != 's' &&
		argv[1][1] != 'r')) {
		print_error_message();
		return 0;
	}

	switch(argv[1][1]) {
		case 'a': // AJOUTER MULTIPLICITÉ ARGUMENTS
			if (argc != 3 && argc != 4) {
				print_error_message();
				return -1;
			}
			if (argc == 3) add_tag(NULL, argv[2]); // <-- ajoute le tag avec root comme père, en vérifiant qu'un tag du même nom n'existe pas déjà 
			else if (argc == 4) add_tag(argv[3], argv[2]); // <--- ajoute le tag avec le père spécifié, en vérifiant l'existence du père ou qu'un tag du même nom n'existe pas déjà
			break;

		case 'p': // OK
			if (argc != 2) {
				print_error_message();
				return -1;
			}
			print_hierarchy(); // <-- affiche la hiérarchie des tags créés 
			break;

		case 'd': // OK 
			if (argc != 3) {
				print_error_message();
				return -1;
			}
			delete_tag(argv[2]); // <-- supprime le tag et ses enfants de la hiérarchie des tags, en demandant confirmation à l'utilisateur
			break;

		case 'l':
			if (argc != 4) {
				print_error_message();
				return -1;
			}

			if (!tag_exists(argv[3])) { // <-- ici vérifie que le tag existe dans la hiérarchie
				printf("The tag you want to link does not exist in your system yet. Please add it first with `tag -a`\n");
				return -1;
			}
			printf("Linking\n");
			link_tag(argv[2], argv+3, argc - 3);

			break;

		case 'u':
			if (argc != 4) {
				print_error_message();
				return -1;
			}
			printf("Unlinking\n");
			// <---- vérifier que fichier donné en paramètre existe dans la liste des fichiers taggés
			// + délier le tag 
			// + si nombre de tag du fichier = 0 alors supprimer le fichier de la liste des fichiers taggés
			unlink_tag(argv[2], argv+3, argc - 3);
			break;

		case 's':
			printf("Searching\n");
			if (argc < 3) {
				print_error_message();
				return -1;
			}
			research(argc, argv);
			break;

		case 'r':
			if (argc != 2) {
				print_error_message();
				return -1;
			}
			break;

		default:
			printf("Error occured\n");
			exit(1);
			break;
	}
	return 0;
}






