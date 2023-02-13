	.globl	m2f1
	.type	m2f1, function
	.text
	.align	4
m2f1:
	ret
	.size	m2f1, . - m2f1

	.data
	.word	0x3456789a
	.globl	m2v1
	.type	m2v1, object
m2v1:
	.word	m1v1 # GLOB_DAT
	.word	m2v2 # GLOB_DAT
	.size	m2v1, . - m2v1

	.globl	m2v2
	.type	m2v2, object
m2v2:
	.word	m2v3 # relative reloc
	.size	m2v2, . - m2v2

m2v3:
	.word	0x456789ab
