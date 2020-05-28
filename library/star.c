#include <stddef.h>
#include "vector.h"
#include "list.h"
#include "polygon.h"
#include <math.h>
#include <stdlib.h>
#include "star.h"
#include <assert.h>

star_t *init_star(vector_t start_pos, int pts, double rad, double vx, double vy) {
    assert(pts > 1 && rad > 0);
    star_t *toReturn = malloc(sizeof(star_t));
    toReturn->points = pts;
    toReturn->radius = rad;
    toReturn->coords = list_init(pts * 2, free);
    // Initializes star vertices
    for (int k = 0; k < pts * 2; k++) {
        double factor = M_PI / pts;
        vector_t *vertex = malloc(sizeof(vector_t));
        assert(vertex != NULL);
        if (k % 2 == 0) {
            vertex->x = start_pos.x + rad * cos(k * factor + M_PI / 2);
            vertex->y = start_pos.y + rad * sin(k * factor + M_PI / 2);
        }
        else {
            vertex->x = start_pos.x + rad / 2 * cos(k * factor + M_PI / 2);
            vertex->y = start_pos.y + rad / 2 * sin(k * factor + M_PI / 2);
        }
        list_add(toReturn->coords, vertex);
    }
    toReturn->velocity = malloc(sizeof(vector_t));
    toReturn->velocity->x = vx;
    toReturn->velocity->y = vy;
    toReturn->color = (rgb_color_t) {0, 0, 0};
    return toReturn;
}

void star_set_color(star_t *star, rgb_color_t c) {
    star->color = c;
}

void star_free(star_t *star) {
    free(star->velocity);
    list_free(star->coords);
    free(star);
}

vector_t *get_velocity(star_t *star) {
    return star->velocity;
}

int get_num_pts(star_t *star) {
    return star->points;
}

list_t *get_coords(star_t *star) {
    return star->coords;
}

double get_radius(star_t *star) {
    return star->radius;
}

vector_t get_center(star_t *star) {
    return polygon_centroid(star->coords);
}

void translate_star(star_t *star, vector_t translation) {
    polygon_translate(star->coords, translation);
}

void add_velocity(star_t *star, double vx, double vy) {
    star->velocity->x += vx;
    star->velocity->y += vy;
}

void rotate_star(star_t *star, double angle) {
    polygon_rotate(star->coords, angle, get_center(star));
}
