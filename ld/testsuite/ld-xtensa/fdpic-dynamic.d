#source: fdpic-dynamic-m1.s
#as:
#ld: -pie -e0 tmpdir/fdpic-dynamic-m2.so
#objdump: -dsR -j.got -j.text
#...
Contents of section .got:
.* ........ ........ ........ ........  .*
.* ........ ........ ........ ........  .*
.* ........ 00000000 ........ ........  .*
.* ........ ........ 00000000 ........  .*
.* ........ ........                    .*
#...
.*l32r.*a9, .*(38 .*)
.*l32r.*a10, .*(24 .*)
#...
.*R_XTENSA_FUNCDESC_VALUE.*m2w2
#...
.*R_XTENSA_GLOB_DAT.*m2w1
#...
