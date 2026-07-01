#define BUILDING_GAME_DLL
#include "game_shared.h"
#include <math.h>
#include <string.h>

static const char *initial_map[MAP_H] = {
    "################",
    "#..A...........#",
    "#..###..D......#",
    "#..............#",
    "#......##......#",
    "#..............#",
    "#..H...........#",
    "#...........#..#",
    "#..............#",
    "#......###.....#",
    "#..............#",
    "#..D...........#",
    "#..............#",
    "#.....A........#",
    "#..............#",
    "################"
};

static float clampf(float v, float mn, float mx) {
    if (v < mn) return mn;
    if (v > mx) return mx;
    return v;
}

static float absf(float v) {
    return v < 0.0f ? -v : v;
}

static float normalize_angle(float a) {
    while (a < -3.1415926f) a += 6.2831852f;
    while (a >  3.1415926f) a -= 6.2831852f;
    return a;
}

static void put_pixel(int x, int y, uint32_t c, uint32_t *pixels) {
    if (pixels == NULL) return;
    if (x >= 0 && x < SCREEN_W && y >= 0 && y < SCREEN_H) {
        pixels[y * SCREEN_W + x] = c;
    }
}

static int cell_solid(char c) {
    return (c == '#' || c == 'D');
}

static int is_solid_map(char mapa[MAP_H][MAP_W + 1], int x, int y) {
    if (mapa == NULL) return 1;
    if (x < 0 || x >= MAP_W || y < 0 || y >= MAP_H) return 1;
    return cell_solid(mapa[y][x]);
}

static float raycast_distance(char mapa[MAP_H][MAP_W + 1], float ox, float oy, float ang, float maxd) {
    if (mapa == NULL) return maxd;
    
    float step = 0.02f;
    float dx = sinf(ang) * step;
    float dy = cosf(ang) * step;
    float dist = 0.0f;
    float x = ox;
    float y = oy;

    while (dist < maxd) {
        x += dx;
        y += dy;
        dist += step;

        int mx = (int)x;
        int my = (int)y;

        if (mx < 0 || mx >= MAP_W || my < 0 || my >= MAP_H) {
            return maxd;
        }

        if (cell_solid(mapa[my][mx])) {
            return dist;
        }
    }
    return maxd;
}

static int line_of_sight(GameState *g, float x1, float y1, float x2, float y2) {
    if (g == NULL || g->mapa == NULL) return 0;
    
    float dx = x2 - x1;
    float dy = y2 - y1;
    float dist = sqrtf(dx * dx + dy * dy);
    if (dist < 0.001f) return 1;

    float step = 0.05f;
    float vx = dx / dist * step;
    float vy = dy / dist * step;
    float x = x1;
    float y = y1;

    for (float d = 0.0f; d < dist; d += step) {
        x += vx;
        y += vy;
        if (is_solid_map(g->mapa, (int)x, (int)y)) return 0;
    }
    return 1;
}

static void try_move(GameState *g, float nx, float ny) {
    if (g == NULL || g->mapa == NULL) return;
    
    if (!is_solid_map(g->mapa, (int)nx, (int)g->player.y)) g->player.x = nx;
    if (!is_solid_map(g->mapa, (int)g->player.x, (int)ny)) g->player.y = ny;
}

static void open_near_door(GameState *g) {
    if (g == NULL || g->mapa == NULL) return;
    
    int px = (int)g->player.x;
    int py = (int)g->player.y;

    for (int y = py - 1; y <= py + 1; y++) {
        for (int x = px - 1; x <= px + 1; x++) {
            if (x < 0 || x >= MAP_W || y < 0 || y >= MAP_H) continue;
            if (g->mapa[y][x] == 'D') g->mapa[y][x] = 'd';
        }
    }
}

static void pickup_items(GameState *g) {
    if (g == NULL || g->mapa == NULL) return;
    
    int x = (int)g->player.x;
    int y = (int)g->player.y;
    
    if (x < 0 || x >= MAP_W || y < 0 || y >= MAP_H) return;
    
    char *cell = &g->mapa[y][x];

    if (*cell == 'A') {
        g->player.ammo += 8;
        *cell = '.';
    } else if (*cell == 'H') {
        g->player.hp += 25;
        if (g->player.hp > 100) g->player.hp = 100;
        *cell = '.';
    }
}

