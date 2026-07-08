package com.app.doomandroid

import android.util.Log
import com.app.doomandroid.nativejni.DoomJNI
import android.opengl.GLSurfaceView
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class DoomGLRenderer : GLSurfaceView.Renderer {
    
    private var isInitialized = false
    private var surfaceWidth = 0
    private var surfaceHeight = 0
    private var readyToInit = false

    override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
        Log.d("DoomRenderer", "onSurfaceCreated")
        // ✅ Aquí NO inicializamos aún porque no tenemos dimensiones
        // Esperamos a onSurfaceChanged
        readyToInit = true
    }

    override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
        Log.d("DoomRenderer", "onSurfaceChanged: $width x $height")
        surfaceWidth = width
        surfaceHeight = height
        
        // ✅ Inicializar AQUÍ cuando ya tenemos dimensiones válidas
        if (readyToInit && !isInitialized) {
            isInitialized = DoomJNI.init(surfaceWidth, surfaceHeight)
            Log.d("DoomRenderer", "DoomJNI.init result: $isInitialized")
            
            if (!isInitialized) {
                Log.e("DoomRenderer", "Error: No se pudo inicializar DoomJNI")
            }
        }
        
        DoomJNI.setViewport(width, height)
    }

    override fun onDrawFrame(gl: GL10?) {
        if (isInitialized) {
            try {
                DoomJNI.update(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
                DoomJNI.render()
            } catch (e: Exception) {
                Log.e("DoomRenderer", "Error en onDrawFrame", e)
            }
        }
    }

    fun cleanup() {
        if (isInitialized) {
            try {
                DoomJNI.cleanup()
            } catch (e: Exception) {
                Log.e("DoomRenderer", "Error en cleanup", e)
            }
            isInitialized = false
        }
    }

    fun reset() {
        if (isInitialized) {
            DoomJNI.reset()
        }
    }
}

data class GameState(
    val hp: Int,
    val ammo: Int,
    val score: Int,
    val level: Int,
    val gameOver: Boolean
)