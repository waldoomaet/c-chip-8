import sys

with open(sys.argv[1], 'rb') as file:
    content = bytearray(file.read())
    print(f"unsigned char source[] = {{{', '.join(map(hex, content))}}};")
