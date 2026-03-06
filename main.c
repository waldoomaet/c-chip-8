#include "external/raylib/include/raylib.h"
#include <stdio.h>
#include "src/chip_8.h"
#include "src/timer.h"

#define SCALE 13
#define REFRESH 500

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

void draw(Color background, Color pixel, unsigned char buffer[][32])
{
    BeginDrawing();
    ClearBackground(background);
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            DrawRectangle(i * SCALE, j * SCALE, SCALE, SCALE, buffer[i][j] ? pixel : background);
        }
    }
    EndDrawing();
}

int main(int argc, char *argv[])
{
    if (argc == 1 || argc > 2)
    {
        printf("Required:\n");
        printf("  * ROM\n");
        return 1;
    }
    const int width = 64 * SCALE;
    const int height = 32 * SCALE;

    InitWindow(width, height, "chip 8 emulator");
    SetWindowMonitor(1);
    SetTargetFPS(60);

    int size = GetFileLength(argv[1]);

    Cpu cpu;
    CpuInit(&cpu, isChip8KeyDown);
    load(&cpu, LoadFileData(argv[1], &size), size);

    Args args;
    args.cpu = &cpu;
    args.tick = tick;
    startTimerThread(&args);

    int count = 0;
    while (!WindowShouldClose())
    {
        switch (step(&cpu))
        {
        case 1:
            draw(RAYWHITE, BLUE, cpu.v);
            break;
        default:
            // this is just to keep the window responsive
            count++;
            if (count == REFRESH)
            {
                draw(RAYWHITE, BLUE, cpu.v);
                count = 0;
            }
            break;
        }
    }

    CloseWindow();

    return 0;
}