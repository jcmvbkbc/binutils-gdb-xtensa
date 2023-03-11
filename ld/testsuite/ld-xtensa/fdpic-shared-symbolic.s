	.data
	.globl	m1v1
	.word	0x12345678
m1v1:
#.word	m1f1@funcdesc
	.word	m1f2@gotofffuncdesc

	.text
	.globl _start
_start:
	movi	a2, m1f1@gotfuncdesc
	movi	a3, m1f2@gotofffuncdesc
	movi	a4, m1f3@gotofffuncdesc
	movi	a5, .L1@gotofffuncdesc
	movi	a6, .L2@gotofffuncdesc

	.globl	m1f1
	.align	4
m1f1:
	ret

	.align	4
	.type m1f2,@function
m1f2:
	ret
	.size m1f2, . - m1f2

	.align	4
m1f3:
	ret

	.align	4
.L1:
	ret

	.align	4
.L2:
	ret
