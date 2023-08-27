/* FreeMiNT/m68k specific low level interface, for the remote server for GDB.
   Copyright (C) 1995-2023 Free Software Foundation, Inc.

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

#include "server.h"
#include "mint-low.h"

#define M68K_D0_REGNUM  0
#define M68K_PS_REGNUM  16
#define M68K_PC_REGNUM  17
#define M68K_FP0_REGNUM 18

#if 1
#define DTRACE(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
#define DTRACE(fmt, ...)
#endif

/* FreeMiNT target op definitions for the m68k architecture.  */

class mint_m68k_target : public mint_process_target
{
public:

  const mint_regset_info *get_regs_info (void) override;

  const gdb_byte *sw_breakpoint_from_kind (int kind, int *size) override;

protected:

  void low_arch_setup (void) override;

  CORE_ADDR low_get_pc (regcache *regcache) override;

  void low_set_pc (regcache *regcache, CORE_ADDR newpc) override;

  int low_decr_pc_after_break (void) override;

  bool low_breakpoint_at (CORE_ADDR pc) override;
};

CORE_ADDR
mint_m68k_target::low_get_pc (regcache *regcache)
{
  uint32_t pc;

  collect_register (regcache, M68K_PC_REGNUM, &pc);
  DTRACE("%s: pc=%08x\n", __func__, pc);
  return pc;
}

void
mint_m68k_target::low_set_pc (regcache *regcache, CORE_ADDR pc)
{
  uint32_t newpc = pc;

  DTRACE("%s: pc=%08x\n", __func__, (unsigned int)pc);
  supply_register (regcache, M68K_PC_REGNUM, &newpc);
}

int
mint_m68k_target::low_decr_pc_after_break (void)
{
  return 2;
}

/* Defined in auto-generated file reg-m68k.c.  */
void init_registers_m68k (void);
extern const struct target_desc *tdesc_m68k;


#define m68k_num_gregs 18
#define m68k_num_fpregs 11

#define PT_D0 0
#define PT_D1 1
#define PT_D2 2
#define PT_D3 3
#define PT_D4 4
#define PT_D5 5
#define PT_D6 6
#define PT_D7 7
#define PT_A0 8
#define PT_A1 9
#define PT_A2 10
#define PT_A3 11
#define PT_A4 12
#define PT_A5 13
#define PT_A6 14
#define PT_A7 15
#define PT_USP PT_A7
#define PT_SR 16
#define PT_PC 17

#if defined(__mcoldfire__) || defined(__mfido__)
/* 2 longs per register */
#define PT_FP_SIZE 2
#else
/* 3 longs per register */
#define PT_FP_SIZE 3
#endif
#define PT_FP0 (0 * PT_FP_SIZE)
#define PT_FP1 (1 * PT_FP_SIZE)
#define PT_FP2 (2 * PT_FP_SIZE)
#define PT_FP3 (3 * PT_FP_SIZE)
#define PT_FP4 (4 * PT_FP_SIZE)
#define PT_FP5 (5 * PT_FP_SIZE)
#define PT_FP6 (6 * PT_FP_SIZE)
#define PT_FP7 (7 * PT_FP_SIZE)
#define PT_FPCR 24
#define PT_FPSR 25
#define PT_FPIAR 26

/* This table must line up with REGISTER_NAMES in tm-m68k.h */
static int const m68k_regmap[] =
{
  PT_D0 * 4, PT_D1 * 4, PT_D2 * 4, PT_D3 * 4,
  PT_D4 * 4, PT_D5 * 4, PT_D6 * 4, PT_D7 * 4,
  PT_A0 * 4, PT_A1 * 4, PT_A2 * 4, PT_A3 * 4,
  PT_A4 * 4, PT_A5 * 4, PT_A6 * 4, PT_USP * 4,
  PT_SR * 4, PT_PC * 4,
  PT_FP0 * 4, PT_FP1 * 4, PT_FP2 * 4, PT_FP3 * 4,
  PT_FP4 * 4, PT_FP5 * 4, PT_FP6 * 4, PT_FP7 * 4,
  PT_FPCR * 4, PT_FPSR * 4, PT_FPIAR * 4
};

#include "nat/gdb_ptrace.h"

static void
m68k_fill_gregset (struct regcache *regcache, char *buf)
{
  int i;

  for (i = 0; i < m68k_num_gregs; i++)
    collect_register (regcache, M68K_D0_REGNUM + i, (char *) buf + m68k_regmap[i]);
}

static void
m68k_store_gregset (struct regcache *regcache, const char *buf)
{
  int i;

  for (i = 0; i < m68k_num_gregs; i++)
    supply_register (regcache, M68K_D0_REGNUM + i, (const char *) buf + m68k_regmap[i]);
}

static void
m68k_fill_fpregset (struct regcache *regcache, char *buf)
{
  int i;

  for (i = 0; i < m68k_num_fpregs; i++)
    collect_register (regcache, M68K_FP0_REGNUM + i, (char *) buf + m68k_regmap[i]);
}

static void
m68k_store_fpregset (struct regcache *regcache, const char *buf)
{
  int i;

  for (i = 0; i < m68k_num_fpregs; i++)
    supply_register (regcache, M68K_FP0_REGNUM + i, (const char *) buf + m68k_regmap[i]);
}

static struct mint_regset_info m68k_regsets[] = {
  { PTRACE_GETREGS, PTRACE_SETREGS, m68k_num_gregs * 4,
    GENERAL_REGS,
    m68k_fill_gregset, m68k_store_gregset },
  { PTRACE_GETFPREGS, PTRACE_SETFPREGS, (PT_FPIAR + 1 - PT_FP0) * 4,
    FP_REGS,
    m68k_fill_fpregset, m68k_store_fpregset },
  /* End of list marker.  */
  NULL_REGSET
};

static const gdb_byte m68k_breakpoint[] = { 0x4a, 0xfc }; /* "illegal" instruction */
#define m68k_breakpoint_len 2

/* Implementation of target ops method "sw_breakpoint_from_kind".  */

const gdb_byte *
mint_m68k_target::sw_breakpoint_from_kind (int kind, int *size)
{
  *size = m68k_breakpoint_len;
  return m68k_breakpoint;
}

bool
mint_m68k_target::low_breakpoint_at (CORE_ADDR pc)
{
  unsigned char c[2];

  DTRACE("%s: pc=%08x\n", __func__, (unsigned int)pc);
  read_inferior_memory (pc, c, 2);
  if (c[0] == m68k_breakpoint[0] && c[1] == m68k_breakpoint[1])
    return true;

  return false;
}

const mint_regset_info *
mint_m68k_target::get_regs_info (void)
{
  return m68k_regsets;
}

void
mint_m68k_target::low_arch_setup (void)
{
  current_process ()->tdesc = tdesc_m68k;
}

/* The singleton target ops object.  */

static mint_m68k_target the_mint_m68k_target;

/* The mint target ops object.  */

mint_process_target *the_mint_target = &the_mint_m68k_target;

void
initialize_low_arch (void)
{
  /* Initialize the mint target descriptions.  */
  init_registers_m68k ();
}
