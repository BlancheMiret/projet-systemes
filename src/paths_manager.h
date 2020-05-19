#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/xattr.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int check_file_existence(char * filename);
char * absolute_path(char * filename);
int find_path(char * file_paths, char * file_to_tag );
int add_path(char * filename);
int delete_path(char * filename);
void * init_iterator();
void * next_path(FILE * file);

