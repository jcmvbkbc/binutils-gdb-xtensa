#source: fdpic-shared.s
#as:
#ld: -shared
#objdump: -dRs -j.got -j.data -j.text
#...
Contents of section .got:
 1424 00000000 00000000 00000000 ........  .*
 .... ........ ........ ........ ........  .*
 .... ........ ........ 0000039c 00001424  .*
 .... 00001474                             .*
Contents of section .data:
 1458 12345678 ........ 0000144c ........  .*
 .... 00001474 ........ 23456789 ........  .*
#...
.*l32r\s+a2,.*(1c .*)
.*l32r\s+a3,.*(28 .*)
#...
.*l32r\s+a5,.*(50 .*)
.*l32r\s+a6,.*(20 .*)
.*l32r\s+a7,.*(30 .*)
#...
0000039c\s+<m1f2>:
#...
\s+1440:\s+R_XTENSA_FUNCDESC\s+m1f1
#...
\s+1444:\s+R_XTENSA_GLOB_DAT\s+m1v1
#...
\s+144c:\s+R_XTENSA_32\s+.text\+0x44
#...
\s+1450:\s+R_XTENSA_32\s+.got
#...
\s+1454:\s+R_XTENSA_32\s+.data\+0x1c
#...
\s+145c:\s+R_XTENSA_FUNCDESC\s+m1f1
#...
\s+1460:\s+R_XTENSA_32\s+.got\+0x28
#...
\s+1468:\s+R_XTENSA_32\s+.data\+0x18
#...
