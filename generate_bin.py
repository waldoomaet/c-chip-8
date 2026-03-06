code = bytearray([0xe0, 0x9e, 0, 0, 0xe0, 0xa1, 0x12, 0, 0x12, 0])
with open("./roms/mine.ch8", "wb") as file:
    file.write(code)
