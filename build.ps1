rm *.exe
rm result.bmp
rm result_tested.bmp
nasm -f elf64 byteswap.asm
gcc -m64 -fpack-struct -mno-ms-bitfields graph_io.c byteswap.o -g3
gdb a.exe