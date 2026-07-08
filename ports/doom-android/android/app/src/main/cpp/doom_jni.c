#include <jni.h>
#include <android/log.h>
#include <GLES2/gl2.h>
#include <stdlib.h>
#include <string.h>
#include "doom_core.h"

#define LOG_TAG "DoomJNI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static GameState *g_game = NULL;
static int g_initialized = 0;

JNIEXPORT jint JNICALL Java_com_app_doomandroid_DoomJNI_init(
    JNIEnv *env, jclass clazz, jint width, jint height) {
    
    LOGI("=== INIT START: width=%d, height=%d ===", width, height);

    if (g_game) {
        LOGI("Freeing existing game state");
        free(g_game);
    }

    g_game = (GameState *)malloc(sizeof(GameState));
    if (!g_game) {
        LOGE("Failed to allocate game state");
        return 0;
    }
    LOGI("Game state allocated");

    LOGI("Calling opengl_es_init...");
    if (!opengl_es_init(width, height)) {
        LOGE("Failed to initialize OpenGL ES");
        free(g_game);
        g_game = NULL;
        return 0;
    }
    LOGI("OpenGL ES initialized");

    LOGI("Calling game_reset...");
    game_reset(g_game);
    LOGI("Game reset complete");

    g_initialized = 1;
    LOGI("=== INIT SUCCESS ===");
    return 1;
}

JNIEXPORT void JNICALL Java_com_app_doomandroid_DoomJNI_cleanup(
    JNIEnv *env, jclass clazz) {
    
    LOGI("Cleanup called");
    opengl_es_cleanup();

    if (g_game) {
        free(g_game);
        g_game = NULL;
    }

    g_initialized = 0;
    LOGI("Cleanup complete");
}

JNIEXPORT void JNICALL Java_com_app_doomandroid_DoomJNI_update(
    JNIEnv *env, jclass clazz,
    jint w, jint a, jint s, jint d,
    jint left, jint right,
    jint space, jint e, jint tab, jint r,
    jint mouse_dx) {
    
    if (!g_initialized || !g_game) {
        LOGE("Update called but not initialized");
        return;
    }

    InputState input;
    memset(&input, 0, sizeof(input));
    input.w = w;
    input.a = a;
    input.s = s;
    input.d = d;
    input.left = left;
    input.right = right;
    input.space = space;
    input.e = e;
    input.tab = tab;
    input.r = r;
    input.mouse_dx = mouse_dx;

    game_tick(g_game, &input);
}

JNIEXPORT void JNICALL Java_com_app_doomandroid_DoomJNI_render(
    JNIEnv *env, jclass clazz) {
    
    if (!g_initialized || !g_game) return;

    opengl_es_begin_frame();
    opengl_es_draw_scene(g_game);
    opengl_es_end_frame();
}

JNIEXPORT jint JNICALL Java_com_app_doomandroid_DoomJNI_getPlayerHP(
    JNIEnv *env, jclass clazz) {
    if (!g_initialized || !g_game) return 0;
    return g_game->player.hp;
}

JNIEXPORT jint JNICALL Java_com_app_doomandroid_DoomJNI_getPlayerAmmo(
    JNIEnv *env, jclass clazz) {
    if (!g_initialized || !g_game) return 0;
    return g_game->player.ammo;
}

JNIEXPORT jint JNICALL Java_com_app_doomandroid_DoomJNI_getPlayerScore(
    JNIEnv *env, jclass clazz) {
    if (!g_initialized || !g_game) return 0;
    return g_game->player.score;
}

JNIEXPORT jint JNICALL Java_com_app_doomandroid_DoomJNI_isGameOver(
    JNIEnv *env, jclass clazz) {
    if (!g_initialized || !g_game) return 0;
    return g_game->game_over;
}

JNIEXPORT jint JNICALL Java_com_app_doomandroid_DoomJNI_getLevel(
    JNIEnv *env, jclass clazz) {
    if (!g_initialized || !g_game) return 1;
    return g_game->level;
}

JNIEXPORT void JNICALL Java_com_app_doomandroid_DoomJNI_reset(
    JNIEnv *env, jclass clazz) {
    if (!g_initialized || !g_game) return;
    game_reset(g_game);
}

JNIEXPORT void JNICALL Java_com_app_doomandroid_DoomJNI_setViewport(
    JNIEnv *env, jclass clazz, jint width, jint height) {
    if (!g_initialized) return;
    opengl_es_set_viewport(width, height);
}