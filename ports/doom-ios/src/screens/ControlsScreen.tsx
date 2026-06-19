import React from 'react';
import { View, Text, StyleSheet, ScrollView } from 'react-native';

export default function ControlsScreen() {
  return (
    <ScrollView style={styles.container}>
      <View style={styles.section}>
        <Text style={styles.sectionTitle}>Touch Controls</Text>
        <View style={styles.controlItem}>
          <Text style={styles.controlLabel}>Left Side (Movement)</Text>
          <Text style={styles.controlDesc}>Virtual joystick for movement</Text>
        </View>
        <View style={styles.controlItem}>
          <Text style={styles.controlLabel}>Right Side (Actions)</Text>
          <Text style={styles.controlDesc}>Fire, Jump, Use buttons</Text>
        </View>
      </View>

      <View style={styles.section}>
        <Text style={styles.sectionTitle}>Gestures</Text>
        <View style={styles.controlItem}>
          <Text style={styles.controlLabel}>Tap</Text>
          <Text style={styles.controlDesc}>Fire weapon</Text>
        </View>
        <View style={styles.controlItem}>
          <Text style={styles.controlLabel}>Swipe Up</Text>
          <Text style={styles.controlDesc}>Next weapon</Text>
        </View>
        <View style={styles.controlItem}>
          <Text style={styles.controlLabel}>Swipe Down</Text>
          <Text style={styles.controlDesc}>Previous weapon</Text>
        </View>
      </View>
    </ScrollView>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#0a0a0a',
    paddingHorizontal: 16,
  },
  section: {
    marginVertical: 20,
  },
  sectionTitle: {
    fontSize: 20,
    fontWeight: 'bold',
    color: '#FF6B00',
    marginBottom: 12,
  },
  controlItem: {
    backgroundColor: '#1a1a1a',
    padding: 12,
    borderRadius: 8,
    marginBottom: 8,
  },
  controlLabel: {
    color: '#fff',
    fontSize: 14,
    fontWeight: '600',
  },
  controlDesc: {
    color: '#999',
    fontSize: 12,
    marginTop: 4,
  },
});
