#as: --fdpic
#objdump: -p -r -j .literal -j .text
#source: fdpic-tls.s

#...
FDPIC ABI supplement
#...
.* R_XTENSA_GOTTLSDESC *v1
.* R_XTENSA_TLS_GOTTPOFF *v2
.* R_XTENSA_TLS_DTPOFF *v3
.* R_XTENSA_TLS_TPOFF *v4
#...
.* R_XTENSA_TLS_CALL *v5
