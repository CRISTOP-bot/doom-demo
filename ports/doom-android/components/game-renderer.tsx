import React, { useEffect, useRef, useState } from "react";
import { View, Text, StyleSheet, Platform, Dimensions } from "react-native";
import { useDoom } from "@/hooks/use-doom";
import DoomGLView  from "@/components/gl"

interface GameRendererProps {
  onGameOver?: () => void;
  onScoreChange?: (score: number) => void;
}

/**
 * GameRenderer - Componente para renderizar el juego Doom
 * 
 * Este componente maneja la inicialización del motor de juego,
 * el loop de renderizado y la actualización del estado.
 */
export function GameRenderer({ onGameOver, onScoreChange }: GameRendererProps) {
  const { width: screenWidth, height: screenHeight } = Dimensions.get("window");
  const gameRef = useRef<View>(null);
  const [isRunning, setIsRunning] = useState(false);
  const animationFrameRef = useRef<number | null>(null);

  const {
    initialized,
    gameState,
    error,
    init,
    cleanup,
    updateInput,
    update
  } = useDoom({
    width: screenWidth,
    height: screenHeight,
    autoInit: false,
  });

  // Notificar cuando el juego termina
  useEffect(() => {
    if (gameState.gameOver && onGameOver) {
      onGameOver();
    }
  }, [gameState.gameOver, onGameOver]);

  // Notificar cambios de puntuación
  useEffect(() => {
    if (onScoreChange) {
      onScoreChange(gameState.score);
    }
  }, [gameState.score, onScoreChange]);

  if (error) {
    return (
      <View style={styles.errorContainer}>
        <Text style={styles.errorText}>Error: {error}</Text>
      </View>
    );
  }

  if (initialized) {
    return (
      <View style={styles.loadingContainer}>
        <Text style={styles.loadingText}>Initializing game...</Text>
      </View>
    );
  }

  return (
    <View ref={gameRef} style={styles.container}>
      {/* Renderizado OpenGL ES (manejado por GLSurfaceView en Android) */}
      <DoomGLView style={styles.gameView} surfaceWidth={screenWidth} surfaceHeight={screenHeight} />

      {/* HUD Overlay */}
      <View style={styles.hudContainer}>
        {/* Salud */}
        <View style={styles.hudItem}>
          <Text style={styles.hudLabel}>HP</Text>
          <Text style={styles.hudValue}>{gameState.hp}</Text>
        </View>

        {/* Munición */}
        <View style={styles.hudItem}>
          <Text style={styles.hudLabel}>AMMO</Text>
          <Text style={styles.hudValue}>{gameState.ammo}</Text>
        </View>

        {/* Puntuación */}
        <View style={styles.hudItem}>
          <Text style={styles.hudLabel}>SCORE</Text>
          <Text style={styles.hudValue}>{gameState.score}</Text>
        </View>

        {/* Nivel */}
        <View style={styles.hudItem}>
          <Text style={styles.hudLabel}>LVL</Text>
          <Text style={styles.hudValue}>{gameState.level}</Text>
        </View>
      </View>

      {/* Estado del juego */}
      {gameState.gameOver && (
        <View style={styles.gameOverContainer}>
          <Text style={styles.gameOverText}>GAME OVER</Text>
          <Text style={styles.gameOverScore}>Score: {gameState.score}</Text>
        </View>
      )}
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: "#000",
    position: "relative",
  },
  gameView: {
    flex: 1,
  },
  hudContainer: {
    position: "absolute",
    top: 16,
    left: 16,
    right: 16,
    flexDirection: "row",
    justifyContent: "space-between",
    zIndex: 10,
  },
  hudItem: {
    backgroundColor: "rgba(0, 0, 0, 0.7)",
    paddingHorizontal: 12,
    paddingVertical: 8,
    borderRadius: 4,
    borderWidth: 1,
    borderColor: "#00FF00",
  },
  hudLabel: {
    color: "#00FF00",
    fontSize: 10,
    fontWeight: "bold",
  },
  hudValue: {
    color: "#00FF00",
    fontSize: 14,
    fontWeight: "bold",
    marginTop: 4,
  },
  loadingContainer: {
    flex: 1,
    backgroundColor: "#000",
    justifyContent: "center",
    alignItems: "center",
  },
  loadingText: {
    color: "#00FF00",
    fontSize: 18,
    fontWeight: "bold",
  },
  errorContainer: {
    flex: 1,
    backgroundColor: "#000",
    justifyContent: "center",
    alignItems: "center",
    padding: 16,
  },
  errorText: {
    color: "#FF0000",
    fontSize: 16,
    textAlign: "center",
  },
  gameOverContainer: {
    position: "absolute",
    top: 0,
    left: 0,
    right: 0,
    bottom: 0,
    backgroundColor: "rgba(0, 0, 0, 0.8)",
    justifyContent: "center",
    alignItems: "center",
    zIndex: 100,
  },
  gameOverText: {
    color: "#FF0000",
    fontSize: 48,
    fontWeight: "bold",
    marginBottom: 16,
  },
  gameOverScore: {
    color: "#00FF00",
    fontSize: 24,
    fontWeight: "bold",
  },
});
