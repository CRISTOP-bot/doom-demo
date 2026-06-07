#include "doom_graphics.h"
#include "opengl_render.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define PI 3.14159265f
#define MAP_SCALE 1.0f
#define FOV_RADIANS (60.0f * PI / 180.0f)

/* ==================== TEXTURAS PROCEDURALES ==================== */

void doom_generate_stone_texture(void) {
    /* Textura de piedra: patrones gris oscuro con variacion */
    glColor3f(0.5f, 0.5f, 0.5f);
    
    int x, y;
    for (y = 0; y < 64; y++) {
        for (x = 0; x < 64; x++) {
            float noise = (float)((x * 7 + y * 11) % 256) / 256.0f;
            float shade = 0.4f + (noise * 0.3f);
            
            if ((x % 8 == 0) || (y % 8 == 0)) {
                shade *= 0.7f;  /* Grietas */
            }
            
            glColor3f(shade, shade, shade);
            glBegin(GL_POINTS);
            glVertex2f((float)x, (float)y);
            glEnd();
        }
    }
}

void doom_generate_brick_texture(void) {
    /* Textura de ladrillo: patron rectangular rojo */
    int x, y;
    for (y = 0; y < 64; y++) {
        for (x = 0; x < 64; x++) {
            float brick_x = (float)(x / 16);
            float brick_y = (float)(y / 8);
            
            float r = 0.6f + (brick_x * 0.1f);
            float g = 0.2f;
            float b = 0.1f;
            
            /* Lineas de mortero */
            if ((x % 16 == 0) || (y % 8 == 0)) {
                r *= 0.5f;
                g *= 0.5f;
                b *= 0.5f;
            }
            
            glColor3f(r, g, b);
            glBegin(GL_POINTS);
            glVertex2f((float)x, (float)y);
            glEnd();
        }
    }
}

void doom_generate_metal_texture(void) {
    /* Textura metalica: plateado con rayaduras */
    int x, y;
    for (y = 0; y < 64; y++) {
        for (x = 0; x < 64; x++) {
            float base = 0.7f;
            
            /* Rayaduras horizontales */
            if ((y % 4) == 0) {
                base = 0.6f;
            }
            
            /* Puntos oxidados */
            if (((x + y) % 13) == 0) {
                base = 0.5f;
            }
            
            glColor3f(base, base, base * 0.95f);
            glBegin(GL_POINTS);
            glVertex2f((float)x, (float)y);
            glEnd();
        }
    }
}

void doom_generate_blood_texture(void) {
    /* Textura de sangre: rojo oscuro con salpicaduras */
    int x, y;
    for (y = 0; y < 64; y++) {
        for (x = 0; x < 64; x++) {
            int hash = (x * 73 + y * 31) % 256;
            float intensity = (float)hash / 256.0f;
            
            float r = 0.4f + (intensity * 0.3f);
            float g = 0.0f + (intensity * 0.1f);
            float b = 0.0f + (intensity * 0.05f);
            
            glColor3f(r, g, b);
            glBegin(GL_POINTS);
            glVertex2f((float)x, (float)y);
            glEnd();
        }
    }
}

void doom_generate_lava_texture(void) {
    /* Textura de lava: naranja y rojo oscilante */
    int x, y;
    for (y = 0; y < 64; y++) {
        for (x = 0; x < 64; x++) {
            float wave = sinf((x + y) * 0.1f) * 0.5f + 0.5f;
            
            float r = 0.8f + (wave * 0.2f);
            float g = 0.3f + (wave * 0.2f);
            float b = 0.0f;
            
            glColor3f(r, g, b);
            glBegin(GL_POINTS);
            glVertex2f((float)x, (float)y);
            glEnd();
        }
    }
}

void doom_generate_skull_texture(void) {
    /* Textura con calavera: patron blanco sobre fondo oscuro */
    int x, y;
    for (y = 0; y < 64; y++) {
        for (x = 0; x < 64; x++) {
            /* Crear patron de calavera basico */
            int dx = x - 32;
            int dy = y - 32;
            int dist = dx * dx + dy * dy;
            
            float r, g, b;
            
            if (dist < 200) {
                /* Ojos */
                if ((dist > 50 && dist < 100) && ((dx > 5 && dx < 10) || (dx < -5 && dx > -10))) {
                    r = g = b = 0.0f;
                } else {
                    r = g = b = 0.9f;
                }
            } else {
                r = g = b = 0.1f;
            }
            
            glColor3f(r, g, b);
            glBegin(GL_POINTS);
            glVertex2f((float)x, (float)y);
            glEnd();
        }
    }
}

/* ==================== RENDERIZADO 3D ==================== */

