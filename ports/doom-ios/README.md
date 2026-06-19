# Doom Demo - iOS Edition

Doom Demo portado a iOS usando React Native con integración de código C nativo.

## 🚀 Características

- ✅ Interfaz moderna con React Native
- ✅ Navegación por tabs (Home, Play, Controls, Settings)
- ✅ Controles táctiles optimizados para iOS
- ✅ Soporte para Metal rendering
- ✅ Integración con código C nativo (Objective-C bridge)
- ✅ Haptic feedback
- ✅ Dark mode nativo

## 📋 Requisitos

- macOS 12+
- Xcode 14+
- Node.js 18+
- CocoaPods
- iOS 13+

## 🔧 Instalación

\`\`\`bash
npm install
cd ios && pod install && cd ..
npm run ios
\`\`\`

## 📁 Estructura del Proyecto

\`\`\`
doom-ios/
├── src/
│   ├── App.tsx
│   ├── screens/
│   │   ├── HomeScreen.tsx
│   │   ├── GameScreen.tsx
│   │   ├── ControlsScreen.tsx
│   │   └── SettingsScreen.tsx
├── ios/
├── native/
└── package.json
\`\`\`

## 🎮 Controles

- **Lado Izquierdo:** Joystick virtual
- **Lado Derecho:** Botones de acción
- **Gestos:** Swipe para cambiar armas

## 📊 Especificaciones

| Aspecto | Detalle |
|--------|---------|
| Framework | React Native 0.73 |
| Lenguaje | TypeScript 5.0 |
| Rendering | Metal (iOS) |
| Navegación | React Navigation 6 |

---

**Versión:** 1.0.0 | **Licencia:** MIT | **iOS:** 13+
