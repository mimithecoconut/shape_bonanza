#include "sdl_wrapper.h"

const char WINDOW_TITLE[] = "Shape Bonanza";
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 500;
const double MS_PER_S = 1e3;

/**
 * The keypress handler, or NULL if none has been configured.
 */
key_handler_t key_handler = NULL;

/**
 * The mouse click handler, or NULL if none has been configured.
 */
mouse_handler_t mouse_handler = NULL;

clock_t last_clock = 0;

void *body = NULL;

void *scene = NULL;

/**
  * x, y: upper left corner.
  * texture, rect: outputs.
*/
void get_text_and_rect(SDL_Renderer *renderer, int x, int y, char *text,
        TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect) {
    int text_width;
    int text_height;
    SDL_Surface *surface;
    SDL_Color textColor = {0, 0, 0, 0};

    surface = TTF_RenderText_Solid(font, text, textColor);
    *texture = SDL_CreateTextureFromSurface(renderer, surface);
    text_width = surface->w;
    text_height = surface->h;
    SDL_FreeSurface(surface);
    rect->x = x;
    rect->y = y;
    rect->w = text_width;
    rect->h = text_height;
}

/** concatenates two strings */
char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

/** Computes the center of the window in pixel coordinates */
vector_t get_window_center(void) {
    int *width = malloc(sizeof(*width)),
        *height = malloc(sizeof(*height));
    assert(width != NULL);
    assert(height != NULL);
    SDL_GetWindowSize(window, width, height);
    vector_t dimensions = {.x = *width, .y = *height};
    free(width);
    free(height);
    return vec_multiply(0.5, dimensions);
}

/**
 * Computes the scaling factor between scene coordinates and pixel coordinates.
 * The scene is scaled by the same factor in the x and y dimensions,
 * chosen to maximize the size of the scene while keeping it in the window.
 */
double get_scene_scale(vector_t window_center) {
    // Scale scene so it fits entirely in the window
    double x_scale = window_center.x / max_diff.x,
           y_scale = window_center.y / max_diff.y;
    return x_scale < y_scale ? x_scale : y_scale;
}

/** Maps a scene coordinate to a window coordinate */
vector_t get_window_position(vector_t scene_pos, vector_t window_center) {
    // Scale scene coordinates by the scaling factor
    // and map the center of the scene to the center of the window
    vector_t scene_center_offset = vec_subtract(scene_pos, center);
    double scale = get_scene_scale(window_center);
    vector_t pixel_center_offset = vec_multiply(scale, scene_center_offset);
    vector_t pixel = {
        .x = round(window_center.x + pixel_center_offset.x),
        // Flip y axis since positive y is down on the screen
        .y = round(window_center.y - pixel_center_offset.y)
    };
    return pixel;
}

/**
 * Converts an SDL key code to a char.
 * 7-bit ASCII characters are just returned
 * and arrow keys are given special character codes.
 */
char get_keycode(SDL_Keycode key) {
    switch (key) {
        case SDLK_LEFT:     return LEFT_ARROW;
        case SDLK_UP:       return UP_ARROW;
        case SDLK_RIGHT:    return RIGHT_ARROW;
        case SDLK_DOWN:     return DOWN_ARROW;
        case SDLK_SPACE:    return SPACE;
        default:
            // Only process 7-bit ASCII characters
            return key == (SDL_Keycode) (char) key ? key : '\0';
    }
}

void init_SDL_audio(){
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 ){
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    }
  //Initialize SDL_mixer
  if (Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0){
     printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
  }
}

void load_bmusic(){
  Mix_Music *gMusic = NULL;
  //Load music
  gMusic = Mix_LoadMUS("resources/pokecenter.wav");
  if (gMusic == NULL){
     printf ("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
  }
  //If there is no music playing
  if(Mix_PlayingMusic() == 0){
      //Play the music
      Mix_PlayMusic( gMusic, -1 );
  }
}

void load_soundeffect(){
  Mix_Chunk *bloop = NULL;
  bloop = Mix_LoadWAV("resources/bloop_x.wav");
  if (bloop == NULL){
     printf ("Failed to load sound effect! SDL_mixer Error: %s\n", Mix_GetError());
  }
  Mix_PlayChannel(-1, bloop, 0);
}


void sdl_init(vector_t min, vector_t max) {
    // Check parameters
    assert(min.x < max.x);
    assert(min.y < max.y);

    center = vec_multiply(0.5, vec_add(min, max));
    max_diff = vec_subtract(max, center);
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_RESIZABLE
    );
    renderer = SDL_CreateRenderer(window, -1, 0);
    sdl_init_background();
    init_SDL_audio();
    load_bmusic();
}

