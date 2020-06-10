#include "update_paths.c"

int find_path2(char * path){

	char *line_buf = NULL;
	size_t line_buf_size = 0;
	ssize_t line_size;
	FILE *file = fopen(file_paths, "r");

	if(file == NULL) {
        perror("fopen error: ");
        exit(EXIT_FAILURE);
    }

	line_size = getline(&line_buf, &line_buf_size, file);
		
	while (line_size>= 0) {

		line_buf[strcspn(line_buf, "\r\n")] = 0;

		if (strcmp(path, line_buf) == 0) {
            
			return 1;
		}

		line_size = getline(&line_buf, &line_buf_size, file);
	}

	return 0;
}

/*
int delete_path2(char * filename){
    
    printf("deletepath\n");
	char *path = filename;
	char *line_buf = NULL;
	size_t line_buf_size = 0;
	ssize_t line_size;
	char *temp_file = "temp.txt";
	FILE *file = fopen(file_paths, "r");

	if(file == NULL) {
        perror("fopen error: ");
        exit(EXIT_FAILURE);
    }
    printf("file path ok\n");
	FILE *file2 = fopen(temp_file, "a+");

	if(file2 == NULL) {
        perror("fopen error: ");
        exit(EXIT_FAILURE);
    }

	line_size = getline(&line_buf, &line_buf_size, file);

	while (line_size>= 0) {

		if (strncmp(path, line_buf,strlen(line_buf)-1) != 0)
			fprintf(file2, "%s", line_buf);
		line_size = getline(&line_buf, &line_buf_size, file);
	}
    free(line_buf);
	fclose(file);
	fclose(file2);
	remove(file_paths);
	rename(temp_file, file_paths);
    printf("fin deletepath\n");
	return 1;
  
}
*/

int main(int argc, char const *argv[])
{
    init_file_paths();
    printf("debut\n");
    
    char *filedelete = buildfiledelete();
    char *fileremp = buildfileremplace();
    char *filedest = buildfiledest();
    
    FILE *delete = fopen(filedelete, "r");
    FILE *remplace = fopen(fileremp, "r");
    FILE *dest = fopen(filedest, "r");
    if (delete == NULL || remplace == NULL || dest == NULL)
        erreur("ERREUR open\n");
    char *line_buff = NULL;
    size_t line_buf_size = 0;
	ssize_t line_size;
    line_size = getline(&line_buff, &line_buf_size, delete);

    while (line_size >= 0)
    {
        //line_buff[strcmp(line_buff, "\r\n")] = '\0';
        char *tmp = realpath(line_buff, NULL);
        if (tmp == NULL)
            delete_path2(line_buff);
        line_size = getline(&line_buff, &line_size, delete);
        free(tmp);
    }
    fclose(delete);
    printf("----------------------DEST-----------------------------\n");
    line_size = getline(&line_buff, &line_buf_size, dest);
    struct stat sb;
    char *pwd = getcwd(NULL, 200);

    line_buff[strcspn(line_buff, "\n")] = '\0';

    char *pathdest = getdestination();
    if (pathdest == NULL)
        exit(EXIT_SUCCESS);
    
    add_delete_in_dir(pathdest);
    free(pathdest);
    free(pwd);

    
    printf("________________________88REMPLACE______________________\n");
    line_size = getline(&line_buff, &line_buf_size, remplace);
    while (line_size >= 0)
    {
        line_buff[strcspn(line_buff, "\n")] = '\0';
        char *remp =realpath(line_buff, NULL);
        if (remp != NULL && find_path(remp) == 0)
            add_path(line_buff);
        line_size = getline(&line_buff, &line_size, remplace);
        free(remp);
    }

    fclose(remplace);
    fclose(dest);
    printf("remove\n");
    free(line_buff);
    free(filedelete);
    free(filedest);
    free(fileremp);
    return 0;
}

