#include "tag_manager_test.h"

int main() {

	clean_hierarchy();
	
	printf("----- ADDING COULEUR -----\n");
	add_tag(NULL, "Couleur");
	printf("------ ADDING ROUGE ------\n");
	add_tag("Couleur", "Rouge");
	printf("------ ADDING JAUNE ------\n");
	add_tag("Couleur", "Jaune"); 
	printf("------ ADDING GENRE ------\n");
	add_tag(NULL, "Genre");
	printf("----- ADDING WESTERN -----\n");
	add_tag("Genre", "Western");
	printf("------ ADDING DRAME ------\n");
	add_tag("Genre", "Drame");
	printf("------ ADDING J FLUO -----\n");
	add_tag("Jaune", "Jaune fluo");
	printf("------- ADDING LILA ------\n");
	add_tag("Couleur", "Lila");

	printf("\n------- PRINT TREE -------\n");
	print_hierarchy();

	printf("\n------ DELETE JAUNE ------\n");
	delete_tag("Jaune");

	printf("\n------- PRINT TREE -------\n");
	print_hierarchy();

}