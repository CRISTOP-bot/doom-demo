#ifndef DOOM_CORE_H
#define DOOM_CORE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Game constants */
#define GAME_WIDTH 1280
#define GAME_HEIGHT 960
#define MAX_ENEMIES 64
#define MAX_PROJECTILES 256

/* Input structure */
typedef struct {
  int w, a, s, d;
  int mouseX, mouseY;
  int fire;
  int door;
  int pause;
} InputState;

/* Player structure */
typedef struct {
  float x, y, z;
  float angleX, angleY;
  int health;
  int ammo;
  int score;
} Player;

/* Enemy structure */
typedef struct {
  float x, y, z;
  int health;
  bool active;
  float angle;
} Enemy;

/* Projectile structure */
typedef struct {
  float x, y, z;
  float vx, vy, vz;
  bool active;
  int damage;
} Projectile;

/* Game state structure */
typedef struct {
  Player player;
  Enemy enemies[MAX_ENEMIES];
  Projectile projectiles[MAX_PROJECTILES];
  int numEnemies;
  int numProjectiles;
  bool isPaused;
  uint32_t frameCount;
} GameState;

/* Function declarations */
int game_init(int width, int height);
void game_update(const InputState* input);
void game_render(uint32_t* frameBuffer);
void game_cleanup(void);
GameState* game_get_state(void);

#ifdef __cplusplus
}
#endif

#endif /* DOOM_CORE_H */
