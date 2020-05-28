#ifndef __TAG_FILE_H__
#define __TAG_FILE_H__


int link_tag(char *filename, char * tags[], size_t tags_size);
int unlink_tag(char * filename, char * tags[], size_t tags_size, int ask);
void * get_file_tag_list(char * path);
int delete_all_tags(char * filename);
int for_all_files_delete(char * tag[]);
int reset_all_files();
#endif