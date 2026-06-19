import React, { useState } from 'react';
import { NavigationContainer } from '@react-navigation/native';
import { createNativeStackNavigator } from '@react-navigation/native-stack';
import { createBottomTabNavigator } from '@react-navigation/bottom-tabs';
import { View, Text, TouchableOpacity, StyleSheet } from 'react-native';

// Screens
import HomeScreen from './screens/HomeScreen';
import GameScreen from './screens/GameScreen';
import ControlsScreen from './screens/ControlsScreen';
import SettingsScreen from './screens/SettingsScreen';

const Stack = createNativeStackNavigator();
const Tab = createBottomTabNavigator();

export default function App() {
  return (
    <NavigationContainer>
      <Tab.Navigator
        screenOptions={{
          headerShown: true,
          tabBarStyle: styles.tabBar,
          tabBarActiveTintColor: '#FF6B00',
          tabBarInactiveTintColor: '#999',
        }}
      >
        <Tab.Screen
          name="Home"
          component={HomeScreen}
          options={{
            title: 'Doom Demo',
            tabBarLabel: 'Home',
          }}
        />
        <Tab.Screen
          name="Game"
          component={GameScreen}
          options={{
            title: 'Play',
            tabBarLabel: 'Play',
          }}
        />
        <Tab.Screen
          name="Controls"
          component={ControlsScreen}
          options={{
            title: 'Controls',
            tabBarLabel: 'Controls',
          }}
        />
        <Tab.Screen
          name="Settings"
          component={SettingsScreen}
          options={{
            title: 'Settings',
            tabBarLabel: 'Settings',
          }}
        />
      </Tab.Navigator>
    </NavigationContainer>
  );
}

const styles = StyleSheet.create({
  tabBar: {
    backgroundColor: '#1a1a1a',
    borderTopColor: '#333',
    borderTopWidth: 1,
  },
});
