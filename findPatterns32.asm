global findPatterns1	;(imgInfo*	pImg, int	pSize, int*	ptrn, Point*	pDst, int*	fCnt)
			;		rcx		rdx		r8		r9			
global _findPatterns1
section .text
;9
%define pImg	ebp+8
%define pSize	ebp+12
%define	ptrn	ebp+16
%define	pDst	ebp+20
%define	fCnt 	ebp+24
%define widith 	ebp-4 
%define height 	ebp-8
%define mask	ebp-12
%define curMask	ebp-16
%define curPatr ebp-20
%define x	esi
%define	y	ebp-24
%define firstH	ebp-28
%define cLine	ebp-32

%define padding	2




_findPatterns1:
findPatterns1:
		push	ebp		;save stack base pointer
		mov	ebp, esp	;set base pointer
		sub	esp, 40
		push	ebx
		push 	edi
		push	esi

		mov	eax, [pSize]
		and	eax, 0xFFFF
		mov	[widith], eax	;rbp-4	widith
		mov 	ebx, eax	;save mask widith
		mov	eax, [pSize]
		shr	eax, 16

		mov	[height], eax		;rbp-8	height
		;jmp	end
		mov 	x, 0
		mov 	DWORD[y], 0
;		make mask
		mov	eax, 0
maskLoop:	shl	eax, 1
		or	eax, 1		;mask
		sub	ebx, 1
		jnz	maskLoop
		shr	eax, 1
		mov	[mask], eax
		mov	[curMask], eax

		mov	DWORD[firstH], -1		;hit begin
		mov	DWORD[cLine], 0		; n

;		mask made
;		call getWord
begin:				
		mov	eax, [pImg]
		mov	eax, [eax]
                add     eax, 31
                shr     eax, 5
                shl     eax, 2  


		mov	ebx, [y]
		add	ebx, [cLine]
                mul	ebx

                add     eax, x
		
		mov	edx, [pImg]
		add	eax, [edx+8]
                mov     eax, [eax]
                bswap   eax






		mov	edx, [curMask]
		tzcnt	ecx, edx
		
		mov	edi, [ptrn]
		mov	ebx, [cLine]
		mov	edi, [edi+4*ebx]	;pattern mask
		and	edi, 0xFFFF
		shl	edi, cl
		mov	ecx, edi

cPL1:		mov	edi, eax
		mov	[curMask], edx
		mov	[curPatr], ecx
		and	edi, edx
		xor	edi, ecx

		test	edi,edi
		jz	ptrnL1Found
adv:		shl	ecx, 1
		shl	edx, 1
		
		jc	moveX	; if overflow, increment
		jmp	cPL1

ptrnL1Found:
		add	DWORD[firstH], 0
		jns	nextPaternLine
		mov	eax, [cLine],
		mov	[firstH], eax
		add	DWORD[cLine], 1
		cmp	eax, DWORD[height]
		jne	begin
		mov	DWORD[cLine], 0
		jmp	begin

nextPaternLine:
		mov	eax, [cLine]
		cmp	eax, DWORD[firstH]
		je	patternFound
		add	eax, 1
		cmp	eax, DWORD[height]
		mov	[cLine], eax
		jne	begin
		mov	DWORD[cLine], 0
		jmp	begin
patternFound:
		mov	ebx, x
		shl	ebx, 3
		tzcnt	edx, [curMask]

		add	ebx, [widith]
		sub	ebx, edx
		add	ebx, 32
		sub	ebx, padding
		mov	edx, [pDst]
		mov	[edx], bx
		mov	ebx, [y]
		mov	[edx+4], bx
		add	edx, 8
		mov	[pDst], edx

		mov	edx, [fCnt]
		add	DWORD[edx], 1
		mov	ecx, [curPatr]
		mov	edx, [curMask]
		mov	DWORD[firstH], -1
		jmp	adv


moveX:
		mov	DWORD[cLine], 0
		mov	DWORD[firstH], -1
		mov	eax, [mask]
		mov	[curMask], eax
		add	x, 3
		mov	edx, [pImg]
		mov	edx, [edx]	;widith
		and	edx, 0xFFFF
		shr	edx, 3
		cmp	edx, x
		jg	begin
		mov	x, 0
		add	DWORD[y], 1
		mov	edx, [pImg]
		mov	edx, [edx+4]	;height
		and	edx, 0xFFFF
		cmp	edx, [y]
		jg	begin


;		epilogue


end:		

		pop	esi
		pop	edi
		pop	ebx
		add	esp, 40
		pop	ebp
		ret


		
getWord:
		mov	eax, [esp+12]
                add     eax, 31
                shr     eax, 5
                shl     eax, 2  



                mul     DWORD[esp+20]


                add     eax, [esp+8]
		add	eax, [esp+16]
                mov     eax, [eax]
                bswap   eax
                ret