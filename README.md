# Mini DOOM Plus

Proyecto estilo DOOM hecho en C con WinAPI y una DLL para la logica.

## Estructura

- src/ - codigo fuente
- bin/ - compilados

## Compilacion

DLL:
`gcc -shared -o game_dll.dll src\game_dll.c -lm`

EXE:
`gcc src\main.c -o doom_plus_dll.exe -mwindows -lgdi32 -luser32`

## Controles

- WASD: mover
- Mouse: mirar
- TAB: bloquear o desbloquear mouse
- ESPACIO: disparar
- E: abrir puertas
- R: reiniciar al perder
