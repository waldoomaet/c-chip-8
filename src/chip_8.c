#include "chip_8.h"

void cpuReset(Cpu *cpu)
{
    cpu->pc = 0x200;
    cpu->i = 0;
    cpu->dt = 0;
    cpu->st = 0;
    cpu->_instCount = 0;
    memset(cpu->v, 0, sizeof(cpu->v));
    memset(cpu->m, 0, sizeof(cpu->m));
    memset(cpu->gp, 0, sizeof(cpu->gp));
    setFont(cpu);
    StackInit(&cpu->s);
}

void CpuInit(Cpu *cpu, KeyFunc keyDown)
{
    cpu->keyDown = keyDown;
    cpuReset(cpu);
}

int doBeep(Cpu *cpu)
{
    return cpu->st > 0;
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
    uint8_t font[] = {0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
                      0x20, 0x60, 0x20, 0x20, 0x70,  // 1
                      0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
                      0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
                      0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
                      0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
                      0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
                      0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
                      0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
                      0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
                      0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
                      0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
                      0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
                      0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
                      0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
                      0xF0, 0x80, 0xF0, 0x80, 0x80}; // F
    memcpy(cpu->m + FONT_START, font, sizeof(font));
}

void load(Cpu *cpu, unsigned char insts[], int size)
{
    memcpy(cpu->m + cpu->pc, insts, size);
}

uint16_t join(uint8_t b1, uint8_t b2)
{
    uint16_t result = b1;
    result = result << 8 | b2;
    return result;
}

uint16_t fetch(Cpu *cpu)
{
    uint16_t inst = join(cpu->m[cpu->pc], cpu->m[cpu->pc + 1]);
    cpu->pc += 2;
    return inst;
}

int decode(Cpu *cpu, uint16_t inst)
{
    uint8_t instl1 = (inst & 0xf000) >> 12;
    uint8_t instl2 = (inst & 0xf00) >> 8;
    uint8_t insth3 = (inst & 0xf0) >> 4;
    uint8_t insth4 = inst & 0xf;
    int code = 0;

    switch (instl1)
    {
    case 0:
        // 00e0 - clear screen
        if (inst == 0xe0)
        {
            memset(cpu->v, 0, sizeof(cpu->v));
            code = 1;
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
        if (cpu->gp[instl2] == (uint8_t)inst)
            cpu->pc += 2;
        break;
    case 4:
        // 4xnn - skip if x != nn
        if (cpu->gp[instl2] != (uint8_t)inst)
            cpu->pc += 2;
        break;
    case 5:
        // 5xy0 - skip if x == y
        if (cpu->gp[instl2] == cpu->gp[insth3])
            cpu->pc += 2;
        break;
    case 6:
        // 6xnn - set reg x to nn
        cpu->gp[instl2] = (uint8_t)inst;
        break;
    case 7:
        // 7xnn - add to reg x value nn
        cpu->gp[instl2] = cpu->gp[instl2] + (uint8_t)inst;
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
            {
                int result = cpu->gp[instl2] | cpu->gp[insth3];
                cpu->gp[instl2] = (uint8_t)result;
                // cpu->gp[0xf] = 0;
                break;
            }
        case 2:
            // 8xy2 - x = x & y
            {
                int result = cpu->gp[instl2] & cpu->gp[insth3];
                cpu->gp[instl2] = (uint8_t)result;
                // cpu->gp[0xf] = result > 0xff;
                break;
            }
        case 3:
            // 8xy3 - x = x ^ y
            {
                int result = cpu->gp[instl2] ^ cpu->gp[insth3];
                cpu->gp[instl2] = (uint8_t)result;
                // cpu->gp[0xf] = result > 0xff;
                break;
            }
        case 4:
            // 8xy4 - x = x + y
            {
                int result = cpu->gp[instl2] + cpu->gp[insth3];
                cpu->gp[instl2] = (uint8_t)result;
                cpu->gp[0xf] = result > 0xff;
                break;
            }
        case 5:
            // 8xy5 - x = x - y
            {
                uint8_t o1 = cpu->gp[instl2];
                uint8_t o2 = cpu->gp[insth3];
                cpu->gp[instl2] = o1 - o2;
                cpu->gp[0xf] = o1 >= o2;
                break;
            }

        case 6:
            // 8xy6 - shit 1 to the right
            {
                uint8_t x = cpu->gp[instl2];
                cpu->gp[instl2] = cpu->gp[insth3];
                cpu->gp[instl2] >>= 1;
                cpu->gp[0xf] = x & 1;
                break;
            }
        case 7:
            // 8xy5 - x = y - x
            {
                uint8_t o1 = cpu->gp[insth3];
                uint8_t o2 = cpu->gp[instl2];
                cpu->gp[instl2] = o1 - o2;
                cpu->gp[0xf] = o1 >= o2;
                break;
            }
        case 0xe:
            // 8xye - shit 1 to the left
            {
                uint8_t x = cpu->gp[instl2];
                cpu->gp[instl2] = cpu->gp[insth3];
                cpu->gp[instl2] <<= 1;
                cpu->gp[0xf] = (x & 0x80) >> 7;
                break;
            }
        default:
            printf("Something really wrong happened here...\n");
            break;
        }
        break;
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
        cpu->gp[instl2] = (random() % 0xff) & inst;
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
        if ((uint8_t)inst == 0x9e)
        {
            if (key >= 0 && key == cpu->gp[instl2])
                cpu->pc += 2;
        }
        // exa1 - skip if not pressed
        else if ((uint8_t)inst == 0xa1)
        {
            if (key != cpu->gp[instl2])
                cpu->pc += 2;
        }
        break;
    case 0xf:
        switch ((uint8_t)inst)
        {
        case 7:
            // fx07 - set x to dt
            cpu->gp[instl2] = cpu->dt;
            break;
        case 0x15:
            // fx15 - set dt to x
            cpu->dt = cpu->gp[instl2];
            break;
        case 0x18:
            // fx18 - set st to x
            cpu->st = cpu->gp[instl2];
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
            cpu->i = FONT_START + cpu->gp[instl2] * 5;
            break;
        case 0x33:
            // fx33 - bcd
            int num = cpu->gp[instl2];
            for (int i = 2; i > -1; i--)
            {
                cpu->m[cpu->i + i] = num % 10;
                num /= 10;
            }
            break;
        case 0x55:
            // fx55 - move memory to regs until x
            for (int i = 0; i <= instl2; i++)
            {
                cpu->m[cpu->i + i] = cpu->gp[i];
            }

            break;
        case 0x65:
            // fx55 - move regs to memory until x
            for (int i = 0; i <= instl2; i++)
            {
                cpu->gp[i] = cpu->m[cpu->i + i];
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
    cpu->_instCount++;
    return decode(cpu, inst);
}