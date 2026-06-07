#!/bin/bash
# Script de compilación para Mini DOOM Plus - OpenGL Edition
# Compilador requerido: GCC

echo ""
echo "===================================="
echo "Mini DOOM Plus - Build Script"
echo "===================================="
echo ""

# Crear carpeta bin si no existe
mkdir -p bin

echo "[1/2] Compilando DLL del juego..."
gcc -shared -o bin/game_dll.dll src/game_dll.c src/doom_graphics.c src/opengl_render.c -lm
if [ $? -ne 0 ]; then
    echo "ERROR: No se pudo compilar la DLL"
    exit 1
fi
echo "DLL compilada exitosamente: bin/game_dll.dll"

echo ""
echo "[2/2] Compilando ejecutable principal..."
gcc src/main.c -o bin/doom_plus_gl.exe -mwindows -lgdi32 -luser32 -lopengl32 -lglu32
if [ $? -ne 0 ]; then
    echo "ERROR: No se pudo compilar el ejecutable"
    exit 1
fi
echo "Ejecutable compilado exitosamente: bin/doom_plus_gl.exe"

echo ""
echo "===================================="
echo "Compilación completada exitosamente"
echo "===================================="
echo ""
echo "Para ejecutar el juego:"
echo "  ./bin/doom_plus_gl.exe"
echo ""
