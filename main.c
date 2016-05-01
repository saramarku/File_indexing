#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include "dict.c"
#include "dict.h"
#include <pthread.h>

#define MAX_WORDS 2000
char** read_words(char * filename, my_dict_t* my_dict);

int main (int argc, char** argv) {

  char* dir_line = NULL;    // Pointer that will hold the line we read in
  size_t dir_length;   // Space for the length of the line
  char* query_line  = NULL;  //the line for the word the user searches for
  size_t query_length;
  char** words;
  DIR *dir;
  struct dirent *ent;


  my_dict_t* my_dictionary = dict_create();

  
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


  // size_t length = strlen(dir_line) -1;
  
  dir_line = strtok(dir_line, "\n");

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
  char buf[MAX_WORDS + 1];
  //"/home/markusar/Desktop/213-project-test"
  if ((dir = opendir (dir_line)) != NULL) {
    /* print all the files and directories within directory */
    while ((ent = readdir (dir)) != NULL) {
      //printf ("%s\n", ent->d_name);
      words = read_words(realpath(ent->d_name,buf), my_dictionary);
      //printf ("%s\n", buf);
    }
  } else {
    /* could not open directory */
    perror ("");
  }

  // const char* wrd = dict_get(my_dictionary, "Hello2");//check if the word not found
  printf("%s\n",dict_get(my_dictionary, "Hello2"));
  
  closedir (dir);
  free(dir_line);
  free(query_line);
  free(words);
  return 0;
    
}
 
char** read_words(char * filename,my_dict_t* my_dict){
  FILE *file = fopen(filename, "r"); // checking for NULL is boring; i omit it
  int i=0;
  char temp[100]; // assuming the words cannot be too long
  char**words = (char**) malloc(sizeof(char*)*MAX_WORDS);
char buf[MAX_WORDS + 1];
  while (!feof(file))
    {
      // Read a word from the file
      if (fscanf(file, "%s", temp) != 1)
        break;
      // note: "!=1" checks for end-of-file; using feof for that is usually a bug
      // Allocate memory for the word, because temp is too temporary
      words[i] = strdup(temp);
      dict_set(my_dict, words[i],realpath(filename, buf));
      printf("%s\n", filename);
      printf("%s\n", words[i]);
    }
   
  fclose(file);
  return words;

}
