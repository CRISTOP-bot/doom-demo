#include "doom_core.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

static GameState g_gameState = {0};
static int g_screenWidth = 0;
static int g_screenHeight = 0;

int game_init(int width, int height) {
  g_screenWidth = width;
  g_screenHeight = height;

  memset(&g_gameState, 0, sizeof(GameState));

  /* Initialize player */
  g_gameState.player.x = 0.0f;
  g_gameState.player.y = 1.7f;
  g_gameState.player.z = 0.0f;
  g_gameState.player.angleX = 0.0f;
  g_gameState.player.angleY = 0.0f;
  g_gameState.player.health = 100;
  g_gameState.player.ammo = 300;
  g_gameState.player.score = 0;

  /* Spawn initial enemies */
  for (int i = 0; i < 8; i++) {
    g_gameState.enemies[i].x = (float)(rand() % 20 - 10);
    g_gameState.enemies[i].y = 1.0f;
    g_gameState.enemies[i].z = (float)(rand() % 20 + 5);
    g_gameState.enemies[i].health = 30;
    g_gameState.enemies[i].active = true;
    g_gameState.enemies[i].angle = 0.0f;
  }
  g_gameState.numEnemies = 8;

  return 1;
}

void game_update(const InputState* input) {
  if (input == NULL) return;

  /* Update player position */
  float moveSpeed = 0.1f;
  float rotSpeed = 0.05f;

  if (input->w) {
    g_gameState.player.x += sinf(g_gameState.player.angleY) * moveSpeed;
    g_gameState.player.z += cosf(g_gameState.player.angleY) * moveSpeed;
  }
  if (input->s) {
    g_gameState.player.x -= sinf(g_gameState.player.angleY) * moveSpeed;
    g_gameState.player.z -= cosf(g_gameState.player.angleY) * moveSpeed;
  }
  if (input->a) {
    g_gameState.player.x -= cosf(g_gameState.player.angleY) * moveSpeed;
    g_gameState.player.z += sinf(g_gameState.player.angleY) * moveSpeed;
  }
  if (input->d) {
    g_gameState.player.x += cosf(g_gameState.player.angleY) * moveSpeed;
    g_gameState.player.z -= sinf(g_gameState.player.angleY) * moveSpeed;
  }

  /* Update camera angle based on mouse */
  g_gameState.player.angleY += (input->mouseX - g_screenWidth / 2) * 0.001f;
  g_gameState.player.angleX += (input->mouseY - g_screenHeight / 2) * 0.001f;

  /* Clamp vertical angle */
  if (g_gameState.player.angleX > 1.57f) g_gameState.player.angleX = 1.57f;
  if (g_gameState.player.angleX < -1.57f) g_gameState.player.angleX = -1.57f;

  /* Update enemies */
  for (int i = 0; i < g_gameState.numEnemies; i++) {
    if (!g_gameState.enemies[i].active) continue;

    Enemy* enemy = &g_gameState.enemies[i];

    /* Simple AI: move towards player */
    float dx = g_gameState.player.x - enemy->x;
    float dz = g_gameState.player.z - enemy->z;
    float dist = sqrtf(dx * dx + dz * dz);

    if (dist > 0.1f) {
      enemy->x += (dx / dist) * 0.02f;
      enemy->z += (dz / dist) * 0.02f;
      enemy->angle = atan2f(dx, dz);
    }

    /* Check collision with player */
    if (dist < 0.5f) {
      g_gameState.player.health -= 1;
    }

    /* Despawn if too far */
    if (dist > 50.0f) {
      enemy->active = false;
    }
  }

  /* Update projectiles */
  for (int i = 0; i < g_gameState.numProjectiles; i++) {
    if (!g_gameState.projectiles[i].active) continue;

    Projectile* proj = &g_gameState.projectiles[i];
    proj->x += proj->vx;
    proj->y += proj->vy;
    proj->z += proj->vz;

    /* Check collision with enemies */
    for (int j = 0; j < g_gameState.numEnemies; j++) {
      if (!g_gameState.enemies[j].active) continue;

      Enemy* enemy = &g_gameState.enemies[j];
      float dx = proj->x - enemy->x;
      float dz = proj->z - enemy->z;
      float dist = sqrtf(dx * dx + dz * dz);

      if (dist < 0.3f) {
        enemy->health -= proj->damage;
        proj->active = false;

        if (enemy->health <= 0) {
          enemy->active = false;
          g_gameState.player.score += 100;

          /* Spawn new enemy */
          if (g_gameState.numEnemies < MAX_ENEMIES) {
            g_gameState.enemies[g_gameState.numEnemies].x = (float)(rand() % 20 - 10);
            g_gameState.enemies[g_gameState.numEnemies].y = 1.0f;
            g_gameState.enemies[g_gameState.numEnemies].z = (float)(rand() % 20 + 5);
            g_gameState.enemies[g_gameState.numEnemies].health = 30;
            g_gameState.enemies[g_gameState.numEnemies].active = true;
            g_gameState.numEnemies++;
          }
        }
        break;
      }
    }

    /* Despawn if out of bounds */
    if (proj->x < -50 || proj->x > 50 || proj->z < -50 || proj->z > 50) {
      proj->active = false;
    }
  }

  /* Fire weapon */
  if (input->fire && g_gameState.player.ammo > 0) {
    if (g_gameState.numProjectiles < MAX_PROJECTILES) {
      Projectile* proj = &g_gameState.projectiles[g_gameState.numProjectiles];
      proj->x = g_gameState.player.x;
      proj->y = g_gameState.player.y;
      proj->z = g_gameState.player.z;
      proj->vx = sinf(g_gameState.player.angleY) * 0.5f;
      proj->vy = sinf(g_gameState.player.angleX) * 0.5f;
      proj->vz = cosf(g_gameState.player.angleY) * 0.5f;
      proj->active = true;
      proj->damage = 25;
      g_gameState.numProjectiles++;
      g_gameState.player.ammo--;
    }
  }

  g_gameState.frameCount++;
}

