#include <dirent.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/xattr.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

void erreur(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char const *argv[])
{
    char filedest[150] = "";
    strcat(filedest, getenv("HOME"));
    strcat(filedest, "/.tag/remplace.txt");
    FILE *dest = fopen(filedest, "r");

     char *line_buff = NULL;
    size_t line_buf_size = 0;
	ssize_t line_size;

    line_size = getline(&line_buff, &line_buf_size, dest);
    line_buff[strcspn(line_buff, "\n")] = '\0';
    
    printf("realpath : %s\n", realpath(line_buff, NULL));

    printf("dir : %s\n", getcwd(NULL, 100));
    free(NULL);
    /*
    char *pwd = getcwd(NULL, 200);
    struct stat sb;
    if (stat(line_buff, &sb) == -1)
        erreur("ERREUR stat\n");
    printf("ouais\n");
     if (S_ISDIR(sb.st_mode))
    {
        printf("c'est repertoire\n");

        DIR *dir = opendir(line_buff);
        struct dirent *entry;
        chdir(line_buff);
        printf("%s\n", getcwd(NULL, 100));
        while ((entry=readdir(dir)))
        {
            char buff_tag[1024];
            printf("    entry->dname : %s\n", entry->d_name);
            ssize_t istag = getxattr(realpath(entry->d_name, NULL), "user.tags", buff_tag, sizeof(buff_tag));
            printf("istag : %d\n", istag);
            
            if (istag >= 0)
            {
                printf("fichier taggé\n");
            }
            else
            {
                printf("fichier pas taggé\n");
            }   
        }    
    }
    chdir(pwd);
    printf("%s\n", getcwd(NULL, 100));*/
    return 0;
}
