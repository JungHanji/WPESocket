import sys
from os import system

if len(sys.argv) <= 1:
    print(f"Usage: {sys.argv[0]} <name without extension>")
    print(f"Example: {sys.argv[0]} main")
    exit(1)
name = " ".join(sys.argv[1:])

system(f'g++ -c {name}.cpp -I src/include -w')
system(f'g++ -o {name} {name}.o -L src/lib -l "sfml-graphics-d" '
        '-l "sfml-window-d" -l "sfml-system-d" '
        '-l ws2_32 -l winmm -l glu32 -l gdi32')
#system(f'{name}')