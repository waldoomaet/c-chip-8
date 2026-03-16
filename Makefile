CC = gcc
EMCC = emcc

# Source files
SRC = main.c src/*.c

# Include and library paths
CFLAGS = -I$(HOME)/.local/include -I src
LDFLAGS = -L$(HOME)/.local/lib -lraylib -lm -lpthread -ldl -lrt -lX11 -Wall

TARGET = chip_8

WEB_BUILD_DIR = web_build
WEB_TARGET = $(WEB_BUILD_DIR)/chip_8.html

all: $(TARGET)

$(WEB_BUILD_DIR):
	mkdir -p $(WEB_BUILD_DIR)

$(TARGET): $(SRC)
	$(CC) $(SRC) $(CFLAGS) $(LDFLAGS) -o $(TARGET)

web: $(SRC) | $(WEB_BUILD_DIR)
	$(EMCC) -o $(WEB_TARGET) $(SRC) -Os -Wall \
		$$HOME/.local/lib/libraylib.web.a \
		-I. -I$$HOME/.local/include \
		-L. -L$$HOME/.local/lib -lraylib.web \
		-s USE_GLFW=3 \
		-s WASM=1 \
		-s GL_ENABLE_GET_PROC_ADDRESS \
		-s EXPORT_ES6=1 \
		--shell-file src/minshell.html \
		-DPLATFORM_WEB

clean:
	rm -f $(TARGET) $(WEB_TARGET) game.js game.wasm

debug:
	gdb --args $(TARGET) ./roms/4-flags.ch8