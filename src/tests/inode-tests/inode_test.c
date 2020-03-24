/* For read */
//#include <sys/types.h>
//#include <sys/uio.h>
//#include <unistd.h>
/* For system */
#include <stdlib.h> //exit
/* For snprintf */
#include <stdio.h> // perror
//#include <ctype.h>
//#include <string.h> //perror

#include <sys/stat.h> //stat


int main() {
  struct stat buf;
  int rc = stat("S6.pdf", &buf);
  if (rc == -1) {
    perror("stat");
    exit(1);
  }
  printf("%llu\n", buf.st_ino);

}
