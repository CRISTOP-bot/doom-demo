# Mini DOOM Plus - Compilados

Este directorio contiene los ejecutables compilados del proyecto Mini DOOM Plus con renderizado OpenGL.

## Archivos

- **doom_plus_gl.exe** - Ejecutable principal del juego (cuando se compila)
- **game_dll.dll** - Librería dinámica con lógica de juego y gráficos DOOM (cuando se compila)
- **BUILD.md** - Instrucciones detalladas de compilación

## Compilación Rápida

### Windows
```batch
..\build.bat
```

### Linux/Mac/Unix
```bash
../build.sh
```

## Ejecución

Después de compilar, ejecuta:

**Windows:**
```batch
doom_plus_gl.exe
```

**Linux/Mac/Unix:**
```bash
./doom_plus_gl.exe
```

## Requisitos para Compilar

- GCC (MinGW en Windows)
- OpenGL 1.1
- GLU (Graphics Utility Library)
- Librerías de desarrollo de Windows (gdi32, user32)

## Más Información

Ver `BUILD.md` en este directorio para instrucciones detalladas de compilación y resolución de problemas.
