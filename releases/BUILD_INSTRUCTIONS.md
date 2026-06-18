# 📦 Instrucciones de Compilación

## Compilación Completa del Proyecto

### Requisitos Globales

```bash
# Instalar Node.js 18+
curl -fsSL https://deb.nodesource.com/setup_18.x | sudo -E bash -
sudo apt-get install -y nodejs

# Instalar pnpm
npm install -g pnpm

# Instalar herramientas de compilación
sudo apt-get install -y build-essential git
```

---

## 1️⃣ Android (APK + AAB)

### Requisitos Específicos

```bash
# Android SDK
export ANDROID_SDK_ROOT=$HOME/Android/Sdk
export ANDROID_NDK_HOME=$ANDROID_SDK_ROOT/ndk/24.0.8215888
export PATH=$PATH:$ANDROID_SDK_ROOT/tools:$ANDROID_NDK_HOME
```

### Compilación

```bash
cd ports/doom-android

# Instalar dependencias
pnpm install

# Compilar APK Debug
cd android
./gradlew assembleDebug
# Resultado: app/build/outputs/apk/debug/app-debug.apk

# Compilar APK Release
./gradlew assembleRelease
# Resultado: app/build/outputs/apk/release/app-release.apk

# Compilar AAB (Android App Bundle)
./gradlew bundleRelease
# Resultado: app/build/outputs/bundle/release/app-release.aab
```

### Firma de APK

```bash
# Crear keystore (una sola vez)
keytool -genkey -v -keystore doom-key.keystore \
  -keyalg RSA -keysize 2048 -validity 10000 -alias doom

# Firmar APK
jarsigner -verbose -sigalg SHA1withRSA -digestalg SHA1 \
  -keystore doom-key.keystore \
  app-release-unsigned.apk doom
```

---

## 2️⃣ Linux (AppImage + DEB)

### Requisitos Específicos

```bash
# Herramientas de compilación
sudo apt-get install -y gcc g++ make cmake

# Dependencias de Electron
sudo apt-get install -y libgtk-3-0 libxss1 libgconf-2-4
```

### Compilación

```bash
cd ports/doom-linux

# Instalar dependencias
pnpm install

# Compilar módulo nativo
pnpm build:native

# Compilar todo
pnpm build

# Generar distribuciones
pnpm dist
# Resultado: 
#   - dist/Doom-1.0.0.AppImage
#   - dist/doom_1.0.0_amd64.deb
```

### Instalación desde DEB

```bash
sudo dpkg -i dist/doom_1.0.0_amd64.deb
doom  # Ejecutar
```

---

## 3️⃣ Windows (EXE)

### Requisitos Específicos

```
- Visual Studio 2019+ o MinGW
- Windows SDK
- OpenGL drivers
```

### Compilación

```batch
cd src
make
REM Resultado: doom.exe
```

---

## 🔧 Compilación Optimizada

### Android (Release Optimizado)

```bash
cd ports/doom-android/android

# Compilar con optimizaciones
./gradlew assembleRelease \
  -x test \
  --parallel \
  --max-workers=4 \
  -Dorg.gradle.jvmargs="-Xmx4096m"
```

### Linux (Release Optimizado)

```bash
cd ports/doom-linux

# Compilar con optimizaciones
pnpm build -- \
  --mode production \
  --minify terser
```

---

## 📊 Tamaños Esperados

| Plataforma | Archivo | Tamaño |
|-----------|---------|--------|
| Android | app-debug.apk | 45 MB |
| Android | app-release.apk | 38 MB |
| Android | app-release.aab | 35 MB |
| Linux | Doom-1.0.0.AppImage | 120 MB |
| Linux | doom_1.0.0_amd64.deb | 85 MB |
| Windows | doom.exe | 95 MB |

---

## ✅ Verificación Post-Compilación

### Android

```bash
# Verificar APK
aapt dump badging app-release.apk

# Instalar y probar
adb install -r app-release.apk
adb shell am start -n com.example.doom/.MainActivity
```

### Linux

```bash
# Verificar AppImage
./Doom-1.0.0.AppImage --version

# Instalar desde DEB
sudo dpkg -i doom_1.0.0_amd64.deb
doom
```

### Windows

```batch
doom.exe
REM Verificar que la ventana se abre correctamente
```

---

## 🚀 CI/CD Automatizado

### GitHub Actions (Ejemplo)

```yaml
name: Build Releases

on:
  push:
    tags:
      - 'v*'

jobs:
  build-android:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - uses: actions/setup-node@v3
        with:
          node-version: '18'
      - run: cd ports/doom-android/android && ./gradlew assembleRelease
      - uses: actions/upload-artifact@v3
        with:
          name: android-release
          path: ports/doom-android/android/app/build/outputs/apk/release/

  build-linux:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - uses: actions/setup-node@v3
        with:
          node-version: '18'
      - run: cd ports/doom-linux && pnpm install && pnpm dist
      - uses: actions/upload-artifact@v3
        with:
          name: linux-release
          path: ports/doom-linux/dist/
```

---

## 🐛 Troubleshooting

### Android

**Error: NDK not found**
```bash
export ANDROID_NDK_HOME=/path/to/ndk
```

**Error: Gradle build failed**
```bash
./gradlew clean
./gradlew assembleDebug
```

### Linux

**Error: Module not found**
```bash
pnpm build:native
```

**Error: Permission denied**
```bash
chmod +x Doom-1.0.0.AppImage
./Doom-1.0.0.AppImage
```

---

**Última actualización:** Junio 2026
