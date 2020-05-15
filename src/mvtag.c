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

int main(int argc, char const *argv[])
{
    if (execlp("alias", "alias", "'mvt=upd'", NULL) < 0)
    {
        printf("ERREUR allias\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}
