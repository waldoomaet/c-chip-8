#include "chip_8.h"

void CpuInit(Cpu *cpu, int (*keyDown)())
{
    cpu->keyDown = keyDown;
    cpu->pc = 0x200;
    cpu->i = 0;
    cpu->dt = 0;
    cpu->st = 0;
    memset(cpu->v, 0, sizeof(cpu->v));
    memset(cpu->m, 0, sizeof(cpu->m));
    memset(cpu->gp, 0, sizeof(cpu->gp));
    StackInit(&cpu->s);
}

void tick(Cpu *cpu)
{
    if (cpu->dt)
        cpu->dt--;
    if (cpu->st)
        cpu->st--;
}

void setFont(Cpu *cpu)
{
    char font[] = {0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
                   0x20, 0x60, 0x20, 0x20, 0x70, // 1
                   0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
                   0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
                   0x90, 0x90, 0xF0, 0x10, 0x10, // 4
                   0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
                   0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
                   0xF0, 0x10, 0x20, 0x40, 0x40, // 7
                   0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
                   0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
                   0xF0, 0x90, 0xF0, 0x90, 0x90, // A
                   0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
                   0xF0, 0x80, 0x80, 0x80, 0xF0, // C
                   0xE0, 0x90, 0x90, 0x90, 0xE0, // D
                   0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
                   0xF0, 0x80, 0xF0, 0x80, 0x80};
    memcpy(cpu->m + FONT_START, font, sizeof(font));
}

void load(Cpu *cpu, unsigned char insts[], int size)
{
    memcpy(cpu->m + cpu->pc, insts, size);
}

int fetch(Cpu *cpu)
{
    int inst = cpu->m[cpu->pc] << 8 | cpu->m[cpu->pc + 1];
    cpu->pc += 2;
    return inst;
}

