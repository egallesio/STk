	.verstamp	7 10
	.option	pic2
	.extern	stk_argv 128
	.text	
	.align	2
	.file	2 "lowstack-MIPS32.c"
	.globl	stk_copy_float_registers
	.loc	2 36
 #  36	{
	.ent	stk_copy_float_registers 2
stk_copy_float_registers:
	.option	O1
	.set	 noreorder
	.cpload	$25
	.set	 reorder
	subu	$sp, 16
	.frame	$sp, 16, $31
	.loc	2 36
	.loc	2 37
 #  37	  register double f0 /* asm ("$f12") */ = stk_argv[0].d;
	la	$14, stk_argv
	l.d	$f12, 0($14)
	.loc	2 38
 #  38	  register double f1 /* asm ("$f14") */ = stk_argv[1].d;
	l.d	$f14, 8($14)
	.loc	2 43
 #  39	#ifdef D0
 #  40	   register double f2 /* asm ("$f6") */  = stk_argv[2].d;
 #  41	   register double f3 /* asm ("$f7") */  = stk_argv[3].d;
 #  42	#endif
 #  43	}
	.livereg	0x0000FF0E,0x00000FFF
	addu	$sp, 16
	j	$31
	.end	stk_copy_float_registers
