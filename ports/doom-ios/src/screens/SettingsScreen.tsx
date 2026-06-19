import React, { useState } from 'react';
import { View, Text, StyleSheet, ScrollView, Switch } from 'react-native';

export default function SettingsScreen() {
  const [soundEnabled, setSoundEnabled] = useState(true);
  const [haptics, setHaptics] = useState(true);
  const [brightness, setBrightness] = useState(true);

  return (
    <ScrollView style={styles.container}>
      <View style={styles.section}>
        <Text style={styles.sectionTitle}>Audio</Text>
        <View style={styles.settingItem}>
          <Text style={styles.settingLabel}>Sound Effects</Text>
          <Switch
            value={soundEnabled}
            onValueChange={setSoundEnabled}
            trackColor={{ false: '#333', true: '#FF6B00' }}
          />
        </View>
      </View>

      <View style={styles.section}>
        <Text style={styles.sectionTitle}>Haptics</Text>
        <View style={styles.settingItem}>
          <Text style={styles.settingLabel}>Vibration Feedback</Text>
          <Switch
            value={haptics}
            onValueChange={setHaptics}
            trackColor={{ false: '#333', true: '#FF6B00' }}
          />
        </View>
      </View>

      <View style={styles.section}>
        <Text style={styles.sectionTitle}>Display</Text>
        <View style={styles.settingItem}>
          <Text style={styles.settingLabel}>Keep Screen On</Text>
          <Switch
            value={brightness}
            onValueChange={setBrightness}
            trackColor={{ false: '#333', true: '#FF6B00' }}
          />
        </View>
      </View>

      <View style={styles.footer}>
        <Text style={styles.footerText}>Doom Demo v1.0.0</Text>
        <Text style={styles.footerText}>iOS Edition</Text>
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
    fontSize: 18,
    fontWeight: 'bold',
    color: '#FF6B00',
    marginBottom: 12,
  },
  settingItem: {
    backgroundColor: '#1a1a1a',
    padding: 12,
    borderRadius: 8,
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
    marginBottom: 8,
  },
  settingLabel: {
    color: '#fff',
    fontSize: 14,
    fontWeight: '500',
  },
  footer: {
    paddingVertical: 40,
    alignItems: 'center',
  },
  footerText: {
    color: '#666',
    fontSize: 12,
  },
});
