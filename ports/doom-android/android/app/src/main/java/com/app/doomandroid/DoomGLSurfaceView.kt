package com.app.doomandroid

import android.content.Context
import android.opengl.GLSurfaceView

/**
 * GLSurfaceView que retiene su Renderer para poder llamar queueEvent()
 * desde fuera (por ejemplo, desde el ViewManager o el Module).
 */
class DoomGLSurfaceView(context: Context) : GLSurfaceView(context) {

    init {
        setEGLContextClientVersion(2)
        setRenderer(DoomGLRenderer())
        renderMode = GLSurfaceView.RENDERMODE_CONTINUOUSLY
    }
}