#ifndef DOOM_CORE_H
#define DOOM_CORE_H

#include <stdint.h>
#include <math.h>

/* ==================== Tipos de Datos ==================== */

typedef struct {
    float x, y, z;
} Vector3;

typedef struct {
    float x, y, a;  /* posicion x,y y angulo */
    int hp;
    int ammo;
    int score;
    int mouse_locked;
} Player;

typedef struct {
    float x, y, z;
    int hp;
    int active;
} Enemy;

typedef struct {
    float x, y, z;
    float vx, vy, vz;
    int active;
} Bullet;

typedef struct {
    Player player;
    Enemy enemies[32];
    Bullet bullets[256];
    int enemy_count;
    int bullet_count;
    int map[32][32];  /* mapa simple */
    int game_over;
    int level;
} GameState;

typedef struct {
    int w, a, s, d;
    int left, right;
    int space, e, tab, r;
    int mouse_dx;
} InputState;

typedef struct {
    float x, y, z;
    float r, g, b;
} Light;

/* ==================== Funciones del Juego ==================== */

void game_reset(GameState *game);
void game_tick(GameState *game, const InputState *input);
void game_render(GameState *game, uint32_t *framebuffer, float *depth_buffer);

/* ==================== Funciones de Renderizado OpenGL ES ==================== */

int opengl_es_init(int width, int height);
void opengl_es_cleanup(void);
void opengl_es_begin_frame(void);
void opengl_es_end_frame(void);
void opengl_es_draw_scene(GameState *game);
void opengl_es_set_viewport(int width, int height);

#endif /* DOOM_CORE_H */
