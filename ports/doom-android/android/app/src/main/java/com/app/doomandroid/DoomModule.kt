package com.app.doomandroid

import android.util.Log
import com.app.doomandroid.nativejni.DoomJNI
import com.facebook.react.bridge.Arguments
import com.facebook.react.bridge.Promise
import com.facebook.react.bridge.ReactApplicationContext
import com.facebook.react.bridge.ReactContextBaseJavaModule
import com.facebook.react.bridge.WritableMap
import com.facebook.react.module.annotations.ReactModule
import com.facebook.react.bridge.ReactMethod

@ReactModule(name = DoomModule.NAME)
class DoomModule(reactContext: ReactApplicationContext) : ReactContextBaseJavaModule(reactContext) {

    companion object {
        const val NAME = "DoomModule"
    }

    override fun getName() = NAME

    // init ya NO llama DoomJNI.init directamente: el init real ocurre en
    // DoomRenderer.onSurfaceCreated cuando GLSurfaceView crea el contexto EGL.
    // Aqui solo esperamos a que ese contexto ya este listo antes de resolver.
    @ReactMethod
    fun init(width: Double, height: Double, promise: Promise) {
        DoomJNI.init(width.toInt(), height.toInt())
        promise.resolve(true)
    }

    @ReactMethod
    fun update(
        w: Double, a: Double, s: Double, d: Double,
        left: Double, right: Double,
        space: Double, e: Double, tab: Double, r: Double,
        mouseDx: Double,
        promise: Promise
    ) {
        DoomJNI.update(
            w.toInt(), a.toInt(), s.toInt(), d.toInt(),
            left.toInt(), right.toInt(),
            space.toInt(), e.toInt(), tab.toInt(), r.toInt(),
            mouseDx.toInt()
        )
        promise.resolve(null)
    }

    @ReactMethod
    fun getGameState(promise: Promise) {
        // Lectura de estado: son getters simples (int), no tocan GL,
        // se pueden dejar fuera del hilo GL sin problema.
        try {
            val map: WritableMap = Arguments.createMap()
            map.putInt("hp", DoomJNI.getPlayerHP())
            map.putInt("ammo", DoomJNI.getPlayerAmmo())
            map.putInt("score", DoomJNI.getPlayerScore())
            map.putBoolean("gameOver", DoomJNI.isGameOver())
            map.putInt("level", DoomJNI.getLevel())
            promise.resolve(map)
        } catch (e: Exception) {
            Log.e(NAME, "Error getting game state", e)
            promise.reject("DOOM_GET_STATE_ERROR", e.message, e)
        }
    }

    @ReactMethod
    fun render(promise: Promise) {
        DoomJNI.render()
        promise.resolve(null)
    }

    @ReactMethod
    fun reset(promise: Promise) {
        DoomJNI.reset()
        promise.resolve(null)
    }

    @ReactMethod
    fun cleanup(promise: Promise) {
        DoomJNI.cleanup()
        promise.resolve(null)
    }

    @ReactMethod
    fun setViewport(w: Double, h: Double, promise: Promise) {
        DoomJNI.setViewport(w.toInt(), h.toInt())
        promise.resolve(null)
    }
}