static void enemy_ai(GameState *g) {
    if (g == NULL || g->enemies == NULL || g->mapa == NULL) return;
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!g->enemies[i].alive) continue;

        // Actualizar animación
        g->enemies[i].animation_counter++;
        if (g->enemies[i].animation_counter > 10) {
            g->enemies[i].animation_frame = (g->enemies[i].animation_frame + 1) % 4;
            g->enemies[i].animation_counter = 0;
        }

        float dx = g->player.x - g->enemies[i].x;
        float dy = g->player.y - g->enemies[i].y;
        float dist = sqrtf(dx * dx + dy * dy);

        if (dist < 0.75f) {
            int damage = 1;
            if (g->player.armor > 0) {
                g->player.armor -= damage;
                if (g->player.armor < 0) {
                    g->player.hp += g->player.armor;
                    g->player.armor = 0;
                }
            } else {
                g->player.hp -= damage;
            }
            if (g->player.hp <= 0) {
                g->player.hp = 0;
                g->game_over = 1;
            }
        } else if (dist < 6.0f && line_of_sight(g, g->enemies[i].x, g->enemies[i].y, g->player.x, g->player.y)) {
            float vx = dx / dist * 0.02f;
            float vy = dy / dist * 0.02f;

            float nx = g->enemies[i].x + vx;
            float ny = g->enemies[i].y + vy;

            if (!is_solid_map(g->mapa, (int)nx, (int)g->enemies[i].y)) g->enemies[i].x = nx;
            if (!is_solid_map(g->mapa, (int)g->enemies[i].x, (int)ny)) g->enemies[i].y = ny;
        }
    }
}

static void shoot(GameState *g) {
    if (g == NULL || g->bullets == NULL) return;
    if (g->player.ammo <= 0) return;

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (g->bullets[i].alive) continue;

        float bx = g->player.x + sinf(g->player.a) * 0.25f;
        float by = g->player.y + cosf(g->player.a) * 0.25f;

        g->bullets[i].x = bx;
        g->bullets[i].y = by;
        g->bullets[i].px = bx;
        g->bullets[i].py = by;
        g->bullets[i].dx = sinf(g->player.a) * 0.18f;
        g->bullets[i].dy = cosf(g->player.a) * 0.18f;
        g->bullets[i].alive = 1;

        g->player.ammo--;
        break;
    }
}

static void update_bullets(GameState *g) {
    if (g == NULL || g->bullets == NULL || g->enemies == NULL || g->mapa == NULL) return;
    
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!g->bullets[i].alive) continue;

        g->bullets[i].px = g->bullets[i].x;
        g->bullets[i].py = g->bullets[i].y;

        g->bullets[i].x += g->bullets[i].dx;
        g->bullets[i].y += g->bullets[i].dy;

        int mx = (int)g->bullets[i].x;
        int my = (int)g->bullets[i].y;

        if (mx < 0 || mx >= MAP_W || my < 0 || my >= MAP_H || is_solid_map(g->mapa, mx, my)) {
            g->bullets[i].alive = 0;
            continue;
        }

        for (int e = 0; e < MAX_ENEMIES; e++) {
            if (!g->enemies[e].alive) continue;

            float ex = g->enemies[e].x - g->bullets[i].x;
            float ey = g->enemies[e].y - g->bullets[i].y;
            float dist = sqrtf(ex * ex + ey * ey);

            if (dist < 0.35f) {
                g->enemies[e].hp--;
                g->bullets[i].alive = 0;

                if (g->enemies[e].hp <= 0) {
                    g->enemies[e].alive = 0;
                    g->score += 100;
                }
                break;
            }
        }
    }
}

