#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "vector.h"

const vector_t VEC_ZERO = {0.0, 0.0};

vector_t vec_add(vector_t v1, vector_t v2) {
    double x_sum = v1.x + v2.x;
    double y_sum = v1.y + v2.y;
    vector_t vec_sum = {x_sum, y_sum};
    return vec_sum;
}

vector_t vec_subtract(vector_t v1, vector_t v2) {
    double x_diff = v1.x - v2.x;
    double y_diff = v1.y - v2.y;
    vector_t vec_diff = {x_diff, y_diff};
    return vec_diff;
}

vector_t vec_negate(vector_t v){
    double x_inv = -1 * v.x;
    double y_inv = -1 * v.y;
    vector_t inv = {x_inv, y_inv};
    return inv;
}

vector_t vec_multiply(double scalar, vector_t v){
    double x_mult = scalar * v.x;
    double y_mult = scalar * v.y;
    vector_t mult = {x_mult, y_mult};
    return mult;
}

double vec_dot(vector_t v1, vector_t v2){
      double x_dot = v1.x * v2.x;
      double y_dot = v1.y * v2.y;
      return x_dot + y_dot;
}

double vec_cross(vector_t v1, vector_t v2){
    return v1.x * v2.y - v1.y * v2.x;
}

vector_t vec_rotate(vector_t v, double angle){
    double x_rot = v.x * cos(angle) + v.y * -sin(angle);
    double y_rot = v.x * sin(angle) + v.y * cos(angle);
    vector_t rotated = {x_rot, y_rot};
    return rotated;
}
