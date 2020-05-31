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
const int MASS = 100.0;
const int PTS = 5;
const double DROPPED_V = 400.0;
const int SIZE_ALL = 25.0;
const double GRAVITY = 100.0;
const double FLOOR_THICKNESS = 50.0;
const double FLOOR_MASS = 10000000000.0;
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

/**
 * Returns a pointer to a body representing a rectangle
 *
 * @param width the width of rectangle
 * @param height the height of rectangle
 * @param center initial starting position of centroid of rectangle, as a vector
 * @param s status of body (w for wall, b for block)
 * @return body_t pointer to rectangle
 */
body_t *init_rectangle(double width, double height, vector_t centroid, char s) {
    list_t *points = list_init(INIT_LIST, (free_func_t) free);
    vector_t *c1 = malloc(sizeof(vector_t));
    c1->x = 0;
    c1->y = 0;
    list_add(points, c1);
    vector_t *c2 = malloc(sizeof(vector_t));
    c2->x = width;
    c2->y = 0;
    list_add(points, c2);
    vector_t *c3 = malloc(sizeof(vector_t));
    c3->x = width;
    c3->y = height;
    list_add(points, c3);
    vector_t *c4 = malloc(sizeof(vector_t));
    c4->x = 0;
    c4->y = height;
    list_add(points, c4);
    char *status = malloc(sizeof(char));
    *status = s;
    body_t *toReturn = body_init_with_info(points, INFINITY,
      (rgb_color_t) {1, 0, 0}, status, free);
    body_set_centroid(toReturn, centroid);
    return toReturn;
}
/**
* Initializes the floor of the game offscreen and gives it infinite mass
*
* @param scene to add floor
*/
body_t *init_floor(scene_t *scene){
  body_t *floor = init_rectangle(WIDTH, FLOOR_THICKNESS, \
  (vector_t) {WIDTH / 2, FLOOR_THICKNESS / 2}, 'f');
  body_set_color(floor, (rgb_color_t){0, 1, 1});
  body_set_mass(floor, FLOOR_MASS);
  scene_add_body(scene, floor);
  return floor;
}
/**
* Returns random integer from 0 to upperbound inclusive
*
* @param upper as upperbound
*/
int rand_int(int upper){
  return (int) floor((upper + 1) *((float)rand()/ (float)RAND_MAX));
}
/**
* Resets the dropped shape at the top with random color and n
*
*@param scene where dropped shape is added to
*/
body_t *reset_dropped(scene_t *scene){
  list_t *colors = init_colors();
  int rand_index_c = rand_int(6);
  rgb_color_t *color = list_get(colors, rand_index_c);
  int rand_index_n = 8 - rand_int(5);
  body_t *dropped = init_polygon(rand_index_n, SIZE_ALL, \
    (vector_t) {WIDTH / 2, HEIGHT - SIZE_ALL});
  body_set_color(dropped, *color);
  scene_add_body(scene, dropped);
  return dropped;
}

void *init_pit(scene_t *scene){
  body_t *floor = init_floor(scene);
  //
  //int rand_index_n = 8 - rand_int(5);
  body_t *shape = init_polygon(3, SIZE_ALL, (vector_t) {100.0, 100.0});
  scene_add_body(scene, shape);
  create_gravity_one(scene, GRAVITY, shape, floor);
  create_physics_collision(scene, 0.0, shape, floor);
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

/**
 * When left mouse button is pressed, the shape at the top gets dropped
 * Initilizes new shape to drop
 *
 * @param button representing which key is MOUSE_PRESSED
 * @param type representing if mouse is MOUSE_PRESSED
 * @param held_time representing how long key is held
 * @param dropped representing dropped shape object
 * @param s representing current scene
 */

void on_mouse(char button, mouse_event_type_t type, void* dropped, void *s){
    body_t *floor = init_floor(s);
    switch(type){
      case MOUSE_PRESSED:
          if (button == LEFT_BUTTON){
            printf("mouse_pressed\n");
            create_gravity_one(s, GRAVITY, dropped, floor);
            create_physics_collision(s, 0.0, dropped, floor);
          }
          break;
      case MOUSE_RELEASED:
        printf("mouse_released\n");
        dropped = reset_dropped(s);
        break;
    }
  }

/**
 * Prevents shape to be dropped from going offscreen
 *
 * @param dropped body representing the shape being dropped
 */
void bound(body_t *dropped) {
    vector_t curr_pos = body_get_centroid(dropped);
    if (curr_pos.x + SIZE_ALL > WIDTH) {
        body_set_centroid(dropped, (vector_t) {WIDTH - SIZE_ALL, curr_pos.y});
    }
    else if (curr_pos.x - SIZE_ALL < 0) {
        body_set_centroid(dropped, (vector_t) {SIZE_ALL, curr_pos.y});
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
  init_pit(scene);
  sdl_on_key((key_handler_t) on_key, dropped, scene);
  sdl_on_mouse((mouse_handler_t) on_mouse, dropped, scene);
  while (!sdl_is_done()){
    double time_elapsed = time_since_last_tick();
    scene_tick(scene, time_elapsed);
    bound(dropped);
    sdl_render_scene(scene);
  }
  scene_free(scene);

}