void doom_setup_3d_view(float player_x, float player_y, float player_angle) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    /* Camara en la posicion del jugador */
    float cam_x = player_x;
    float cam_y = player_y;
    float cam_z = 0.5f;  /* Altura del ojo */
    
    /* Punto hacia donde mira */
    float look_x = cam_x + cosf(player_angle);
    float look_y = cam_y + sinf(player_angle);
    float look_z = cam_z;
    
    /* Vector "arriba" */
    gluLookAt(
        cam_x, cam_z, cam_y,           /* Posicion camara */
        look_x, look_z, look_y,         /* Punto mirada */
        0.0f, 1.0f, 0.0f                /* Vector arriba */
    );
}

void doom_render_walls(GameState *game) {
    if (!game) return;
    
    int x, y;
    float wall_height = 2.0f;
    
    /* Renderizar las paredes del mapa */
    for (y = 0; y < MAP_H; y++) {
        for (x = 0; x < MAP_W; x++) {
            char cell = game->mapa[y][x];
            
            /* '#' representa una pared */
            if (cell == '#') {
                float px = (float)x * MAP_SCALE;
                float py = (float)y * MAP_SCALE;
                
                /* Determinar tipo de textura basado en posicion */
                TextureType texture = TEXTURE_STONE;
                if ((x + y) % 3 == 0) texture = TEXTURE_BRICK;
                if ((x + y) % 5 == 0) texture = TEXTURE_METAL;
                
                /* Dibujar cubo de pared */
                opengl_push_matrix();
                opengl_translate(px + 0.5f, wall_height / 2.0f, py + 0.5f);
                
                /* Aplicar color basado en textura */
                if (texture == TEXTURE_BRICK) {
                    opengl_draw_cube(0, 0, 0, 1.0f, 0.6f, 0.2f, 0.1f);
                } else if (texture == TEXTURE_METAL) {
                    opengl_draw_cube(0, 0, 0, 1.0f, 0.7f, 0.7f, 0.7f);
                } else {
                    opengl_draw_cube(0, 0, 0, 1.0f, 0.5f, 0.5f, 0.5f);
                }
                
                opengl_pop_matrix();
            }
        }
    }
}

void doom_render_floor_ceiling(GameState *game) {
    if (!game) return;
    
    float floor_level = 0.0f;
    float ceil_level = 2.0f;
    float tile_size = MAP_SCALE;
    
    int x, y;
    
    /* Piso */
    for (y = 0; y < MAP_H; y++) {
        for (x = 0; x < MAP_W; x++) {
            char cell = game->mapa[y][x];
            
            if (cell != '#') {  /* Espacio vacio */
                float px = (float)x * tile_size;
                float py = (float)y * tile_size;
                
                /* Textura alternada del piso */
                TextureType texture = ((x + y) % 2 == 0) ? TEXTURE_STONE : TEXTURE_BRICK;
                
                if (texture == TEXTURE_BRICK) {
                    glColor3f(0.4f, 0.2f, 0.1f);
                } else {
                    glColor3f(0.3f, 0.3f, 0.3f);
                }
            }
        }
    }
    
    /* Techo */
    for (y = 0; y < MAP_H; y++) {
        for (x = 0; x < MAP_W; x++) {
            char cell = game->mapa[y][x];
            
            if (cell != '#') {  /* Espacio vacio */
                glColor3f(0.2f, 0.2f, 0.2f);
            }
        }
    }
}

void doom_render_sprites(GameState *game) {
    if (!game) return;
    
    /* Renderizar enemigos */
    int i;
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (game->enemies[i].alive) {
            Enemy *enemy = &game->enemies[i];
            
            /* Dibujar enemigo como piramide (simple representacion demon) */
            float health_ratio = (float)enemy->hp / 100.0f;
            float r = (1.0f - health_ratio);
            float g = health_ratio * 0.5f;
            float b = 0.0f;
            
            opengl_push_matrix();
            opengl_translate(enemy->x, 0.3f, enemy->y);
            opengl_draw_pyramid(0, 0, 0, 0.4f, r, g, b);
            opengl_pop_matrix();
        }
    }
    
    /* Renderizar balas */
    for (i = 0; i < MAX_BULLETS; i++) {
        if (game->bullets[i].alive) {
            Bullet *bullet = &game->bullets[i];
            
            /* Dibujar bala como esfera pequena */
            opengl_push_matrix();
            opengl_translate(bullet->x, 0.2f, bullet->y);
            opengl_draw_sphere(0, 0, 0, 0.1f, 1.0f, 1.0f, 0.0f);
            opengl_pop_matrix();
        }
    }
}

