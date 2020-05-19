#ifndef __PATHS_MANAGER_H__
#define __PATHS_MANAGER_H__

int check_file_existence(char * filename);
char * absolute_path(char * filename);
int find_path(char * file_paths, char * file_to_tag );
int add_path(char * filename);
int delete_path(char * filename);
void * init_iterator();
void * next_path(FILE * file);

#endif