void game_render(uint32_t* frameBuffer) {
  if (frameBuffer == NULL) return;

  /* Simple software rendering - fill with gradient */
  for (int y = 0; y < g_screenHeight; y++) {
    for (int x = 0; x < g_screenWidth; x++) {
      int idx = y * g_screenWidth + x;
      
      /* Sky gradient */
      if (y < g_screenHeight / 2) {
        uint8_t r = (y * 50) / (g_screenHeight / 2);
        uint8_t g = (y * 100) / (g_screenHeight / 2);
        uint8_t b = 200;
        frameBuffer[idx] = (r << 16) | (g << 8) | b;
      } else {
        /* Ground */
        uint8_t r = 100;
        uint8_t g = 100;
        uint8_t b = 50;
        frameBuffer[idx] = (r << 16) | (g << 8) | b;
      }
    }
  }

  /* Draw simple sprites for enemies */
  for (int i = 0; i < g_gameState.numEnemies; i++) {
    if (!g_gameState.enemies[i].active) continue;

    Enemy* enemy = &g_gameState.enemies[i];
    float dx = enemy->x - g_gameState.player.x;
    float dz = enemy->z - g_gameState.player.z;
    
    /* Simple 2D projection */
    float screenX = (g_screenWidth / 2) + (dx * 100);
    float screenY = (g_screenHeight / 2) - (dz * 100);

    if (screenX > 0 && screenX < g_screenWidth && screenY > 0 && screenY < g_screenHeight) {
      int idx = (int)screenY * g_screenWidth + (int)screenX;
      if (idx >= 0 && idx < g_screenWidth * g_screenHeight) {
        frameBuffer[idx] = 0xFF0000; /* Red for enemies */
      }
    }
  }
}

void game_cleanup(void) {
  memset(&g_gameState, 0, sizeof(GameState));
}

GameState* game_get_state(void) {
  return &g_gameState;
}
