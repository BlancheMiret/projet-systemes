
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>



void erreur(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
int main(int argc, char const *argv[])
{

    execlp("sudo","sudo", "apt-get", "install", "xattr", NULL);	
    const char *HOME = getenv("HOME");
    if (HOME == NULL)
        erreur("ERREUR getenv\n");

    char dir [100] = "";
    char pathfile1 [100] = "";
    char pathfile2 [100] = "";
    char path [100] = "";
    strcat(dir, getenv("HOME"));
    strcat(pathfile1,HOME);
    strcat(pathfile2, getenv("HOME"));
    strcat(path, HOME);
    
    strcat(path, "/.bashrc");
    strcat(dir, "/.tag");
    strcat(pathfile1, "/.tag/tag_hierarchy");
    strcat(pathfile2, "/.tag/tag_list");	
	
	int fd3 = open(path, O_APPEND | O_WRONLY, 0600);
    if (fd3 < 0)
        erreur("ERREUR fd\n");
    write(fd3, "export PATH=$PATH:~/.tag\n", 24);
    close(fd3);
    if (mkdir(dir, 0777) < 0)                            
        erreur("ERREUR repertoire existe déjà\n");

    int fd = open(pathfile1, O_CREAT, 0600);       
    int fd2 = open(pathfile2, O_CREAT, 0600);
    if (fd < 0 || fd2 < 0)
        erreur("ERREUR open\n");  
    execlp("mv", "mv", "exe", dir, NULL);

    close(fd2);close(fd);
    

    return 0;
}
