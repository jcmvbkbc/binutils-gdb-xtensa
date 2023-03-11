	.section	.tbss,"awT",@nobits
	.align	4
	.globl	m1g1
	.type	m1g1, @object
m1g1:
	.zero	4
	.size   m1g1, . - m1g1
	.globl	m1g2
	.type	m1g2, @object
m1g2:
	.zero	4
	.size   m1g2, . - m1g2
m1l1:
	.zero	4

	.text
	movi	a2, m1g1@gottpoff
	movi	a3, m1l1@gottpoff
	movi	a4, m2g1@gottpoff

	movi	a5, m1g2@tlsarg
