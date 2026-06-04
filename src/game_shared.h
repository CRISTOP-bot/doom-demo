#ifndef GAME_SHARED_H
#define GAME_SHARED_H

#include <stdint.h>

#define SCREEN_W 320
#define SCREEN_H 200
#define MAP_W 16
#define MAP_H 16
#define MAX_ENEMIES 6
#define MAX_BULLETS 32
#define FOV 1.0f

typedef struct {
    int w, a, s, d;
    int left, right;
    int space, e, tab, r;
    int mouse_dx;
} InputState;

typedef struct {
    float x, y;
    float px, py;
    float dx, dy;
    int alive;
} Bullet;

typedef struct {
    float x, y;
    int hp;
    int alive;
} Enemy;

typedef struct {
    float x, y;
    float a;
    int hp;
    int ammo;
} Player;

typedef struct {
    Player player;
    Enemy enemies[MAX_ENEMIES];
    Bullet bullets[MAX_BULLETS];
    char mapa[MAP_H][MAP_W + 1];
    int score;
    int game_over;
    int mouse_locked;
} GameState;

#ifdef BUILDING_GAME_DLL
#define API __declspec(dllexport)
#else
#define API
#endif

API void game_reset(GameState *g);
API void game_tick(GameState *g, const InputState *in);
API void game_render(GameState *g, uint32_t *pixels, float *zbuffer);

#endif