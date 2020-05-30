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
#include "star.h"

const int INIT_LIST = 5;
const int WIDTH = 700.0;
const int HEIGHT = 1000.0;
const int MASS = 10.0;
const int PTS = 5;
const double DROPPED_V = 400.0;
const int SIZE_ALL = 50.0;
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
* @param centroid
*/
body_t *init_polygon(double n, double size, vector_t centroid){
  list_t *points = list_init(n, (free_func_t) free);
  double angle = 2 * M_PI / n;
  for (int i = 0; i < n; i ++){
    vector_t *point = malloc(sizeof(vector_t));
    point->x = centroid.x + size * cos(i * angle);
    point->y = centroid.y + size * sin(i * angle);
    list_add(points, point);
  }
  char *status = malloc(sizeof(char));
  *status = 's';
  return body_init_with_info(points, MASS, (rgb_color_t){1,0,0}, status, free);
}

/**
* Returns a pointer to the special object (star)
*
* @param size the radius of special object
* @param centroid
*/
body_t *init_special(double size, vector_t centroid){
  star_t *str = init_star(centroid, PTS, size, 0.0, 0.0);
  char *status = malloc(sizeof(char));
  *status = 'b';
  return body_init_with_info(get_coords(str), MASS, \
  (rgb_color_t){0,0,0}, status, free);
}

body_t *reset_dropped(scene_t *scene){
  list_t *colors = init_colors();
  int rand_index_c = (int) floor(7 *((float)rand()/ (float)RAND_MAX));
  rgb_color_t *color = list_get(colors, rand_index_c);
  int rand_index_n = 8 - rand_index_c;
  body_t *dropped = init_polygon(rand_index_n, SIZE_ALL, \
    (vector_t) {WIDTH / 2, HEIGHT - SIZE_ALL});
  body_set_color(dropped, *color);
  scene_add_body(scene, dropped);
  return dropped;
}

/**
 * Adjusts the velocity of the dropped shape based on keyboard input
 *
 * @param key representing which key is KEY_PRESSED
 * @param type representing if key is KEY_PRESSED
 * @param held_time representing how long key is held
 * @param dropped representing dropped shape object
 * @param s representing current scene
 */

void on_key(char key, key_event_type_t type, double held_time, void* dropped,
  void *s) {
    double angle = body_get_orientation(dropped);
    bool press = false;
    switch (type) {
        case KEY_RELEASED:
            switch (key) {
                case LEFT_ARROW:
                    body_set_velocity(dropped, VEC_ZERO);
                    break;
                case RIGHT_ARROW:
                    body_set_velocity(dropped, VEC_ZERO);
                    break;
            }
            break;
        case KEY_PRESSED:
            switch (key) {
                case LEFT_ARROW:
                    angle = M_PI;
                    press = true;
                    break;
                case RIGHT_ARROW:
                    angle = 0;
                    press = true;
                    break;
            }
        if (press) {
            double new_x = DROPPED_V * cos(angle);
            double new_y = DROPPED_V * sin(angle);
            vector_t new_v = {new_x, new_y};
            body_set_velocity(dropped, new_v);
        }
    }
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
  body_t *dropped = reset_dropped(scene);
  sdl_on_key((key_handler_t) on_key, dropped, scene);
  while (!sdl_is_done()){
    double time_elapsed = time_since_last_tick();
    scene_tick(scene, time_elapsed);
    sdl_render_scene(scene);
  }
  scene_free(scene);

}
