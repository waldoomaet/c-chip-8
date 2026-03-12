#include "debug.h"

int regsArea = 300;
int memArea = 350;
int scroll = 0;

const char *instString(uint16_t inst)
{
    int instl1 = (inst & 0xF000) >> 12;
    int insth4 = inst & 0x000F;
    int b2 = inst & 0xFF;

    switch (instl1)
    {
    case 0:
        if (inst == 0xE0)
            return "Clear screen";
        if (inst == 0xEE)
            return "Return from subroutine";
        break;

    case 1:
        return "Jump to address";

    case 2:
        return "Call subroutine";

    case 3:
        return "Skip if Vx == nn";

    case 4:
        return "Skip if Vx != nn";

    case 5:
        return "Skip if Vx == Vy";

    case 6:
        return "Vx = nn";

    case 7:
        return "Vx += nn";

    case 8:
        switch (insth4)
        {
        case 0:
            return "Vx = Vy";
        case 1:
            return "Vx |= Vy";
        case 2:
            return "Vx &= Vy";
        case 3:
            return "Vx ^= Vy";
        case 4:
            return "Vx += Vy (with carry)";
        case 5:
            return "Vx = Vx - Vy (with carry)";
        case 6:
            return "Vx >>= 1";
        case 7:
            return "Vx = Vy - Vx (with carry)";
        case 0xE:
            return "Vx <<= 1";
        default:
            return "Unknown 8-series instruction";
        }

    case 9:
        return "Skip if Vx != Vy";

    case 0xA:
        return "I = address";

    case 0xB:
        return "Jump with V0 offset";

    case 0xC:
        return "Vx = Random & nn";

    case 0xD:
        return "Display sprite";

    case 0xE:
        if (b2 == 0x9E)
            return "Skip if key pressed";
        if (b2 == 0xA1)
            return "Skip if key not pressed";
        break;

    case 0xF:
        switch (inst & 0xFF)
        {
        case 0x07:
            return "Vx = delay timer";
        case 0x15:
            return "delay timer = Vx";
        case 0x18:
            return "sound timer = Vx";
        case 0x1E:
            return "I += Vx";
        case 0x0A:
            return "Wait for keypress";
        case 0x29:
            return "Load font for Vx into I";
        case 0x33:
            return "BCD of Vx";
        case 0x55:
            return "Store V0..Vx in memory";
        case 0x65:
            return "Load V0..Vx from memory";
        default:
            return "Unknown F-series instruction";
        }
    }

    return "Unknown instruction";
}

void drawDebug(Cpu *cpu)
{
    int size = 20;
    int currY = 10;
    Color color = RAYWHITE;

    for (int i = 0; i < 35; i++)
    {
        int iOffset = (i + scroll) * 8;
        if (iOffset + 8 < 4096)
        {
            DrawText(TextFormat("%04x: ", iOffset), 10, currY + i * 20, size, color);
            // DrawText(TextFormat("%02x %02x %02x %02x %02x %02x %02x %02x", cpu->m[iOffset], cpu->m[iOffset + 1], cpu->m[iOffset + 2], cpu->m[iOffset + 3], cpu->m[iOffset + 4], cpu->m[iOffset + 5], cpu->m[iOffset + 6], cpu->m[iOffset + 7]), 80, currY + i * 20, size, color);
            for (int j = 0; j < 8; j++)
            {
                Color memColor = color;
                if (iOffset + j == cpu->pc || iOffset + j - 1 == cpu->pc)
                    memColor = RED;
                DrawText(TextFormat("%02x", cpu->m[iOffset + j]),
                         80 + j * 30,
                         currY + i * 20,
                         size,
                         memColor);
            }
        }
    }

    int currX = memArea + 10;

    // DrawText(TextFormat("Executed inst: %x", inst), currX, currY, size, color);

    uint16_t nextInst = join(cpu->m[cpu->pc], cpu->m[cpu->pc + 1]);
    const char *text1 = "Next inst: %x - ";
    const char *text2 = instString(nextInst);
    char *concat = malloc(strlen(text1) + strlen(text2) + 1);
    strcpy(concat, text1);
    strcat(concat, text2);
    DrawText(TextFormat(concat, nextInst), currX + 300, currY, size, color);
    free(concat);

    currY += 30;

    DrawText(TextFormat("PC: 0x%x", cpu->pc), currX, currY, 20, color);
    DrawText(TextFormat("I: 0x%x", cpu->i), currX + 110, currY, 20, color);
    DrawText(TextFormat("DT: 0x%x", cpu->dt), currX + 200, currY, 20, color);
    DrawText(TextFormat("ST: 0x%x", cpu->st), currX + 300, currY, 20, color);

    currY += 50;

    for (int i = 0; i < 16; i++)
    {
        if (i < 8)
            DrawText(TextFormat("V%x: %x", i, cpu->gp[i]), currX + i * 70, currY, 20, color);
        else
            DrawText(TextFormat("V%x: %x", i, cpu->gp[i]), currX + (i - 8) * 70, currY + 30, 20, color);
    }

    currY += 80;

    for (int i = 0; i < 16; i++)
    {
        if (i < 8)
            DrawText(TextFormat("S%x: %x", i, cpu->s.m[i]), currX + i * 90, currY, 20, color);
        else
            DrawText(TextFormat("S%x: %x", i, cpu->s.m[i]), currX + (i - 8) * 90, currY + 30, 20, color);
    }
}

int debugControls(Cpu *cpu, int cpuSteps, unsigned char *insts, int size)
{
    int action = 0;
    int wheel = (int)GetMouseWheelMove();
    if (wheel == 1)
        scroll = scroll == 0 ? 0 : scroll - 1;
    if (wheel == -1)
        scroll++;

    if (IsKeyPressed(KEY_RIGHT) || IsKeyDown(KEY_DOWN))
    {
        if (cpu->dt > 0 || cpu->st > 0)
        {
            int refreshRate = ceil(cpuSteps / TIMER_REFRESH);
            if (cpu->_instCount % refreshRate == 0)
                tick(cpu);
        }
        action = step(cpu);
    }
    return action;
}