#include "opengl_render.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Variables globales para OpenGL */
static OpenGLContext *g_ctx = NULL;

/* Prototipos internos */
static void setup_pixel_format(HDC hdc);
static void setup_initial_gl_settings(void);

/* ==================== Inicializacion ==================== */

static void setup_pixel_format(HDC hdc) {
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32,         /* 32-bit color */
        0, 0, 0, 0, 0, 0, /* color bits ignored */
        0,          /* no alpha buffer */
        0,          /* shift bit ignored */
        0,          /* no accumulation buffer */
        0, 0, 0, 0, /* accumulation bits ignored */
        24,         /* 24-bit depth buffer */
        8,          /* 8-bit stencil buffer */
        0,          /* no auxiliary buffer */
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };

    int pixel_format = ChoosePixelFormat(hdc, &pfd);
    if (!pixel_format) {
        MessageBoxA(NULL, "No se pudo seleccionar formato de pixel", "Error OpenGL", MB_ICONERROR);
        return;
    }

    if (!SetPixelFormat(hdc, pixel_format, &pfd)) {
        MessageBoxA(NULL, "No se pudo establecer formato de pixel", "Error OpenGL", MB_ICONERROR);
        return;
    }
}

static void setup_initial_gl_settings(void) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

int opengl_init(HWND hwnd, OpenGLContext *ctx) {
    g_ctx = ctx;
    ctx->hwnd = hwnd;
    ctx->hdc = GetDC(hwnd);

    if (!ctx->hdc) {
        MessageBoxA(NULL, "No se pudo obtener el Device Context", "Error OpenGL", MB_ICONERROR);
        return 0;
    }

    setup_pixel_format(ctx->hdc);

    ctx->hglrc = wglCreateContext(ctx->hdc);
    if (!ctx->hglrc) {
        MessageBoxA(NULL, "No se pudo crear el contexto OpenGL", "Error OpenGL", MB_ICONERROR);
        ReleaseDC(hwnd, ctx->hdc);
        return 0;
    }

    if (!wglMakeCurrent(ctx->hdc, ctx->hglrc)) {
        MessageBoxA(NULL, "No se pudo activar el contexto OpenGL", "Error OpenGL", MB_ICONERROR);
        wglDeleteContext(ctx->hglrc);
        ReleaseDC(hwnd, ctx->hdc);
        return 0;
    }

    /* Obtener informacion de OpenGL */
    const char *vendor = (const char *)glGetString(GL_VENDOR);
    const char *renderer = (const char *)glGetString(GL_RENDERER);
    const char *version = (const char *)glGetString(GL_VERSION);
    
    printf("[OpenGL] Vendor: %s\n", vendor ? vendor : "Unknown");
    printf("[OpenGL] Renderer: %s\n", renderer ? renderer : "Unknown");
    printf("[OpenGL] Version: %s\n", version ? version : "Unknown");

    setup_initial_gl_settings();

    /* Calcular aspect ratio */
    RECT rc;
    GetClientRect(hwnd, &rc);
    ctx->aspect_ratio = (float)(rc.right - rc.left) / (float)(rc.bottom - rc.top);

    opengl_set_projection(GL_SCREEN_W, GL_SCREEN_H);
    
    ctx->initialized = 1;
    return 1;
}

void opengl_cleanup(OpenGLContext *ctx) {
    if (!ctx) return;

    if (ctx->hglrc) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(ctx->hglrc);
    }

    if (ctx->hdc) {
        ReleaseDC(ctx->hwnd, ctx->hdc);
    }

    ctx->initialized = 0;
}

/* ==================== Renderizado Basico ==================== */

