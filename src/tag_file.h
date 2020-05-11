#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/xattr.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>



int find_path(char * file_paths, char * file_to_tag );
int add_path(char * filename);
int link_tag(char *filename, char * tags[], size_t tags_size);
int unlink_tag(char * filename, char * tagname);