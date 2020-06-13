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
const int WIDTH = 800.0;
const int HEIGHT = 1010.0;
const int MASS = 1000.0;
const int PTS = 5;
const double DROPPED_V = 400.0;
const int SIZE_ALL = 25;
const double GRAVITY = 250.0;
const double FLOOR_THICKNESS = 50.0;
const double BIG_MASS = 10000000000.0;

/**
 * Returns a list of rgb_color_t pointers for the colors of shape
 *
 * @return list_t of colors for shape
 */
list_t *init_colors(){
  list_t *ans = list_init(INIT_LIST, free, NULL);
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
  list_t *points = list_init(n, (free_func_t) free, NULL);
  double angle = 2 * M_PI / n;
  for (int i = 0; i < n; i ++){
    vector_t *point = malloc(sizeof(vector_t));
    point->x = centroid.x + (size + 2.5) * cos(i * angle);
    point->y = centroid.y + (size + 2.5) * sin(i * angle);
    list_add(points, point);
  }
  char *status = malloc(sizeof(char));
  *status = 't';
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
  list_t *coords = get_coords(str);
  star_free(str);
  return body_init_with_info(coords, MASS, (rgb_color_t){0,0,0}, status, free);
}

/**
 * Returns a pointer to a body representing a rectangle
 *
 * @param width the width of rectangle
 * @param height the height of rectangle
 * @param center initial starting position of centroid of rectangle, as a vector
 * @param s status of body (f for floor)
 * @return body_t pointer to rectangle
 */