bool sdl_is_done() {
    SDL_Event *event = malloc(sizeof(*event));
    assert(event != NULL);
    while (SDL_PollEvent(event)) {
        switch (event->type) {
            case SDL_QUIT:
                SDL_DestroyTexture(texture);
                SDL_FreeSurface(image);
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                free(event);
                return true;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                if (mouse_handler == NULL) break;

                uint8_t button = event->button.button;
                char b = 'n';
                if (button == SDL_BUTTON_LEFT){
                  b = LEFT_BUTTON;
                }
                mouse_event_type_t type1 =
                    event->type == SDL_MOUSEBUTTONDOWN ? MOUSE_PRESSED : MOUSE_RELEASED;
                mouse_handler(b, type1, scene);
                load_soundeffect();
                break;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                // Skip the keypress if no handler is configured
                // or an unrecognized key was pressed
                if (key_handler == NULL) break;
                char key = get_keycode(event->key.keysym.sym);
                if (key == '\0') break;

                uint32_t timestamp = event->key.timestamp;
                if (!event->key.repeat) {
                    key_start_timestamp = timestamp;
                }
                key_event_type_t type2 =
                    event->type == SDL_KEYDOWN ? KEY_PRESSED : KEY_RELEASED;
                double held_time = (timestamp - key_start_timestamp) / MS_PER_S;
                key_handler(key, type2, held_time, scene);
                break;
        }
    }
    free(event);
    return false;
}

void sdl_clear(void) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
}

void sdl_draw_polygon(list_t *points, rgb_color_t color) {
    // Check parameters
    size_t n = list_size(points);
    assert(n >= 3);
    assert(0 <= color.r && color.r <= 1);
    assert(0 <= color.g && color.g <= 1);
    assert(0 <= color.b && color.b <= 1);

    vector_t window_center = get_window_center();

    // Convert each vertex to a point on screen
    int16_t *x_points = malloc(sizeof(*x_points) * n),
            *y_points = malloc(sizeof(*y_points) * n);
    assert(x_points != NULL);
    assert(y_points != NULL);
    for (size_t i = 0; i < n; i++) {
        vector_t *vertex = list_get(points, i);
        vector_t pixel = get_window_position(*vertex, window_center);
        x_points[i] = pixel.x;
        y_points[i] = pixel.y;
    }

    // Draw polygon with the given color
    filledPolygonRGBA(
        renderer,
        x_points, y_points, n,
        color.r * 255, color.g * 255, color.b * 255, 255
    );
    free(x_points);
    free(y_points);
}

void sdl_show(void) {
    // Draw boundary lines
    vector_t window_center = get_window_center();
    vector_t max = vec_add(center, max_diff),
             min = vec_subtract(center, max_diff);
    vector_t max_pixel = get_window_position(max, window_center),
             min_pixel = get_window_position(min, window_center);
    SDL_Rect *boundary = malloc(sizeof(*boundary));
    boundary->x = min_pixel.x;
    boundary->y = max_pixel.y;
    boundary->w = max_pixel.x - min_pixel.x;
    boundary->h = min_pixel.y - max_pixel.y;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, boundary);
    free(boundary);

    SDL_RenderPresent(renderer);
}

void sdl_render_scene(scene_t *scene) {
    sdl_clear();
    SDL_RenderCopy(renderer, texture, NULL, dstrect);
    size_t body_count = scene_bodies(scene);
    for (size_t i = 0; i < body_count; i++) {
        body_t *body = scene_get_body(scene, i);
        list_t *shape = body_get_shape(body);
        sdl_draw_polygon(shape, body_get_color(body));
        list_free(shape);
    }
    Mix_Quit();
    //close_audio();
}

void sdl_on_key(key_handler_t handler, void *b, void *s) {
    key_handler = handler;
    body = b;
    scene = s;
}

void sdl_on_mouse(mouse_handler_t handler, void *b, void *s){
  mouse_handler = handler;
  body = b;
  scene = s;
}

double time_since_last_tick(void) {
    clock_t now = clock();
    double difference = last_clock
        ? (double) (now - last_clock) / CLOCKS_PER_SEC
        : 0.0; // return 0 the first time this is called
    last_clock = now;
    return difference;
}

void sdl_init_game_over(void) {
    image = SDL_LoadBMP("resources/game_over.bmp");
    texture = SDL_CreateTextureFromSurface(renderer, image);
    SDL_RenderCopy(renderer, texture, NULL, dstrect);
}

void sdl_init_background(void) {
    image = SDL_LoadBMP("resources/background.bmp");
    texture = SDL_CreateTextureFromSurface(renderer, image);
    vector_t window_center = get_window_center();
    vector_t max = vec_add(center, max_diff),
             min = vec_subtract(center, max_diff);
    vector_t max_pixel = get_window_position(max, window_center),
             min_pixel = get_window_position(min, window_center);
    dstrect = malloc(sizeof(*dstrect));
    dstrect->x = min_pixel.x;
    dstrect->y = max_pixel.y;
    dstrect->w = max_pixel.x - min_pixel.x;
    dstrect->h = min_pixel.y - max_pixel.y;
}
