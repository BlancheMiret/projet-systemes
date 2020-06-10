#include "update_paths.c"

int main(int argc, char const *argv[])
{
    init_file_paths();

    FILE *delete = fopen(file_paths, "r");
    if (delete == NULL)
        erreur("ERREUR fopen\n");
    
    char *line_buff = NULL;
    size_t line_buf_size = 0;
	ssize_t line_size;
    line_size = getline(&line_buff, &line_buf_size, delete);

    while (line_size >= 0)
    {
        line_buff[strcspn(line_buff, "\n")] = '\0';
        printf("line : %s\n", line_buff);
        char *tmp = realpath(line_buff, NULL);
        if (tmp == NULL)
            delete_path2(line_buff);
        line_size = getline(&line_buff, &line_size, delete);
        free(tmp);
    }
    fclose(delete);
    free(line_buff);
    return 0;
}