body_t *init_rectangle(double width, double height, vector_t centroid, char s) {
    list_t *points = list_init(INIT_LIST, (free_func_t) free, NULL);
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
 * Initializes bodies offscreen representing walls bounding the windows
 *
 * @param scene the scene to add blocks to
 */
void init_walls(scene_t *scene) {
    body_t *floor = init_rectangle(2 * WIDTH, FLOOR_THICKNESS,
      (vector_t) {WIDTH / 2, -FLOOR_THICKNESS / 2 + 10.0}, 'f');
    body_t *left_wall = init_rectangle(FLOOR_THICKNESS, HEIGHT,
      (vector_t) {-FLOOR_THICKNESS / 2, HEIGHT / 2}, 'w');
    body_t *right_wall = init_rectangle(FLOOR_THICKNESS, HEIGHT,
      (vector_t) {WIDTH + FLOOR_THICKNESS / 2, HEIGHT / 2}, 'w');
    body_set_color(left_wall, (rgb_color_t) {1, 1, 1});
    body_set_color(right_wall, (rgb_color_t) {1, 1, 1});
    body_set_color(floor, (rgb_color_t) {1, 1, 1});
    body_set_mass(floor, BIG_MASS);
    scene_add_body(scene, floor);
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
 * @param scene where dropped shape is added to
 */
body_t *reset_dropped(scene_t *scene){
  list_t *colors = init_colors();
  int rand_index_c = rand_int(6);
  rgb_color_t *color = list_get(colors, rand_index_c);
  int rand_index_n = 9 - rand_int(5);
  body_t *dropped;
  if (rand_index_n == 9) {
      dropped = init_special(SIZE_ALL, \
        (vector_t) {WIDTH / 2, HEIGHT - SIZE_ALL});
  }
  else {
      dropped = init_polygon(rand_index_n, SIZE_ALL, \
        (vector_t) {WIDTH / 2, HEIGHT - SIZE_ALL});
      body_set_color(dropped, *color);
  }
  scene_add_body(scene, dropped);
  return dropped;
}
/**
 * Moves shapes in the pit up by one row
 *
 * @param the scene with the shapes
 */
void pit_up(scene_t *scene){
  for (size_t i = 0; i < scene_bodies(scene); i++){
    char *status = malloc(sizeof(char));
    *status = 'p';
    body_t *body = scene_get_body(scene, i);
    vector_t force = body_get_force(body);
    if (*(char *)body_get_info(body) == 'p'){
      vector_t centroid = body_get_centroid(body);
      body_set_centroid(body, (vector_t) {centroid.x, centroid.y + 2 *SIZE_ALL});
    }
    if (force.x == 0.0 && force.y == 0.0 && *(char *)body_get_info(body) == 'd'){
      body_set_info(body, status);
    }
  }
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
void on_key(char key, key_event_type_t type, double held_time, void *s) {
    body_t *top = scene_get_top(s);
    double angle = body_get_orientation(top);
    bool press = false;
    switch (type) {
        case KEY_RELEASED:
            switch (key) {
                case LEFT_ARROW:
                    body_set_velocity(top, VEC_ZERO);
                    break;
                case RIGHT_ARROW:
                    body_set_velocity(top, VEC_ZERO);
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
            body_set_velocity(top, new_v);
        }
    }
}

/**
 * Prevents shape to be dropped from going offscreen
 *
 * @param scene where the shapes are
 */
void bound(scene_t *scene) {
    body_t *top = scene_get_top(scene);
    vector_t curr_pos = body_get_centroid(top);
    if (curr_pos.x + SIZE_ALL > WIDTH) {
        body_set_centroid(top, (vector_t) {WIDTH - SIZE_ALL, curr_pos.y});
        body_set_velocity(top, VEC_ZERO);
    }
    else if (curr_pos.x - SIZE_ALL < 0) {
        body_set_centroid(top, (vector_t) {SIZE_ALL, curr_pos.y});
        body_set_velocity(top, VEC_ZERO);
    }
}

/**
 * Removes all objects in a certain radius around a certain body, including
 * the body itself
 *
 * @param b body to remove bodies around
 * @param s scene to remove bodies from
 */
void remove_nearby(body_t *b, scene_t *s) {
    double x_coord = body_get_centroid(b).x;
    double y_coord = body_get_centroid(b).y;
    double left_bound = x_coord - 4 * SIZE_ALL;
    double right_bound = x_coord + 4 * SIZE_ALL;
    double lower_bound = y_coord - 4 * SIZE_ALL;
    double upper_bound = y_coord + 4 * SIZE_ALL;
    for (size_t i = 0; i < scene_bodies(s); i++){
        body_t *other = scene_get_body(s, i);
        double x = body_get_centroid(other).x;
        double y = body_get_centroid(other).y;
        if (x > left_bound && x < right_bound && y > lower_bound &&
          y < upper_bound && *(char *)body_get_info(other) == 'p'){
            body_remove(other);
        }
    }
    body_remove(b);
}

/**
 * Helper function to count how many objects of the same color as the current
 * object are touching
 *
 * @param b body to check
 * @param s scene containing the bodies
 * @param count current number
 * @param l list of bodies of the same color touching
 * @return number of objects touching b with the same color as b
 */
int count_colors(body_t *b, scene_t *s, int count, list_t *l) {
    for (size_t i = 0; i < scene_bodies(s); i++){
        body_t *body = scene_get_body(s, i);
        rgb_color_t c1 = body_get_color(b);
        rgb_color_t c2 = body_get_color(body);
        vector_t pos1 = body_get_centroid(b);
        vector_t pos2 = body_get_centroid(body);
        double distance = sqrt(pow((pos2.x - pos1.x), 2) + pow((pos2.y - pos1.y), 2));
        if (distance < 2.5 * SIZE_ALL && c1.r == c2.r && c1.g == c2.g && c1.b == c2.b) {
            if (!list_contains(l, body)) {
                list_add(l, body);
                count_colors(body, s, list_size(l), l);
            }
        }
    }
    return list_size(l);
}

/**
 * Checks whether the current object is touching at least 3 other objects of
 * the same color, and if so, removes them
 *
 * @param b body to check
 * @param s scene containing the bodies
 */
void touching_colors(body_t *b, scene_t *s) {
    list_t *touching = list_init(INIT_LIST, (free_func_t) body_free,
      (equality_func_t) body_equals);
    list_add(touching, b);
    int num_touching = count_colors(b, s, list_size(touching), touching);
    if (num_touching >= 3) {
        for (size_t i = 0; i < list_size(touching); i++) {
            body_remove(list_get(touching, i));
        }
        scene_add_score(s, num_touching);
    }
    free(touching);
}



/**
 * Collision handler to destroy objects if at least 3 of one color are touching
 *
 * @param body_1 first body to check
 * @param body_2 second body to check
 * @param axis axis of collision
 * @param aux auxiliary value
 */
void destroy(body_t *body_1, body_t *body_2, vector_t axis, void *aux) {
    if (*(char*)body_get_info(body_1) == 'b') {
        remove_nearby(body_1, (scene_t*) aux);
    }
    else if (*(char*)body_get_info(body_1) != 'f') {
        char *status = malloc(sizeof(char));
        *status = 'p';
        body_set_info(body_1, status);
        touching_colors(body_1, (scene_t*) aux);
    }
}

/**
 * Creates physics collision for shapes in scene that are within
 * 4 radius of the body
 * Creates collisions with disappearance of same colors
 *
 * @param scene with all the bodies
 * @param body that we are finding nearby shapes to
 */
void create_nearby_collision(scene_t *scene, body_t *body){
  double x_coord = body_get_centroid(body).x;
  double left_bound = x_coord - 4 * SIZE_ALL;
  double right_bound = x_coord + 4 * SIZE_ALL;
  for (size_t i = 0; i < scene_bodies(scene); i++){
    body_t *other = scene_get_body(scene, i);
    double x = body_get_centroid(other).x;
    body_set_mass(other, BIG_MASS);
    body_set_velocity(other, VEC_ZERO);
    if (x > left_bound && x < right_bound && *(char *)body_get_info(other) != 't'
      && *(char *)body_get_info(other) != 'b' && *(char *)body_get_info(other) != 'w'){
      create_physics_collision(scene, 0.0, body, other);
    }
    if (*(char *)body_get_info(body) != 'p' && x > left_bound && x < right_bound && *(char *)body_get_info(other) == 'p'){
      create_collision(scene, body, other, destroy, scene, free);
    }
  }
}

/**
 * Initializes one row of shapes in the pit and puts all shapes into scene
 *
 * @param the scene to put the row in
 */
void init_one_row(scene_t *scene){
  int rand_index_n = 8 - rand_int(4);
  list_t *colors = init_colors();
  int rand_index_c = rand_int(6);
  rgb_color_t *color = list_get(colors, rand_index_c);
  for (int i = SIZE_ALL; i < WIDTH; i+= 2 * SIZE_ALL){
    char *status = malloc(sizeof(char));
    *status = 'p';
    body_t *shape1 = init_polygon(rand_index_n, SIZE_ALL, (vector_t){i, 10 + SIZE_ALL});
    body_set_info(shape1, status);
    body_set_velocity(shape1, VEC_ZERO);
    body_set_color(shape1, *color);
    scene_add_body(scene, shape1);
    rand_index_n = 8 - rand_int(4);
    rand_index_c = rand_int(6);
    color = list_get(colors, rand_index_c);
  }
}

/**
 * Initializes the beginning of the game with 4 rows of shapes in the pit
 *
 * @param scene to put shapes
 */
void init_pit(scene_t *scene){
  for (int i = 0; i < 5; i++){
    init_one_row(scene);
    if (i != 4){
      pit_up(scene);
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
void on_mouse(char button, mouse_event_type_t type, void *s){
    body_t *floor = scene_get_body(s, 0);
    body_set_force(floor, VEC_ZERO);
    body_set_velocity(floor, VEC_ZERO);
    char *status = malloc(sizeof(char));
    *status = 'd';
    switch(type){
      case MOUSE_PRESSED:
          if (button == LEFT_BUTTON){
            body_t *dropped = scene_get_top(s);
            create_gravity_one(s, GRAVITY, dropped, floor);
            create_nearby_collision(s, dropped);
            if (*(char *)body_get_info(dropped) == 'b') {
              scene_add_score(s, 16);
            }
            if (*(char *)body_get_info(dropped) == 't') {
              body_set_info(dropped, status);
            }
          }
          break;
      case MOUSE_RELEASED:
        scene_set_top(s, reset_dropped(s));
        break;
    }
}

/**
 * Checks to see if shapes in pit have reached the top of window
 * @ returns true if so, returns false if not
 *
 * @param scene
 */
bool game_over(scene_t *scene){
  for (size_t i = 0; i < scene_bodies(scene); i++){
    body_t *body = scene_get_body(scene, i);
    if (body_get_centroid(body).y > HEIGHT - SIZE_ALL && *(char *)body_get_info(body) == 'p'){
      return true;
    }
  }
  return false;
}

/**
 * Removes all bodies from the scene
 *
 * @param scene the scene containing the game
 */
void scene_clear(scene_t *scene) {
    for (size_t j = 0; j < scene_bodies(scene); j++) {
        body_t *body = scene_get_body(scene, j);
        if (*(char *)body_get_info(body) != 'w' || *(char *)body_get_info(body) != 'f'){
          body_remove(body);
        }
    }
    scene_tick(scene, 0);
}



int main(int argc, char *argv[]) {
  bool game_ended = false;
  SDL_Rect rect1, rect2;
  SDL_Texture *texture1, *texture2;
  char *font_path;

  if (argc == 1) {
      font_path = "resources/FreeSans.ttf";
  } else if (argc == 2) {
      font_path = argv[1];
  } else {
      fprintf(stderr, "error: too many arguments\n");
      exit(EXIT_FAILURE);
  }

  vector_t min = VEC_ZERO;
  vector_t max = {WIDTH, HEIGHT};
  sdl_init(min, max);
  TTF_Init();
  TTF_Font *font = TTF_OpenFont(font_path, 24);
  if (font == NULL) {
      fprintf(stderr, "error: font not found\n");
      exit(EXIT_FAILURE);
  }
  scene_t *scene = scene_init();
  init_walls(scene);
  body_t *dropped = reset_dropped(scene);
  scene_set_top(scene, dropped);
  init_pit(scene);
  double total_time = 0.0;
  double total_time_elapsed = 0.0;
  sdl_on_key((key_handler_t) on_key, dropped, scene);
  sdl_on_mouse((mouse_handler_t) on_mouse, dropped, scene);

  while (!sdl_is_done()){
    double time_elapsed = time_since_last_tick();
    total_time_elapsed += time_elapsed;
    if (!game_ended) {
        total_time += time_elapsed;
    }

    if (total_time_elapsed > 5.0) {
        total_time_elapsed = 0.0;
        pit_up(scene);
        init_one_row(scene);
    }
    int score = scene_get_score(scene);
    char score_msg[15];
    sprintf(score_msg, "%i", score);
    char time_msg[15];
    sprintf(time_msg, "%d", (int) total_time);
    if (!game_over(scene)) {
        get_text_and_rect(renderer, 580, 0, concat("Score: ", score_msg), font, &texture1, &rect1);
        get_text_and_rect(renderer, 590, rect1.y + rect1.h, concat("Time: ", time_msg), font, &texture2, &rect2);
        scene_tick(scene, time_elapsed);
        bound(scene);
        sdl_render_scene(scene);
    }
    else {
        game_ended = true;
        sdl_init_game_over();
        get_text_and_rect(renderer, 410, 240, concat("Score: ", score_msg), font, &texture1, &rect1);
        get_text_and_rect(renderer, 410, 281, concat("Time: ", time_msg), font, &texture2, &rect2);
    }
    SDL_RenderCopy(renderer, texture1, NULL, &rect1);
    SDL_RenderCopy(renderer, texture2, NULL, &rect2);
    sdl_show();
  }
  SDL_DestroyTexture(texture1);
  SDL_DestroyTexture(texture2);
  TTF_Quit();
  scene_free(scene);

  return 0;
}
