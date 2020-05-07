









/**************** TEST ****************/

int main(int argc, char *argv[])
{
     char *buffer;
    //commande: tag filename tag_name sous_tag1 sous_tag2 sous_tag3 ...etc
     //Exemple : ./create_tags test.txt couleur bleu bleu_ciel
     if(argc > 2) link_tag_to_file(argv[1], argv,argc);


      //find_path("paths.txt","test10.txt");

	return 0;
}
 