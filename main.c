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

#include <time.h>
#include <sys/time.h>
#include <signal.h>


size_t time_ms() {
  struct timeval tv;
  if(gettimeofday(&tv, NULL) == -1) {
    perror("gettimeofday");
    exit(2);
  }
  return tv.tv_sec*1000 + tv.tv_usec/1000;
}

int count_files =0;

/*
  Citations:
/home/markusar/Desktop/213-project-test

  For getting the full path: http://stackoverflow.com/questions/19641798/get-the-full-path-of-the-files-in-c
  Reading words from a file: http://stackoverflow.com/questions/15508828/program-to-read-words-from-a-file-and-count-their-occurrence-in-the-file
  Getting input: The shell lab 
  Dictionary: Data-structures lab. 
  Helping read through mulitple directories within a directory: http://stackoverflow.com/questions/19365522/c-programming-how-to-recursively-get-files-in-directories-and-sub-directories
Reading only text files: http://stackoverflow.com/questions/30216643/how-to-list-all-the-txt-files-in-the-current-directory

*/

#define MAX_WORDS 2000
#define MAX_WORD_LENGTH 200

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
 
  time_t start_time = time_ms();
  listDir(dir_line, my_dictionary);
   time_t inter_time = time_ms();
  time_t list_dir_time = inter_time - start_time;
  printf("List_dir_time: %ld \n", list_dir_time);
  
  printf("> Please indicate the word you want to search for\n");

  // Get a line of stdin, storing the string pointer in query_line and length in query_length
 
  if(getline(&query_line, &query_length, stdin) == -1){
    if(errno == EINVAL) {
      perror("Unable to read command line");
      exit(2);
    }
    else {
      // Must have been end of file (ctrl+D)
      printf("\nShutting down...\n");
      exit(0);
    }
  }
  
  time_t inter_start_time = time_ms();
  query_line = strtok(query_line, "\n");
    if (query_line == NULL){
    printf("No word was inputted.\n");
    return;
  }
  printf("Received command: %s\n",query_line);

  ///check if the word not found
  char ** values = (char**) malloc(sizeof(char*)*2000);      //array to store the paths
  values  = dict_get(my_dictionary, query_line);            //getting the paths of the found word
    
  printf("%s is found in the following files: \n", query_line);
  int j=0; //index of path array

  if(values[j] == NULL){
    printf("Word not found in any files\n");
  }

  //getting all the paths and printing them
  while(values[j] != NULL){
    printf("%s\n", values[j]);
    j++;
  }
   time_t end_time = time_ms();
   time_t get_time = end_time - inter_start_time;
   time_t total_time = get_time + list_dir_time;
   printf("Total time: %ld \n", total_time);

  printf("Number of files: %d\n",count_files);
   
  free(values);
  free(dir_line);
  free(query_line);
  return 0;
    
}
 
char** read_words(char * filename, my_dict_t* my_dict){
  
  FILE *file = fopen(filename, "r");
  // checking for NULL
  if(file== NULL)
    return;
  int i=0;
  char temp[MAX_WORD_LENGTH]; // assuming the words cannot be too long
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
      //parse words to not include punctuation 
      words[i] = strtok(words[i], ".!?,():\";");
      //putting all the words and their paths into the dictionary as key-value pairs
      dict_set(my_dict, words[i],strdup(filename));
    }
   
  fclose(file);
  return words;

}

//Helper function for listDir
void listDir_helper(char* path, my_dict_t* my_dict) {
  char slash = '/';
  DIR* dir;
  struct dirent *ent;
  char buf1[MAX_WORDS + 1];//buffer to store the realpath
  char *NulPosition = &path[strlen(path)];

  //Open the directory 
  if ((dir = opendir(path)) != NULL) {
    //while there are files in the directory
    while ((ent = readdir(dir)) != NULL) {
      //inside the directory
      if (ent->d_type == DT_DIR) {
        //if it is not the root or parent directory
        if ((strcmp(ent->d_name, ".") != 0) && (strcmp(ent->d_name, "..") != 0)){
          //attaching the filename into the path
          sprintf(NulPosition, "%c%s", slash, ent->d_name);
          //recurse until we reach the regular file types
          listDir_helper(path, my_dict);
        }
      }
      //if regular file found
      else if(ent->d_type == DT_REG) {
        int length = strlen(ent->d_name);
        //only go into the text files
        if (strncmp(ent->d_name + length - 4, ".txt", 4) == 0){
          count_files++;
          //attaching the filename into the path
        sprintf(NulPosition, "%c%s", slash, ent->d_name);
        //attaching the filename into the full path
        strcat(realpath(path,buf1),NulPosition);
        //go through all the word in the file to put them in the dictionary
        read_words(realpath(path,buf1), my_dict);
        }         
      }   
    }//resetting the path
    *NulPosition = '\0';
  }
  closedir(dir);

  return;
}

//function to go through all the directories and files
void listDir(char* path, my_dict_t* my_dict){
  struct dirent *ent;
  char pathmax[MAX_WORDS+1+sizeof(ent->d_name)+1];
  //if the path length is too long
  if (strlen(path) > MAX_WORDS) return;
  strcpy(pathmax, path);
  listDir_helper(pathmax, my_dict);
}
