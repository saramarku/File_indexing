#ifndef DICT_H
#define DICT_H
#include <pthread.h>
// This makes the header file work for both C and C++
#ifdef __cplusplus
extern "C" {
#endif

  

  typedef struct l_node {
    const char* key;
    const char* val;
    struct l_node* next;
  }list_node;

  typedef struct node{
    list_node *head;
    pthread_rwlock_t lock;
  }arr_node;
  
  typedef struct my_dict{
    pthread_mutex_t destroyLock;
    arr_node arr[100];
  } my_dict_t;

  //
  // list_node* arr[100];

  //

  // Create a new empty dictionary
  my_dict_t* dict_create();

  // Destroy a dictionary
  void dict_destroy(my_dict_t* dict);

  // Set a value in a dictionary
  void dict_set(my_dict_t* dict, const char* key, const char* value);

  // Get a value in a dictionary
  const char* dict_get(my_dict_t* dict, const char* key);

  // Remove a value from a dictionary
  void dict_remove(my_dict_t* dict, const char* key);

  // This makes the header file work for both C and C++
#ifdef __cplusplus
}
#endif

#endif
