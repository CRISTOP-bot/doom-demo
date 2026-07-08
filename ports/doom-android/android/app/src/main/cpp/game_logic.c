#include "doom_core.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAP_WIDTH 32
#define MAP_HEIGHT 32
#define MAX_ENEMIES 32
#define MAX_BULLETS 256

/* ==================== Inicializacion del Juego ==================== */

void game_reset(GameState *game) {
    if (!game) return;

    memset(game, 0, sizeof(GameState));

    /* Inicializar jugador */
    game->player.x = 5.0f;
    game->player.y = 5.0f;
    game->player.a = 0.0f;
    game->player.hp = 100;
    game->player.ammo = 100;
    game->player.score = 0;
    game->player.mouse_locked = 1;

    /* Generar mapa simple */
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (x == 0 || x == MAP_WIDTH - 1 || y == 0 || y == MAP_HEIGHT - 1) {
                game->map[y][x] = 1;  /* Paredes exteriores */
            } else if ((x == 10 && y > 5 && y < 15) || (y == 10 && x > 5 && x < 15)) {
                game->map[y][x] = 1;  /* Paredes interiores */
            } else {
                game->map[y][x] = 0;  /* Espacio libre */
            }
        }
    }

    /* Crear enemigos iniciales */
    game->enemy_count = 3;
    for (int i = 0; i < game->enemy_count; i++) {
        game->enemies[i].x = 15.0f + (i * 3.0f);
        game->enemies[i].y = 15.0f;
        game->enemies[i].z = 0.0f;
        game->enemies[i].hp = 30;
        game->enemies[i].active = 1;
    }

    game->bullet_count = 0;
    game->game_over = 0;
    game->level = 1;
}

/* ==================== Logica del Juego ==================== */

void game_tick(GameState *game, const InputState *input) {
    if (!game || !input) return;

    if (game->game_over) {
        if (input->r) {
            game_reset(game);
        }
        return;
    }

    /* Movimiento del jugador */
    float move_speed = 0.1f;
    float move_x = 0.0f;
    float move_y = 0.0f;

    if (input->w) {
        move_x += cosf(game->player.a) * move_speed;
        move_y += sinf(game->player.a) * move_speed;
    }
    if (input->s) {
        move_x -= cosf(game->player.a) * move_speed;
        move_y -= sinf(game->player.a) * move_speed;
    }
    if (input->a) {
        move_x += cosf(game->player.a - 3.14159f / 2.0f) * move_speed;
        move_y += sinf(game->player.a - 3.14159f / 2.0f) * move_speed;
    }
    if (input->d) {
        move_x += cosf(game->player.a + 3.14159f / 2.0f) * move_speed;
        move_y += sinf(game->player.a + 3.14159f / 2.0f) * move_speed;
    }

    /* Colisión simple */
    int new_x = (int)(game->player.x + move_x);
    int new_y = (int)(game->player.y + move_y);

    if (new_x > 0 && new_x < MAP_WIDTH && new_y > 0 && new_y < MAP_HEIGHT) {
        if (game->map[new_y][new_x] == 0) {
            game->player.x += move_x;
            game->player.y += move_y;
        }
    }

    /* Rotación del jugador */
    float turn_speed = 0.02f;
    if (input->left) {
        game->player.a -= turn_speed;
    }
    if (input->right) {
        game->player.a += turn_speed;
    }

    /* Mouse look (simulado con mouse_dx) */
    if (input->mouse_dx != 0) {
        game->player.a += (input->mouse_dx * 0.005f);
    }

    /* Disparar */
    if (input->space && game->player.ammo > 0) {
        if (game->bullet_count < MAX_BULLETS) {
            Bullet *bullet = &game->bullets[game->bullet_count];
            bullet->x = game->player.x;
            bullet->y = game->player.y;
            bullet->z = 0.5f;
            bullet->vx = cosf(game->player.a) * 0.3f;
            bullet->vy = sinf(game->player.a) * 0.3f;
            bullet->vz = 0.0f;
            bullet->active = 1;
            game->bullet_count++;
            game->player.ammo--;
        }
    }

    /* Actualizar balas */
    for (int i = 0; i < game->bullet_count; i++) {
        if (!game->bullets[i].active) continue;

        game->bullets[i].x += game->bullets[i].vx;
        game->bullets[i].y += game->bullets[i].vy;
        game->bullets[i].z += game->bullets[i].vz;

        /* Colisión con paredes */
        int bx = (int)game->bullets[i].x;
        int by = (int)game->bullets[i].y;

        if (bx < 0 || bx >= MAP_WIDTH || by < 0 || by >= MAP_HEIGHT ||
            game->map[by][bx] == 1) {
            game->bullets[i].active = 0;
        }

        /* Colisión con enemigos */
        for (int j = 0; j < game->enemy_count; j++) {
            if (!game->enemies[j].active) continue;

            float dx = game->bullets[i].x - game->enemies[j].x;
            float dy = game->bullets[i].y - game->enemies[j].y;
            float dist = sqrtf(dx * dx + dy * dy);

            if (dist < 0.5f) {
                game->enemies[j].hp -= 10;
                if (game->enemies[j].hp <= 0) {
                    game->enemies[j].active = 0;
                    game->player.score += 100;
                }
                game->bullets[i].active = 0;
            }
        }
    }

    /* Actualizar enemigos */
    for (int i = 0; i < game->enemy_count; i++) {
        if (!game->enemies[i].active) continue;

        /* IA simple: perseguir al jugador */
        float dx = game->player.x - game->enemies[i].x;
        float dy = game->player.y - game->enemies[i].y;
        float dist = sqrtf(dx * dx + dy * dy);

        if (dist > 0.1f) {
            float move_speed_enemy = 0.02f;
            game->enemies[i].x += (dx / dist) * move_speed_enemy;
            game->enemies[i].y += (dy / dist) * move_speed_enemy;
        }

        /* Daño al jugador si está cerca */
        if (dist < 0.5f) {
            game->player.hp -= 1;
            if (game->player.hp <= 0) {
                game->game_over = 1;
            }
        }
    }

    /* Abrir puertas */
    if (input->e) {
        int door_x = (int)(game->player.x + cosf(game->player.a) * 1.5f);
        int door_y = (int)(game->player.y + sinf(game->player.a) * 1.5f);

        if (door_x > 0 && door_x < MAP_WIDTH && door_y > 0 && door_y < MAP_HEIGHT) {
            if (game->map[door_y][door_x] == 1) {
                game->map[door_y][door_x] = 0;  /* Abrir puerta */
            }
        }
    }

    /* Verificar victoria */
    int enemies_alive = 0;
    for (int i = 0; i < game->enemy_count; i++) {
        if (game->enemies[i].active) {
            enemies_alive++;
        }
    }

    if (enemies_alive == 0 && game->enemy_count > 0) {
        game->level++;
        game_reset(game);
        game->player.score += 500;
    }
}

/* ==================== Renderizado (Placeholder) ==================== */

void game_render(GameState *game, uint32_t *framebuffer, float *depth_buffer) {
    if (!game || !framebuffer) return;

    /* Este es un placeholder. El renderizado real se hará con OpenGL ES */
    /* en la capa de JNI/Android */
}
