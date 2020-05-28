#include "sdl_wrapper.h"
#include "vector.h"
#include "body.h"
#include "scene.h"
#include "list.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "forces.h"
#include "collision.h"

const int INIT_LIST = 5;
const int WIDTH = 700.0;
const int HEIGHT = 1000.0;
/**
 * Returns a list of rgb_color_t pointers for the colors of shape
 *
 * @return list_t of colors for shape
 */
list_t *init_colors(){
  list_t *ans = list_init(INIT_LIST, free);
  rgb_color_t *red = malloc(sizeof(rgb_color_t));
  red->r = 1, red->g = 0, red->b = 0;
  rgb_color_t *orange = malloc(sizeof(rgb_color_t));
  orange->r = 1, orange->g = 0.5, orange->b = 0;
  rgb_color_t *yellow = malloc(sizeof(rgb_color_t));
  yellow->r = 1, yellow->g = 1, yellow->b = 0;
  rgb_color_t *green = malloc(sizeof(rgb_color_t));
  green->r = 0.5, green->g = 1, green->b = 0;
  rgb_color_t *blue = malloc(sizeof(rgb_color_t));
  blue->r = 0, blue->g = 1, blue->b = 1;
  rgb_color_t *purple = malloc(sizeof(rgb_color_t));
  purple->r = 0.5, purple->g = 0, purple->b = 1;
  rgb_color_t *pink = malloc(sizeof(rgb_color_t));
  pink->r = 1, pink->g = 0, pink->b = 1;
  list_add(ans, red);
  list_add(ans, orange);
  list_add(ans, yellow);
  list_add(ans, green);
  list_add(ans, blue);
  list_add(ans, purple);
  list_add(ans, pink);
  return ans;
}

/**
* Returns a pointer to a regular n-sided polygon
*
* @param n the number of sides of init_polygon
* @param size the radius of the polygon
*/

body_t *init_polygon(double n, double size){
  
}

int main(int argc, char *argv[]) {
  if (argc != 1) {
      printf("USAGE: %s\n", argv[0]);
      return 1;
  }
  vector_t min = VEC_ZERO;
  vector_t max = {WIDTH, HEIGHT};
  sdl_init(min, max);
  scene_t *scene = scene_init();

}