void doom_render_3d_scene(GameState *game) {
    if (!game) return;
    
    /* Configurar vista 3D */
    doom_setup_3d_view(game->player.x, game->player.y, game->player.a);
    
    /* Fondo */
    opengl_clear_screen(0.0f, 0.0f, 0.0f, 1.0f);
    
    /* Renderizar componentes */
    doom_render_floor_ceiling(game);
    doom_render_walls(game);
    doom_render_sprites(game);
    
    /* Aplicar niebla/oscuridad en distancia */
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, 5.0f);
    glFogf(GL_FOG_END, 15.0f);
    glFogfv(GL_FOG_COLOR, (GLfloat[]){ 0.0f, 0.0f, 0.0f, 1.0f });
}

/* ==================== EFECTOS VISUALES ==================== */

void doom_apply_dynamic_light(float x, float y, float z, float radius, float intensity) {
    GLfloat position[] = { x, z, y, 1.0f };
    GLfloat ambient[] = { intensity * 0.3f, intensity * 0.3f, intensity * 0.3f, 1.0f };
    GLfloat diffuse[] = { intensity, intensity, intensity, 1.0f };
    
    glLightfv(GL_LIGHT1, GL_POSITION, position);
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
    glEnable(GL_LIGHT1);
}

void doom_apply_muzzle_flash(float x, float y, float intensity) {
    /* Flash de disparo */
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 320, 200, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glDisable(GL_LIGHTING);
    
    opengl_set_color(1.0f, 1.0f, 0.5f, intensity);
    
    glEnable(GL_LIGHTING);
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void doom_draw_blood_pool(float x, float y, int size) {
    opengl_set_color(0.5f, 0.0f, 0.0f, 0.7f);
    opengl_enable_blend();
    
    opengl_push_matrix();
    opengl_translate(x, 0.01f, y);
    opengl_draw_quad(
        -size, 0, -size,
        size, 0, -size,
        size, 0, size,
        -size, 0, size,
        0.5f, 0.0f, 0.0f
    );
    opengl_pop_matrix();
    
    opengl_disable_blend();
}

void doom_draw_corpse(float x, float y) {
    opengl_set_color(0.3f, 0.0f, 0.0f, 1.0f);
    opengl_push_matrix();
    opengl_translate(x, 0.1f, y);
    opengl_draw_cube(0, 0, 0, 0.3f, 0.3f, 0.0f, 0.0f);
    opengl_pop_matrix();
}

void doom_draw_explosion(float x, float y, float z, int frame) {
    /* Explosion: esfera que crece */
    float scale = 0.1f + (frame * 0.05f);
    float intensity = 1.0f - (frame * 0.1f);
    
    if (intensity < 0.0f) intensity = 0.0f;
    
    opengl_set_color(1.0f, 0.5f, 0.0f, intensity);
    opengl_push_matrix();
    opengl_translate(x, z, y);
    opengl_draw_sphere(0, 0, 0, scale, 1.0f, 0.5f, 0.0f);
    opengl_pop_matrix();
}

void doom_draw_particle_effect(float x, float y, float z, int type, int intensity) {
    /* Efectos de particulas simples */
    int i;
    
    for (i = 0; i < intensity; i++) {
        float angle = (float)(i * 360 / intensity) * PI / 180.0f;
        float px = x + cosf(angle) * 0.3f;
        float py = y + 0.2f;
        float pz = z + sinf(angle) * 0.3f;
        
        opengl_push_matrix();
        opengl_translate(px, py, pz);
        opengl_draw_sphere(0, 0, 0, 0.05f, 1.0f, 0.5f, 0.0f);
        opengl_pop_matrix();
    }
}

/* ==================== HUD ==================== */

void doom_draw_hud(GameState *game) {
    if (!game) return;
    
    /* Cambiar a modo 2D */
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 320, 200, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    
    /* Barra de salud */
    doom_draw_health_bar(game->player.hp, game->player.armor);
    
    /* Contador de municion */
    doom_draw_ammo_counter(game->player.ammo);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void doom_draw_weapon(GameState *game, int frame) {
    if (!game) return;
    /* Renderizar arma en pantalla */
}

void doom_draw_ammo_counter(int ammo) {
    char ammo_text[16];
    sprintf(ammo_text, "AMMO: %d", ammo);
    opengl_set_color(1.0f, 1.0f, 0.0f, 1.0f);
    opengl_draw_text_2d(250.0f, 10.0f, ammo_text);
}

void doom_draw_health_bar(int hp, int armor) {
    /* Barra de vida roja */
    float health_ratio = (float)hp / 100.0f;
    float bar_width = 150.0f;
    
    opengl_set_color(0.3f, 0.3f, 0.3f, 1.0f);
    opengl_draw_quad(10, 10, 10 + bar_width, 10, 10 + bar_width, 25, 10, 25, 
                     0.3f, 0.3f, 0.3f);
    
    opengl_set_color(1.0f - health_ratio, health_ratio, 0.0f, 1.0f);
    opengl_draw_quad(10, 10, 10 + (bar_width * health_ratio), 10, 
                     10 + (bar_width * health_ratio), 25, 10, 25,
                     1.0f - health_ratio, health_ratio, 0.0f);
}

void doom_draw_minimap(GameState *game) {
    if (!game) return;
    /* Renderizar mini mapa */
}

void doom_draw_face_status(int hp) {
    /* Dibujar cara indicadora de salud */
    float base_color = (float)hp / 100.0f;
    opengl_set_color(base_color, base_color * 0.5f, 0.0f, 1.0f);
}

/* ==================== UTILIDADES ==================== */

Material doom_create_material(float r, float g, float b, TextureType type) {
    Material mat;
    mat.r = r;
    mat.g = g;
    mat.b = b;
    mat.type = type;
    mat.roughness = 0.5f;
    mat.has_pattern = 1;
    return mat;
}

void doom_apply_wall_shadow(float distance) {
    /* Aplicar sombra basada en distancia */
    float shadow_intensity = 1.0f - (distance * 0.05f);
    if (shadow_intensity < 0.0f) shadow_intensity = 0.0f;
    glColor4f(0.0f, 0.0f, 0.0f, 1.0f - shadow_intensity);
}

void doom_apply_fog_effect(float distance) {
    /* Niebla roja/negra en distancia */
    float fog_intensity = distance * 0.01f;
    if (fog_intensity > 1.0f) fog_intensity = 1.0f;
    
    glEnable(GL_FOG);
    GLfloat fog_color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glFogfv(GL_FOG_COLOR, fog_color);
    glFogf(GL_FOG_DENSITY, fog_intensity);
}

void doom_draw_textured_wall(float x1, float y1, float x2, float y2, 
                             float h1, float h2, TextureType texture) {
    /* Dibujar pared con textura */
    float r, g, b;
    
    switch (texture) {
        case TEXTURE_BRICK:
            r = 0.6f; g = 0.2f; b = 0.1f;
            break;
        case TEXTURE_METAL:
            r = 0.7f; g = 0.7f; b = 0.7f;
            break;
        case TEXTURE_BLOOD:
            r = 0.5f; g = 0.0f; b = 0.0f;
            break;
        case TEXTURE_SKULL:
            r = 0.9f; g = 0.9f; b = 0.9f;
            break;
        default:
            r = 0.5f; g = 0.5f; b = 0.5f;
    }
    
    opengl_set_color(r, g, b, 1.0f);
}

void doom_draw_floor_tile(float x, float y, float size, TextureType texture) {
    /* Dibujar un tile del piso */
    float r, g, b;
    
    if (texture == TEXTURE_BRICK) {
        r = 0.4f; g = 0.2f; b = 0.1f;
    } else {
        r = 0.3f; g = 0.3f; b = 0.3f;
    }
    
    opengl_set_color(r, g, b, 1.0f);
}

void doom_draw_ceiling_tile(float x, float y, float size, TextureType texture) {
    /* Dibujar un tile del techo */
    opengl_set_color(0.15f, 0.15f, 0.15f, 1.0f);
}

void doom_apply_sector_lighting(Sector *sector, float ambient) {
    if (!sector) return;
    /* Aplicar iluminacion del sector */
}

void doom_draw_demon_sprite(float x, float y, float z, int animation_frame) {
    /* Dibujar sprite de demon animado */
    opengl_push_matrix();
    opengl_translate(x, z, y);
    opengl_draw_pyramid(0, 0, 0, 0.4f, 1.0f, 0.0f, 0.0f);
    opengl_pop_matrix();
}

void doom_draw_item_sprite(float x, float y, int item_type) {
    /* Dibujar sprite de item */
    opengl_push_matrix();
    opengl_translate(x, 0.5f, y);
    opengl_draw_sphere(0, 0, 0, 0.2f, 1.0f, 0.8f, 0.0f);
    opengl_pop_matrix();
}

void doom_draw_blood_splatter(float x, float y, int intensity) {
    /* Dibujar salpicaduras de sangre */
    int i;
    for (i = 0; i < intensity; i++) {
        float angle = (float)(i * 360 / intensity) * 3.14159f / 180.0f;
        float px = x + cosf(angle) * 0.5f;
        float py = y + sinf(angle) * 0.5f;
        doom_draw_blood_pool(px, py, 1);
    }
}

void doom_apply_torch_light(float x, float y, float z, float radius) {
    /* Aplicar luz de antorcha */
    doom_apply_dynamic_light(x, y, z, radius, 0.8f);
}
