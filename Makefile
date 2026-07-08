# ============================================================
# Mini DOOM Plus - OpenGL Edition
# Linux Makefile
# ============================================================

PROJECT := doom_plus_gl

CC      := gcc
MODE    ?= release

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

TARGET := $(BIN_DIR)/$(PROJECT)

# ------------------------------------------------------------
# Buscar automáticamente todos los .c
# ------------------------------------------------------------

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

# ------------------------------------------------------------
# Flags
# ------------------------------------------------------------

WARNINGS := \
    -Wall \
    -Wextra \
    -Wpedantic \
    -Wshadow \
    -Wconversion \
    -Wformat=2

COMMON := \
    $(WARNINGS) \
    -std=c11 \
    -I$(SRC_DIR)

ifeq ($(MODE),debug)
CFLAGS := $(COMMON) -O0 -g3 -DDEBUG
LDFLAGS :=
else
CFLAGS := $(COMMON) -O3 -DNDEBUG -flto -march=native
LDFLAGS := -flto -s
endif

# ------------------------------------------------------------
# Librerías Linux
# ------------------------------------------------------------

LIBS := \
    -lGL \
    -lGLU \
    -lX11 \
    -lm

# ============================================================

.PHONY: all debug release clean rebuild run install uninstall info

all: $(TARGET)

debug:
	$(MAKE) MODE=debug

release:
	$(MAKE) MODE=release

info:
	@echo "Proyecto : $(PROJECT)"
	@echo "Modo     : $(MODE)"
	@echo "Compilador: $(CC)"

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "[CC] $<"
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(TARGET): $(OBJS) | $(BIN_DIR)
	@echo "[LD] $@"
	$(CC) $(OBJS) -o $@ $(LIBS) $(LDFLAGS)

run: all
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(BIN_DIR)

rebuild: clean all

install: all
	install -Dm755 $(TARGET) /usr/local/bin/$(PROJECT)

uninstall:
	rm -f /usr/local/bin/$(PROJECT)

-include $(DEPS)