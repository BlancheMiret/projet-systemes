#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/xattr.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>




//Code pour atribuer un tag et un sous-tag à un fichier, 


int main(int argc, char const *argv[]) {

    char buff[1024];

    int fd = open("test.txt", O_RDWR);

    if(fsetxattr(fd,"user.couleur","Bleu",sizeof("couleur"),XATTR_CREATE) > -1){
        printf("tag set\n");
    } 
    else {
        perror("error set");
    }
    if(fsetxattr(fd,"user.annee","2019",sizeof("couleur"),XATTR_CREATE) > -1){
        printf("tag set\n");
    } 
    else {
        perror("error set");
    }

    return 0;
}