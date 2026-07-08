import React, { useRef, useState, useCallback } from "react";
import {
  View,
  StyleSheet,
  Pressable,
  GestureResponderEvent,
  PanResponder,
  Animated,
  Text,
} from "react-native";

interface TouchControlsProps {
  onInputChange?: (input: InputState) => void;
}

export interface InputState {
  w: number;
  a: number;
  s: number;
  d: number;
  left: number;
  right: number;
  space: number;
  e: number;
  tab: number;
  r: number;
  mouseDx: number;
}

/**
 * TouchControls - Componente para controles táctiles
 * 
 * Proporciona un joystick virtual y botones de acción para el juego
 */
export function TouchControls({ onInputChange }: TouchControlsProps) {
  const [inputState, setInputState] = useState<InputState>({
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

  const joystickPosRef = useRef({ x: 0, y: 0 });
  const joystickCenterRef = useRef({ x: 0, y: 0 });
  const joystickAnimRef = useRef(new Animated.ValueXY({ x: 0, y: 0 })).current;

  // Responder a gestos del joystick
  const panResponder = useRef(
    PanResponder.create({
      onStartShouldSetPanResponder: () => true,
      onMoveShouldSetPanResponder: () => true,
      onPanResponderGrant: (evt) => {
        const { pageX, pageY } = evt.nativeEvent;
        joystickCenterRef.current = { x: pageX, y: pageY };
      },
      onPanResponderMove: (evt) => {
        const { pageX, pageY } = evt.nativeEvent;
        const x = pageX;
        const y = pageY;
        const dx = x - joystickCenterRef.current.x;
        const dy = y - joystickCenterRef.current.y;
        const distance = Math.sqrt(dx * dx + dy * dy);
        const maxDistance = 50;

        if (distance > 0) {
          const ratio = Math.min(distance / maxDistance, 1);
          const normalizedX = (dx / distance) * ratio * maxDistance;
          const normalizedY = (dy / distance) * ratio * maxDistance;

          joystickPosRef.current = { x: normalizedX, y: normalizedY };

          // Actualizar entrada basada en la posición del joystick
          const newInput = { ...inputState };
          newInput.w = normalizedY < -20 ? 1 : 0;
          newInput.s = normalizedY > 20 ? 1 : 0;
          newInput.a = normalizedX < -20 ? 1 : 0;
          newInput.d = normalizedX > 20 ? 1 : 0;

          setInputState(newInput);
          onInputChange?.(newInput);

          // Animar joystick
          Animated.spring(joystickAnimRef, {
            toValue: { x: normalizedX, y: normalizedY },
            useNativeDriver: false,
          }).start();
        }
      },
      onPanResponderRelease: () => {
        joystickPosRef.current = { x: 0, y: 0 };
        const newInput = { ...inputState, w: 0, a: 0, s: 0, d: 0 };
        setInputState(newInput);
        onInputChange?.(newInput);

        Animated.spring(joystickAnimRef, {
          toValue: { x: 0, y: 0 },
          useNativeDriver: false,
        }).start();
      },
    })
  ).current;

  // Manejadores de botones
  const handleFirePress = useCallback(() => {
    const newInput = { ...inputState, space: 1 };
    setInputState(newInput);
    onInputChange?.(newInput);
  }, [inputState, onInputChange]);

  const handleFireRelease = useCallback(() => {
    const newInput = { ...inputState, space: 0 };
    setInputState(newInput);
    onInputChange?.(newInput);
  }, [inputState, onInputChange]);

  const handleDoorPress = useCallback(() => {
    const newInput = { ...inputState, e: 1 };
    setInputState(newInput);
    onInputChange?.(newInput);

    setTimeout(() => {
      const resetInput = { ...inputState, e: 0 };
      setInputState(resetInput);
      onInputChange?.(resetInput);
    }, 100);
  }, [inputState, onInputChange]);

  const handlePausePress = useCallback(() => {
    const newInput = { ...inputState, tab: 1 };
    setInputState(newInput);
    onInputChange?.(newInput);

    setTimeout(() => {
      const resetInput = { ...inputState, tab: 0 };
      setInputState(resetInput);
      onInputChange?.(resetInput);
    }, 100);
  }, [inputState, onInputChange]);

  return (
    <View style={styles.container}>
      {/* Joystick izquierdo */}
      <View style={styles.joystickContainer} {...panResponder.panHandlers}>
        <View style={styles.joystickBase}>
          <Animated.View
            style={[
              styles.joystickStick,
              {
                transform: [
                  { translateX: joystickAnimRef.x },
                  { translateY: joystickAnimRef.y },
                ],
              },
            ]}
          />
        </View>
      </View>

      {/* Controles derechos */}
      <View style={styles.rightControls}>
        {/* Botón de pausa */}
        <Pressable
          style={({ pressed }) => [
            styles.pauseButton,
            pressed && styles.buttonPressed,
          ]}
          onPress={handlePausePress}
        >
          <View style={styles.buttonContent}>
            <View style={styles.pauseIcon} />
          </View>
        </Pressable>

        {/* Botón de puerta */}
        <Pressable
          style={({ pressed }) => [
            styles.doorButton,
            pressed && styles.buttonPressed,
          ]}
          onPress={handleDoorPress}
        >
          <Text style={styles.buttonText}>E</Text>
        </Pressable>

        {/* Botón de disparo */}
        <Pressable
          style={({ pressed }) => [
            styles.fireButton,
            pressed && styles.buttonPressed,
          ]}
          onPressIn={handleFirePress}
          onPressOut={handleFireRelease}
        >
          <Text style={styles.buttonText}>FIRE</Text>
        </Pressable>
      </View>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    position: "absolute",
    bottom: 0,
    left: 0,
    right: 0,
    height: 150,
    flexDirection: "row",
    justifyContent: "space-between",
    paddingHorizontal: 16,
    paddingBottom: 16,
    zIndex: 50,
  },
  joystickContainer: {
    width: 120,
    height: 120,
    justifyContent: "center",
    alignItems: "center",
  },
  joystickBase: {
    width: 100,
    height: 100,
    borderRadius: 50,
    backgroundColor: "rgba(100, 100, 100, 0.5)",
    borderWidth: 2,
    borderColor: "#00FF00",
    justifyContent: "center",
    alignItems: "center",
  },
  joystickStick: {
    width: 40,
    height: 40,
    borderRadius: 20,
    backgroundColor: "#00FF00",
  },
  rightControls: {
    flex: 1,
    justifyContent: "flex-end",
    alignItems: "flex-end",
    gap: 12,
  },
  pauseButton: {
    width: 50,
    height: 50,
    borderRadius: 25,
    backgroundColor: "#666",
    borderWidth: 2,
    borderColor: "#999",
    justifyContent: "center",
    alignItems: "center",
  },
  doorButton: {
    width: 50,
    height: 50,
    borderRadius: 8,
    backgroundColor: "#666",
    borderWidth: 2,
    borderColor: "#999",
    justifyContent: "center",
    alignItems: "center",
  },
  fireButton: {
    width: 80,
    height: 80,
    borderRadius: 40,
    backgroundColor: "#8B0000",
    borderWidth: 2,
    borderColor: "#FF0000",
    justifyContent: "center",
    alignItems: "center",
  },
  buttonPressed: {
    opacity: 0.7,
    transform: [{ scale: 0.95 }],
  },
  buttonContent: {
    width: "100%",
    height: "100%",
    justifyContent: "center",
    alignItems: "center",
  },
  pauseIcon: {
    width: 20,
    height: 20,
    backgroundColor: "#FFF",
    opacity: 0.7,
  },
  buttonText: {
    color: "#FFF",
    fontSize: 12,
    fontWeight: "bold" as const,
    textAlign: "center" as const,
  } as any,
});
