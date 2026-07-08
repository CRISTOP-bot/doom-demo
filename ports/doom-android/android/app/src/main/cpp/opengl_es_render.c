#include "doom_core.h"
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <math.h>
#include <stdio.h>
#include <android/log.h>

#define LOG_TAG "DoomCore"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

/* ==================== Contexto OpenGL ES ==================== */

static struct {
    GLuint program;
    GLint position_attr;
    GLint color_attr;
    GLint matrix_uniform;
    int initialized;
    int width;
    int height;
} gl_context = {0};

/* ==================== Shaders ==================== */

static const char *vertex_shader_src = 
    "#version 100\n"
    "uniform mat4 uMatrix;\n"
    "attribute vec4 aPosition;\n"
    "attribute vec4 aColor;\n"
    "varying vec4 vColor;\n"
    "void main() {\n"
    "  gl_Position = uMatrix * aPosition;\n"
    "  vColor = aColor;\n"
    "}\n";

static const char *fragment_shader_src =
    "#version 100\n"
    "precision mediump float;\n"
    "varying vec4 vColor;\n"
    "void main() {\n"
    "  gl_FragColor = vColor;\n"
    "}\n";

/* ==================== Utilidad de logging de errores GL ==================== */

static void log_gl_error(const char *tag) {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        LOGE("[GL_ERROR] %s -> 0x%x", tag, err);
    }
}

/* ==================== Utilidades de Shaders ==================== */

static GLuint compile_shader(GLenum type, const char *src) {
    LOGD("compile_shader: type=%s", type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");

    GLuint shader = glCreateShader(type);
    if (!shader) {
        LOGE("glCreateShader devolvio 0 para type=%d", type);
        return 0;
    }

    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        char log[512];
        glGetShaderInfoLog(shader, sizeof(log), NULL, log);
        LOGE("Shader compilation failed (type=%d): %s", type, log);
        printf("[ERROR] Shader compilation failed: %s\n", log);
        glDeleteShader(shader);
        return 0;
    }

    LOGD("compile_shader OK: id=%u", shader);
    return shader;
}

static GLuint create_program(const char *vs_src, const char *fs_src) {
    LOGI("create_program: compilando vertex y fragment shaders");

    GLuint vs = compile_shader(GL_VERTEX_SHADER, vs_src);
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fs_src);

    if (!vs || !fs) {
        LOGE("create_program: fallo compilacion (vs=%u, fs=%u)", vs, fs);
        if (vs) glDeleteShader(vs);
        if (fs) glDeleteShader(fs);
        return 0;
    }

    GLuint program = glCreateProgram();
    if (!program) {
        LOGE("glCreateProgram devolvio 0");
        glDeleteShader(vs);
        glDeleteShader(fs);
        return 0;
    }

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    GLint linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        char log[512];
        glGetProgramInfoLog(program, sizeof(log), NULL, log);
        LOGE("Program linking failed: %s", log);
        printf("[ERROR] Program linking failed: %s\n", log);
        glDeleteProgram(program);
        glDeleteShader(vs);
        glDeleteShader(fs);
        return 0;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    LOGI("create_program OK: program_id=%u", program);
    return program;
}

/* ==================== Inicializacion ==================== */

int opengl_es_init(int width, int height) {
    LOGI("=== opengl_es_init START: width=%d, height=%d ===", width, height);

    gl_context.width = width;
    gl_context.height = height;

    /* Crear programa de shaders */
    gl_context.program = create_program(vertex_shader_src, fragment_shader_src);
    if (!gl_context.program) {
        LOGE("Failed to create shader program");
        printf("[ERROR] Failed to create shader program\n");
        return 0;
    }

    /* Obtener ubicaciones de atributos y uniformes */
    gl_context.position_attr = glGetAttribLocation(gl_context.program, "aPosition");
    gl_context.color_attr = glGetAttribLocation(gl_context.program, "aColor");
    gl_context.matrix_uniform = glGetUniformLocation(gl_context.program, "uMatrix");

    LOGD("Locations: position_attr=%d color_attr=%d matrix_uniform=%d",
         gl_context.position_attr, gl_context.color_attr, gl_context.matrix_uniform);

    if (gl_context.position_attr < 0 || gl_context.color_attr < 0) {
        LOGE("Atributo no encontrado en el shader (position=%d, color=%d)",
             gl_context.position_attr, gl_context.color_attr);
    }
    // PENDIENTE: matrix_uniform nunca se usa con glUniformMatrix4fv en ningun
    // lugar del archivo. Si el driver no deja el uniform en identidad por
    // default, gl_Position se calcula contra una matriz en ceros y nada se
    // renderiza. Verificar esto antes de seguir buscando el bug en otro lado.
    if (gl_context.matrix_uniform < 0) {
        LOGE("uMatrix uniform no encontrado (esta declarado pero sin uso -> "
             "el linker pudo haberlo optimizado)");
    }

    /* Configuración inicial de OpenGL ES */
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glViewport(0, 0, width, height);
    log_gl_error("opengl_es_init setup");

    gl_context.initialized = 1;
    LOGI("=== opengl_es_init SUCCESS ===");
    printf("[OpenGL ES] Initialized successfully\n");

    return 1;
}

void opengl_es_cleanup(void) {
    LOGI("opengl_es_cleanup called");
    if (gl_context.program) {
        glDeleteProgram(gl_context.program);
        gl_context.program = 0;
    }
    gl_context.initialized = 0;
    LOGI("opengl_es_cleanup complete");
}

/* ==================== Renderizado ==================== */

