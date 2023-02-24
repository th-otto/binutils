/* A.out "format 1" file handling code for BFD.
   Copyright (C) 1990-2018 Free Software Foundation, Inc.
   Written by Cygnus Support.

   This file is part of BFD, the Binary File Descriptor library.

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

#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"

#include "aout/sun4.h"
#include "libaout.h"		/* BFD a.out internal data structures.  */

#include "aout/aout64.h"
#include "aout/stab_gnu.h"
#include "aout/ar.h"

/* This is needed to reject a NewsOS file, e.g. in
   gdb/testsuite/gdb.t10/crossload.exp. <kingdon@cygnus.com>
   I needed to add M_UNKNOWN to recognize a 68000 object, so this will
   probably no longer reject a NewsOS object.  <ian@cygnus.com>.  */
#ifndef MACHTYPE_OK
#define MACHTYPE_OK(mtype) \
  (((mtype) == M_SPARC && bfd_lookup_arch (bfd_arch_sparc, 0) != NULL) \
   || (((mtype) == M_UNKNOWN || (mtype) == M_68010 || (mtype) == M_68020) \
       && bfd_lookup_arch (bfd_arch_m68k, 0) != NULL))
#endif

/* The file @code{aoutf1.h} contains the code for BFD's
   a.out back end.  Control over the generated back end is given by these
   two preprocessor names:
   @table @code
   @item ARCH_SIZE
   This value should be either 32 or 64, depending upon the size of an
   int in the target format. It changes the sizes of the structs which
   perform the memory/disk mapping of structures.

   The 64 bit backend may only be used if the host compiler supports 64
   ints (eg long long with gcc), by defining the name @code{BFD_HOST_64_BIT} in @code{bfd.h}.
   With this name defined, @emph{all} bfd operations are performed with 64bit
   arithmetic, not just those to a 64bit target.

   @item TARGETNAME
   The name put into the target vector.
   @item
   @end table.  */

#if ARCH_SIZE == 64
#define sunos_set_arch_mach sunos_64_set_arch_mach
#define sunos_write_object_contents aout_64_sunos4_write_object_contents
#else
#define sunos_set_arch_mach sunos_32_set_arch_mach
#define sunos_write_object_contents aout_32_sunos4_write_object_contents
#endif

/* Merge backend data into the output file.
   This is necessary on sparclet-aout where we want the resultant machine
   number to be M_SPARCLET if any input file is M_SPARCLET.  */

#define MY_bfd_merge_private_bfd_data sunos_merge_private_bfd_data

static bool
sunos_merge_private_bfd_data (bfd *ibfd, struct bfd_link_info *info)
{
  bfd *obfd = info->output_bfd;
  if (bfd_get_flavour (ibfd) != bfd_target_aout_flavour
      || bfd_get_flavour (obfd) != bfd_target_aout_flavour)
    return true;

  if (bfd_get_arch (obfd) == bfd_arch_sparc)
    {
      if (bfd_get_mach (obfd) < bfd_get_mach (ibfd))
	bfd_set_arch_mach (obfd, bfd_arch_sparc, bfd_get_mach (ibfd));
    }

  return true;
}

/* This is either sunos_32_set_arch_mach or sunos_64_set_arch_mach,
   depending upon ARCH_SIZE.  */

static void
sunos_set_arch_mach (bfd *abfd, enum machine_type machtype)
{
  /* Determine the architecture and machine type of the object file.  */
  enum bfd_architecture arch;
  unsigned long machine;

  switch (machtype)
    {
    case M_UNKNOWN:
      /* Some Sun3s make magic numbers without cpu types in them, so
	 we'll default to the 68000.  */
      arch = bfd_arch_m68k;
      machine = bfd_mach_m68000;
      break;

    case M_68010:
    case M_HP200:
      arch = bfd_arch_m68k;
      machine = bfd_mach_m68010;
      break;

    case M_68020:
    case M_HP300:
      arch = bfd_arch_m68k;
      machine = bfd_mach_m68020;
      break;

    case M_SPARC:
      arch = bfd_arch_sparc;
      machine = 0;
      break;

    case M_SPARCLET:
      arch = bfd_arch_sparc;
      machine = bfd_mach_sparc_sparclet;
      break;

    case M_SPARCLITE_LE:
      arch = bfd_arch_sparc;
      machine = bfd_mach_sparc_sparclite_le;
      break;

    case M_386:
    case M_386_DYNIX:
      arch = bfd_arch_i386;
      machine = 0;
      break;

    case M_HPUX:
      arch = bfd_arch_m68k;
      machine = 0;
      break;

    default:
      arch = bfd_arch_obscure;
      machine = 0;
      break;
    }
  bfd_set_arch_mach (abfd, arch, machine);
}