static void draw_minimap(GameState *g, uint32_t *pixels) {
    if (g == NULL || g->mapa == NULL || pixels == NULL) return;
    
    for (int my = 0; my < MAP_H; my++) {
        for (int mx = 0; mx < MAP_W; mx++) {
            uint32_t col = 0xFF303030;
            if (g->mapa[my][mx] == '#') col = 0xFFFFFFFF;
            else if (g->mapa[my][mx] == 'D') col = 0xFFAA7733;
            else if (g->mapa[my][mx] == 'A') col = 0xFF3366FF;
            else if (g->mapa[my][mx] == 'H') col = 0xFF33FF66;

            for (int yy = 0; yy < 3; yy++) {
                for (int xx = 0; xx < 3; xx++) {
                    put_pixel(mx * 3 + xx, my * 3 + yy, col, pixels);
                }
            }
        }
    }

    int px = (int)(g->player.x * 3.0f);
    int py = (int)(g->player.y * 3.0f);
    for (int yy = -1; yy <= 1; yy++) {
        for (int xx = -1; xx <= 1; xx++) {
            put_pixel(px + xx, py + yy, 0xFFFF0000, pixels);
        }
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!g->enemies[i].alive) continue;
        int ex = (int)(g->enemies[i].x * 3.0f);
        int ey = (int)(g->enemies[i].y * 3.0f);
        put_pixel(ex, ey, 0xFFFFAA00, pixels);
    }
}

static void draw_crosshair(uint32_t *pixels) {
    if (pixels == NULL) return;
    
    int cx = SCREEN_W / 2;
    int cy = SCREEN_H / 2;
    for (int i = -4; i <= 4; i++) {
        put_pixel(cx + i, cy, 0xFFFFFFFF, pixels);
        put_pixel(cx, cy + i, 0xFFFFFFFF, pixels);
    }
}

static void draw_bullets(GameState *g, uint32_t *pixels, float *zbuffer) {
    if (g == NULL || g->bullets == NULL || pixels == NULL || zbuffer == NULL) return;
    
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!g->bullets[i].alive) continue;

        float dx = g->bullets[i].x - g->player.x;
        float dy = g->bullets[i].y - g->player.y;
        float dist = sqrtf(dx * dx + dy * dy);
        if (dist < 0.01f) continue;

        float ang = normalize_angle(atan2f(dx, dy) - g->player.a);
        if (absf(ang) > FOV * 0.7f) continue;

        int sx = (int)((ang / (FOV * 0.5f)) * (SCREEN_W / 2.0f) + (SCREEN_W / 2.0f));
        int size = (int)(45.0f / dist);
        if (size < 2) size = 2;
        if (sx < 0 || sx >= SCREEN_W) continue;
        if (dist >= zbuffer[sx]) continue;

        int x0 = sx - size / 2;
        int x1 = sx + size / 2;
        int y0 = SCREEN_H / 2 - size / 2;
        int y1 = SCREEN_H / 2 + size / 2;

        for (int x = x0; x <= x1; x++) {
            if (x < 0 || x >= SCREEN_W) continue;
            if (dist >= zbuffer[x]) continue;

            for (int y = y0; y <= y1; y++) {
                if (y < 0 || y >= SCREEN_H) continue;
                put_pixel(x, y, 0xFFFFFF00, pixels);
            }
        }
    }
}

// Dibujar sprites de enemigos con animación Doom-like
static void draw_enemies(GameState *g, uint32_t *pixels, float *zbuffer) {
    if (g == NULL || g->enemies == NULL || pixels == NULL || zbuffer == NULL) return;
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!g->enemies[i].alive) continue;

        float dx = g->enemies[i].x - g->player.x;
        float dy = g->enemies[i].y - g->player.y;
        float dist = sqrtf(dx * dx + dy * dy);
        if (dist < 0.15f) continue;

        float ang = normalize_angle(atan2f(dx, dy) - g->player.a);
        if (absf(ang) > FOV * 0.7f) continue;

        int sx = (int)((ang / (FOV * 0.5f)) * (SCREEN_W / 2.0f) + (SCREEN_W / 2.0f));
        int size = (int)(SCREEN_H / dist);
        if (size < 2) size = 2;

        int x0 = sx - size / 2;
        int x1 = sx + size / 2;
        int y0 = SCREEN_H / 2 - size / 2;
        int y1 = SCREEN_H / 2 + size / 2;

        // Animación del sprite
        int frame = g->enemies[i].animation_frame;
        
        for (int x = x0; x <= x1; x++) {
            if (x < 0 || x >= SCREEN_W) continue;
            if (dist >= zbuffer[x]) continue;

            for (int y = y0; y <= y1; y++) {
                if (y < 0 || y >= SCREEN_H) continue;
                
                // Aplicar patrón de sprite animado
                uint32_t c;
                int px = x - x0;
                int py = y - y0;
                
                // Patrón de sprite básico simulando forma de demonio
                if (py < size / 3) {
                    // Cabeza
                    if ((px + py + frame) % 3 == 0) c = 0xFFFF0000;
                    else c = 0xFFCC0000;
                } else if (py < 2 * size / 3) {
                    // Cuerpo
                    if ((px - py + frame * 2) % 2 == 0) c = 0xFF880000;
                    else c = 0xFF660000;
                } else {
                    // Patas
                    if (px % 4 < 2) c = 0xFF440000;
                    else c = 0xFF220000;
                }
                
                put_pixel(x, y, c, pixels);
            }
        }
    }
}

