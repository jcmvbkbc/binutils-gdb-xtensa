	.section	.tbss,"awT",@nobits
	.align	4
	.zero	4
	.globl	m1g1
m1g1:
	.zero	4
	.globl	m1g2
m1g2:
	.zero	4

	.hidden m1h1
m1h1:
	.zero	4
	.hidden m1h2
m1h2:
	.zero	4

m1l1:
	.zero	4

m1l2:
	.zero	4

	.weak	m0w1
	.weak	m0w2

	.text
	.globl _start
_start:
	movi	a2, m0w1@gottlsdesc # converted to GOTTPOFF b/o the other GOTTPOFF reference
	movi	a3, m0w1@gottpoff
#	movi	a4, m0w1@dtpoff # error: dtpoff to a dynamic symbol when in SO
#	movi	a5, m0w1@tpoff # error: tpoff in a shared object

	movi	a2, m0w2@gottlsdesc # stays GOTTLSDESC

	movi	a2, m1g1@gottlsdesc # converted to GOTTPOFF b/o the other GOTTPOFF reference
	movi	a3, m1g1@gottpoff
#	movi	a4, m1g1@dtpoff # error: dtpoff to a dynamic symbol when in SO
#	movi	a5, m1g1@tpoff # error: tpoff in a shared object

	movi	a2, m1g2@gottlsdesc # stays GOTTLSDESC

	movi	a2, m1h1@gottlsdesc # converted to GOTTPOFF b/o the other GOTTPOFF reference
	movi	a3, m1h1@gottpoff
	movi	a4, m1h1@dtpoff
#	movi	a5, m1h1@tpoff # error: tpoff in a shared object

	movi	a2, m1h2@gottlsdesc # stays GOTTLSDESC

	movi	a2, m1l1@gottlsdesc
	movi	a3, m1l1@gottpoff
	movi	a4, m1l1@dtpoff
#	movi	a5, m1l1@tpoff # error: tpoff in a shared object

	movi	a2, m1l2@gottlsdesc

GD:
	movi	a3, m1g2@gottlsdesc
	.reloc	., R_XTENSA_TLS_ARG, m1g2
	_add	a2, a3, a12
	.reloc	., R_XTENSA_TLS_ARG, m1g2
	_add.n	a2, a3, a12
	.reloc	., R_XTENSA_TLS_FUNCDESC, m1g2
	_l32i	a3, a2, 0
	.reloc	., R_XTENSA_TLS_FUNCDESC, m1g2
	_l32i.n	a3, a2, 0
	.reloc	., R_XTENSA_TLS_GOT, m1g2
	_l32i	a11, a3, 4
	.reloc	., R_XTENSA_TLS_GOT, m1g2
	_l32i.n	a11, a3, 4
	.reloc	., R_XTENSA_TLS_FUNC, m1g2
	_l32i	a9, a3, 0
	.reloc	., R_XTENSA_TLS_CALL, m1g2
	callx0	a9

	# GD -> IE
GD_IE:
	movi	a3, m1g1@gottlsdesc
	.reloc	., R_XTENSA_TLS_ARG, m1g1
	_add	a2, a3, a12
	.reloc	., R_XTENSA_TLS_ARG, m1g1
	_add.n	a2, a3, a12
	.reloc	., R_XTENSA_TLS_FUNCDESC, m1g1
	_l32i	a3, a2, 0
	.reloc	., R_XTENSA_TLS_FUNCDESC, m1g1
	_l32i.n	a3, a2, 0
	.reloc	., R_XTENSA_TLS_GOT, m1g1
	_l32i	a11, a3, 4
	.reloc	., R_XTENSA_TLS_GOT, m1g1
	_l32i.n	a11, a3, 4
	.reloc	., R_XTENSA_TLS_FUNC, m1g1
	_l32i	a9, a3, 0
	.reloc	., R_XTENSA_TLS_CALL, m1g1
	callx0	a9
