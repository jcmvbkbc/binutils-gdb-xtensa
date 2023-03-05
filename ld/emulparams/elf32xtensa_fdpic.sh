source_sh ${srcdir}/emulparams/elf32xtensa.sh

BIG_OUTPUT_FORMAT="elf32-xtensa-be-fdpic"
LITTLE_OUTPUT_FORMAT="elf32-xtensa-le-fdpic"
GOT=".got          ${RELOCATING-0} : {${RELOCATING+ *(.got.plt)} *(.got) }"
DATA_GOT=" "
OTHER_READONLY_SECTIONS="
  .xt_except_table ${RELOCATING-0} : ONLY_IF_RO { KEEP (*(.xt_except_table${RELOCATING+ .xt_except_table.* .gnu.linkonce.e.*})) }
  .rofixup : {
	${RELOCATING+__ROFIXUP_LIST__ = .;}
	*(.rofixup)
	${RELOCATING+__ROFIXUP_END__ = .;}
  }"