#define SET_ARCH_MACH(ABFD, EXECP) \
  NAME(sunos,set_arch_mach) (ABFD, N_MACHTYPE (EXECP)); \
  choose_reloc_size(ABFD);

/* Determine the size of a relocation entry, based on the architecture.  */

static void
choose_reloc_size (bfd *abfd)
{
  switch (bfd_get_arch (abfd))
    {
    case bfd_arch_sparc:
      obj_reloc_entry_size (abfd) = RELOC_EXT_SIZE;
      break;
    default:
      obj_reloc_entry_size (abfd) = RELOC_STD_SIZE;
      break;
    }
}

/* Write an object file in SunOS format.  Section contents have
   already been written.  We write the file header, symbols, and
   relocation.  The real name of this function is either
   aout_64_sunos4_write_object_contents or
   aout_32_sunos4_write_object_contents, depending upon ARCH_SIZE.  */

static bool
sunos_write_object_contents (bfd *abfd)
{
  struct external_exec exec_bytes;
  struct internal_exec *execp = exec_hdr (abfd);

  /* Magic number, maestro, please!  */
  switch (bfd_get_arch (abfd))
    {
    case bfd_arch_m68k:
      switch (bfd_get_mach (abfd))
	{
	case bfd_mach_m68000:
	  N_SET_MACHTYPE (execp, M_UNKNOWN);
	  break;
	case bfd_mach_m68010:
	  N_SET_MACHTYPE (execp, M_68010);
	  break;
	default:
	case bfd_mach_m68020:
	  N_SET_MACHTYPE (execp, M_68020);
	  break;
	}
      break;
    case bfd_arch_sparc:
      switch (bfd_get_mach (abfd))
	{
	case bfd_mach_sparc_sparclet:
	  N_SET_MACHTYPE (execp, M_SPARCLET);
	  break;
	case bfd_mach_sparc_sparclite_le:
	  N_SET_MACHTYPE (execp, M_SPARCLITE_LE);
	  break;
	default:
	  N_SET_MACHTYPE (execp, M_SPARC);
	  break;
	}
      break;
    case bfd_arch_i386:
      N_SET_MACHTYPE (execp, M_386);
      break;
    default:
      N_SET_MACHTYPE (execp, M_UNKNOWN);
    }

  choose_reloc_size (abfd);

  N_SET_FLAGS (execp, aout_backend_info (abfd)->exec_hdr_flags);

  N_SET_DYNAMIC (execp, (long)(bfd_get_file_flags (abfd) & DYNAMIC));

  WRITE_HEADERS (abfd, execp);

  return true;
}

/* Core files.  */

#define CORE_MAGIC   0x080456
#define CORE_NAMELEN 16

/* The core structure is taken from the Sun documentation.
  Unfortunately, they don't document the FPA structure, or at least I
  can't find it easily.  Fortunately the core header contains its own
  length.  So this shouldn't cause problems, except for c_ucode, which
  so far we don't use but is easy to find with a little arithmetic.  */

/* But the reg structure can be gotten from the SPARC processor handbook.
  This really should be in a GNU include file though so that gdb can use
  the same info.  */
struct regs
{
  int r_psr;
  int r_pc;
  int r_npc;
  int r_y;
  int r_g1;
  int r_g2;
  int r_g3;
  int r_g4;
  int r_g5;
  int r_g6;
  int r_g7;
  int r_o0;
  int r_o1;
  int r_o2;
  int r_o3;
  int r_o4;
  int r_o5;
  int r_o6;
  int r_o7;
};

/* Taken from Sun documentation: */

/* FIXME:  It's worse than we expect.  This struct contains TWO substructs
  neither of whose size we know, WITH STUFF IN BETWEEN THEM!  We can't
  even portably access the stuff in between!  */

