real_hex = hex

def hex(i):
    return real_hex(i)[2:].upper()

def decode(i, b1, b2):
    inst = b1 << 8 | b2
    instl1 = (inst & 0xf000) >> 12
    instl2 = (inst & 0xf00) >> 8
    insth3 = (inst & 0xf0) >> 4
    insth4 = inst & 0xf

    print(f"{hex(i).zfill(4)}. ({hex(inst)}) ", end='')

    match instl1:
        case 0:
            if inst == 0:
                print("Nothing")
            elif inst == 0xe0:
                print("Clear screen")
            elif inst == 0xee:
                print("Return from subroutine")
            else:
                print("IDK")
        case 1:
            print(f"Jump to 0x{hex(inst & 0xfff)}")
        case 2:
            print(f"Call subroutine in 0x{hex(inst & 0xfff)}")
        case 3:
            print(f"Skip if V{hex(instl2)} == 0x{hex(inst & 0xff)}")
        case 4:
            print(f"Skip if V{hex(instl2)} != 0x{hex(inst & 0xff)}")
        case 5:
            print(f"Skip if V{hex(instl2)} == V{hex(insth3)}")
        case 6:
            print(f"V{hex(instl2)} = 0x{hex(inst & 0xff)}")
        case 7:
            print(f"V{hex(instl2)} += 0x{hex(inst & 0xff)}")
        case 8:
            match insth4:
                case 0:
                    print(f"V{hex(instl2)} = V{hex(insth3)}")
                case 1:
                    print(f"V{hex(instl2)} |= V{hex(insth3)}")
                case 2:
                    print(f"V{hex(instl2)} &= V{hex(insth3)}")
                case 3:
                    print(f"V{hex(instl2)} ^= V{hex(insth3)}")
                case 4:
                    print(f"V{hex(instl2)} += V{hex(insth3)} (with carry)")
                case 5:
                    print(f"V{hex(instl2)} = V{hex(instl2)} - V{hex(insth3)} (with carry)")
                case 6:
                    print(f"V{hex(instl2)} >>= 1")
                case 7:
                    print(f"V{hex(instl2)} = V{hex(insth3)} - V{hex(instl2)} (with carry)")
                case 0xe:
                    print(f"V{hex(instl2)} <<= 1")
                case _:
                    print("Something really wrong happened here...")

        case 9:
            print(f"Skip if V{hex(instl2)} != V{hex(insth3)}")
        case 0xa:
            print(f"I = 0x{hex(inst & 0xfff)}")
        case 0xb:
            print(f"Jump to 0x{hex(inst & 0xfff)} with V0 offset")
        case 0xc:
            print(f"V{hex(instl2)} = Random & 0x{b2}")
        case 0xd:
            print(f"Draw/display a {insth4} tall sprite (V{hex(instl2)}, V{hex(insth3)})")
        case 0xe:
            if b2 == 0x9e:
                print("Skip if pressed")
            if b2 == 0xa1:
                print("Skip if NOT pressed")
        case 0xf:
            match inst & 0xff:
                case 7:
                    print(f"V{hex(instl2)} = dt")
                case 0x15:
                    print(f"dt = V{hex(instl2)}")
                case 0x18:
                    print(f"st = V{hex(instl2)}")
                case 0x1e:
                    print(f"I += V{hex(instl2)}")
                case 0xa:
                    print(f"Get key to V{hex(instl2)} (block)")
                case 0x29:
                    print(f"Load font V{hex(instl2)} in I")
                case 0x33:
                    print(f"BCD of V{hex(instl2)}")
                case 0x55:
                    print(f"Move memory to regs from V0 to V{hex(instl2)}")
                case 0x65:
                    print(f"Move regs from V0 to V{hex(instl2)} to memory")
                case _:
                    print("Something really wrong happened here...")

        case _:
            print("Something really wrong happened here...")

import sys

file = sys.argv[1]

with open(file, 'rb') as file:
    content = bytearray(file.read())
    for i in range(0, len(content), 2):
        decode(i, content[i], content[i+1] if i + 1 < len(content) else 0)