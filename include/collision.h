#ifndef __COLLISION_H__
#define __COLLISION_H__

#include <stdbool.h>
#include "list.h"
#include <stdlib.h>
#include <stdio.h>
#include "body.h"
#include "vector.h"
#include <math.h>

/**
 * Represents the status of a collision between two shapes.
 * The shapes are either not colliding, or they are colliding along some axis.
 */
typedef struct {
    /** Whether the two shapes are colliding */
    bool collided;
    /**
     * If the shapes are colliding, the axis they are colliding on.
     * This is a unit vector pointing from the first shape towards the second.
     * Normal impulses are applied along this axis.
     * If collided is false, this value is undefined.
     */
    vector_t axis;
    // // /** Whether the two shapes collided before in previous tick */
    //  bool collided_last_tick;
} collision_info_t;

/**
 * Computes the status of the collision between two convex polygons.
 * The shapes are given as lists of vertices in counterclockwise order.
 * There is an edge between each pair of consecutive vertices,
 * and one between the first vertex and the last vertex.
 *
 * @param shape1 the first shape
 * @param shape2 the second shape
 * @return whether the shapes are colliding, and if so, the collision axis.
 * The axis should be a unit vector pointing from shape1 towards shape2.
 */
collision_info_t find_collision(list_t *shape1, list_t *shape2);

double find_min(double first, double second);

vector_t *edge_perp(vector_t vec);

list_t *get_axes1(list_t *shape);

list_t *get_axes2(list_t *shape1, list_t *shape2);

double polygon_proj_min(list_t *shape, vector_t line);

double polygon_proj_max(list_t *shape, vector_t line);

#endif // #ifndef __COLLISION_H__
