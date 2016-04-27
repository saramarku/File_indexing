#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>



int main (int argc, char** argv) {

  char* dir_line = NULL;    // Pointer that will hold the line we read in
  size_t dir_length;   // Space for the length of the line
  char* query_line  = NULL;  //the line for the word the user searches for
  size_t query_length;


  // Print the shell prompt
  printf("> Please indicate the path of directory you want to search\n");
    
  // Get a line of stdin, storing the string pointer in dir_line and length in dir_length
  if(getline(&dir_line, &dir_length, stdin) == -1){
    if(errno == EINVAL) {
      perror("Unable to read command line");
      exit(2);
    } else {
      // Must have been end of file (ctrl+D)
      printf("\nShutting down...\n");
      exit(0);
    }
  }

    
  printf("Received command: %s\n",dir_line);
 printf("> Please indicate the word you want to search for\n");

// Get a line of stdin, storing the string pointer in query_line and length in query_length
 
  if(getline(&query_line, &query_length, stdin) == -1){
    if(errno == EINVAL) {
      perror("Unable to read command line");
      exit(2);
    } else {
      // Must have been end of file (ctrl+D)
      printf("\nShutting down...\n");
      exit(0);
    }

  }
    printf("Received command: %s\n",query_line);
    
    //Get into the path directory 
    chdir(dir_line);


    

    
  free(dir_line);
  free(query_line);
  return 0;
}
