#include <jni.h>
#include <android/log.h>

#define LOG_TAG "DoomJNI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

JNIEXPORT jint JNICALL Java_com_app_doomandroid_DoomJNI_init(
    JNIEnv *env, jclass clazz, jint width, jint height) {
    
    LOGI("Init called with width=%d, height=%d", width, height);
    return 1;
}

JNIEXPORT void JNICALL Java_com_app_doomandroid_DoomJNI_cleanup(
    JNIEnv *env, jclass clazz) {
    LOGI("Cleanup called");
}

JNIEXPORT void JNICALL Java_com_app_doomandroid_DoomJNI_update(
    JNIEnv *env, jclass clazz,
    jint w, jint a, jint s, jint d,
    jint left, jint right,
    jint space, jint e, jint tab, jint r,
    jint mouse_dx) {
    LOGI("Update called");
}

JNIEXPORT void JNICALL Java_com_app_doomandroid_DoomJNI_render(
    JNIEnv *env, jclass clazz) {
    LOGI("Render called");
}

JNIEXPORT jint JNICALL Java_com_app_doomandroid_DoomJNI_getPlayerHP(
    JNIEnv *env, jclass clazz) {
    return 100;
}

JNIEXPORT jint JNICALL Java_com_app_doomandroid_DoomJNI_getPlayerAmmo(
    JNIEnv *env, jclass clazz) {
    return 100;
}

JNIEXPORT jint JNICALL Java_com_app_doomandroid_DoomJNI_getPlayerScore(
    JNIEnv *env, jclass clazz) {
    return 0;
}

JNIEXPORT jint JNICALL Java_com_app_doomandroid_DoomJNI_isGameOver(
    JNIEnv *env, jclass clazz) {
    return 0;
}

JNIEXPORT jint JNICALL Java_com_app_doomandroid_DoomJNI_getLevel(
    JNIEnv *env, jclass clazz) {
    return 1;
}

JNIEXPORT void JNICALL Java_com_app_doomandroid_DoomJNI_reset(
    JNIEnv *env, jclass clazz) {
    LOGI("Reset called");
}

JNIEXPORT void JNICALL Java_com_app_doomandroid_DoomJNI_setViewport(
    JNIEnv *env, jclass clazz, jint width, jint height) {
    LOGI("SetViewport called with %d x %d", width, height);
}