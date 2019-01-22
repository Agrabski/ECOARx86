rm *.exe
rm result.bmp
rm result_tested.bmp

nasm -f elf32 findPatterns32.asm
gcc -m32 -fpack-struct -mno-ms-bitfields graph_io.c findPatterns32.o -g3
gdb32 a.exe