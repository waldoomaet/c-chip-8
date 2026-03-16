# CHIP8 Emulator

Mostly compliant CHIP8 emulator/debug tools written in C. Followed the [this guide](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/). 

Roms can be found in these repos:

- https://github.com/kripod/chip8-roms

- https://github.com/Timendus/chip8-test-suite

By default the emulator runs in normal mode at 700 steps (or cycles) per second. It also contains a debug mode that allows to step through the CPU's cycles, and showcases info such as the memory and stack contents, registers, timers, PC and I register. 

There's some other tools written in python that:

- create a .chip8 file from a list of bytes
- dissembles (VERY CRUDELY) a .chip8 into a more readable format
- hardcodes a .chip8 file into a C array 

# Source build

[Raylib](https://github.com/raysan5/raylib) must be installed (and the makefile may need slight modifications to account for raylib's installation path). 

Follow [this guide](https://github.com/raysan5/raylib/wiki/Working-for-Web-(HTML5)) to get raylib to compile to wasm (this may also need some slight modification of the makefile).

``` bash
# for native desktop
make

# for wasm in the web
# generates a build dir with all the required html/wasm/js
# can then be served with something like python -m http.server 8080
make web
```

# How it works

For a native build run:
``` bash
./chip_8 <ROM> [-d] [-s cpu_steps_per_sec]
```

The d flag launches the emulator in debug mode.

For a web build you can toggle between normal mode and debug mode with the o key.

During debug mode press the right key to go perform a single cycle of the CHIP8. Press and hold the down key to execute cycles for as long as the down key is held down. 

Both the native and web builds share the following controls:

- p key: restart the emulator

- 1 key: 1 in the CHIP8 keypad
- 2 key: 2 in the CHIP8 keypad
- 3 key: 3 in the CHIP8 keypad
- 4 key: c in the CHIP8 keypad

- q key: 4 in the CHIP8 keypad
- w key: 5 in the CHIP8 keypad
- e key: 6 in the CHIP8 keypad
- r key: d in the CHIP8 keypad

- a key: 7 in the CHIP8 keypad
- s key: 8 in the CHIP8 keypad
- d key: 9 in the CHIP8 keypad
- f key: e in the CHIP8 keypad

- z key: a in the CHIP8 keypad
- x key: 0 in the CHIP8 keypad
- c key: b in the CHIP8 keypad
- v key: f in the CHIP8 keypad

# Limitations

- Some games seem to not work 100% well, which means that there's some incorrect behaviors of the CPU

- The emulator only implements the quirks of the base CHIP8 (so no SUPER-CHIP or XO-CHIP support)

- As the web build just came as an afterthought it can only play a hardcoded pong rom.\

- The debug mode in the web build works, but it's slow. 

- There's no sound in the web build

- The displaying on canvas could also be improved for normal execution.