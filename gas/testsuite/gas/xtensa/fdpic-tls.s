	.text
	movi	a2, v1@gottlsdesc
	movi	a3, v2@gottpoff
	movi	a4, v3@dtpoff
	movi	a5, v4@tpoff
	callx0.tls a6, v5@tlscall
#	.reloc	., R_XTENSA_TLS_ARG, v6
#	mov	a11, a2
