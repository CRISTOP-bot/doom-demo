#ifndef OPENGL_CONFIG_H
#define OPENGL_CONFIG_H

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdint.h>
#include <math.h>

/* Configuracion de OpenGL */
#define OPENGL_VERSION_MAJOR 1
#define OPENGL_VERSION_MINOR 1

/* Dimensiones de pantalla */
#define GL_SCREEN_W 960
#define GL_SCREEN_H 640

/* Configuracion de perspectiva */
#define FOV_ANGLE 60.0f
#define NEAR_PLANE 0.1f
#define FAR_PLANE 1000.0f

/* Colores predefinidos */
#define COLOR_BLACK 0, 0, 0
#define COLOR_WHITE 1, 1, 1
#define COLOR_RED 1, 0, 0
#define COLOR_GREEN 0, 1, 0
#define COLOR_BLUE 0, 0, 1
#define COLOR_YELLOW 1, 1, 0
#define COLOR_CYAN 0, 1, 1
#define COLOR_MAGENTA 1, 0, 1
#define COLOR_DARK_GRAY 0.2f, 0.2f, 0.2f
#define COLOR_LIGHT_GRAY 0.8f, 0.8f, 0.8f

/* Velocidad de animacion */
#define ANIMATION_SPEED 0.016f  /* ~60 FPS */

/* Tipos de luz */
typedef struct {
    float x, y, z;
    float r, g, b;
    float intensity;
} Light;

typedef struct {
    float x, y, z;
    float r, g, b, a;
} Vertex;

#endif
