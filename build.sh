#!/usr/bin/env bash
set -Eeuo pipefail

==========================================================

Mini DOOM Plus - OpenGL Edition

Build Script mejorado

==========================================================

PROJECT_NAME="Mini DOOM Plus - OpenGL Edition"
SRC_DIR="src"
BIN_DIR="bin"
OBJ_DIR="obj"
DIST_DIR="dist"
LOG_DIR="logs"

EXE_NAME="doom_plus_gl.exe"
DLL_NAME="game_dll.dll"

EXE_OUT="${BIN_DIR}/${EXE_NAME}"
DLL_OUT="${BIN_DIR}/${DLL_NAME}"

MODE="${1:-release}"
ACTION="${2:-build}"

Compilador: primero uno cross-compiler, luego gcc normal

CC="${CC:-}"
if [[ -z "${CC}" ]]; then
if command -v x86_64-w64-mingw32-gcc >/dev/null 2>&1; then
CC="x86_64-w64-mingw32-gcc"
elif command -v i686-w64-mingw32-gcc >/dev/null 2>&1; then
CC="i686-w64-mingw32-gcc"
elif command -v gcc >/dev/null 2>&1; then
CC="gcc"
else
echo "ERROR: No se encontró gcc ni mingw-w64-gcc en PATH."
exit 1
fi
fi

mkdir -p "$BIN_DIR" "$OBJ_DIR" "$DIST_DIR" "$LOG_DIR"

DLL_LOG="${LOG_DIR}/dll_build.log"
EXE_LOG="${LOG_DIR}/exe_build.log"

cleanup() {
local code=$?
if [[ $code -ne 0 ]]; then
echo
echo "===================================="
echo "BUILD FALLIDO"
echo "Revisa los logs en: ${LOG_DIR}"
echo "===================================="
fi
exit "$code"
}
trap cleanup EXIT

show_banner() {
echo
echo "===================================="
echo "$PROJECT_NAME"
echo "===================================="
echo
echo "Compilador: $CC"
echo "Modo: $MODE"
echo
}

check_sources() {
local missing=0
for f in 
"${SRC_DIR}/main.c" 
"${SRC_DIR}/game_dll.c" 
"${SRC_DIR}/doom_graphics.c" 
"${SRC_DIR}/opengl_render.c"
do
if [[ ! -f "$f" ]]; then
echo "ERROR: No existe el archivo: $f"
missing=1
fi
done
[[ $missing -eq 0 ]]
}

compile_common_flags() {
local cflags ldf
if [[ "$MODE" == "debug" ]]; then
cflags=(-Wall -Wextra -Wshadow -Wformat=2 -Wconversion -std=c11 -O0 -g3 -DDEBUG)
ldf=(-g3)
else
cflags=(-Wall -Wextra -Wshadow -Wformat=2 -Wconversion -std=c11 -O2 -DNDEBUG -flto)
ldf=(-O2 -s -flto)
fi

printf '%s\n' "${cflags[]}"
printf '%s\n' "${ldf[]}"
}

clean() {
echo "[CLEAN] Eliminando binarios, objetos y logs..."
rm -rf "$BIN_DIR" "$OBJ_DIR" "$DIST_DIR" "$LOG_DIR"
echo "[OK] Limpieza completada."
}

build_dll() {
echo "[1/2] Compilando DLL del juego..."
: > "$DLL_LOG"

local cflags ldf
mapfile -t _flags < <(compile_common_flags)
cflags="${_flags[0]}"
ldf="${_flags[1]}"

Objetos

"$CC" -c "${SRC_DIR}/game_dll.c" -o "${OBJ_DIR}/game_dll.o" 
-I"$SRC_DIR" $cflags >>"$DLL_LOG" 2>&1
"$CC" -c "${SRC_DIR}/doom_graphics.c" -o "${OBJ_DIR}/doom_graphics.o" 
-I"$SRC_DIR" $cflags >>"$DLL_LOG" 2>&1
"$CC" -c "${SRC_DIR}/opengl_render.c" -o "${OBJ_DIR}/opengl_render.o" 
-I"$SRC_DIR" $cflags >>"$DLL_LOG" 2>&1

Link DLL

"$CC" -shared -o "$DLL_OUT" 
"${OBJ_DIR}/game_dll.o" 
"${OBJ_DIR}/doom_graphics.o" 
"${OBJ_DIR}/opengl_render.o" 
-lm $ldf >>"$DLL_LOG" 2>&1

echo "DLL compilada exitosamente: $DLL_OUT"
}

build_exe() {
echo
echo "[2/2] Compilando ejecutable principal..."
: > "$EXE_LOG"

local cflags ldf
mapfile -t _flags < <(compile_common_flags)
cflags="${_flags[0]}"
ldf="${_flags[1]}"

"$CC" -c "${SRC_DIR}/main.c" -o "${OBJ_DIR}/main.o" 
-I"$SRC_DIR" $cflags >>"$EXE_LOG" 2>&1

"$CC" -o "$EXE_OUT" 
"${OBJ_DIR}/main.o" 
-lgdi32 -luser32 -lopengl32 -lglu32 -lm 
-mwindows $ldf >>"$EXE_LOG" 2>&1

echo "Ejecutable compilado exitosamente: $EXE_OUT"
}

package_release() {
echo
echo "[PACK] Creando paquete ZIP..."
local zip_name="MiniDOOMPlus_${MODE}.zip"
local zip_path="${DIST_DIR}/${zip_name}"

if command -v zip >/dev/null 2>&1; then
(cd "$BIN_DIR" && zip -r "../${zip_path}" . >/dev/null)
echo "Paquete creado: $zip_path"
else
echo "AVISO: 'zip' no está instalado, se omite el empaquetado."
fi
}

run_game() {
echo
echo "[RUN] Ejecutable listo:"
echo "  $EXE_OUT"
echo
if [[ -f "$EXE_OUT" ]]; then
echo "Para ejecutarlo en Windows:"
echo "  $EXE_OUT"
fi
}

rebuild() {
clean
mkdir -p "$BIN_DIR" "$OBJ_DIR" "$DIST_DIR" "$LOG_DIR"
build
}

build() {
show_banner
check_sources

if ! command -v "$CC" >/dev/null 2>&1; then
echo "ERROR: El compilador '$CC' no está disponible."
exit 1
fi

build_dll
build_exe

echo
echo "===================================="
echo "Compilación completada exitosamente"
echo "===================================="
echo
echo "DLL: $DLL_OUT"
echo "EXE: $EXE_OUT"
echo
}

case "${ACTION,,}" in
build)
build
;;
clean)
clean
;;
rebuild)
rebuild
;;
run)
build
run_game
;;
pack|package)
build
package_release
;;
*)
echo "Uso:"
echo "  ./build.sh [debug|release] [build|clean|rebuild|run|pack]"
echo
echo "Ejemplos:"
echo "  ./build.sh release build"
echo "  ./build.sh debug rebuild"
echo "  ./build.sh release run"
echo "  ./build.sh release pack"
exit 1
;;
esac