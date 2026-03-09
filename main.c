#include "external/raylib/include/raylib.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "src/chip_8.h"
#include "src/timer.h"
#include "src/debug.h"

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

int main(int argc, char *argv[])
{
    if (argc == 1 || argc > 4)
    {
        printf("Required:\n");
        printf("  * ROM\n");
        printf("  * (optional) -d\n");
        printf("  * CPU steps per sec\n");
        return 1;
    }

    char *romPath = argv[1];
    int debug = argc >= 3;
    int cpuSteps = argc == 4 ? atoi(argv[3]) : 700;

    if (debug)
    {
        // this probably is not good as these
        // are defined in a separate file...
        memArea = 350;
        regsArea = 300;
    }
    const int width = memArea + 64 * SCALE;
    const int height = regsArea + 32 * SCALE;

    InitWindow(width, height, "chip 8 emulator");
    InitAudioDevice();

    Wave wave = GenerateBeepWave(440.0f, 0.5f, 44100);
    Sound sound = LoadSoundFromWave(wave);

    int size = GetFileLength(romPath);
    unsigned char *program = LoadFileData(romPath, &size);

    Cpu cpu;
    CpuInit(&cpu, isChip8KeyDown);
    load(&cpu, program, size);

    int inst = -1;
    float acc1 = 0;
    float acc2 = 0;
    const float dtCpuSteps = 1.0f / cpuSteps;
    while (!WindowShouldClose())
    {
        int action = 0;
        if (debug)
            debugControls(&cpu, cpuSteps, program, size);
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
                action = step(&cpu);
                acc1 -= dtCpuSteps;
            }
        }

        if (IsKeyPressed(KEY_KP_0))
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

        if (debug)
            drawDebug(&cpu);

        drawBuffer(DARKGRAY, GOLD, cpu.v);
        // we need this drawing loop here as EndDrawing calls the input polling
        EndDrawing();
    }

    UnloadSound(sound);
    UnloadWave(wave);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}