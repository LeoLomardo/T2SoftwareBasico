	.file	"main.c"
	.text
	.section	.rodata
.LC0:
	.string	"r"
.LC1:
	.string	"teste1.txt"
.LC2:
	.string	"\n\nPrimeiro print\n"
.LC3:
	.string	"Erro ao abrir o arquivo"
.LC4:
	.string	"%d"
.LC5:
	.string	"\n\nSegundo print\n"
.LC6:
	.string	"%d "
	.text
	.globl	main
	.type	main, @function
main:
.LFB6:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$1056, %rsp
	movl	%edi, -1044(%rbp)
	movq	%rsi, -1056(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	leaq	.LC0(%rip), %rsi
	leaq	.LC1(%rip), %rdi
	call	fopen@PLT
	movq	%rax, -1024(%rbp)
	leaq	.LC2(%rip), %rdi
	call	puts@PLT
	cmpq	$0, -1024(%rbp)
	jne	.L2
	leaq	.LC3(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$1, %edi
	call	exit@PLT
.L2:
	leaq	-1008(%rbp), %rdx
	movq	-1024(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	geracodigo@PLT
	movq	%rax, -1016(%rbp)
	leaq	-1008(%rbp), %rax
	movq	-1016(%rbp), %rdx
	movq	%rax, %rdi
	movl	$0, %eax
	call	*%rdx
	movl	%eax, -1028(%rbp)
	movl	-1028(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC4(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	leaq	.LC5(%rip), %rdi
	call	puts@PLT
	movl	$0, -1032(%rbp)
	jmp	.L3
.L4:
	movl	-1032(%rbp), %eax
	cltq
	movzbl	-1008(%rbp,%rax), %eax
	movzbl	%al, %eax
	movl	%eax, %esi
	leaq	.LC6(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	addl	$1, -1032(%rbp)
.L3:
	cmpl	$999, -1032(%rbp)
	jle	.L4
	movq	-1024(%rbp), %rax
	movq	%rax, %rdi
	call	fclose@PLT
	movl	$0, %eax
	movq	-8(%rbp), %rcx
	xorq	%fs:40, %rcx
	je	.L6
	call	__stack_chk_fail@PLT
.L6:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	 1f - 0f
	.long	 4f - 1f
	.long	 5
0:
	.string	 "GNU"
1:
	.align 8
	.long	 0xc0000002
	.long	 3f - 2f
2:
	.long	 0x3
3:
	.align 8
4:
