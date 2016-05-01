#include "dict.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

unsigned int hash_func(const char* key);

// Create a new empty dictionary
my_dict_t* dict_create() {
  
  my_dict_t* dictionary = (my_dict_t*)malloc(sizeof(my_dict_t));
  if(dictionary == NULL){
    printf("Malloc failed");
    exit(2);
  }
  pthread_mutex_init(&dictionary->destroyLock, NULL);
  int i;
  for (i = 0; i<100; i++){
    dictionary->arr[i].head= NULL;//setting each linked list to NULL
    pthread_rwlock_init(&dictionary->arr[i].lock,NULL);
  }
  return dictionary;
}

//Destroy a dictionary
void dict_destroy(my_dict_t* dict) {
  if( dict == NULL ) {
    return;
  }
  pthread_mutex_lock(&dict->destroyLock);
  int i;
  list_node* current;
  list_node* temp;
  for (i = 0; i<100; i++){
    pthread_rwlock_wrlock(&(dict->arr[i].lock));
    current = dict->arr[i].head;
    while(current != NULL){
      temp = current;
      current = current->next;
      free(temp);
    }
  }
  free(dict);
}



// Set a value in a dictionary
void dict_set(my_dict_t* dict, const char* key, const char* value) {
  unsigned int index = hash_func(key);
  // printf("Index: %u\n", index);
  pthread_rwlock_wrlock(&(dict->arr[index].lock));
  list_node* new_node = (list_node*) malloc(sizeof(list_node*));
  //if the first item in the list
  if (dict->arr[index].head == NULL){
    new_node->key = key;
    new_node->val = value;
    new_node->next = NULL;
    dict->arr[index].head = new_node;
  }
  else{
    list_node* current = dict->arr[index].head;
    list_node* prev = NULL;
    while(current != NULL){
      //first check if there is already a node with the same key
      if(strcmp(current->key, key) ==0){
	current->val = value;
        pthread_rwlock_unlock(&(dict->arr[index].lock));
        return;
      }
      prev = current;
      current = current->next;
    }
    new_node->key = key;
    new_node->val = value;
    new_node->next = NULL;
    prev->next = new_node;
  }
  pthread_rwlock_unlock(&(dict->arr[index].lock));
}

  


// Get a value in a dictionary
const char* dict_get(my_dict_t* dict, const char* key) {
  unsigned int index = hash_func(key);
  printf("index=%u\n", index);
  pthread_rwlock_tryrdlock(&dict->arr[index].lock);
  list_node* current = dict->arr[index].head;
  while(current != NULL){
    printf("key = %s\n", key);
    printf("current key = %s\n", current->key);
      printf("current val = %s\n", current->val);
    if(strcmp(current->key, key) ==0)
      {
        pthread_rwlock_unlock(&(dict->arr[index].lock));
        // printf("should be key: %s\n", current->val);
        return current->val;
      }
    else
      current = current->next;
    //  printf("current: %s\n", current);
  }//value not found so return null
  pthread_rwlock_unlock(&(dict->arr[index].lock));
  return NULL;
  
}

// Remove a value from a dictionary
void dict_remove(my_dict_t* dict, const char* key) {
  unsigned int index = hash_func(key);
  pthread_rwlock_wrlock(&(dict->arr[index].lock));
  list_node* current = dict->arr[index].head;
  list_node* previous = current;
  while(current != NULL) {
    if(strcmp(current->key, key) ==0){
      //if the first element in list
      if(current == dict->arr[index].head){
        dict->arr[index].head = current->next;
        free(current);
        pthread_rwlock_unlock(&(dict->arr[index].lock));
        return;
      }else
      previous->next = current->next;
      free(current);
      pthread_rwlock_unlock(&(dict->arr[index].lock));
      return;
    }
    else{
    previous = current;
    current = current->next;
    }
  }
  //printf("Key not found!\n");
  pthread_rwlock_unlock(&(dict->arr[index].lock));
}

//Citation: djb2 hash function
//from http://www.cse.yorku.ca/~oz/hash.html
unsigned int hash_func(const char *str)
{
  unsigned int hash = 5381;
  int c;

  while ((c = *str++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  
  hash = hash%100;
  return hash;
}
//take the sum of all chars and modulo by 100