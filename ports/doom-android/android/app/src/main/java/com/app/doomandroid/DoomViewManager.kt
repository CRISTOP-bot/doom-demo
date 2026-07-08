package com.app.doomandroid

import com.facebook.react.bridge.ReactApplicationContext
import com.facebook.react.uimanager.SimpleViewManager
import com.facebook.react.uimanager.ThemedReactContext
import com.facebook.react.uimanager.annotations.ReactProp

class DoomViewManager(
    reactContext: ReactApplicationContext
) : SimpleViewManager<DoomGLSurfaceView>() {

    companion object {
        const val NAME = "DoomGLView"
    }

    override fun getName() = NAME

    override fun createViewInstance(context: ThemedReactContext): DoomGLSurfaceView {
        return DoomGLSurfaceView(context)
    }
}