struct external_sparc_core
{
  int c_magic;				/* Corefile magic number.  */
  int c_len;				/* Sizeof (struct core).  */
#define	SPARC_CORE_LEN	432
  struct regs c_regs;			/* General purpose registers -- MACHDEP SIZE.  */
  struct external_exec c_aouthdr;	/* A.out header.  */
  int c_signo;				/* Killing signal, if any.  */
  int c_tsize;				/* Text size (bytes).  */
  int c_dsize;				/* Data size (bytes).  */
  int c_ssize;				/* Stack size (bytes).  */
  char c_cmdname[CORE_NAMELEN + 1];	/* Command name.  */
  double fp_stuff[1];			/* External FPU state (size unknown by us).  */
  /* The type "double" is critical here, for alignment.
     SunOS declares a struct here, but the struct's
     alignment is double since it contains doubles.  */
  int c_ucode;				/* Exception no. from u_code.  */
  /* This member is not accessible by name since
     we don't portably know the size of fp_stuff.  */
};

/* Core files generated by the BCP (the part of Solaris which allows
   it to run SunOS4 a.out files).  */
struct external_solaris_bcp_core
{
  int c_magic;				/* Corefile magic number.  */
  int c_len;				/* Sizeof (struct core).  */
#define	SOLARIS_BCP_CORE_LEN	456
  struct regs c_regs;			/* General purpose registers -- MACHDEP SIZE.  */
  int c_exdata_vp;			/* Exdata structure.  */
  int c_exdata_tsize;
  int c_exdata_dsize;
  int c_exdata_bsize;
  int c_exdata_lsize;
  int c_exdata_nshlibs;
  short c_exdata_mach;
  short c_exdata_mag;
  int c_exdata_toffset;
  int c_exdata_doffset;
  int c_exdata_loffset;
  int c_exdata_txtorg;
  int c_exdata_datorg;
  int c_exdata_entloc;
  int c_signo;				/* Killing signal, if any.  */
  int c_tsize;				/* Text size (bytes).  */
  int c_dsize;				/* Data size (bytes).  */
  int c_ssize;				/* Stack size (bytes).  */
  char c_cmdname[CORE_NAMELEN + 1];	/* Command name.  */
  double fp_stuff[1];			/* External FPU state (size unknown by us).  */
  /* The type "double" is critical here, for alignment.
     SunOS declares a struct here, but the struct's
     alignment is double since it contains doubles.  */
  int c_ucode;				/* Exception no. from u_code.  */
  /* This member is not accessible by name since
     we don't portably know the size of fp_stuff.  */
};

struct external_sun3_core
{
  int c_magic;				/* Corefile magic number.  */
  int c_len;				/* Sizeof (struct core).  */
#define	SUN3_CORE_LEN	826		/* As of SunOS 4.1.1.  */
  int c_regs[18];			/* General purpose registers -- MACHDEP SIZE.  */
  struct external_exec c_aouthdr;	/* A.out header.  */
  int c_signo;				/* Killing signal, if any.  */
  int c_tsize;				/* Text size (bytes).  */
  int c_dsize;				/* Data size (bytes).  */
  int c_ssize;				/* Stack size (bytes).  */
  char c_cmdname[CORE_NAMELEN + 1];	/* Command name.  */
  double fp_stuff[1];			/* External FPU state (size unknown by us).  */
  /* The type "double" is critical here, for alignment.
     SunOS declares a struct here, but the struct's
     alignment is double since it contains doubles.  */
  int c_ucode;				/* Exception no. from u_code.  */
  /* This member is not accessible by name since
     we don't portably know the size of fp_stuff.  */
};

struct internal_sunos_core
{
  int c_magic;				/* Corefile magic number.  */
  int c_len;				/* Sizeof (struct core).  */
  long c_regs_pos;			/* File offset of General purpose registers.  */
  int c_regs_size;			/* Size of General purpose registers.  */
  struct internal_exec c_aouthdr;	/* A.out header.  */
  int c_signo;				/* Killing signal, if any.  */
  int c_tsize;				/* Text size (bytes).  */
  int c_dsize;				/* Data size (bytes).  */
  bfd_vma c_data_addr;			/* Data start (address).  */
  int c_ssize;				/* Stack size (bytes).  */
  bfd_vma c_stacktop;			/* Stack top (address).  */
  char c_cmdname[CORE_NAMELEN + 1];	/* Command name.  */
  long fp_stuff_pos;			/* File offset of external FPU state (regs).  */
  int fp_stuff_size;			/* Size of it.  */
  int c_ucode;				/* Exception no. from u_code.  */
};

