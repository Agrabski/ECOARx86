global findPatterns1	;(imgInfo*	pImg, int	pSize, int*	ptrn, Point*	pDst, int*	fCnt)
			;		rcx		rdx		r8		r9			
extern getWord
section .text
;9
%define pImg	rbp+16
%define pSize	rbp+24
%define	ptrn	rbp+32
%define	pDst	rbp+40
%define	fCnt 	rbp+48
%define widith 	rbp-8
%define height 	rbp-16
%define mask	rbp-24
%define x	rsi
%define	y	rbp-32

%define padding	2
findPatterns1:
		push	rbp		;save stack base pointer
		mov	rbp, rsp	;set base pointer
		sub	rsp, 40

		mov	[pImg],rcx
		mov	[ptrn], r8
		mov	[pDst], r9
		push	rbx
				
		push 	rdi
		
		push	rsi
		push	r12
		push	r13
		push	r14
		push	r15		;rbp-56
		mov	rbx, r8		;save pattern pointer
		mov	rax, rdx
		and	rax, 0xFFFF
		mov	[widith], rax	;rbp-4	widith
		mov 	r10, rax	;save mask widith
		mov	rax, rdx
		shr	rax, 16

		;mov	[height], rax		;rbp-8	height
		;jmp	end
		mov 	x, 0
		mov 	QWORD[y], 0
;		make mask
		mov	r11, 0
maskLoop:	shl	r11, 1
		or	r11, 1		;mask
		sub	r10, 1
		jnz	maskLoop
		shr	r11, 1

		mov	r14, -1		;hit begin
		mov	r15, 0		; n

		mov	[mask], r11		;rbp-12 mask
;		mask made
		mov	r12, 0		;current line
;		call getWord
begin:		mov	rdx, [pImg]
		mov	r11, [mask]
		mov	rcx, [rdx+8]	;char*
		mov	rdx, [rdx]	;widith
		shr	rdx, 32
		mov	r8, x
		mov	r9, [y]
		add	r9, r12
		call	getWord

		mov	r13, [rbx+4*r12]	;pattern mask
		and	r13, 0xFFFF
cPL1:		mov	rdi, rax
		and	rdi, r11
		xor	rdi, r13

		test	rdi,rdi
		jz	ptrnL1Found
adv:		shl	r13, 1
		shl	r11, 1
		jc	moveX	; if overflow, increment
		jmp	cPL1

ptrnL1Found:
		add	r14, 0
		jns	nextPaternLine
		mov	r14, r12,
		add	r12, 1
		cmp	r12, [height]
		jne	begin
		mov	r12, 0
		jmp	begin

nextPaternLine:
		cmp	r14, r12
		je	patternFound
		add	r12, 1
		cmp	r12, [height]
		jne	begin
		mov	r12, 0
		jmp	begin
patternFound:
		mov	rdx, x
		shl	rdx, 3
		tzcnt	r15, r13

		add	rdx, [widith]
		sub	rdx, r15
		add	rdx, 32
		sub	rdx, padding
		mov	r15, [pDst]
		shl	rdx, 16
		mov	[r15], dx
		mov	rdx, [y]
		shl	rdx, 16
		mov	[r15+4], dx
		add	r15, 8
		mov	[pDst], r15

		mov	r15, [fCnt]
		add	WORD[r15], 1
		jmp	adv


moveX:
		mov	r12, 0
		mov	r14, -1
		add	x, 7
		mov	rdx, [pImg]
		mov	rdx, [rdx]	;widith
		and	rdx, 0xFFFF
		shr	rdx, 3
		cmp	rdx, x
		jg	begin
		mov	x, 0
		add	WORD[y], 1
		mov	rdx, [pImg]
		mov	rdx, [rdx+4]	;height
		and	rdx, 0xFFFF
		cmp	rdx, [y]
		jg	begin


;		epilogue


end:		
		pop	r15
		pop	r14
		pop	r13
		pop	r12
		pop	rsi
		pop	rdi
		pop	rbx
		add	rsp, 40
		pop	rbp
		ret