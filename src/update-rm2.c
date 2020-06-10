#include "update_paths.c"

int main(int argc, char const *argv[])
{
    init_file_paths();

    char *filedelete = buildfiledelete();  
    FILE *delete = fopen(filedelete, "r");
    if (delete == NULL)
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
    free(filedelete);
    free(line_buff);
    return 0;
}