/* Need this cast because ptr is really void *.  */
#define core_hdr(bfd)      ((bfd)->tdata.sun_core_data)
#define core_datasec(bfd)  (core_hdr (bfd)->data_section)
#define core_stacksec(bfd) (core_hdr (bfd)->stack_section)
#define core_regsec(bfd)   (core_hdr (bfd)->reg_section)
#define core_reg2sec(bfd)  (core_hdr (bfd)->reg2_section)

/* These are stored in the bfd's tdata.  */
struct sun_core_struct
{
  struct internal_sunos_core *hdr;	/* Core file header.  */
  asection *data_section;
  asection *stack_section;
  asection *reg_section;
  asection *reg2_section;
};

static char *
sunos4_core_file_failing_command (bfd *abfd)
{
  return core_hdr (abfd)->hdr->c_cmdname;
}

static int
sunos4_core_file_failing_signal (bfd *abfd)
{
  return core_hdr (abfd)->hdr->c_signo;
}

static bool
sunos4_core_file_matches_executable_p (bfd *core_bfd, bfd *exec_bfd)
{
  if (core_bfd->xvec != exec_bfd->xvec)
    {
      bfd_set_error (bfd_error_system_call);
      return false;
    }

  /* Solaris core files do not include an aouthdr.  */
  if ((core_hdr (core_bfd)->hdr)->c_len == SOLARIS_BCP_CORE_LEN)
    return true;

  return memcmp ((char *) &((core_hdr (core_bfd)->hdr)->c_aouthdr),
		 (char *) exec_hdr (exec_bfd),
		 sizeof (struct internal_exec)) == 0;
}

#define MY_set_sizes sunos4_set_sizes

static bool
sunos4_set_sizes (bfd *abfd)
{
  switch (bfd_get_arch (abfd))
    {
    default:
      return false;
    case bfd_arch_sparc:
      adata (abfd).page_size = 0x2000;
      adata (abfd).segment_size = 0x2000;
      adata (abfd).exec_bytes_size = EXEC_BYTES_SIZE;
      return true;
    case bfd_arch_m68k:
      adata (abfd).page_size = 0x2000;
      adata (abfd).segment_size = 0x20000;
      adata (abfd).exec_bytes_size = EXEC_BYTES_SIZE;
      return true;
    }
}

/* We default to setting the toolversion field to 1, as is required by
   SunOS.  */
#ifndef MY_exec_hdr_flags
#define MY_exec_hdr_flags 1
#endif

#ifndef MY_entry_is_text_address
#define MY_entry_is_text_address 0
#endif
#ifndef MY_add_dynamic_symbols
#define MY_add_dynamic_symbols 0
#endif
#ifndef MY_add_one_symbol
#define MY_add_one_symbol 0
#endif
#ifndef MY_link_dynamic_object
#define MY_link_dynamic_object 0
#endif
#ifndef MY_write_dynamic_symbol
#define MY_write_dynamic_symbol 0
#endif
#ifndef MY_check_dynamic_reloc
#define MY_check_dynamic_reloc 0
#endif
#ifndef MY_finish_dynamic_link
#define MY_finish_dynamic_link 0
#endif

static const struct aout_backend_data sunos4_aout_backend =
{
  0,				/* Zmagic files are not contiguous.  */
  1,				/* Text includes header.  */
  MY_entry_is_text_address,
  MY_exec_hdr_flags,
  0,				/* Default text vma.  */
  sunos4_set_sizes,
  0,				/* Header is counted in zmagic text.  */
  MY_add_dynamic_symbols,
  MY_add_one_symbol,
  MY_link_dynamic_object,
  MY_write_dynamic_symbol,
  MY_check_dynamic_reloc,
  MY_finish_dynamic_link
};

#define	MY_core_file_failing_command	sunos4_core_file_failing_command
#define	MY_core_file_failing_signal	sunos4_core_file_failing_signal
#define	MY_core_file_matches_executable_p sunos4_core_file_matches_executable_p

#define MY_bfd_debug_info_start		_bfd_void_bfd
#define MY_bfd_debug_info_end		_bfd_void_bfd
#define MY_bfd_debug_info_accumulate	_bfd_void_bfd_asection
#define MY_core_file_p			_bfd_dummy_target
#define MY_write_object_contents	NAME(aout, sunos4_write_object_contents)
#define MY_backend_data			& sunos4_aout_backend

#ifndef TARGET_IS_LITTLE_ENDIAN_P
#define TARGET_IS_BIG_ENDIAN_P
#endif

#include "aout-target.h"
