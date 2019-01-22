global getWord ;(char*pointer, int widith, int x, int y)
                ;rcx            rdx             r8      r9
section .text

getWord:
                add     rdx, 31
                shr     rdx, 5
                shl     rdx, 2  

                mov     rax, rdx


                mul     r9


                add     rax, rcx
                mov     rax, [rax+r8]
                bswap   rax
                ret