package com.app.doomandroid.nativejni

/**
 * DoomJNI - Interfaz Java Native Interface para el motor de juego Doom
 * 
 * Esta clase proporciona métodos para interactuar con el código C compilado
 * del motor de juego Doom a través de JNI.
 */
object DoomJNI {
    init {
        System.loadLibrary("doom")
    }

    /**
     * Inicializa el motor de juego
     */
    external fun init(width: Int, height: Int): Boolean

    /**
     * Limpia y libera recursos del motor de juego
     */
    external fun cleanup()

    /**
     * Actualiza el estado del juego basado en la entrada del usuario
     */
    external fun update(
        w: Int, a: Int, s: Int, d: Int,
        left: Int, right: Int,
        space: Int, e: Int, tab: Int, r: Int,
        mouseDx: Int
    )

    /**
     * Renderiza un frame del juego
     */
    external fun render()

    /**
     * Obtiene la salud actual del jugador
     */
    external fun getPlayerHP(): Int

    /**
     * Obtiene la munición actual del jugador
     */
    external fun getPlayerAmmo(): Int

    /**
     * Obtiene la puntuación actual del jugador
     */
    external fun getPlayerScore(): Int

    /**
     * Verifica si el juego ha terminado
     */
    external fun isGameOver(): Boolean

    /**
     * Obtiene el nivel actual
     */
    external fun getLevel(): Int

    /**
     * Reinicia el juego
     */
    external fun reset()

    /**
     * Establece el viewport (área de renderizado)
     */
    external fun setViewport(width: Int, height: Int)
}