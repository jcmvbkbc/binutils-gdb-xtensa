	.data
	.weak	m0v1
	.globl	m1v1
	.word	0x12345678
m1v1:
	.word	m1f1@funcdesc
	.word	m1f2@funcdesc
	.word	m2f1@funcdesc
	.word	m1v2 # relative reloc

	.section ".data.b"
	.word	0x23456789
m1v2:
	.word	m1v1 # relative reloc
	.word	m0v1 # no reloc: undef weak

	.text
	.globl _start
_start:
	movi	a2, m1f1@gotfuncdesc
	movi	a3, m1f2@gotofffuncdesc
	movi	a4, m2f1@gotfuncdesc

	movi	a5, m1v1@gotoff
	movi	a6, m1v2@gotoff

	movi	a7, m1v1 # no reloc
	movi	a8, m1v2 # no reloc

	#movi	a9, m0v1 # no reloc
	#movi	a10, m0v1@gotoff # error: gotoff for undef weak
	#movi	a11, m0v1@got

	.globl	m1f1
	.align	4
m1f1:
	ret

	.align	4
m1f2:
	ret
