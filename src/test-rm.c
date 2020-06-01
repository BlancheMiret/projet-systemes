#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <unistd.h>
#include<sys/wait.h> 
#include "paths_manager.c"

int main(int argc, char *argv[])
{
  
    init_file_paths();

    pid_t pid;

    /* fork a child process */ 
    pid = fork();
    if (pid < 0)
    { 
        /* error occurred */ 
        fprintf(stderr, "Fork Failed"); 
        return 1; 
    } 
    else if (pid == 0) 
    { 
   
        /* child process */ 
      if(strcmp(argv[1],"-i") == 0){
        argv[0] = "rm";

        execvp("rm", argv);

      }
     
    } 

    else 
    {   
      
         /* parent process */ 
        /* parent will wait for the child to complete */ 
    
    char *arg = NULL;
    for(int i=2; i< argc-2 ; i++){

    arg = realpath(argv[i],NULL);
    if (arg == NULL) exit(EXIT_FAILURE);
  
    if (find_path(arg) == 1) {delete_path(arg);  printf("lol");}

    }
   
    
    wait(NULL); 

    } 

    return 0;
}