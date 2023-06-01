#source: fdpic-dynamic-m1.s
#as:
#ld: -pie -e0 tmpdir/fdpic-dynamic-m2.so
#objdump: -dsR -j.data -j.got -j.text
#...
Contents of section .got:
.* 00000000 00000000 00000000 ........  .*
.* 00000000 00000000 ........ ........  .*
.* ........ ........ 00000000 ........  .*
.* ........ ........                    .*
Contents of section .data:
.* 12345678 ........ ........ 00000000  .*
.* ........ 00000000 23456789 ........  .*
.* 00000000                             .*
#...
.*l32r.*a4, .*(10 .*)
#...
.*l32r.*a9, .*(28 .*)
.*l32r.*a10, .*(14 .*)
#...
.*R_XTENSA_GLOB_DAT.*m2w1
#...
