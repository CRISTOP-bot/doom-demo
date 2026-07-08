import { useEffect, useState, useCallback, useRef } from "react";
import { NativeModules, Platform } from "react-native";

import DoomModule from '@/specs/NativeDoomModuleSpec';

export interface GameState {
  hp: number;
  ammo: number;
  score: number;
  gameOver: boolean;
  level: number;
}

export interface UseDoomOptions {
  width?: number;
  height?: number;
  autoInit?: boolean;
}

/**
 * Hook para usar el motor de juego Doom
 * 
 * Proporciona métodos para inicializar, actualizar y renderizar el juego
 */
export function useDoom(options: UseDoomOptions = {}) {
  const {
    width = 1080,
    height = 1920,
    autoInit = true,
  } = options;

  const [initialized, setInitialized] = useState(false);
  const [gameState, setGameState] = useState<GameState>({
    hp: 100,
    ammo: 100,
    score: 0,
    gameOver: false,
    level: 1,
  });
  const [error, setError] = useState<string | null>(null);

  const inputStateRef = useRef({
    w: 0,
    a: 0,
    s: 0,
    d: 0,
    left: 0,
    right: 0,
    space: 0,
    e: 0,
    tab: 0,
    r: 0,
    mouseDx: 0,
  });

  // Inicializar el motor de juego
  const init = useCallback(async () => {
    if (!Platform.OS.includes("android")) {
      setError("Doom game is only available on Android");
      return false;
    }

    try {
      if (!DoomModule) {
        setError("DoomModule not available");
        return false;
      }

      const result = await DoomModule.init(width, height);
      if (result) {
        setInitialized(true);
        setError(null);
        return true;
      } else {
        setError("Failed to initialize Doom engine");
        return false;
      }
    } catch (err) {
      const errorMessage = err instanceof Error ? err.message : String(err);
      setError(`Initialization error: ${errorMessage}`);
      return false;
    }
  }, [width, height]);

  // Limpiar recursos
  const cleanup = useCallback(async () => {
    try {
      if (DoomModule) {
        await DoomModule.cleanup();
      }
      setInitialized(false);
    } catch (err) {
      const errorMessage = err instanceof Error ? err.message : String(err);
      setError(`Cleanup error: ${errorMessage}`);
    }
  }, []);

  // Actualizar entrada del usuario
  const updateInput = useCallback(
    (keys: Partial<typeof inputStateRef.current>) => {
      Object.assign(inputStateRef.current, keys);
    },
    []
  );

  // Actualizar estado del juego
  const update = useCallback(async () => {
    if (!initialized || !DoomModule) return;

    try {
      const input = inputStateRef.current;
      await DoomModule.update(
        input.w,
        input.a,
        input.s,
        input.d,
        input.left,
        input.right,
        input.space,
        input.e,
        input.tab,
        input.r,
        input.mouseDx
      );

      // Obtener estado actual del juego
      const state = await DoomModule.getGameState();
      setGameState(state);
    } catch (err) {
      const errorMessage = err instanceof Error ? err.message : String(err);
      setError(`Update error: ${errorMessage}`);
    }
  }, [initialized]);

  // Reiniciar juego
  const reset = useCallback(async () => {
    if (!initialized || !DoomModule) return;

    try {
      await DoomModule.reset();
      setGameState({
        hp: 100,
        ammo: 100,
        score: 0,
        gameOver: false,
        level: 1,
      });
    } catch (err) {
      const errorMessage = err instanceof Error ? err.message : String(err);
      setError(`Reset error: ${errorMessage}`);
    }
  }, [initialized]);

  // Auto-inicializar si está habilitado
  useEffect(() => {
    if (autoInit) {
      init();
    }

    return () => {
      if (autoInit) {
        cleanup();
      }
    };
  }, [autoInit, init, cleanup]);

  return {
    initialized,
    gameState,
    error,
    init,
    cleanup,
    updateInput,
    update,
    reset
  };
}
