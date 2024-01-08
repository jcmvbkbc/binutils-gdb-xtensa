#source: fdpic-static-m1.s
#source: fdpic-static-m2.s
#as:
#ld: -static --no-dynamic-linker
#objdump: -ds -j.data -j.got -j.text
#...
Contents of section .got:
.* 00000000 00000000 00000000 ........  .*
.* ........ ........ ........ 00000000  .*
.* ........ ........ ........ 00000000  .*
#...
.*l32r.*a2, .*(28 .*)
.*l32r.*a3, .*(30 .*)
.*l32r.*a4, .*(18 .*)
.*l32r.*a5, .*(3c .*)
.*l32r.*a6, .*(50 .*)
#...
.*l32r.*a9, .*(0 .*)
.*l32r.*a11, .*(2c .*)
.*l32r.*a12, .*(1c .*)
#...
