#!/bin/bash
AWKPROG='\
function x(v) { printf "\\0\\%o\\%o\\%o", (v / 65536) % 256, (v / 256) % 256, v % 256 } \
{ \
        printf "\047\\0\\207\\01\\07"; \
        x($$1); \
        x($$2); \
        x($$3); \
        printf "\\0\\0\\0\\0\\04\\070\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\047" \
}'

m68k-apple-macos-gcc -m68040 -I. -x assembler-with-cpp -c srt0.s
m68k-apple-macos-gcc -m68040 -I. -c boot.c
m68k-apple-macos-gcc -m68040 -I. -c int.c
m68k-apple-macos-ld -N -Ttext 4380000 -e start srt0.o boot.o int.o -o boot.elf
m68k-apple-macos-size boot.elf
m68k-apple-macos-objcopy -O binary boot.elf boot.raw
# (m68k-apple-macos-size boot.elf | tail +2 | gawk ${AWKPROG} | xargs printf ; cat boot.raw) > boot
(m68k-apple-macos-size boot.elf | tail +2 | awk ' function x(v) { printf "\\0\\%o\\%o\\%o", (v / 65536) % 256, (v / 256) % 256, v % 256 }  {  printf "\047\\0\\207\\01\\07";  x($1);  x($2);  x($3);  printf "\\0\\0\\0\\0\\04\\070\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\047"  }' |  xargs printf ; cat boot.raw) > boot

