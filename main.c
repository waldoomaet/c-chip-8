#include <raylib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "src/chip_8.h"
#include "src/debug.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#define SCALE 13

int isChip8KeyDown()
{
    if (IsKeyDown(KEY_ONE))
        return 1;
    if (IsKeyDown(KEY_TWO))
        return 2;
    if (IsKeyDown(KEY_THREE))
        return 3;
    if (IsKeyDown(KEY_FOUR))
        return 0xc;

    if (IsKeyDown(KEY_Q))
        return 4;
    if (IsKeyDown(KEY_W))
        return 5;
    if (IsKeyDown(KEY_E))
        return 6;
    if (IsKeyDown(KEY_R))
        return 0xd;

    if (IsKeyDown(KEY_A))
        return 7;
    if (IsKeyDown(KEY_S))
        return 8;
    if (IsKeyDown(KEY_D))
        return 9;
    if (IsKeyDown(KEY_F))
        return 0xe;

    if (IsKeyDown(KEY_Z))
        return 0xa;
    if (IsKeyDown(KEY_X))
        return 0;
    if (IsKeyDown(KEY_C))
        return 0xb;
    if (IsKeyDown(KEY_V))
        return 0xf;

    return -1;
}

void drawBuffer(Color background, Color pixel, uint8_t buffer[][32])
{
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            DrawRectangle(memArea + i * SCALE, regsArea + j * SCALE, SCALE, SCALE, buffer[i][j] ? pixel : background);
        }
    }
}

Wave GenerateBeepWave(float frequency, float duration, int sampleRate)
{
    int sampleCount = (int)(sampleRate * duration);
    short *data = (short *)MemAlloc(sampleCount * sizeof(short));

    for (int i = 0; i < sampleCount; i++)
    {
        float t = (float)i / sampleRate;
        data[i] = (short)(32000 * sinf(2 * PI * frequency * t));
    }

    Wave wave = {
        .frameCount = sampleCount,
        .sampleRate = sampleRate,
        .sampleSize = 16,
        .channels = 1,
        .data = data};

    return wave;
}

typedef struct
{
    char *romPath;
    int debug;
    int cpuSteps;
} Args;

int parseArgs(int argc, char **argv, Args *args)
{
    args->romPath = NULL;
    args->debug = 0;
    args->cpuSteps = 700;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-d") == 0)
        {
            args->debug = 1;
        }
        else if (strcmp(argv[i], "-s") == 0)
        {
            if (i + 1 >= argc)
            {
                fprintf(stderr, "Missing value after -s\n");
                return 0;
            }

            args->cpuSteps = atoi(argv[++i]);
        }
        else if (args->romPath == NULL)
        {
            args->romPath = argv[i];
        }
        else
        {
            fprintf(stderr, "Unknown argument: %s\n", argv[i]);
            return 0;
        }
    }

    if (args->romPath == NULL)
    {
        fprintf(stderr, "ROM path required\n");
        return 0;
    }

    return 1;
}

Args args;
Cpu cpu;
Sound sound;
float acc1 = 0;
float acc2 = 0;
float dtCpuSteps;
int size;
unsigned char *program;
void UpdateDrawFrame(void)
{
    if (args.debug)
        debugControls(&cpu, args.cpuSteps, program, size);
    else
    {
        float dt = GetFrameTime();

        acc2 += dt;
        while (acc2 >= DT_TIMER_REFRESH)
        {
            tick(&cpu);
            acc2 -= DT_TIMER_REFRESH;
        }

        acc1 += dt;
        while (acc1 >= dtCpuSteps)
        {
            step(&cpu);
            acc1 -= dtCpuSteps;
        }
    }

    if (IsKeyPressed(KEY_P))
    {
        cpuReset(&cpu);
        load(&cpu, program, size);
        acc1 = 0;
        acc2 = 0;
    }

    if (doBeep(&cpu))
        PlaySound(sound);
    else
        StopSound(sound);

    BeginDrawing();

    ClearBackground(DARKGRAY);

    if (args.debug)
        drawDebug(&cpu);

    drawBuffer(DARKGRAY, GOLD, cpu.v);
    // we need this drawing loop here as EndDrawing calls the input polling
    EndDrawing();
}