void opengl_begin_frame(void) {
    if (!g_ctx || !g_ctx->initialized) return;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void opengl_end_frame(OpenGLContext *ctx) {
    if (!ctx || !ctx->initialized) return;
    SwapBuffers(ctx->hdc);
}

void opengl_clear_screen(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void opengl_set_projection(float width, float height) {
    if (!g_ctx || !g_ctx->initialized) return;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    float aspect = width / height;
    gluPerspective(FOV_ANGLE, aspect, NEAR_PLANE, FAR_PLANE);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void opengl_set_viewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}

/* ==================== Dibujado de Primitivas ==================== */

void opengl_draw_cube(float x, float y, float z, float size, float r, float g, float b) {
    if (!g_ctx || !g_ctx->initialized) return;

    opengl_push_matrix();
    opengl_translate(x, y, z);
    opengl_set_color(r, g, b, 1.0f);

    float s = size / 2.0f;

    glBegin(GL_QUADS);
    
    /* Frente */
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-s, -s, s);
    glVertex3f(s, -s, s);
    glVertex3f(s, s, s);
    glVertex3f(-s, s, s);

    /* Atras */
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-s, -s, -s);
    glVertex3f(-s, s, -s);
    glVertex3f(s, s, -s);
    glVertex3f(s, -s, -s);

    /* Arriba */
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-s, s, -s);
    glVertex3f(-s, s, s);
    glVertex3f(s, s, s);
    glVertex3f(s, s, -s);

    /* Abajo */
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-s, -s, -s);
    glVertex3f(s, -s, -s);
    glVertex3f(s, -s, s);
    glVertex3f(-s, -s, s);

    /* Derecha */
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(s, -s, -s);
    glVertex3f(s, s, -s);
    glVertex3f(s, s, s);
    glVertex3f(s, -s, s);

    /* Izquierda */
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-s, -s, -s);
    glVertex3f(-s, -s, s);
    glVertex3f(-s, s, s);
    glVertex3f(-s, s, -s);

    glEnd();

    opengl_pop_matrix();
}

void opengl_draw_pyramid(float x, float y, float z, float size, float r, float g, float b) {
    if (!g_ctx || !g_ctx->initialized) return;

    opengl_push_matrix();
    opengl_translate(x, y, z);
    opengl_set_color(r, g, b, 1.0f);

    float s = size / 2.0f;
    float h = size;

    glBegin(GL_TRIANGLES);
    
    /* Frente */
    glNormal3f(0.0f, 1.0f, 1.0f);
    glVertex3f(0.0f, h, 0.0f);
    glVertex3f(-s, 0.0f, s);
    glVertex3f(s, 0.0f, s);

    /* Atras */
    glNormal3f(0.0f, 1.0f, -1.0f);
    glVertex3f(0.0f, h, 0.0f);
    glVertex3f(s, 0.0f, -s);
    glVertex3f(-s, 0.0f, -s);

    /* Derecha */
    glNormal3f(1.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, h, 0.0f);
    glVertex3f(s, 0.0f, s);
    glVertex3f(s, 0.0f, -s);

    /* Izquierda */
    glNormal3f(-1.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, h, 0.0f);
    glVertex3f(-s, 0.0f, -s);
    glVertex3f(-s, 0.0f, s);

    glEnd();

    /* Base */
    glBegin(GL_QUADS);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-s, 0.0f, -s);
    glVertex3f(s, 0.0f, -s);
    glVertex3f(s, 0.0f, s);
    glVertex3f(-s, 0.0f, s);
    glEnd();

    opengl_pop_matrix();
}

void opengl_draw_sphere(float x, float y, float z, float radius, float r, float g, float b) {
    if (!g_ctx || !g_ctx->initialized) return;

    opengl_push_matrix();
    opengl_translate(x, y, z);
    opengl_set_color(r, g, b, 1.0f);

    /* Usar la funcion de utilidad de GLU */
    GLUquadric *quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);
    gluSphere(quad, radius, 32, 32);

    opengl_pop_matrix();
}

void opengl_draw_quad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, 
                      float r, float g, float b) {
    if (!g_ctx || !g_ctx->initialized) return;

    opengl_set_color(r, g, b, 1.0f);

    glBegin(GL_QUADS);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glVertex2f(x4, y4);
    glEnd();
}

