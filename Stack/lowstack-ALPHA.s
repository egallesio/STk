	.set noreorder
	.set volatile
	.set noat
.text
	.align 3
	.globl stk_copy_float_registers
	.ent stk_copy_float_registers
stk_copy_float_registers:
	ldgp $29,0($27)
	lda $30,-16($30)
	.frame $15,16,$26,0
	stq $26,0($30)
	stq $15,8($30)
	.mask 0x4008000,-16
	bis $30,$30,$15
	.prologue 1
	lda $1,stk_argv
	ldt $f16,0($1)
	lda $1,stk_argv
	ldt $f17,8($1)
	lda $1,stk_argv
	ldt $f18,16($1)
	lda $1,stk_argv
	ldt $f19,24($1)
	lda $1,stk_argv
	ldt $f20,32($1)
	lda $1,stk_argv
	ldt $f21,40($1)
$33:
	bis $15,$15,$30
	ldq $26,0($30)
	ldq $15,8($30)
	addq $30,16,$30
	ret $31,($26),1
	.end stk_copy_float_registers