void opengl_es_begin_frame(void) {
    if (!gl_context.initialized) {
        LOGE("opengl_es_begin_frame llamado sin inicializar");
        return;
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void opengl_es_end_frame(void) {
    if (!gl_context.initialized) {
        LOGE("opengl_es_end_frame llamado sin inicializar");
        return;
    }
    /* El buffer se intercambia en la capa de Android */
}

void opengl_es_set_viewport(int width, int height) {
    if (!gl_context.initialized) {
        LOGE("opengl_es_set_viewport llamado sin inicializar (w=%d, h=%d)", width, height);
        return;
    }
    LOGI("opengl_es_set_viewport: width=%d, height=%d", width, height);
    gl_context.width = width;
    gl_context.height = height;
    glViewport(0, 0, width, height);
}

/* ==================== Dibujado de Primitivas ==================== */

static void draw_cube(float x, float y, float z, float size, 
                      float r, float g, float b) {
    if (!gl_context.initialized) {
        LOGE("draw_cube llamado sin inicializar");
        return;
    }

    float s = size / 2.0f;

    /* Vertices del cubo */
    GLfloat vertices[] = {
        /* Frente */
        -s, -s, s,   s, -s, s,   s, s, s,   -s, s, s,
        /* Atras */
        -s, -s, -s,  -s, s, -s,  s, s, -s,  s, -s, -s,
        /* Arriba */
        -s, s, -s,   -s, s, s,   s, s, s,   s, s, -s,
        /* Abajo */
        -s, -s, -s,  s, -s, -s,  s, -s, s,  -s, -s, s,
        /* Derecha */
        s, -s, -s,   s, s, -s,   s, s, s,   s, -s, s,
        /* Izquierda */
        -s, -s, -s,  -s, -s, s,  -s, s, s,  -s, s, -s,
    };

    GLfloat colors[] = {
        r, g, b, 1.0f,  r, g, b, 1.0f,  r, g, b, 1.0f,  r, g, b, 1.0f,
        r, g, b, 1.0f,  r, g, b, 1.0f,  r, g, b, 1.0f,  r, g, b, 1.0f,
        r, g, b, 1.0f,  r, g, b, 1.0f,  r, g, b, 1.0f,  r, g, b, 1.0f,
        r, g, b, 1.0f,  r, g, b, 1.0f,  r, g, b, 1.0f,  r, g, b, 1.0f,
        r, g, b, 1.0f,  r, g, b, 1.0f,  r, g, b, 1.0f,  r, g, b, 1.0f,
        r, g, b, 1.0f,  r, g, b, 1.0f,  r, g, b, 1.0f,  r, g, b, 1.0f,
    };

    glUseProgram(gl_context.program);

    glEnableVertexAttribArray(gl_context.position_attr);
    glEnableVertexAttribArray(gl_context.color_attr);

    glVertexAttribPointer(gl_context.position_attr, 3, GL_FLOAT, GL_FALSE, 12, vertices);
    glVertexAttribPointer(gl_context.color_attr, 4, GL_FLOAT, GL_FALSE, 16, colors);

    // PENDIENTE: GL_TRIANGLE_STRIP con estos 24 vertices conecta las 6 caras
    // (pensadas como quads independientes de 4 vertices) como si fueran un
    // strip continuo. Esto genera triangulos "puente" entre caras que no
    // deberian existir (ej. entre el ultimo vertice de "Frente" y el primero
    // de "Atras"). Si ves geometria rara/rota entre caras, es por esto, no
    // por los datos de vertices en si. Se necesita glDrawElements con indices
    // o 6 llamadas a GL_TRIANGLE_FAN (una por cara).
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 24);
    log_gl_error("draw_cube glDrawArrays");

    glDisableVertexAttribArray(gl_context.position_attr);
    glDisableVertexAttribArray(gl_context.color_attr);
}

/* ==================== Renderizado de Escena ==================== */

void opengl_es_draw_scene(GameState *game) {
    if (!gl_context.initialized || !game) {
        LOGE("opengl_es_draw_scene: gl_context.initialized=%d, game=%p",
             gl_context.initialized, (void *)game);
        return;
    }

    glUseProgram(gl_context.program);

    LOGD("draw_scene: player=(%.2f,%.2f) enemies=%d bullets=%d",
         game->player.x, game->player.y, game->enemy_count, game->bullet_count);

    /* Dibujar jugador (cubo azul) */
    draw_cube(game->player.x, game->player.y, 0.0f, 0.5f, 0.0f, 0.5f, 1.0f);

    /* Dibujar enemigos (pirámides rojas) */
    int enemies_drawn = 0;
    for (int i = 0; i < game->enemy_count; i++) {
        if (game->enemies[i].active) {
            draw_cube(game->enemies[i].x, game->enemies[i].y, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f);
            enemies_drawn++;
        }
    }

    /* Dibujar balas (esferas amarillas) */
    int bullets_drawn = 0;
    for (int i = 0; i < game->bullet_count; i++) {
        if (game->bullets[i].active) {
            draw_cube(game->bullets[i].x, game->bullets[i].y, game->bullets[i].z, 
                     0.1f, 1.0f, 1.0f, 0.0f);
            bullets_drawn++;
        }
    }

    /* Dibujar paredes */
    int walls_drawn = 0;
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 32; x++) {
            if (game->map[y][x] == 1) {
                draw_cube(x, y, 0.5f, 1.0f, 0.5f, 0.5f, 0.5f);
                walls_drawn++;
            }
        }
    }

    LOGD("draw_scene: cubos dibujados -> enemies=%d bullets=%d walls=%d",
         enemies_drawn, bullets_drawn, walls_drawn);
}