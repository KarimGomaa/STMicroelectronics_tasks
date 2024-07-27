	.file	"main.c"
	.text
	.globl	global_main1
	.data
	.align 4
	.type	global_main1, @object
	.size	global_main1, 4
global_main1:
	.long	1
	.globl	global_main2
	.align 4
	.type	global_main2, @object
	.size	global_main2, 4
global_main2:
	.long	2
	.section	.rodata
.LC0:
	.string	"Before Function \n "
	.align 8
.LC1:
	.string	"global_main1=%d , global_main2=%d , local_main1= %d \n"
.LC2:
	.string	"After Function \n "
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movl	$3, -12(%rbp)
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	-12(%rbp), %ecx
	movl	global_main2(%rip), %edx
	movl	global_main1(%rip), %eax
	movl	%eax, %esi
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	leaq	-12(%rbp), %rax
	movq	%rax, %rdx
	leaq	global_main2(%rip), %rax
	movq	%rax, %rsi
	leaq	global_main1(%rip), %rax
	movq	%rax, %rdi
	call	fun_ChangeToGlobalValues@PLT
	leaq	.LC2(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	-12(%rbp), %ecx
	movl	global_main2(%rip), %edx
	movl	global_main1(%rip), %eax
	movl	%eax, %esi
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$0, %eax
	movq	-8(%rbp), %rdx
	subq	%fs:40, %rdx
	je	.L3
	call	__stack_chk_fail@PLT
.L3:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