int main(int argc, char *argv[])
{

#if defined(PLATFORM_WEB)
    args.debug = 0;
    args.cpuSteps = 700;
    args.romPath = "./roms/pong.ch8";
#else
    if (!parseArgs(argc, argv, &args))
    {
        printf("Usage:\n");
        printf("  emulator <ROM> [-d] [-s cpu_steps_per_sec]\n");
        return 1;
    }
#endif
    if (args.debug == 0)
    {
        // this probably is not good as these
        // are defined in a separate file...
        memArea = 0;
        regsArea = 0;
    }
    const int width = memArea + 64 * SCALE;
    const int height = regsArea + 32 * SCALE;

    dtCpuSteps = 1.0f / args.cpuSteps;

    InitWindow(width, height, "chip 8 emulator");
    InitAudioDevice();

    Wave wave = GenerateBeepWave(440.0f, 0.5f, 44100);
    sound = LoadSoundFromWave(wave);

    CpuInit(&cpu, isChip8KeyDown);
#if defined(PLATFORM_WEB)
    unsigned char source[] = {0x6a, 0x2, 0x6b, 0xc, 0x6c, 0x3f, 0x6d, 0xc, 0xa2, 0xea, 0xda, 0xb6, 0xdc, 0xd6, 0x6e, 0x0, 0x22, 0xd4, 0x66, 0x3, 0x68, 0x2, 0x60, 0x60, 0xf0, 0x15, 0xf0, 0x7, 0x30, 0x0, 0x12, 0x1a, 0xc7, 0x17, 0x77, 0x8, 0x69, 0xff, 0xa2, 0xf0, 0xd6, 0x71, 0xa2, 0xea, 0xda, 0xb6, 0xdc, 0xd6, 0x60, 0x1, 0xe0, 0xa1, 0x7b, 0xfe, 0x60, 0x4, 0xe0, 0xa1, 0x7b, 0x2, 0x60, 0x1f, 0x8b, 0x2, 0xda, 0xb6, 0x8d, 0x70, 0xc0, 0xa, 0x7d, 0xfe, 0x40, 0x0, 0x7d, 0x2, 0x60, 0x0, 0x60, 0x1f, 0x8d, 0x2, 0xdc, 0xd6, 0xa2, 0xf0, 0xd6, 0x71, 0x86, 0x84, 0x87, 0x94, 0x60, 0x3f, 0x86, 0x2, 0x61, 0x1f, 0x87, 0x12, 0x46, 0x2, 0x12, 0x78, 0x46, 0x3f, 0x12, 0x82, 0x47, 0x1f, 0x69, 0xff, 0x47, 0x0, 0x69, 0x1, 0xd6, 0x71, 0x12, 0x2a, 0x68, 0x2, 0x63, 0x1, 0x80, 0x70, 0x80, 0xb5, 0x12, 0x8a, 0x68, 0xfe, 0x63, 0xa, 0x80, 0x70, 0x80, 0xd5, 0x3f, 0x1, 0x12, 0xa2, 0x61, 0x2, 0x80, 0x15, 0x3f, 0x1, 0x12, 0xba, 0x80, 0x15, 0x3f, 0x1, 0x12, 0xc8, 0x80, 0x15, 0x3f, 0x1, 0x12, 0xc2, 0x60, 0x20, 0xf0, 0x18, 0x22, 0xd4, 0x8e, 0x34, 0x22, 0xd4, 0x66, 0x3e, 0x33, 0x1, 0x66, 0x3, 0x68, 0xfe, 0x33, 0x1, 0x68, 0x2, 0x12, 0x16, 0x79, 0xff, 0x49, 0xfe, 0x69, 0xff, 0x12, 0xc8, 0x79, 0x1, 0x49, 0x2, 0x69, 0x1, 0x60, 0x4, 0xf0, 0x18, 0x76, 0x1, 0x46, 0x40, 0x76, 0xfe, 0x12, 0x6c, 0xa2, 0xf2, 0xfe, 0x33, 0xf2, 0x65, 0xf1, 0x29, 0x64, 0x14, 0x65, 0x0, 0xd4, 0x55, 0x74, 0x15, 0xf2, 0x29, 0xd4, 0x55, 0x0, 0xee, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0};
    program = source;
    size = sizeof(source);
    load(&cpu, program, size);
#else
    size = GetFileLength(args.romPath);
    program = LoadFileData(args.romPath, &size);
    load(&cpu, program, size);
#endif

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        UpdateDrawFrame();
    }
#endif

    UnloadSound(sound);
    UnloadWave(wave);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}