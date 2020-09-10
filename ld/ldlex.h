/* ldlex.h -
   Copyright (C) 1991-2024 Free Software Foundation, Inc.

   This file is part of the GNU Binutils.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#ifndef LDLEX_H
#define LDLEX_H

#include <stdio.h>

/* Codes used for the long options with no short synonyms.  150 isn't
   special; it's just an arbitrary non-ASCII char value.  */
enum option_values
{
  OPTION_ASSERT = 150,
  OPTION_CALL_SHARED,
  OPTION_CREF,
  OPTION_DEFSYM,
  OPTION_DEMANGLE,
  OPTION_DYNAMIC_LINKER,
  OPTION_NO_DYNAMIC_LINKER,
  OPTION_SYSROOT,
  OPTION_OUT_IMPLIB,
  OPTION_EB,
  OPTION_EL,
  OPTION_EMBEDDED_RELOCS,
  OPTION_EXPORT_DYNAMIC,
  OPTION_NO_EXPORT_DYNAMIC,
  OPTION_HELP,
  OPTION_IGNORE,
  OPTION_MAP,
  OPTION_NO_DEMANGLE,
  OPTION_NO_KEEP_MEMORY,
  OPTION_NO_WARN_MISMATCH,
  OPTION_NO_WARN_SEARCH_MISMATCH,
  OPTION_NOINHIBIT_EXEC,
  OPTION_NON_SHARED,
  OPTION_NO_WHOLE_ARCHIVE,
  OPTION_OFORMAT,
  OPTION_RELAX,
  OPTION_NO_RELAX,
  OPTION_NO_SYMBOLIC,
  OPTION_RETAIN_SYMBOLS_FILE,
  OPTION_RPATH,
  OPTION_RPATH_LINK,
  OPTION_SHARED,
  OPTION_SONAME,
  OPTION_SORT_COMMON,
  OPTION_SORT_SECTION,
  OPTION_SECTION_ORDERING_FILE,
  OPTION_STATS,
  OPTION_SYMBOLIC,
  OPTION_SYMBOLIC_FUNCTIONS,
  OPTION_TASK_LINK,
  OPTION_TBSS,
  OPTION_TDATA,
  OPTION_TTEXT,
  OPTION_TTEXT_SEGMENT,
  OPTION_TRODATA_SEGMENT,
  OPTION_TLDATA_SEGMENT,
  OPTION_TRADITIONAL_FORMAT,
  OPTION_UR,
  OPTION_VERBOSE,
  OPTION_VERSION,
  OPTION_VERSION_SCRIPT,
  OPTION_VERSION_EXPORTS_SECTION,
  OPTION_DYNAMIC_LIST,
  OPTION_DYNAMIC_LIST_CPP_NEW,
  OPTION_DYNAMIC_LIST_CPP_TYPEINFO,
  OPTION_DYNAMIC_LIST_DATA,
  OPTION_EXPORT_DYNAMIC_SYMBOL,
  OPTION_EXPORT_DYNAMIC_SYMBOL_LIST,
  OPTION_WARN_COMMON,
  OPTION_WARN_CONSTRUCTORS,
  OPTION_WARN_FATAL,
  OPTION_NO_WARN_FATAL,
  OPTION_NO_WARNINGS,
  OPTION_WARN_MULTIPLE_GP,
  OPTION_WARN_ONCE,
  OPTION_WARN_SECTION_ALIGN,
  OPTION_SPLIT_BY_RELOC,
  OPTION_SPLIT_BY_FILE ,
  OPTION_WHOLE_ARCHIVE,
  OPTION_ADD_DT_NEEDED_FOR_DYNAMIC,
  OPTION_NO_ADD_DT_NEEDED_FOR_DYNAMIC,
  OPTION_ADD_DT_NEEDED_FOR_REGULAR,
  OPTION_NO_ADD_DT_NEEDED_FOR_REGULAR,
  OPTION_WRAP,
  OPTION_FORCE_EXE_SUFFIX,
  OPTION_GC_SECTIONS,
  OPTION_NO_GC_SECTIONS,
  OPTION_PRINT_GC_SECTIONS,
  OPTION_NO_PRINT_GC_SECTIONS,
  OPTION_GC_KEEP_EXPORTED,
  OPTION_HASH_SIZE,
  OPTION_CHECK_SECTIONS,
  OPTION_NO_CHECK_SECTIONS,
  OPTION_NO_UNDEFINED,
  OPTION_INIT,
  OPTION_FINI,
  OPTION_SECTION_START,
  OPTION_UNIQUE,
  OPTION_TARGET_HELP,
  OPTION_ALLOW_SHLIB_UNDEFINED,
  OPTION_NO_ALLOW_SHLIB_UNDEFINED,
  OPTION_ALLOW_MULTIPLE_DEFINITION,
#if SUPPORT_ERROR_HANDLING_SCRIPT
  OPTION_ERROR_HANDLING_SCRIPT,
#endif
  OPTION_UNDEFINED_VERSION,
  OPTION_NO_UNDEFINED_VERSION,
  OPTION_DEFAULT_SYMVER,
  OPTION_DEFAULT_IMPORTED_SYMVER,
  OPTION_DISCARD_NONE,
  OPTION_SPARE_DYNAMIC_TAGS,
  OPTION_NO_DEFINE_COMMON,
  OPTION_NOSTDLIB,
  OPTION_NO_OMAGIC,
  OPTION_STRIP_DISCARDED,
  OPTION_NO_STRIP_DISCARDED,
  OPTION_ACCEPT_UNKNOWN_INPUT_ARCH,
  OPTION_NO_ACCEPT_UNKNOWN_INPUT_ARCH,
  OPTION_PIE,
  OPTION_NO_PIE,
  OPTION_UNRESOLVED_SYMBOLS,
  OPTION_WARN_UNRESOLVED_SYMBOLS,
  OPTION_ERROR_UNRESOLVED_SYMBOLS,
  OPTION_WARN_TEXTREL,
  OPTION_WARN_ALTERNATE_EM,
  OPTION_REDUCE_MEMORY_OVERHEADS,
  OPTION_MAX_CACHE_SIZE,
#if BFD_SUPPORTS_PLUGINS
  OPTION_PLUGIN,
  OPTION_PLUGIN_OPT,
  OPTION_PLUGIN_SAVE_TEMPS,
#endif /* BFD_SUPPORTS_PLUGINS */
  OPTION_DEFAULT_SCRIPT,
  OPTION_PRINT_OUTPUT_FORMAT,
  OPTION_PRINT_SYSROOT,
  OPTION_IGNORE_UNRESOLVED_SYMBOL,
  OPTION_PUSH_STATE,
  OPTION_POP_STATE,
  OPTION_DISABLE_MULTIPLE_DEFS_ABS,
  OPTION_PRINT_MEMORY_USAGE,
  OPTION_REQUIRE_DEFINED_SYMBOL,
  OPTION_ORPHAN_HANDLING,
  OPTION_FORCE_GROUP_ALLOCATION,
  OPTION_PRINT_MAP_DISCARDED,
  OPTION_NO_PRINT_MAP_DISCARDED,
  OPTION_PRINT_MAP_LOCALS,
  OPTION_NO_PRINT_MAP_LOCALS,
  OPTION_NON_CONTIGUOUS_REGIONS,
  OPTION_NON_CONTIGUOUS_REGIONS_WARNINGS,
  OPTION_DEPENDENCY_FILE,
  OPTION_CTF_VARIABLES,
  OPTION_NO_CTF_VARIABLES,
  OPTION_CTF_SHARE_TYPES,
  OPTION_ERROR_EXECSTACK,
  OPTION_NO_ERROR_EXECSTACK,
  OPTION_WARN_EXECSTACK_OBJECTS,
  OPTION_WARN_EXECSTACK,
  OPTION_NO_WARN_EXECSTACK,
  OPTION_WARN_RWX_SEGMENTS,
  OPTION_NO_WARN_RWX_SEGMENTS,
  OPTION_ERROR_RWX_SEGMENTS,
  OPTION_NO_ERROR_RWX_SEGMENTS,
  OPTION_ENABLE_LINKER_VERSION,
  OPTION_DISABLE_LINKER_VERSION,
  OPTION_REMAP_INPUTS,
  OPTION_REMAP_INPUTS_FILE,
  /* Used by emultempl/elf.em, emultempl/pe.em and emultempl/pep.em.  */
  OPTION_BUILD_ID,
  OPTION_EXCLUDE_LIBS,
  /* Used by emulparams/elf32mcore.sh, emultempl/beos.em, emultempl/pe.em
     and emultempl/pep.em.  */
  OPTION_BASE_FILE,
  /* Used by emultempl/ppc32elf.em and emultempl/ppc64elf.em.  */
  OPTION_NO_INLINE_OPT,
  OPTION_NO_PLT_ALIGN,
  OPTION_NO_STUBSYMS,
  OPTION_NO_TLS_GET_ADDR_OPT,
  OPTION_NO_TLS_OPT,
  OPTION_PLT_ALIGN,
  OPTION_STUBSYMS,
  /* Used by emultempl/armelf.em and emultempl/tic6xdsbt.em.  */
  OPTION_NO_MERGE_EXIDX_ENTRIES,
  /* Used by emultempl/aarch64elf.em and emultempl/armelf.em.  */
  OPTION_PIC_VENEER,
  OPTION_NO_ENUM_SIZE_WARNING,
  OPTION_NO_WCHAR_SIZE_WARNING,
  /* Used by emultempl/armelf.em and emultempl/pe.em.  */
  OPTION_THUMB_ENTRY,
  /* Used by emultempl/beos.em, emultempl/nto.em, emultempl/pe.em and
     emultempl/pep.em.  */
  OPTION_STACK,
  /* Used by emultempl/aarch64elf.em, emultempl/armelf.em,
     emultempl/cskyelf.em, emultempl/hppaelf.em, emultempl/metagelf.em
     and emultempl/ppc64elf.em.  */
  OPTION_STUBGROUP_SIZE,
  /* Used by emulparams/plt_unwind.sh.  */
  OPTION_LD_GENERATED_UNWIND_INFO,
  OPTION_NO_LD_GENERATED_UNWIND_INFO,
  /* Used by emultempl/aarch64elf.em.  */
  OPTION_FIX_ERRATUM_835769,
  OPTION_FIX_ERRATUM_843419,
  OPTION_NO_APPLY_DYNAMIC_RELOCS,
  /* Used by emultempl/aix.em.  */
  OPTION_AUTOIMP,
  OPTION_ERNOTOK,
  OPTION_EROK,
  OPTION_EXPALL,
  OPTION_EXPFULL,
  OPTION_EXPORT,
  OPTION_IMPORT,
  OPTION_INITFINI,
  OPTION_LOADMAP,
  OPTION_MAXDATA,
  OPTION_MAXSTACK,
  OPTION_MODTYPE,
  OPTION_NOAUTOIMP,
  OPTION_NOEXPALL,
  OPTION_NOEXPFULL,
  OPTION_NOSTRCMPCT,
  OPTION_PD,
  OPTION_PT,
  OPTION_STRCMPCT,
  OPTION_UNIX,
  OPTION_32,
  OPTION_64,
  OPTION_LIBPATH,
  OPTION_NOLIBPATH,
  /* Used by emultempl/alphaelf.em.  */
  OPTION_TASO,
  OPTION_SECUREPLT,
  OPTION_NO_SECUREPLT,
  /* Used by emultempl/armelf.em.  */
  OPTION_BE8,
  OPTION_TARGET1_REL,
  OPTION_TARGET1_ABS,
  OPTION_TARGET2,
  OPTION_FIX_V4BX,
  OPTION_USE_BLX,
  OPTION_VFP11_DENORM_FIX,
  OPTION_FIX_V4BX_INTERWORKING,
  OPTION_FIX_CORTEX_A8,
  OPTION_NO_FIX_CORTEX_A8,
  OPTION_FIX_ARM1176,
  OPTION_NO_FIX_ARM1176,
  OPTION_LONG_PLT,
  OPTION_STM32L4XX_FIX,
  OPTION_CMSE_IMPLIB,
  OPTION_IN_IMPLIB,
  /* Used by emultempl/avrelf.em.  */
  OPTION_NO_CALL_RET_REPLACEMENT,
  OPTION_PMEM_WRAP_AROUND,
  OPTION_NO_STUBS,
  OPTION_DEBUG_STUBS,
  OPTION_DEBUG_RELAX,
  /* Used by emultempl/bfin.em.  */
  OPTION_CODE_IN_L1,
  OPTION_DATA_IN_L1,
  /* Used by emultempl/cskyelf.em.  */
  OPTION_BRANCH_STUB,
  OPTION_NO_BRANCH_STUB,
  /* Used by emultempl/elf.em.  */
  OPTION_DISABLE_NEW_DTAGS,
  OPTION_ENABLE_NEW_DTAGS,
  OPTION_GROUP,
  OPTION_EH_FRAME_HDR,
  OPTION_NO_EH_FRAME_HDR,
  OPTION_HASH_STYLE,
  OPTION_PACKAGE_METADATA,
  OPTION_AUDIT,
  OPTION_COMPRESS_DEBUG,
  OPTION_ROSEGMENT,
  OPTION_NO_ROSEGMENT,
  /* Used by emultempl/hppaelf.em.  */
  OPTION_MULTI_SUBSPACE,
  /* Used by emultempl/ia64elf.em.  */
  OPTION_ITANIUM,
  /* Used by emultempl/m68hc1xelf.em.  */
  OPTION_NO_TRAMPOLINE,
  OPTION_BANK_WINDOW,
  /* Used by emultempl/m68kelf.em.  */
  OPTION_GOT,
  /* Used by emultempl/mipself.em.  */
  OPTION_INSN32,
  OPTION_NO_INSN32,
  OPTION_IGNORE_BRANCH_ISA,
  OPTION_NO_IGNORE_BRANCH_ISA,
  OPTION_COMPACT_BRANCHES,
  OPTION_NO_COMPACT_BRANCHES,
  /* Used by emultempl/msp430.em.  */
  OPTION_CODE_REGION,
  OPTION_DATA_REGION,
  OPTION_DISABLE_TRANS,
  /* Used by emultempl/nds32elf.em.  */
  OPTION_BASELINE,
  OPTION_ELIM_GC_RELOCS,
  OPTION_FP_AS_GP,
  OPTION_NO_FP_AS_GP,
  OPTION_REDUCE_FP_UPDATE,
  OPTION_NO_REDUCE_FP_UPDATE,
  OPTION_EXPORT_SYMBOLS,
  OPTION_HYPER_RELAX,
  OPTION_TLSDESC_TRAMPOLINE,
  OPTION_NO_TLSDESC_TRAMPOLINE,
  /* Used by emultempl/nto.em.  */
  OPTION_LAZY_STACK,
  /* Used by emultempl/pe.em, emultempl/pep.em and emultempl/beos.em.  */
  OPTION_DLL,
  OPTION_FILE_ALIGNMENT,
  OPTION_IMAGE_BASE,
  OPTION_MAJOR_IMAGE_VERSION,
  OPTION_MAJOR_OS_VERSION,
  OPTION_MAJOR_SUBSYSTEM_VERSION,
  OPTION_MINOR_IMAGE_VERSION,
  OPTION_MINOR_OS_VERSION,
  OPTION_MINOR_SUBSYSTEM_VERSION,
  OPTION_SECTION_ALIGNMENT,
  OPTION_SUBSYSTEM,
  OPTION_HEAP,
  OPTION_SUPPORT_OLD_CODE,
  OPTION_OUT_DEF,
  OPTION_EXPORT_ALL,
  OPTION_EXCLUDE_SYMBOLS,
  OPTION_EXCLUDE_ALL_SYMBOLS,
  OPTION_KILL_ATS,
  OPTION_STDCALL_ALIASES,
  OPTION_ENABLE_STDCALL_FIXUP,
  OPTION_DISABLE_STDCALL_FIXUP,
  OPTION_WARN_DUPLICATE_EXPORTS,
  OPTION_IMP_COMPAT,
  OPTION_ENABLE_AUTO_IMAGE_BASE,
  OPTION_DISABLE_AUTO_IMAGE_BASE,
  OPTION_DLL_SEARCH_PREFIX,
  OPTION_NO_DEFAULT_EXCLUDES,
  OPTION_DLL_ENABLE_AUTO_IMPORT,
  OPTION_DLL_DISABLE_AUTO_IMPORT,
  OPTION_ENABLE_EXTRA_PE_DEBUG,
  OPTION_DLL_ENABLE_RUNTIME_PSEUDO_RELOC,
  OPTION_DLL_DISABLE_RUNTIME_PSEUDO_RELOC,
  OPTION_LARGE_ADDRESS_AWARE,
  OPTION_DISABLE_LARGE_ADDRESS_AWARE,
  OPTION_DLL_ENABLE_RUNTIME_PSEUDO_RELOC_V1,
  OPTION_DLL_ENABLE_RUNTIME_PSEUDO_RELOC_V2,
  OPTION_EXCLUDE_MODULES_FOR_IMPLIB,
  OPTION_USE_NUL_PREFIXED_IMPORT_TABLES,
  OPTION_NO_LEADING_UNDERSCORE,
  OPTION_LEADING_UNDERSCORE,
  OPTION_ENABLE_LONG_SECTION_NAMES,
  OPTION_DISABLE_LONG_SECTION_NAMES,
  /* DLLCharacteristics flags.  */
  OPTION_DYNAMIC_BASE,
  OPTION_FORCE_INTEGRITY,
  OPTION_NX_COMPAT,
  OPTION_NO_ISOLATION,
  OPTION_NO_SEH,
  OPTION_NO_BIND,
  OPTION_WDM_DRIVER,
  OPTION_TERMINAL_SERVER_AWARE,
  OPTION_DEFAULT_IMAGE_BASE_LOW,
  OPTION_DEFAULT_IMAGE_BASE_HIGH,
  /* Determinism.  */
  OPTION_INSERT_TIMESTAMP,
  OPTION_NO_INSERT_TIMESTAMP,
  OPTION_PDB,
  OPTION_ENABLE_RELOC_SECTION,
  OPTION_DISABLE_RELOC_SECTION,
  /* DLL Characteristics flags.  */
  OPTION_DISABLE_DYNAMIC_BASE,
  OPTION_DISABLE_FORCE_INTEGRITY,
  OPTION_DISABLE_NX_COMPAT,
  OPTION_DISABLE_NO_ISOLATION,
  OPTION_DISABLE_NO_SEH,
  OPTION_DISABLE_NO_BIND,
  OPTION_DISABLE_WDM_DRIVER,
  OPTION_DISABLE_TERMINAL_SERVER_AWARE,
  /* Used by emultempl/pep.em.  */
  OPTION_DISABLE_HIGH_ENTROPY_VA,
  OPTION_HIGH_ENTROPY_VA,
  /* Used by emultempl/ppc32elf.em.  */
  OPTION_NEW_PLT,
  OPTION_OLD_PLT,
  OPTION_OLD_GOT,
  OPTION_PPC476_WORKAROUND,
  OPTION_NO_PPC476_WORKAROUND,
  OPTION_NO_PICFIXUP,
  OPTION_VLE_RELOC_FIXUP,
  /* Used by emultempl/ppc64elf.em.  */
  OPTION_PLT_STATIC_CHAIN,
  OPTION_NO_PLT_STATIC_CHAIN,
  OPTION_PLT_THREAD_SAFE,
  OPTION_NO_PLT_THREAD_SAFE,
  OPTION_PLT_LOCALENTRY,
  OPTION_NO_PLT_LOCALENTRY,
  OPTION_POWER10_STUBS,
  OPTION_NO_POWER10_STUBS,
  OPTION_NO_PCREL_OPT,
  OPTION_SAVRES,
  OPTION_NO_SAVRES,
  OPTION_DOTSYMS,
  OPTION_NO_DOTSYMS,
  OPTION_TLS_GET_ADDR_OPT,
  OPTION_TLS_GET_ADDR_REGSAVE,
  OPTION_NO_TLS_GET_ADDR_REGSAVE,
  OPTION_NO_OPD_OPT,
  OPTION_NO_TOC_OPT,
  OPTION_NO_MULTI_TOC,
  OPTION_NO_TOC_SORT,
  OPTION_NON_OVERLAPPING_OPD,
  /* Used by emultempl/riscvelf.em.  */
  OPTION_RELAX_GP,
  OPTION_NO_RELAX_GP,
  OPTION_CHECK_ULEB128,
  OPTION_NO_CHECK_ULEB128,
  /* Used by emultempl/rxelf.em.  */
  OPTION_NO_FLAG_MISMATCH_WARNINGS,
  OPTION_IGNORE_LMA,
  OPTION_NO_IGNORE_LMA,
  OPTION_FLAG_MISMATCH_WARNINGS,
  /* Used by emultempl/s390.em.  */
  OPTION_PGSTE,
  /* Used by emultempl/spuelf.em.  */
  OPTION_SPU_PLUGIN,
  OPTION_SPU_NO_OVERLAYS,
  OPTION_SPU_COMPACT_STUBS,
  OPTION_SPU_STUB_SYMS,
  OPTION_SPU_NON_OVERLAY_STUBS,
  OPTION_SPU_LOCAL_STORE,
  OPTION_SPU_STACK_ANALYSIS,
  OPTION_SPU_STACK_SYMS,
  OPTION_SPU_AUTO_OVERLAY,
  OPTION_SPU_AUTO_RELINK,
  OPTION_SPU_OVERLAY_RODATA,
  OPTION_SPU_SOFT_ICACHE,
  OPTION_SPU_LINE_SIZE,
  OPTION_SPU_NUM_LINES,
  OPTION_SPU_LRLIVE,
  OPTION_SPU_NON_IA_TEXT,
  OPTION_SPU_FIXED_SPACE,
  OPTION_SPU_RESERVED_SPACE,
  OPTION_SPU_EXTRA_STACK,
  OPTION_SPU_NO_AUTO_OVERLAY,
  OPTION_SPU_EMIT_FIXUPS,
  /* Used by emultempl/tic6xdsbt.em.  */
  OPTION_DSBT_INDEX,
  OPTION_DSBT_SIZE,
  /* Used by emultempl/ticoff.em.  */
  OPTION_COFF_FORMAT,
  /* Used by emultempl/vms.em.  */
  OPTION_IDENTIFICATION,
  /* Used by emultempl/vxworks.em.  */
  OPTION_FORCE_DYNAMIC,
  /* Used by emultempl/xtensaelf.em.  */
  OPTION_OPT_SIZEOPT,
  OPTION_LITERAL_MOVEMENT,
  OPTION_NO_LITERAL_MOVEMENT,
  OPTION_ABI_WINDOWED,
  OPTION_ABI_CALL0,
};

/* The initial parser states.  */
typedef enum input_enum
{
  input_selected,		/* We've set the initial state.  */
  input_script,
  input_mri_script,
  input_version_script,
  input_section_ordering_script,
  input_dynamic_list,
  input_defsym
} input_type;

extern input_type parser_input;

extern unsigned int lineno;
extern const char *lex_string;

/* In ldlex.l.  */
extern int yylex (void);
extern void lex_push_file (FILE *, const char *, unsigned int);
extern void lex_redirect (const char *, const char *, unsigned int);
extern void ldlex_script (void);
extern void ldlex_inputlist (void);
extern void ldlex_mri_script (void);
extern void ldlex_version_script (void);
extern void ldlex_version_file (void);
extern void ldlex_expression (void);
extern void ldlex_wild (void);
extern void ldlex_popstate (void);
extern void ldlex_backup (void);
extern const char* ldlex_filename (void);

/* In lexsup.c.  */
extern int lex_input (void);
extern void lex_unput (int);
extern void parse_args (unsigned, char **);

#endif
