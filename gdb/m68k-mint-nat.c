/* Native-dependent code for MiNT ELF.

   Copyright (C) 2006-2023 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "defs.h"
#include "inf-ptrace.h"
#include "inferior.h"
#include "m68k-tdep.h"
#include "objfiles.h"
#include "nat/mint-nat.h"

#include <sys/ptrace.h>

/* Set this to 1 to enable debug traces.  */
#if 0
# define TRACE(fmt, ...) printf (fmt, __VA_ARGS__)
#else
# define TRACE(fmt, ...)
#endif

/* From FreeMiNT's sys/mint/arch/register.h */
struct reg
{
  long regs[16]; /* D0-D7/A0-A7 */
  long sr;
  long pc;
};

struct fpreg
{
	union {
		long	regs[8][3];	/* FP0-FP7 */
		long	cfregs[8][2];	/* FP0-FP7 */
	} u;
	long	fpcr;
	long	fpsr;
	long	fpiar;
};

struct mint_nat_target : public inf_ptrace_target
{
  const char *pid_to_exec_file (int pid) override;

  /* Add our register access methods.  */
  void fetch_registers (struct regcache *, int) override;
  void store_registers (struct regcache *, int) override;

  enum target_xfer_status xfer_partial (enum target_object object,
					const char *annex,
					gdb_byte *readbuf,
					const gdb_byte *writebuf,
					ULONGEST offset, ULONGEST len,
					ULONGEST *xfered_len) override;

protected:
  void post_startup_inferior (ptid_t) override;

private:
  void child_initialize (pid_t pid);
};

static mint_nat_target the_mint_nat_target;

const char *
mint_nat_target::pid_to_exec_file (int pid)
{
  return mint_nat::pid_to_exec_file (pid);
}

/* Transferring arbitrary registers between GDB and inferior.  */

/* Fetch register REGNO from the child process.  If REGNO is -1, do
   this for all registers (including the floating point and SSE
   registers).  */

void
mint_nat_target::fetch_registers (struct regcache *regcache, int regno)
{
  pid_t pid = get_ptrace_pid (regcache->ptid ());
  TRACE ("fetch_registers pid=%d\n", (int) pid);

  struct reg reg;
  ptrace (PT_GETREGS, pid, (caddr_t) &reg, 0);

  for (regno = 0; regno <= 15; regno++)
    regcache->raw_supply (regno + M68K_D0_REGNUM, &reg.regs[regno]);

  regcache->raw_supply (M68K_PS_REGNUM, &reg.sr);
  regcache->raw_supply (M68K_PC_REGNUM, &reg.pc);

  struct gdbarch *gdbarch = regcache->arch ();
  m68k_gdbarch_tdep *tdep = gdbarch_tdep<m68k_gdbarch_tdep> (gdbarch);
  struct fpreg fpreg;
  if (tdep->fpregs_present && ptrace (PT_GETFPREGS, pid, (caddr_t) &fpreg, 0) == 0)
  {
    for (regno = 0; regno <= 7; regno++)
    {
      if (tdep->flavour == m68k_coldfire_flavour)
        regcache->raw_supply (regno + M68K_FP0_REGNUM, &fpreg.u.cfregs[regno][0]);
      else
        regcache->raw_supply (regno + M68K_FP0_REGNUM, &fpreg.u.regs[regno][0]);
    }
    regcache->raw_supply (M68K_FPC_REGNUM, &fpreg.fpcr);
    regcache->raw_supply (M68K_FPS_REGNUM, &fpreg.fpsr);
    regcache->raw_supply (M68K_FPI_REGNUM, &fpreg.fpiar);
  }
}

/* Store register REGNO back into the child process.  If REGNO is -1,
   do this for all registers (including the floating point and SSE
   registers).  */
