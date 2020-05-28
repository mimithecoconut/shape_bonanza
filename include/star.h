#ifndef __STAR__
#define __STAR__

#include <stddef.h>
#include "vector.h"
#include "list.h"
#include "polygon.h"
#include "color.h"

/**
 * A star to be represented on frame.
 * A star has the following properties: number of points, vec_list_t of
 * coordinates, a vector representing its velocity, radius, and color values
 * for red, green, and blue.
 */
 typedef struct star {
     int points;
     list_t *coords;
     vector_t *velocity;
     double radius;
     rgb_color_t color;
 } star_t;

/**
 * Allocates memory for a star with the given properties.
 *
 * @param start_pos the starting position of the center of the star, expressed
 * as a vector
 * @param pts integer representing the number of points the star has, must be >1
 * @param rad double representing radius of star, must be >0
 * @param vx initial horizontal velocity of star, expressed as double
 * @param vy initial vertical velocity of star, expressed as double
 * @return a pointer to the newly initialized star
 */
star_t *init_star(vector_t start_pos, int pts, double rad, double vx, double vy);

/**
 * Changes a star's color.
 *
 * @param star a pointer to a body returned from body_init()
 * @param c the star's new color
 */
void star_set_color(star_t *star, rgb_color_t c);

/**
 * Releases memory allocated to a star.
 *
 * @param star a pointer to a star returned from init_star()
 */
void star_free(star_t *star);

/**
 * Returns the current velocity of a star.
 *
 * @param star a pointer to a star returned from init_star()
 * @return vector representing current velocity of star
 */
vector_t *get_velocity(star_t *star);

/**
 * Returns the number of points a star has.
 *
 * @param star a pointer to a star returned from init_star()
 * @return number of points the star has
 */
int get_num_pts(star_t *star);

/**
 * Returns a list of coordinates of star's points.
 *
 * @param star a pointer to a star returned from init_star()
 * @return list of points representing vertices of star
 */
list_t *get_coords(star_t *star);

/**
 * Returns the radius of a star.
 *
 * @param star a pointer to a star returned from init_star()
 * @return radius of star
 */
double get_radius(star_t *star);

/**
 * Returns the location of the center of a star.
 *
 * @param star a pointer to a star returned from init_star()
 * @return vector representing the coordinates of the star's centroid
 */
vector_t get_center(star_t *star);

/**
 * Translates the star by the given displacement.
 *
 * @param star a pointer to a star returned from init_star()
 * @param translation vector of amount star should be translated by
 */
void translate_star(star_t *star, vector_t translation);

/**
 * Accelerates the star by modifying its velocity by the given amount.
 *
 * @param star a pointer to a star returned from init_star()
 * @param vx amount to accelerate horizontal velocity by
 * @param vy amount to accelerate vertical velocity by
 */
void add_velocity(star_t *star, double vx, double vy);

/**
 * Rotates the star by the given angle (in radians) around its center.
 *
 * @param star a pointer to a star returned from init_star()
 * @param angle angle to rotate star by, in radians, where a positive angle
 * signifies a counterclockwise rotation.
 */
void rotate_star(star_t *star, double angle);

#endif
