import React from 'react';
import { View, Text, StyleSheet } from 'react-native';

export default function GameScreen() {
  return (
    <View style={styles.container}>
      <View style={styles.gameArea}>
        <Text style={styles.placeholder}>Game Engine</Text>
        <Text style={styles.subtext}>Metal Rendering (iOS)</Text>
      </View>
      <View style={styles.hud}>
        <Text style={styles.hudText}>Health: 100</Text>
        <Text style={styles.hudText}>Ammo: 50</Text>
        <Text style={styles.hudText}>Score: 0</Text>
      </View>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#000',
  },
  gameArea: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
  },
  placeholder: {
    color: '#FF6B00',
    fontSize: 24,
    fontWeight: 'bold',
  },
  subtext: {
    color: '#999',
    fontSize: 14,
    marginTop: 8,
  },
  hud: {
    backgroundColor: '#1a1a1a',
    paddingVertical: 12,
    paddingHorizontal: 16,
    flexDirection: 'row',
    justifyContent: 'space-around',
  },
  hudText: {
    color: '#FF6B00',
    fontSize: 14,
    fontWeight: 'bold',
  },
});
