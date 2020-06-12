#ifndef __SDL_WRAPPER_H__
#define __SDL_WRAPPER_H__

#include <stdbool.h>
#include "color.h"
#include "list.h"
#include "scene.h"
#include "vector.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/begin_code.h>
#include <stdio.h>
#include <SDL_ttf.h>

// Values passed to a key handler when the given arrow key is pressed
#define LEFT_ARROW 1
#define UP_ARROW 2
#define RIGHT_ARROW 3
#define DOWN_ARROW 4
#define SPACE 5

#define LEFT_BUTTON 6

/**
 * The coordinate at the center of the screen.
 */
vector_t center;
/**
 * The coordinate difference from the center to the top right corner.
 */
vector_t max_diff;
/**
 * The SDL window where the scene is rendered.
 */
SDL_Window *window;
/**
 * The renderer used to draw the scene.
 */
SDL_Renderer *renderer;
/**
 * The background image.
 */
SDL_Surface *image;
/**
 * Texture for the background.
 */
SDL_Texture *texture;
/**
 * Rectangle to constrain the background.
 */
SDL_Rect *dstrect;
// /**
//  * The keypress handler, or NULL if none has been configured.
//  */
// key_handler_t key_handler = NULL;
//
// /**
//  * The mouse click handler, or NULL if none has been configured.
//  */
// mouse_handler_t mouse_handler = NULL;

/**
 * SDL's timestamp when a key was last pressed or released.
 * Used to mesasure how long a key has been held.
 */
uint32_t key_start_timestamp;
/**
 * The value of clock() when time_since_last_tick() was last called.
 * Initially 0.
 */
clock_t last_clock;
/**
 * Body to be passed to key_handler, or NULL if none has been configured.
 */
void *body;
/**
 * Scene to be passed to key_handler, or NULL if none has been configured.
 */
void *scene;

/**
 * The possible types of key events.
 * Enum types in C are much more primitive than in Java; this is equivalent to:
 * typedef unsigned int KeyEventType;
 * #define KEY_PRESSED 0
 * #define KEY_RELEASED 1
 */
typedef enum {
    KEY_PRESSED,
    KEY_RELEASED
} key_event_type_t;

/**
 * A keypress handler.
 * When a key is pressed or released, the handler is passed its char value.
 * Most keys are passed as their char value, e.g. 'a', '1', or '\r'.
 * Arrow keys have the special values listed above.
 *
 * @param key a character indicating which key was pressed
 * @param type the type of key event (KEY_PRESSED or KEY_RELEASED)
 * @param held_time if a press event, the time the key has been held in seconds
 * @param scene scene to be acted upon
 */
typedef void (*key_handler_t)(char key, key_event_type_t type, \
  double held_time, void *scene);

/**
 * The possible types of mouse events.
 * Enum types in C are much more primitive than in Java; this is equivalent to:
 * #define MOUSE_PRESSED 0
 * #define MOUSE_RELEASED 1
 */
typedef enum {
    MOUSE_PRESSED,
    MOUSE_RELEASED
} mouse_event_type_t;

/**
  * x, y: upper left corner.
  * texture, rect: outputs.
*/
void get_text_and_rect(SDL_Renderer *renderer, int x, int y, char *text,
        TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect);

/** concatenates two strings */
char* concat(const char *s1, const char *s2);

typedef void (*mouse_handler_t)(char key, mouse_event_type_t type, void *scene);
/**
 * Initializes the SDL window and renderer.
 * Must be called once before any of the other SDL functions.
 *
 * @param min the x and y coordinates of the bottom left of the scene
 * @param max the x and y coordinates of the top right of the scene
 */
void sdl_init(vector_t min, vector_t max);

/**
 * Processes all SDL events and returns whether the window has been closed.
 * This function must be called in order to handle keypresses.
 *
 * @return true if the window was closed, false otherwise
 */
bool sdl_is_done();

/**
 * Clears the screen. Should be called before drawing polygons in each frame.
 */
void sdl_clear(void);

/**
 * Draws a polygon from the given list of vertices and a color.
 *
 * @param points the list of vertices of the polygon
 * @param color the color used to fill in the polygon
 */
void sdl_draw_polygon(list_t *points, rgb_color_t color);

/**
 * Displays the rendered frame on the SDL window.
 * Must be called after drawing the polygons in order to show them.
 */
void sdl_show(void);

/**
 * Draws all bodies in a scene.
 * This internally calls sdl_clear(), sdl_draw_polygon(), and sdl_show(),
 * so those functions should not be called directly.
 *
 * @param scene the scene to draw
 */
void sdl_render_scene(scene_t *scene);

/**
 * Registers a function to be called every time a key is pressed.
 * Overwrites any existing handler.
 *
 * Example:
 * ```
 * void on_key(char key, key_event_type_t type, double held_time) {
 *     if (type == KEY_PRESSED) {
 *         switch (key) {
 *             case 'a':
 *                 printf("A pressed\n");
 *                 break;
 *             case UP_ARROW:
 *                 printf("UP pressed\n");
 *                 break;
 *         }
 *     }
 * }
 * int main(void) {
 *     sdl_on_key(on_key);
 *     while (!sdl_is_done());
 * }
 * ```
 *
 * @param handler the function to call with each key press
 * @param b body to modify with each key press
 * @param s scene to modify with each key press
 */
void sdl_on_key(key_handler_t handler, void *b, void *s);

/**
 * Registers a function to be called every time a mouse button is pressed.
 * Overwrites any existing handler.
 * @param handler the function to call with each mouse press
 * @param b body to modify with each mouse press
 * @param s scene to modify with each mouse press
 */
void sdl_on_mouse(mouse_handler_t handler, void *b, void *s);

/**
 * Gets the amount of time that has passed since the last time
 * this function was called, in seconds.
 *
 * @return the number of seconds that have elapsed
 */
double time_since_last_tick(void);

/**
 * Initializes the game over screen in the game.
 */
void sdl_init_game_over(void);

/**
 * Initializes the background in the game
 */
void sdl_init_background(void);

#endif // #ifndef __SDL_WRAPPER_H__
