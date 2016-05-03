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




/*
  Citations:

  For getting the full path: http://stackoverflow.com/questions/19641798/get-the-full-path-of-the-files-in-c
  Reading words from a file: http://stackoverflow.com/questions/15508828/program-to-read-words-from-a-file-and-count-their-occurrence-in-the-file
  Getting input: The shell lab 
  Dictionary: Data-structures lab. 

*/
#define MAX_WORDS 2000
char** read_words(char * filename, my_dict_t* my_dict);
void listDir_helper(char* path, my_dict_t* my_dict);
void listDir(char* path, my_dict_t* my_dict);

int main (int argc, char** argv) {

  char* dir_line = NULL;    // Pointer that will hold the line we read in
  size_t dir_length;   // Space for the length of the line
  char* query_line  = NULL;  //the line for the word the user searches for
  size_t query_length;
    char buf[MAX_WORDS + 1];
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
  
  dir_line = strtok(dir_line, "\n");
  //Check for invalid directory path 
    if (chdir(dir_line) != 0 ){
    printf("Not a directory\n");
    return;
  }
  printf("Received command: %s\n",dir_line);
 

  listDir(dir_line, my_dictionary);
  //read_words(realpath(ent->d_name, buf), my_dictionary);
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

   query_line = strtok(query_line, "\n");
  printf("Received command: %s\n",query_line);
    


  // const char* wrd = dict_get(my_dictionary, "Hello2");//check if the word not found
  
  printf("%s is found in the following files: \n %s\n",
         query_line, dict_get(my_dictionary, query_line));

  free(dir_line);
  free(query_line);
  return 0;
    
}
 
char** read_words(char * filename, my_dict_t* my_dict){
  
  FILE *file = fopen(filename, "r"); // checking for NULL
  if(file== NULL)
    return;
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
      dict_set(my_dict, words[i],strdup(filename));
    }
   
  fclose(file);
  return words;

}

void listDir_helper(char* path, my_dict_t* my_dict) {
  char slash = '/';
  DIR* dir;
  struct dirent *ent;
  char buf1[MAX_WORDS + 1];
  char buf2[MAX_WORDS + 1];
    char buf3[MAX_WORDS + 1];
  char *NulPosition = &path[strlen(path)];
  
  if ((dir = opendir(path)) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      
      printf("%s\n", realpath(path,buf1));
      printf("%s\n", ent->d_name);
      if (ent->d_type == DT_DIR) {
        // printf("inside directory call\n");
        if ((strcmp(ent->d_name, ".") != 0) && (strcmp(ent->d_name, "..") != 0)) {
          //printf("inside . or .. call\n");
           sprintf(NulPosition, "%c%s", slash, ent->d_name);
           printf("nulposition = %s \n", NulPosition);
          printf("Reading dir_path %s\n", realpath(ent->d_name,buf2));
          listDir_helper(path, my_dict);
          //listDir_helper((realpath(ent->d_name,buf2)), my_dict);
            // printf("inside recursive call\n");
            //read_words(realpath(ent->d_name,buf2), my_dict); 
            //closedir(dir);
            // return;
          }
        // *NulPosition = '\0';
      }
      else if(ent->d_type == DT_REG) {
      // read_words(realpath(path,buf1), my_dict);
        sprintf(NulPosition, "%c%s", slash, ent->d_name);
        // printf("Reading %s\n", ent->d_name);
         printf("Reading path %s\n",NulPosition);//realpath(ent->d_name,buf3));
         strcat(realpath(path,buf1),NulPosition);
         read_words(realpath(path,buf1), my_dict);
      }   
    }
    *NulPosition = '\0';
  }
  closedir(dir);
return;
}

void listDir(char* path, my_dict_t* my_dict){
  struct dirent *ent;
  char pathmax[MAX_WORDS+1+sizeof(ent->d_name)+1];
  if (strlen(path) > MAX_WORDS) return;
  strcpy(pathmax, path);
  listDir_helper(pathmax, my_dict);
}
