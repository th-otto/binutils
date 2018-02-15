# Copyright (C) 2014-2017 Free Software Foundation, Inc.
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.
#
cat <<EOF
/* Copyright (C) 2014-2017 Free Software Foundation, Inc.

   Copying and distribution of this script, with or without modification,
   are permitted in any medium without royalty provided the copyright
   notice and this notice are preserved.  */

${RELOCATING+OUTPUT_FORMAT(${OUTPUT_FORMAT})}
${RELOCATING-OUTPUT_FORMAT(${RELOCATEABLE_OUTPUT_FORMAT})}
${RELOCATING+${LIB_SEARCH_DIRS}}
SECTIONS
{
  ${RELOCATING+/* The VMA of the .text section is ${TEXT_START_ADDR} instead of 0
     because the extended MiNT header is just before,
     at the beginning of the TEXT segment.  */}
  .text ${RELOCATING+${TEXT_START_ADDR}}: SUBALIGN(2)
  {
    CREATE_OBJECT_SYMBOLS
    *(.text .text.*)
    *(.rodata .rodata.*) /* Only present in ELF objects */
    ${RELOCATING+ *(.ctors)}
    ${RELOCATING+ *(.dtors)}
    ${CONSTRUCTING+CONSTRUCTORS}
    ${RELOCATING+etext = .;}
    ${RELOCATING+_etext = .;}
  }

  .data : SUBALIGN(2)
  {
    *(.data .data.*)
    ${RELOCATING+edata = .;}
    ${RELOCATING+_edata = .;}
  }

  .bss :
  {
    ${RELOCATING+_bss_start = .;}
    *(.bss .bss.*)
    *(COMMON)
    ${RELOCATING+end = .;}
    ${RELOCATING+_end = .;}
  }

  /* Unfortunately, stabs are not mappable from ELF to a.out.
     It can probably be fixed with some amount of work.  */
  /DISCARD/ :
  { *(.stab) *(.stab*) *(.debug) *(.debug*) *(.comment) *(.gnu.warning.*) }

}
EOF