void
mint_nat_target::store_registers (struct regcache *regcache, int regno)
{
  pid_t pid = get_ptrace_pid (regcache->ptid ());
  TRACE ("store_registers pid=%d\n", (int) pid);

  struct reg reg;

  for (regno = 0; regno <= 15; regno++)
    regcache->raw_collect (regno + M68K_D0_REGNUM, &reg.regs[regno]);

  regcache->raw_collect (M68K_PS_REGNUM, &reg.sr);
  regcache->raw_collect (M68K_PC_REGNUM, &reg.pc);

  ptrace (PT_SETREGS, pid, (caddr_t) &reg, 0);

  struct gdbarch *gdbarch = regcache->arch ();
  m68k_gdbarch_tdep *tdep = gdbarch_tdep<m68k_gdbarch_tdep> (gdbarch);
  if (tdep->fpregs_present)
  {
    struct fpreg fpreg;
    for (regno = 0; regno <= 7; regno++)
    {
      if (tdep->flavour == m68k_coldfire_flavour)
        regcache->raw_collect (regno + M68K_FP0_REGNUM, &fpreg.u.cfregs[regno][0]);
      else
        regcache->raw_collect (regno + M68K_FP0_REGNUM, &fpreg.u.regs[regno][0]);
    }

    regcache->raw_collect (M68K_FPC_REGNUM, &fpreg.fpcr);
    regcache->raw_collect (M68K_FPS_REGNUM, &fpreg.fpsr);
    regcache->raw_collect (M68K_FPI_REGNUM, &fpreg.fpiar);

    ptrace (PT_SETFPREGS, pid, (caddr_t) &fpreg, 0);
  }
}

void
mint_nat_target::child_initialize (pid_t pid)
{
  uint32_t basepage;
  if (ptrace (PT_BASEPAGE, pid, 0, (int) &basepage))
    error ("cannot get basepage address for pid %d", pid);

  CORE_ADDR text_vma = basepage + 0x100;
  TRACE ("text_vma=0x%08lx\n", (long) text_vma);

  objfile *objf = current_program_space->symfile_object_file;
  section_offsets new_offsets (objf->section_offsets.size ());

  for (obj_section *osect : objf->sections ())
    {
      if (! (osect->the_bfd_section->flags & SEC_ALLOC))
	continue;

      int osect_idx = osect - objf->sections_start;

      TRACE ("idx=%d name=%s addr=0x%08lx offset=0x%08lx\n", osect_idx, osect->the_bfd_section->name, (long) osect->addr (), (long) objf->section_offsets[osect_idx]);

      new_offsets[osect_idx] = text_vma;
    }

  objfile_relocate (objf, new_offsets);
}

/* Implement the virtual inf_ptrace_target::post_startup_inferior method.  */

void
mint_nat_target::post_startup_inferior (ptid_t ptid)
{
  TRACE ("post_startup_inferior pid=%d\n", (int) ptid.pid ());
  child_initialize (ptid.pid ());
}

/* Implement the "xfer_partial" target_ops method.  */

enum target_xfer_status
mint_nat_target::xfer_partial (enum target_object object,
			       const char *annex, gdb_byte *readbuf,
			       const gdb_byte *writebuf,
			       ULONGEST offset, ULONGEST len,
			       ULONGEST *xfered_len)
{
  TRACE ("xfer_partial %d\n", object);
  pid_t pid = inferior_ptid.pid ();

  switch (object)
    {
    case TARGET_OBJECT_MEMORY:
      {
	size_t xfered;
	int res;
	if (writebuf != nullptr)
	  res = mint_nat::write_memory (pid, writebuf, offset, len, &xfered);
	else
	  res = mint_nat::read_memory (pid, readbuf, offset, len, &xfered);
	if (res != 0)
	  return TARGET_XFER_E_IO;
	if (xfered == 0)
	  return TARGET_XFER_EOF;
	*xfered_len = (ULONGEST) xfered;
	return TARGET_XFER_OK;
      }
    default:
      return inf_ptrace_target::xfer_partial (object, annex,
					      readbuf, writebuf, offset,
					      len, xfered_len);
    }
}

void _initialize_mint_nat ();
void
_initialize_mint_nat ()
{
  /* Register the target.  */
  add_inf_child_target (&the_mint_nat_target);
}
