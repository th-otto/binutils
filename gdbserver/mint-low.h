/* Internal interfaces for the FreeMiNT specific target code for gdbserver.
   Copyright (C) 2002-2023 Free Software Foundation, Inc.

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

#ifndef GDBSERVER_MINT_LOW_H
#define GDBSERVER_MINT_LOW_H

#include "nat/mint-nat.h"

#define PTRACE_TYPE_ARG3 caddr_t
#define USE_SIGTRAP_SIGINFO 0

struct regcache;
struct target_desc;

/*  Some information relative to a given register set.   */

enum regset_type {
  GENERAL_REGS,
  FP_REGS,
};

/* The arch's regsets array initializer must be terminated with a NULL
   regset.  */
#define NULL_REGSET \
  { 0, 0, -1, (enum regset_type) -1, NULL, NULL }

struct mint_regset_info
{
  /* The ptrace request needed to get/set registers of this set.  */
  int get_request, set_request;
  /* The size of the register set.  */
  int size;
  enum regset_type type;
  /* Fill the buffer BUF from the contents of the given REGCACHE.  */
  void (*fill_function) (struct regcache *regcache, char *buf);
  /* Store the register value in BUF in the given REGCACHE.  */
  void (*store_function) (struct regcache *regcache, const char *buf);
};

/* Target ops definitions for a MiNT target.  */

class mint_process_target : public process_stratum_target
{
public:

  int create_inferior (const char *program,
		       const std::vector<char *> &program_args) override;

  void post_create_inferior () override;

  int attach (unsigned long pid) override;

  int kill (process_info *proc) override;

  int detach (process_info *proc) override;

  void mourn (process_info *proc) override;

  void join (int pid) override;

  /* Return true iff the thread with process ID PID is alive.  */
  bool thread_alive (ptid_t pid) override;

  void resume (thread_resume *resume_info, size_t n) override;

  ptid_t wait (ptid_t ptid, target_waitstatus *status,
	       target_wait_flags options) override;

  void fetch_registers (regcache *regcache, int regno) override;

  void store_registers (regcache *regcache, int regno) override;

  int read_memory (CORE_ADDR memaddr, unsigned char *myaddr,
		   int len) override;

  int write_memory (CORE_ADDR memaddr, const unsigned char *myaddr,
		    int len) override;

  void request_interrupt () override;

  const gdb_byte *sw_breakpoint_from_kind (int kind, int *size) override = 0;

  bool supports_z_point_type (char z_type) override;

  int insert_point (enum raw_bkpt_type type, CORE_ADDR addr,
		    int size, struct raw_breakpoint *bp) override;

  int remove_point (enum raw_bkpt_type type, CORE_ADDR addr,
		    int size, struct raw_breakpoint *bp) override;

  bool stopped_by_sw_breakpoint () override;

  bool supports_stopped_by_sw_breakpoint () override;

  bool supports_read_offsets () override;

  int read_offsets (CORE_ADDR *text, CORE_ADDR *data) override;

  bool supports_non_stop () override;

  bool supports_multi_process () override;

  bool supports_fork_events () override;

  bool supports_vfork_events () override;

  bool supports_exec_events () override;

  bool supports_pid_to_exec_file () override;

  const char *pid_to_exec_file (int pid) override;

  bool supports_catch_syscall () override;

  /* Return the information to access registers.  This has public
     visibility because proc-service uses it.  */
  virtual const mint_regset_info *get_regs_info () = 0;

protected:
  /* The architecture-specific "low" methods are listed below.  */

  virtual CORE_ADDR low_get_pc (regcache *regcache) = 0;

  virtual void low_set_pc (regcache *regcache, CORE_ADDR newpc) = 0;

  /* Return true if there is a breakpoint at PC.  */
  virtual bool low_breakpoint_at (CORE_ADDR pc) = 0;

  /* How many bytes the PC should be decremented after a break.  */
  virtual int low_decr_pc_after_break () = 0;

  /* Architecture-specific setup for the current thread.  */
  virtual void low_arch_setup () = 0;
};

extern mint_process_target *the_mint_target;

void initialize_low_arch (void);

#endif /* GDBSERVER_MINT_LOW_H */
