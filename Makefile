CC = gcc

CFLAGS = -Iexternal/raylib/include -I src
LDFLAGS = external/raylib/lib/libraylib.a -lm -lpthread -g

TARGET = main
SRC = main.c src/*.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) $(CFLAGS) $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)

debug:
	gdb --args $(TARGET) ./roms/4-flags.ch8