// Dibujar barra de estado tipo Doom original
static void draw_hud(GameState *g, uint32_t *pixels) {
    if (g == NULL || pixels == NULL) return;
    
    // Fondo de HUD
    for (int y = SCREEN_H - HUD_HEIGHT; y < SCREEN_H; y++) {
        for (int x = 0; x < SCREEN_W; x++) {
            put_pixel(x, y, 0xFF1a1a1a, pixels);
        }
    }
    
    // Línea divisoria
    for (int x = 0; x < SCREEN_W; x++) {
        put_pixel(x, SCREEN_H - HUD_HEIGHT, 0xFFFFAA00, pixels);
    }
    
    // Barra de salud
    int hp_bar_width = 40;
    int hp_bar_x = 10;
    int hp_bar_y = SCREEN_H - 30;
    
    // Marco de barra de salud
    for (int x = hp_bar_x - 1; x <= hp_bar_x + hp_bar_width + 1; x++) {
        put_pixel(x, hp_bar_y - 1, 0xFFFFFFFF, pixels);
        put_pixel(x, hp_bar_y + 8, 0xFFFFFFFF, pixels);
    }
    for (int y = hp_bar_y; y < hp_bar_y + 8; y++) {
        put_pixel(hp_bar_x - 1, y, 0xFFFFFFFF, pixels);
        put_pixel(hp_bar_x + hp_bar_width + 1, y, 0xFFFFFFFF, pixels);
    }
    
    // Relleno de barra de salud
    int hp_fill = (g->player.hp * hp_bar_width) / 100;
    uint32_t hp_color = g->player.hp > 50 ? 0xFF00FF00 : (g->player.hp > 25 ? 0xFFFFAA00 : 0xFFFF0000);
    for (int x = hp_bar_x; x < hp_bar_x + hp_fill; x++) {
        for (int y = hp_bar_y; y < hp_bar_y + 8; y++) {
            put_pixel(x, y, hp_color, pixels);
        }
    }
    
    // Barra de armadura
    int armor_bar_x = 65;
    int armor_bar_width = 40;
    int armor_bar_y = SCREEN_H - 30;
    
    // Marco de barra de armadura
    for (int x = armor_bar_x - 1; x <= armor_bar_x + armor_bar_width + 1; x++) {
        put_pixel(x, armor_bar_y - 1, 0xFFFFFFFF, pixels);
        put_pixel(x, armor_bar_y + 8, 0xFFFFFFFF, pixels);
    }
    for (int y = armor_bar_y; y < armor_bar_y + 8; y++) {
        put_pixel(armor_bar_x - 1, y, 0xFFFFFFFF, pixels);
        put_pixel(armor_bar_x + armor_bar_width + 1, y, 0xFFFFFFFF, pixels);
    }
    
    // Relleno de barra de armadura
    int armor_fill = (g->player.armor * armor_bar_width) / 100;
    for (int x = armor_bar_x; x < armor_bar_x + armor_fill; x++) {
        for (int y = armor_bar_y; y < armor_bar_y + 8; y++) {
            put_pixel(x, y, 0xFF0099FF, pixels);
        }
    }
}

