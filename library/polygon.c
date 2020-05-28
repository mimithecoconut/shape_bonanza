#include "polygon.h"
#include "list.h"
#include "vector.h"

const double SIX = 6.0;

double polygon_area(list_t *polygon) {
    double det_sum = 0.0;
    int num_vertices = (int)(list_size(polygon));
    for (int k = 0; k < num_vertices; k++) {
      vector_t *coord_k = (vector_t*) list_get(polygon, k);
      vector_t *coord_k1 = (vector_t*) list_get(polygon, (k + 1) % num_vertices);
      det_sum += vec_cross(*coord_k, *coord_k1);
    }
    return det_sum / 2;
}

vector_t polygon_centroid(list_t *polygon) {
    double area = polygon_area(polygon);
    double centroid_x = 0.0;
    double centroid_y = 0.0;
    int num_vertices = (int)(list_size(polygon));
    for (int k = 0; k < num_vertices; k++) {
        vector_t *coord_k = (vector_t*) list_get(polygon, k);
        vector_t *coord_k1 = (vector_t*) list_get(polygon, (k + 1) % num_vertices);
        double det = vec_cross(*coord_k, *coord_k1);
        centroid_x += (1 / (SIX * area) * (coord_k->x + coord_k1->x) * det);
        centroid_y += (1 / (SIX * area) * (coord_k->y + coord_k1->y) * det);
    }
    vector_t ans = {centroid_x, centroid_y};
    return ans;
}

void polygon_translate(list_t *polygon, vector_t translation) {
  int num_vertices = (int)(list_size(polygon));
  for (int k = 0; k < num_vertices; k++) {
      vector_t *orig_vertex = (vector_t*) list_get(polygon, k);
      *orig_vertex = vec_add(*orig_vertex, translation);
  }
}

void polygon_rotate(list_t *polygon, double angle, vector_t point) {
    // Translates polygon to frame of reference where it rotates around (0, 0)
    polygon_translate(polygon, vec_negate(point));
    int num_vertices = (int)(list_size(polygon));
    for (int k = 0; k < num_vertices; k++) {
        vector_t *orig_vertex = (vector_t*) list_get(polygon, k);
        *orig_vertex = vec_rotate(*orig_vertex, angle);
    }
    // Translates polygon back to its original frame
    polygon_translate(polygon, point);
}
