#include "body.h"
#include <stdlib.h>
#include <stdio.h>
#include "color.h"
#include <assert.h>
#include "polygon.h"
#include "vector.h"

body_t *body_init(list_t *shape, double mass, rgb_color_t color) {
  body_t *toReturn = malloc(sizeof(body_t));
  assert(toReturn != NULL);
  toReturn->shape = shape;
  toReturn->mass = mass;
  toReturn->color = color;
  toReturn->centroid = polygon_centroid(shape);
  toReturn->velocity = (vector_t) {0.0, 0.0};
  toReturn->orientation = 0.0;
  toReturn->force = (vector_t) {0, 0};
  toReturn->impulse = (vector_t) {0, 0};
  toReturn->forRemoval = 0;
  toReturn->info = NULL;
  toReturn->info_freer = NULL;
  return toReturn;
}

body_t *body_init_with_info(list_t *shape, double mass, rgb_color_t color, void *info, \
  free_func_t info_freer){
  body_t *toReturn = body_init(shape, mass, color);
  toReturn->info = info;
  toReturn->info_freer = info_freer;
  return toReturn;
  }

void body_free(body_t *body){
  list_free(body->shape);
  if (body->info_freer != NULL && body->info != NULL){
    body->info_freer(body->info);
  }
  free(body);
}

list_t *body_get_shape(body_t *body) {
  list_t *copy = list_init(list_size(body->shape), free);
  for (size_t i = 0; i < list_size(body->shape); i++) {
    vector_t *vec_copy = malloc(sizeof(vector_t));
    *vec_copy = *(vector_t *)(list_get(body->shape, i));
    list_add(copy, vec_copy);
  }
  return copy;
}

vector_t body_get_centroid(body_t *body) {
  return body->centroid;
}

vector_t body_get_velocity(body_t *body) {
  return body->velocity;
}

rgb_color_t body_get_color(body_t *body) {
  return body->color;
}

double body_get_mass(body_t *body) {
  return body->mass;
}

double body_get_orientation(body_t *body) {
  return body->orientation;
}

vector_t body_get_force(body_t *body){
  return body->force;
}

vector_t body_get_impulse(body_t *body){
  return body->impulse;
}

void *body_get_info(body_t *body){
  return body->info;
}

void body_set_centroid(body_t *body, vector_t x) {
  double x_disp = x.x - body->centroid.x;
  double y_disp = x.y - body->centroid.y;
  body->centroid = x;
  polygon_translate(body->shape, (vector_t) {x_disp, y_disp});
}

void body_set_velocity(body_t *body, vector_t v) {
  body->velocity = v;
}

void body_set_color(body_t *body, rgb_color_t c) {
  body->color = c;
}

void body_set_mass(body_t *body, double mass) {
  body->mass = mass;
}

void body_set_rotation(body_t *body, double angle) {
  polygon_rotate(body->shape, angle - body->orientation, body->centroid);
  body->centroid = polygon_centroid(body->shape);
  body->orientation = angle;
}

void body_add_force(body_t *body, vector_t force) {
  body->force = vec_add(body->force, force);
}

void body_add_impulse(body_t *body, vector_t impulse) {
  body->impulse = vec_add(body->impulse, impulse);
}

void body_tick(body_t *body, double dt) {
  // Adds force
  double new_x = (body->force.x * dt)/ body->mass + body->velocity.x;
  double new_y = (body->force.y * dt)/ body->mass + body->velocity.y;

  // Adds impulse
  new_x += body->impulse.x/ body->mass;
  new_y += body->impulse.y/ body->mass;

  // Translates body based on average of before and after velocity
  double x_disp = (body->velocity.x + new_x) / 2.0 * dt;
  double y_disp = (body->velocity.y + new_y) / 2.0 * dt;
  body_set_centroid(body, vec_add(body->centroid, (vector_t) {x_disp, y_disp}));

  body->velocity = (vector_t) {new_x, new_y};
  body->force = (vector_t) {0, 0};
  body->impulse = (vector_t) {0, 0};
}

void body_remove(body_t *body){
  body->forRemoval = 1;
}

bool body_is_removed(body_t *body){
  return body->forRemoval;
}
