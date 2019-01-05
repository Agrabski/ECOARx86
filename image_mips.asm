	.data
fname:	.asciiz "small.bmp"		# input file name
outfn:	.asciiz "result.bmp"	
imgInf:	.word 32, 32, pImg, 0, 0, 0
handle: .word 0
fsize:	.word 0
# to avoid memory allocation image buffer is defined
# big enough to store 512x512 black&white image
# note that we know exactly the size of the header
# pImgae is the first byte of image itself
pFile:	.space 62
pImg:	.space 36000

	.text
main:	
	# open input file for reading
	# the file has to be in current working directory
	# (as recognized by mars simulator)
	la $a0, fname
	li $a1, 0
	li $a2, 0
	li $v0, 13
	syscall
	# read the whole file at once into pFile buffer
	# (note the effective size of this buffer)
	move $a0, $v0
	sw $a0, handle
	la $a1, pFile
	la $a2, 36062
	li $v0, 14
	syscall
	# store file size for further use and print it
	move $a0, $v0
	sw $a0, fsize
	li $v0, 1
	syscall
	# close file
	li $v0, 16
	syscall
	
######################################
# instead of project implementation
# just set 8 pixels in row 0 and columns in range 0..7

	la $a0, pImg
	sb $zero, ($a0)

######################################

	# open the result file for writing
	la $a0, outfn
	li $a1, 1
	li $a2, 0
	li $v0, 13
	syscall
	# print handle of the file 
	move $a0, $v0
	sw $a0, handle
	li $v0, 1
	syscall
	# save the file (file size is restored from fsize)
	la $a1, pFile
	lw $a2, fsize
	li $v0, 15
	syscall
	# close file
	li $v0, 16
	syscall
		
	li $v0, 10
	syscall
