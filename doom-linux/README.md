# Doom Demo - Linux Desktop Edition

Un porte del clásico juego DOOM a Linux como aplicación de escritorio usando **Electron**, **React** y **C nativo**.

## 🎮 Características

- **Interfaz Electron + React:** UI moderna y responsiva
- **Motor C Nativo:** Lógica del juego compilada en C para máximo rendimiento
- **Controles Keyboard/Mouse:** Controles clásicos de FPS
- **Renderizado Software:** Sistema de renderizado eficiente
- **HUD Dinámico:** Salud, munición, puntuación en tiempo real
- **IA de Enemigos:** Enemigos con comportamiento básico
- **Empaquetado:** Distribución como AppImage y .deb

## 🚀 Requisitos

- Linux (Ubuntu 20.04+ recomendado)
- Node.js 18+
- pnpm 9+
- GCC/G++ 9+
- build-essential

## 📦 Instalación

```bash
# Instalar dependencias
pnpm install

# Compilar módulo nativo
pnpm build:native
```

## 🎯 Desarrollo

```bash
# Ejecutar en modo desarrollo
pnpm dev

# O ejecutar componentes por separado:
pnpm dev:react      # Terminal 1: React dev server en puerto 3000
pnpm dev:electron   # Terminal 2: Electron app
```

## 🏗️ Compilación

```bash
# Compilar todo
pnpm build

# Generar distribuciones
pnpm dist
```

Los archivos compilados estarán en `dist/`:
- `Doom-1.0.0.AppImage` - Ejecutable portable
- `doom_1.0.0_amd64.deb` - Paquete Debian

## 🎮 Controles

| Tecla | Acción |
|-------|--------|
| **W/A/S/D** | Movimiento |
| **Mouse** | Mirar alrededor |
| **Click Izquierdo** | Disparar |
| **E** | Abrir puerta |
| **P** | Pausar |
| **ESC** | Menú |

## 📁 Estructura del Proyecto

```
doom-linux/
├── src/
│   ├── main.ts                 # Proceso principal de Electron
│   ├── preload.ts              # Preload script
│   ├── App.tsx                 # Componente principal
│   ├── App.css                 # Estilos
│   ├── index.tsx               # Entry point React
│   └── components/
│       ├── MainMenu.tsx        # Menú principal
│       ├── GameRenderer.tsx    # Renderizador del juego
│       └── GameControls.tsx    # Overlay de controles
├── native/
│   ├── doom_core.h             # Headers compartidos
│   ├── game_logic.c            # Lógica del juego
│   └── doom_binding.cc         # Binding de Node.js
├── public/
│   └── index.html              # HTML principal
├── package.json
├── binding.gyp                 # Configuración de compilación nativa
├── tsconfig.json
├── vite.config.ts
└── README.md
```

## 🔧 Desarrollo del Motor C

El motor está en `native/game_logic.c`:

- **game_init()** - Inicializa el juego
- **game_update()** - Actualiza lógica (entrada, IA, física)
- **game_render()** - Renderiza frame a buffer
- **game_cleanup()** - Limpia recursos

### Estructura de Datos

```c
typedef struct {
  float x, y, z;
  float angleX, angleY;
  int health, ammo, score;
} Player;

typedef struct {
  float x, y, z;
  int health;
  bool active;
} Enemy;
```

## 📊 Rendimiento

- **FPS Objetivo:** 60 FPS
- **Resolución:** 1280x960
- **Máximo de Enemigos:** 64
- **Máximo de Proyectiles:** 256

## 🐛 Debugging

### Logs de Electron

```bash
# Ver logs en tiempo real
journalctl -u doom -f

# O ejecutar con logs en consola
ELECTRON_ENABLE_LOGGING=1 pnpm dev:electron
```

### DevTools

Presiona `Ctrl+Shift+I` durante el juego para abrir DevTools.

## 📦 Distribución

### Generar AppImage

```bash
pnpm dist
# Resultado: dist/Doom-1.0.0.AppImage
```

### Generar .deb

```bash
pnpm dist
# Resultado: dist/doom_1.0.0_amd64.deb
```

### Instalar desde .deb

```bash
sudo dpkg -i dist/doom_1.0.0_amd64.deb
doom
```

## 🔐 Seguridad

- Preload script con `contextIsolation: true`
- No hay `nodeIntegration` habilitada
- IPC para comunicación segura

## 🚀 Optimizaciones Futuras

1. **OpenGL ES:** Reemplazar renderizado software con OpenGL
2. **Sonido:** Agregar OpenAL para efectos y música
3. **Texturas:** Sistema de carga de texturas
4. **Multijugador:** Soporte de red
5. **Shaders:** Sistema de shaders para efectos visuales

## 📝 Licencia

Proyecto educativo - Porte de DOOM clásico

## 👨‍💻 Autor

Portado a Linux por el equipo de desarrollo

---

**Última actualización:** Junio 2026
