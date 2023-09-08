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
  .text ${RELOCATING+${TEXT_START_ADDR}}:
  {
    CREATE_OBJECT_SYMBOLS
    *(.text)
    ${CONSTRUCTING+CONSTRUCTORS}
    ${RELOCATING+etext = .;}
    ${RELOCATING+_etext = .;}
    ${RELOCATING+${USER_LABEL_PREFIX}_etext = .;}
  }

  .data :
  {
    *(.data)
    ${RELOCATING+_edata = .;}
    ${RELOCATING+${USER_LABEL_PREFIX}_edata = .;}
  }

  .bss :
  {
    ${RELOCATING+${USER_LABEL_PREFIX}_bss_start = .;}
    *(.bss)
    *(COMMON)
    ${RELOCATING+_end = .;}
    ${RELOCATING+${USER_LABEL_PREFIX}_end = .;}
  }

  /* Discard the following ELF sections.
   * Some of them may be present in ELF libgcc.a.
   */
  /DISCARD/ :
  {
      *(.comment)
      *(.debug*)
      *(.note*)
      *(.gnu.attributes)
  }
}
EOF
