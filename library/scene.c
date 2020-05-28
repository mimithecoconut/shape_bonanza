#include "scene.h"
#include "body.h"
#include <stdlib.h>
#include <stdio.h>
#include "color.h"
#include <assert.h>
#include "polygon.h"
#include "body.h"
#include "list.h"

const int NUMBER_BODIES = 10;

typedef struct force {
  void *aux;
  force_creator_t forcer;
  free_func_t freer;
  list_t *bodies;
  int forRemoval;
} force_t;

force_t *force_init(void *aux, force_creator_t forcer, free_func_t freer) {
  force_t *toReturn = malloc(sizeof(force_t));
  assert(toReturn != NULL);
  toReturn->aux = aux;
  toReturn->forcer = forcer;
  toReturn->freer = freer;
  toReturn->bodies = NULL;
  toReturn->forRemoval = 0;
  return toReturn;
}

force_t *force_init2(void *aux, force_creator_t forcer, free_func_t freer, \
  list_t *bodies){
  force_t *toReturn = force_init(aux, forcer, freer);
  toReturn->bodies = bodies;
  return toReturn;
}

void force_remove(force_t *f){
  f->forRemoval = 1;
}

int force_is_removed(force_t *f){
  return f->forRemoval;
}

void force_free(force_t *f) {
  if (f->freer != NULL) {
    f->freer(f->aux);
  }
  free(f);
}

typedef struct scene {
  list_t *bodies;
  list_t *forces;
} scene_t;

scene_t *scene_init(void) {
  scene_t *toReturn = malloc(sizeof(scene_t));
  assert(toReturn != NULL);
  toReturn->bodies = list_init(NUMBER_BODIES, (free_func_t) body_free);
  toReturn->forces = list_init(1, (free_func_t) force_free);
  return toReturn;
}

void scene_free(scene_t *scene) {
  list_free(scene->bodies);
  list_free(scene->forces);
  free(scene);
}

size_t scene_bodies(scene_t *scene) {
  return list_size(scene->bodies);
}

body_t *scene_get_body(scene_t *scene, size_t index) {
  return (body_t*) list_get(scene->bodies, index);
}

force_t *scene_get_force(scene_t *scene, size_t index) {
  return (force_t*) list_get(scene->forces, index);
}

void scene_add_body(scene_t *scene, body_t *body) {
  list_add(scene->bodies, body);
}

//deprecated
void scene_remove_body(scene_t *scene, size_t index) {
  body_remove(scene_get_body(scene, index));
}

void scene_remove_force(scene_t *scene, size_t index) {
  list_remove(scene->forces, index);
}

//deprecated
void scene_add_force_creator(scene_t *scene, force_creator_t forcer, void *aux,
                             free_func_t freer) {
  scene_add_bodies_force_creator(scene, forcer, aux, list_init(1, freer), freer);

}

void scene_add_bodies_force_creator(scene_t *scene, force_creator_t forcer, \
  void *aux, list_t *bodies, free_func_t freer){
  list_add(scene->forces, force_init2(aux, forcer, freer, bodies));
}

void scene_tick(scene_t *scene, double dt) {

  for (size_t n = 0; n < list_size(scene->forces); n++) {
    force_t *f = list_get(scene->forces, n);
    f->forcer(f->aux);
  }

  for (size_t k = 0; k < list_size(scene->forces); k++){
    force_t *f = scene_get_force(scene, k);
    for (size_t l = 0; l < list_size(f->bodies); l++){
      if (body_is_removed(list_get(f->bodies, l))){
         force_remove(f);
      }
    }
  }

  for (size_t j = 0; j < list_size(scene->forces); j++) {
    force_t *f = scene_get_force(scene, j);
    if (force_is_removed(f)){
      scene_remove_force(scene, j);
      j--;
    }
  }

  for (size_t m = 0; m <scene_bodies(scene); m ++){
    body_t *bod = scene_get_body((scene_t*) scene, m);
    if (body_is_removed(bod)){
      list_remove(scene->bodies, m);
      m--;
    }
  }

  for (size_t i = 0; i < scene_bodies(scene); i++){
    body_tick(scene_get_body((scene_t*) scene, i), dt);
  }
}