int decode(Cpu *cpu, int inst)
{
    int instl1 = (inst & 0xf000) >> 12;
    int instl2 = (inst & 0xf00) >> 8;
    int insth3 = (inst & 0xf0) >> 4;
    int insth4 = inst & 0xf;
    int code = 0;

    switch (instl1)
    {
    case 0:
        // 00e0 - clear screen
        if (inst == 0xe0)
        {
            memset(cpu->v, 0, sizeof(cpu->v));
        }
        // 00ee - return from subroutine
        if (inst == 0xee)
            cpu->pc = pop(&cpu->s);
        break;
    case 1:
        // 1nnn - jump
        cpu->pc = inst & 0xfff;
        break;
    case 2:
        // 2nnn - call subroutine
        push(&cpu->s, cpu->pc);
        cpu->pc = inst & 0xfff;
        break;
    case 3:
        // 3xnn - skip if x == nn
        if (cpu->gp[instl2] == (inst & 0xff))
            cpu->pc += 2;
        break;
    case 4:
        // 4xnn - skip if x != nn
        if (cpu->gp[instl2] != (inst & 0xff))
            cpu->pc += 2;
        break;
    case 5:
        // 5xy0 - skip if x == y
        if (cpu->gp[instl2] == cpu->gp[insth3])
            cpu->pc += 2;
        break;
    case 6:
        // 6xnn - set reg x to nn
        cpu->gp[instl2] = inst & 0xff;
        break;
    case 7:
        // 7xnn - add to reg x value nn
        cpu->gp[instl2] = (cpu->gp[instl2] + inst & 0xff) & 0xff;
        break;
    case 8:
        switch (insth4)
        {
        case 0:
            // 8xy0 - x = y
            cpu->gp[instl2] = cpu->gp[insth3];
            break;
        case 1:
            // 8xy1 - x = x | y
            cpu->gp[instl2] |= cpu->gp[insth3];
            break;
        case 2:
            // 8xy2 - x = x & y
            cpu->gp[instl2] &= cpu->gp[insth3];
            break;
        case 3:
            // 8xy3 - x = x ^ y
            cpu->gp[instl2] ^= cpu->gp[insth3];
            break;
        case 4:
            // 8xy4 - x = x + y
            cpu->gp[0xf] = cpu->gp[instl2] + cpu->gp[insth3] > 0xff;
            cpu->gp[instl2] += cpu->gp[insth3];
            break;
        case 5:
            // 8xy5 - x = x - y
            cpu->gp[instl2] -= cpu->gp[insth3];
            cpu->gp[0xf] = cpu->gp[instl2] >= cpu->gp[insth3];
            break;
        case 6:
            // 8xy6 - shit 1 to the right
            cpu->gp[instl2] = cpu->gp[insth3];
            cpu->gp[0xf] = cpu->gp[instl2] & 1;
            cpu->gp[instl2] >>= 1;
            break;
        case 7:
            // 8xy5 - x = y - x
            cpu->gp[instl2] = cpu->gp[insth3] - cpu->gp[instl2];
            cpu->gp[0xf] = cpu->gp[insth3] >= cpu->gp[instl2];
            break;
        case 0xe:
            // 8xye - shit 1 to the left
            cpu->gp[instl2] = cpu->gp[insth3];
            cpu->gp[0xf] = (cpu->gp[instl2] & 0x80) >> 7;
            cpu->gp[instl2] <<= 1;
            break;
        default:
            printf("Something really wrong happened here...\n");
            break;
        }
    case 9:
        // 9xy0 - skip if x != y
        if (cpu->gp[instl2] != cpu->gp[insth3])
            cpu->pc += 2;
        break;
    case 0xa:
        // annn - set i to nnn
        cpu->i = inst & 0xfff;
        break;
    case 0xb:
        // bnnn - jump with upset
        cpu->pc = (inst & 0xfff) + cpu->gp[0];
        break;
    case 0xc:
        // cxnn - random
        cpu->gp[instl2] = (random() % 0xff) & inst & 0xff;
        break;
    case 0xd:
    {
        // dxyn - display
        int x = cpu->gp[instl2] % WIDTH;
        int y = cpu->gp[insth3] % HEIGHT;
        cpu->gp[0xf] = 0;

        for (int i = 0; i < insth4; i++)
        {
            if (y + i > HEIGHT)
                break;
            ;
            int sprite = cpu->m[cpu->i + i];
            for (int j = 0; j < 8; j++)
            {
                if (x + j > WIDTH)
                    break;
                if (sprite & (0x80 >> j))
                {
                    cpu->v[x + j][y + i] ^= 1;
                    int pixel = cpu->v[x + j][y + i];
                    if (~cpu->gp[0xf])
                    {
                        cpu->gp[0xf] = ~pixel;
                    }
                }
            }
        }
        code = 1;
        break;
    }
    case 0xe:
        int key = cpu->keyDown();
        // ex9e - skip if pressed
        if ((inst & 0xff) == 0x9e)
        {
            if (key >= 0 && key == cpu->gp[instl2])
                cpu->pc += 2;
        }
        // exa1 - skip if not pressed
        if ((inst & 0xff) == 0xa1)
        {
            if (key >= 0 && key != cpu->gp[instl2])
                cpu->pc += 2;
        }
        break;
    case 0xf:
        switch (inst & 0xff)
        {
        case 7:
            // fx07 - set x to dt
            // pthread_mutex_lock(&cpu->timerLock);
            cpu->gp[instl2] = cpu->dt;
            // pthread_mutex_unlock(&cpu->timerLock);
            break;
        case 0x15:
            // fx15 - set dt to x
            // pthread_mutex_lock(&cpu->timerLock);
            cpu->dt = cpu->gp[instl2];
            // pthread_mutex_unlock(&cpu->timerLock);
            break;
        case 0x18:
            // fx18 - set st to x
            // pthread_mutex_lock(&cpu->timerLock);
            cpu->st = cpu->gp[instl2];
            // pthread_mutex_unlock(&cpu->timerLock);
            break;
        case 0x1e:
            // fx1e - i += x
            cpu->i += cpu->gp[instl2];
            break;
        case 0xa:
            // fx0a - get key
            int key = cpu->keyDown();
            // this effectively works as while(key < 0) {}
            if (key < 0)
                cpu->pc -= 2;
            else
                cpu->gp[instl2] = key;
            break;
        case 0x29:
            // fx29 - load font x in i
            cpu->i = FONT_START + cpu->gp[instl2];
            break;
        case 0x33:
            // fx33 - bcd
            int num = cpu->gp[instl2];
            for (int i = 2; i > -1; i--)
            {
                cpu->m[cpu->i + i] = (int)num % 10;
                num /= 10;
            }
            break;
        case 0x55:
            for (int i = 0; i <= instl2; i++)
            {
                cpu->m[cpu->i + i] = cpu->gp[i];
            }

            break;
        case 0x65:
            for (int i = 0; i <= instl2; i++)
            {
                cpu->gp[i] = (int)cpu->m[cpu->i + i];
            }
            break;
        default:
            printf("Something really wrong happened here...\n");
            break;
        }
        break;
    default:
        printf("Something really wrong happened here...\n");
        break;
    }
    return code;
}

int step(Cpu *cpu)
{
    int inst = fetch(cpu);
    return decode(cpu, inst);
}