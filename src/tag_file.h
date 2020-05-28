#ifndef __TAG_FILE_H__
#define __TAG_FILE_H__


int link_tag(char *filename, char * tags[], size_t tags_size);
int unlink_tag(char * filename, char * tags[], size_t tags_size);
void * get_file_tag_list(char * path);
int reset_all_files(char * file_paths);

#endif