API void game_reset(GameState *g) {
    if (g == NULL) return;
    
    memset(g, 0, sizeof(*g));

    for (int y = 0; y < MAP_H; y++) {
        strncpy(g->mapa[y], initial_map[y], MAP_W);
        g->mapa[y][MAP_W] = '\0';
    }

    g->player.x = 8.0f;
    g->player.y = 8.0f;
    g->player.a = 0.0f;
    g->player.hp = 100;
    g->player.ammo = 24;
    g->player.armor = 50;

    g->enemies[0] = (Enemy){ 4.5f, 2.5f, 3, 1, 0, 0 };
    g->enemies[1] = (Enemy){ 12.5f, 4.5f, 3, 1, 0, 0 };
    g->enemies[2] = (Enemy){ 10.5f, 10.5f, 3, 1, 0, 0 };
    g->enemies[3] = (Enemy){ 3.5f, 12.5f, 3, 1, 0, 0 };
    g->enemies[4] = (Enemy){ 13.0f, 13.0f, 3, 1, 0, 0 };
    g->enemies[5] = (Enemy){ 7.5f, 2.5f, 3, 1, 0, 0 };

    g->score = 0;
    g->game_over = 0;
    g->mouse_locked = 1;
    g->ticks = 0;
}

API void game_tick(GameState *g, const InputState *in) {
    if (g == NULL || in == NULL) return;
    
    g->ticks++;
    
    if (g->game_over) {
        if (in->r) {
            game_reset(g);
        }
        return;
    }

    g->player.a += in->mouse_dx * 0.0035f;
    g->player.a = normalize_angle(g->player.a);

    float moveSpeed = 0.05f;
    float nx = g->player.x;
    float ny = g->player.y;

    if (in->w) {
        nx += sinf(g->player.a) * moveSpeed;
        ny += cosf(g->player.a) * moveSpeed;
    }
    if (in->s) {
        nx -= sinf(g->player.a) * moveSpeed;
        ny -= cosf(g->player.a) * moveSpeed;
    }
    if (in->a) {
        nx -= cosf(g->player.a) * moveSpeed;
        ny += sinf(g->player.a) * moveSpeed;
    }
    if (in->d) {
        nx += cosf(g->player.a) * moveSpeed;
        ny -= sinf(g->player.a) * moveSpeed;
    }

    try_move(g, nx, ny);

    if (in->space) shoot(g);
    if (in->e) open_near_door(g);

    update_bullets(g);
    pickup_items(g);
    enemy_ai(g);

    if (g->player.hp <= 0) {
        g->player.hp = 0;
        g->game_over = 1;
    }
}

API void game_render(GameState *g, uint32_t *pixels, float *zbuffer) {
    if (g == NULL || pixels == NULL || zbuffer == NULL) return;
    
    // Cielo y suelo
    for (int y = 0; y < SCREEN_H; y++) {
        for (int x = 0; x < SCREEN_W; x++) {
            if (y < SCREEN_H / 2) pixels[y * SCREEN_W + x] = 0xFF1E1E3A;
            else pixels[y * SCREEN_W + x] = 0xFF383822;
        }
    }

    // Renderizado de raycasting
    for (int x = 0; x < SCREEN_W; x++) {
        float cam = (2.0f * x / (float)SCREEN_W - 1.0f);
        float rayAngle = g->player.a + cam * (FOV * 0.5f);

        float dist = raycast_distance(g->mapa, g->player.x, g->player.y, rayAngle, 20.0f);
        if (dist < 0.001f) dist = 0.001f;
        zbuffer[x] = dist;

        int wallH = (int)(SCREEN_H / dist);
        int drawStart = SCREEN_H / 2 - wallH / 2;
        int drawEnd = SCREEN_H / 2 + wallH / 2;

        if (drawStart < 0) drawStart = 0;
        if (drawEnd >= SCREEN_H) drawEnd = SCREEN_H - 1;

        uint32_t shade = 255 - (uint32_t)(dist * 16.0f);
        if (shade < 25) shade = 25;
        uint32_t color = 0xFF000000 | (shade << 16) | (shade << 8) | shade;

        for (int y = drawStart; y <= drawEnd; y++) {
            put_pixel(x, y, color, pixels);
        }
    }

    // Renderizar objetos
    draw_enemies(g, pixels, zbuffer);
    draw_bullets(g, pixels, zbuffer);
    draw_minimap(g, pixels);
    draw_crosshair(pixels);
    
    // Dibujar HUD mejorado
    draw_hud(g, pixels);

    if (g->game_over) {
        for (int y = 55; y < 145; y++) {
            for (int x = 55; x < 265; x++) {
                if (x == 55 || x == 264 || y == 55 || y == 144) {
                    put_pixel(x, y, 0xFFFFFFFF, pixels);
                } else {
                    put_pixel(x, y, 0xCC000000, pixels);
                }
            }
        }
    }
}
