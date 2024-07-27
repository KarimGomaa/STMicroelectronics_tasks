	.file	"function.c"
	.text
	.globl	first_global
	.data
	.align 4
	.type	first_global, @object
	.size	first_global, 4
first_global:
	.long	5
	.globl	second_global
	.align 4
	.type	second_global, @object
	.size	second_global, 4
second_global:
	.long	10
	.text
	.globl	fun_ChangeToGlobalValues
	.type	fun_ChangeToGlobalValues, @function
fun_ChangeToGlobalValues:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movq	%rdx, -40(%rbp)
	movl	$20, -4(%rbp)
	movl	first_global(%rip), %edx
	movq	-24(%rbp), %rax
	movl	%edx, (%rax)
	movl	second_global(%rip), %edx
	movq	-32(%rbp), %rax
	movl	%edx, (%rax)
	movq	-40(%rbp), %rax
	movl	-4(%rbp), %edx
	movl	%edx, (%rax)
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	fun_ChangeToGlobalValues, .-fun_ChangeToGlobalValues
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
