# Customizer script for m68kmintelf emulation.
# It is sourced by genscripts.sh to customize the templates.

# This is essentially an m68kelf emulation, with a few overrides.
source_sh ${srcdir}/emulparams/m68kelf.sh

# The linker will produce PRG/ELF executables, not plain ELF.
OUTPUT_FORMAT="elf32-atariprg"
RELOCATEABLE_OUTPUT_FORMAT="elf32-m68k"

# Emulation template. Suffix ".em" will be appended.
TEMPLATE_NAME=elf
EXTRA_EM_FILE=m68kmintelf

# Linker script template. Suffix ".sc" will be appended.
SCRIPT_NAME=m68kmintelf

# Additional parameters for above templates.
GENERATE_SHLIB_SCRIPT=
GENERATE_PIE_SCRIPT=

# Use builtin linker script files.
COMPILE_IN=yes

# Not quite right for ELF, but we provide both symbols
USER_LABEL_PREFIX=_

# Don't search for dynamic libraries (yet)
LDEMUL_OPEN_DYNAMIC_ARCHIVE=NULL
