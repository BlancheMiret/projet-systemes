#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tag_hierarchy.h"
#include "research.h"
#include "tag_file.h"

/* Utilisation ? 

------ Dans la liste / hiérarchie
- a(dd) : Ajouter un tag dans la liste / hiérarchie de tags avec un père optionnel
$ tag -a couleur // <-- associe à root par défaut
$ tag -a bleu couleur

- p(rint) : Afficher la hiérarchie
$ tag -p

- d(eleteà : Supprimer un tag de la liste / hiérarchie
$ tag -d couleur

------- Tag / fichiers
- l(ink) : Lier un tag à un fichier // <-- vérifie que tag existe dans la hiérarchie avant de link
$ tag -l <nomfichier> <nomtag>

- u(nlink) : Suppression tag lié à un fichier
$ tag -u <nomfichier> <nomtag>

------- Recherche
- s(earch) : RECHERCHE DE FICHIERS LIÉS À UN TAG !!!
1. Parser la combinaison de tags donnée en ligne de commande
2. Chercher dans la hiérarchie les enfants qui sont liées
3. À l'aide

+ IMPLÉMENTER SUIVI DE MOUVEMENT ET SUPPRESSION DES FICHIERS...!!!

*/

void print_error_message() {
	printf("Wrong use of option or arguments. Enter 'tag' to see manuel page.\n");
}

void print_command_use() {
	printf("Welcome to TAGGER, manager of your tagging file system.\n");
	printf("usage: tag -option [<args>]\n");
	printf("Options are:\n");
	printf("    -a tag_name [father_name] : create a tag named tag_name, having father_name as father in the hierarchy.\n"
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
		case 'a':
			if (argc != 3 && argc != 4) {
				print_error_message();
				return -1;
			}
			if (argc == 3) add_tag(NULL, argv[2]); // <-- ajoute le tag avec root comme père, en vérifiant qu'un tag du même nom n'existe pas déjà 
			else if (argc == 4) add_tag(argv[3], argv[2]); // <--- ajoute le tag avec le père spécifié, en vérifiant l'existence du père ou qu'un tag du même nom n'existe pas déjà
			break;

		case 'p':
			if (argc != 2) {
				print_error_message();
				return -1;
			}
			print_hierarchy(); // <-- affiche la hiérarchie des tags créés 
			break;

		case 'd':
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
			// <---- vérifier que le fichier donné en paramètre existe
			if (!tag_exists(argv[3])) { // <-- ici vérifie que le tag existe dans la hiérarchie
				printf("The tag you want to link does not exist in your system yet. Please add it first with `tag -a`\n");
				return -1;
			}
			printf("Linking\n");

            //tag -l fichier couleur jaune rouge

			link_tag(argv[2], argv+3, argc - 3);
			// Attention besoin de connaître le nom du père du tag : fonction à fournir par tag_manager !!
			// <----- ajouter le tag aux attributs de l'inode du fichier
			// + si le fichier n'avait pas encore de tag, l'ajouter à la liste des fichiers taggés
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
			// <---- parser la commbinaison de tags
			// + obtenir la liste de tous les tags (éventuellement enfants, donc recherche dans la hiérarchie)
			// + analyser chaque fichier présent dans la liste des fichiers taggés pour voir s'il contient les tags recherchés
			research(argc, argv);
			break;

		case 'r':
			if (argc != 2) {
				print_error_message();
				return -1;
			}
			// <---- attention, nécessaire de supprimer tous les tags de tous les fichiers !!
			// + effacer (qu'il existe, mais qu'il soit vide) le fichier contenant la liste des fichiers taggés
			clean_hierarchy(); // <-- ceci ne fait que effacer le fichier contenant la hiérarchie
			break;

		default:
			printf("Error occured\n");
			exit(1);
			break;
	}
	return 0;
}






