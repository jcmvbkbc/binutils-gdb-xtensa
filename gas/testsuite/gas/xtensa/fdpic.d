#as: --fdpic
#objdump: -p -r -j .literal -j .data
#source: fdpic.s

#...
FDPIC ABI supplement
#...
.* R_XTENSA_GOT *v0
.* R_XTENSA_GOTOFF *v1
.* R_XTENSA_GOTFUNCDESC *v2
.* R_XTENSA_GOTOFFFUNCDESC *v3
#...
.* R_XTENSA_FUNCDESC *v4
#...
