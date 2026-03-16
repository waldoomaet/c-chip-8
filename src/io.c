#include "io.h"

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