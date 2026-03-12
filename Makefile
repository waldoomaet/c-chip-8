CC = gcc
EMCC = emcc

# Source files
SRC = main.c src/*.c

# Include and library paths
CFLAGS = -I$(HOME)/.local/include -I src
LDFLAGS = -L$(HOME)/.local/lib -lraylib -lm -lpthread -ldl -lrt -lX11 -Wall

TARGET = main
WEB_TARGET = chip_8.html

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) $(CFLAGS) $(LDFLAGS) -o $(TARGET)

web: $(SRC)
	$(EMCC) -o $(WEB_TARGET) $(SRC) -Os -Wall \
		$$HOME/.local/lib/libraylib.web.a \
		-I. -I$$HOME/.local/include \
		-L. -L$$HOME/.local/lib -lraylib.web \
		-s USE_GLFW=3 \
		--shell-file $$HOME/.local/src/raylib/src/minshell.html \
		-DPLATFORM_WEB

clean:
	rm -f $(TARGET) $(WEB_TARGET) game.js game.wasm

debug:
	gdb --args $(TARGET) ./roms/4-flags.ch8