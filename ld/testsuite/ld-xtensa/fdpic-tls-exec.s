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
	movi	a2, m0w1@gottlsdesc # converted to TPOFF
	movi	a3, m0w1@gottpoff # converted to TPOFF
	movi	a4, m0w1@dtpoff
	movi	a5, m0w1@tpoff

	movi	a2, m0w2@gottlsdesc # stays GOTTLSDESC

	movi	a2, m1g1@gottlsdesc # converted to TPOFF
	movi	a3, m1g1@gottpoff # converted to TPOFF
	movi	a4, m1g1@dtpoff
	movi	a5, m1g1@tpoff

	movi	a2, m1g2@gottlsdesc # converted to TPOFF

	movi	a2, m1h1@gottlsdesc # converted to TPOFF
	movi	a3, m1h1@gottpoff # converted to TPOFF
	movi	a4, m1h1@dtpoff
	movi	a5, m1h1@tpoff

	movi	a2, m1h2@gottlsdesc # converted to TPOFF

	movi	a2, m1l1@gottlsdesc # converted to TPOFF
	movi	a3, m1l1@gottpoff # converted to TPOFF
	movi	a4, m1l1@dtpoff
	movi	a5, m1l1@tpoff

	movi	a2, m1l2@gottlsdesc # converted to TPOFF


	# GD -> LE
GD_LE:
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

	# IE -> LE
IE_LE:
	movi	a3, m1g1@gottpoff
	.reloc	., R_XTENSA_TLS_TPOFF_PTR, m1g1
	_add	a4, a3, a12
	.reloc	., R_XTENSA_TLS_TPOFF_PTR, m1g1
	_add.n	a4, a3, a12
	.reloc	., R_XTENSA_TLS_TPOFF_LOAD, m1g1
	_l32i	a5, a4, 0
	.reloc	., R_XTENSA_TLS_TPOFF_LOAD, m1g1
	_l32i.n	a5, a4, 0
