#include <assert.h>
#include <stddef.h>
#include "list.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct list {
  void **lst;
  size_t size;
  size_t max_size;
  free_func_t freer;
} list_t;


list_t *list_init(size_t initial_size, free_func_t freer){
  list_t *list = malloc(sizeof(list_t));
  assert(list != NULL);
  list->max_size = initial_size;
  list->size = 0;
  list->lst = malloc(initial_size * sizeof(void *));
  list->freer = freer;
  assert(list->lst != NULL);
  return list;
}

void list_free(list_t *list) {
  for (int k = 0; k < (int)(list->size); k++) {
      list->freer(list->lst[k]);
  }
  free(list->lst);
  free(list);
}

size_t list_size(list_t *list) {
  return list->size;
}

void *list_get(list_t *list, size_t index) {
  assert(index < list->size && index >= 0);
  return (list->lst)[index];
}

void list_add(list_t *list, void *value) {
  assert(value != NULL);
  if (list->size == list->max_size) {
    list->max_size = 2 * (list->max_size) + 1;
    list->lst = realloc(list->lst, list->max_size * sizeof(void *));
  }
  (list->lst)[list->size] = value;
  list->size++;
}

void *list_remove(list_t *list, size_t index) {
  assert(list->size > 0 && index < list->size && index >= 0);
  void *toReturn = list->lst[index];
  for (int k = (int)index; k < (int)(list->size) - 1; k++) {
    list->lst[k] = list->lst[k+1];
  }
  list->lst[list->size - 1] = NULL;
  list->size--;
  return toReturn;
}
