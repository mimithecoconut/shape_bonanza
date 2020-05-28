#include "collision.h"
#include <stdbool.h>
#include "list.h"
#include <stdlib.h>
#include <stdio.h>
#include "body.h"
#include "vector.h"
#include <assert.h>
#include <math.h>

const int LARGE = INFINITY;

collision_info_t find_collision(list_t *shape1, list_t *shape2) {
  list_t *axes = get_axes2(shape1, shape2);
  double overlap = LARGE;
  vector_t collision_axis = {0.0, 0.0};
  collision_info_t info = (collision_info_t){false, collision_axis};
  for (size_t i = 0; i < list_size(axes); i++) {
    double min1 = polygon_proj_min(shape1, *(vector_t*) list_get(axes, i));
    double max1 = polygon_proj_max(shape1, *(vector_t*) list_get(axes, i));
    double min2 = polygon_proj_min(shape2, *(vector_t*) list_get(axes, i));
    double max2 = polygon_proj_max(shape2, *(vector_t*) list_get(axes, i));
    if ((max2 < min1) || (max1 < min2)) {
         return info;
       }
       else {
         double min = find_min(fabs(max2 - min1), fabs(max1 - min2));
         if (min < overlap) {
           overlap = min;
           collision_axis = *(vector_t *)(list_get(axes, i));
           double mag = sqrt(pow(collision_axis.x, 2) + pow(collision_axis.y, 2));
           collision_axis = vec_multiply(1 / mag, collision_axis);
         }
       }
  }
  return (collision_info_t){true, collision_axis};
}

double find_min(double first, double second) {
  if (first < second) {
    return first;
  }
  return second;
}
vector_t *edge_perp(vector_t vec) {
  vector_t *vec_normal = malloc(sizeof(vector_t));
  vec_normal->x =  -1 * vec.y;
  vec_normal->y = vec.x;
  return vec_normal;
}
list_t *get_axes1(list_t *shape) {
  size_t len = list_size(shape);
  list_t *result = list_init(len, free);
  for (size_t i = 0; i < len; i++) {
    vector_t vec = *(vector_t *)(list_get(shape, i % len));
    vec = vec_subtract(vec, *(vector_t *)(list_get(shape, (i + 1) % len)));
    list_add(result, (void *) edge_perp(vec));
  }
  return result;
}
list_t *get_axes2(list_t *shape1, list_t *shape2) {
  list_t *result = list_init(list_size(shape1) + list_size(shape2), free);
  list_t *axes1 = get_axes1(shape1);
  list_t *axes2 = get_axes1(shape2);
  for (size_t i = 0; i < list_size(shape1); i++) {
    list_add(result, list_get(axes1, i));
  }
  for (size_t i = 0; i < list_size(shape2); i++) {
    list_add(result, list_get(axes2, i));
  }
  return result;
}

double polygon_proj_min(list_t *shape, vector_t line) {
  double min = vec_dot(*(vector_t*) list_get(shape, 0), line);
  for (size_t i = 0; i < list_size(shape); i++) {
    if (vec_dot(*((vector_t*) list_get(shape, i)), line) < min) {
      min = vec_dot(*((vector_t*) list_get(shape, i)), line);
    }
  }
  return min;
}

double polygon_proj_max(list_t *shape, vector_t line) {
  double max = vec_dot(*((vector_t*) list_get(shape, 0)), line);
  for (size_t i = 0; i < list_size(shape); i++) {
    if (vec_dot(*((vector_t*) list_get(shape, i)), line) > max) {
      max = vec_dot(*((vector_t*) list_get(shape, i)), line);
    }
  }
  return max;
}
