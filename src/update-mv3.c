#include "update_paths.c"

int main(int argc, char const *argv[])
{
    init_file_paths();
    
    char *filedelete = buildfiledelete();
    char *fileremp = buildfileremplace();
    char *filedest = buildfiledest();
    
    FILE *delete = fopen(filedelete, "r");
    FILE *remplace = fopen(fileremp, "r");
    FILE *dest = fopen(filedest, "r");
    if (delete == NULL || remplace == NULL || dest == NULL)
        erreur("ERREUR fopen\n");
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
    free(line_buff);
    free(filedelete);
    free(filedest);
    free(fileremp);
    return 0;
}