void opengl_draw_triangle(float x1, float y1, float x2, float y2, float x3, float y3, 
                          float r, float g, float b) {
    if (!g_ctx || !g_ctx->initialized) return;

    opengl_set_color(r, g, b, 1.0f);

    glBegin(GL_TRIANGLES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glEnd();
}

void opengl_draw_line(float x1, float y1, float x2, float y2, float width, float r, float g, float b) {
    if (!g_ctx || !g_ctx->initialized) return;

    glLineWidth(width);
    opengl_set_color(r, g, b, 1.0f);

    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();

    glLineWidth(1.0f);
}

/* ==================== Renderizado de Game Entities ==================== */

void opengl_draw_player(float x, float y, float angle) {
    if (!g_ctx || !g_ctx->initialized) return;

    opengl_push_matrix();
    opengl_translate(x, y, 0.0f);
    opengl_rotate(angle * 180.0f / 3.14159f, 0.0f, 0.0f, 1.0f);
    
    /* Dibujar un cubo como representacion del jugador */
    opengl_draw_cube(0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.5f, 1.0f);

    opengl_pop_matrix();
}

void opengl_draw_enemy(float x, float y, int hp) {
    if (!g_ctx || !g_ctx->initialized) return;

    /* Color basado en HP */
    float r = (100 - hp) / 100.0f;
    float g = hp / 100.0f;
    float b = 0.0f;

    opengl_draw_pyramid(x, y, 0.0f, 0.5f, r, g, b);
}

void opengl_draw_bullet(float x, float y, float vx, float vy) {
    if (!g_ctx || !g_ctx->initialized) return;

    opengl_draw_sphere(x, y, 0.0f, 0.1f, 1.0f, 1.0f, 0.0f);
}

void opengl_draw_map_wall(int map_x, int map_y, float wall_height) {
    if (!g_ctx || !g_ctx->initialized) return;

    float x = (float)map_x;
    float y = (float)map_y;
    float z = wall_height / 2.0f;

    opengl_draw_cube(x, y, z, 1.0f, 0.5f, 0.5f, 0.5f);
}

void opengl_draw_hud(GameState *game) {
    if (!g_ctx || !g_ctx->initialized) return;
    if (!game) return;

    /* Cambiar a modo 2D para el HUD */
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, GL_SCREEN_W, GL_SCREEN_H, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    /* Dibujar barra de salud */
    float health_bar_width = 200.0f;
    float health_ratio = (float)game->player.hp / 100.0f;

    opengl_draw_quad(10, 10, 10 + health_bar_width, 10, 
                     10 + health_bar_width, 30, 10, 30, 
                     0.2f, 0.2f, 0.2f);
    opengl_draw_quad(10, 10, 10 + (health_bar_width * health_ratio), 10,
                     10 + (health_bar_width * health_ratio), 30, 10, 30,
                     0.0f, 1.0f, 0.0f);

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

/* ==================== Iluminacion ==================== */

void opengl_setup_lighting(void) {
    if (!g_ctx || !g_ctx->initialized) return;

    GLfloat ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat position[] = { 5.0f, 5.0f, 5.0f, 0.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
}

void opengl_set_ambient_light(float r, float g, float b) {
    if (!g_ctx || !g_ctx->initialized) return;

    GLfloat ambient[] = { r, g, b, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
}

void opengl_add_light(Light *light) {
    if (!g_ctx || !g_ctx->initialized || !light) return;

    /* Implementacion de luz adicional */
    GLfloat position[] = { light->x, light->y, light->z, 1.0f };
    GLfloat ambient[] = { light->r * 0.2f, light->g * 0.2f, light->b * 0.2f, 1.0f };
    GLfloat diffuse[] = { light->r, light->g, light->b, 1.0f };

    glLightfv(GL_LIGHT1, GL_POSITION, position);
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
}

/* ==================== Transformaciones ==================== */

void opengl_push_matrix(void) {
    glPushMatrix();
}

void opengl_pop_matrix(void) {
    glPopMatrix();
}

void opengl_translate(float x, float y, float z) {
    glTranslatef(x, y, z);
}

void opengl_rotate(float angle, float x, float y, float z) {
    glRotatef(angle, x, y, z);
}

void opengl_scale(float x, float y, float z) {
    glScalef(x, y, z);
}

/* ==================== Utilidades ==================== */

void opengl_set_color(float r, float g, float b, float a) {
    glColor4f(r, g, b, a);
}

void opengl_enable_blend(void) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void opengl_disable_blend(void) {
    glDisable(GL_BLEND);
}

void opengl_draw_text_2d(float x, float y, const char *text) {
    if (!g_ctx || !g_ctx->initialized || !text) return;

    /* Nota: OpenGL 1.1 no tiene soporte nativo para texto.
     * Esta es una implementacion basica usando bitmap fonts.
     * Para una mejor calidad, considera usar una libreria como FTGL.
     */

    glRasterPos2f(x, y);
}
