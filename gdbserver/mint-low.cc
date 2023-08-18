/* Copyright (C) 2020-2023 Free Software Foundation, Inc.

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
#include "target.h"
#include "mint-low.h"
#include "nat/mint-nat.h"

#include <sys/param.h>
#include <sys/types.h>

#include <sys/ptrace.h>

#include <limits.h>
#include <unistd.h>
#include <signal.h>

#include <elf.h>

#include <type_traits>

#include "gdbsupport/eintr.h"
#include "gdbsupport/gdb_wait.h"
#include "gdbsupport/filestuff.h"
#include "gdbsupport/common-inferior.h"
#include "nat/fork-inferior.h"
#include "hostio.h"

int using_threads = 1;

#if 1
#define DTRACE(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
#define DTRACE(fmt, ...)
#endif

/* Callback used by fork_inferior to start tracing the inferior.  */

static void
mint_ptrace_fun (void)
{
  DTRACE("%s\n", __func__);
  /* Switch child to its own process group so that signals won't
     directly affect GDBserver. */
  if (setpgid (0, 0) < 0)
    trace_start_error_with_name (("setpgid"));

  if (ptrace (PT_TRACE_ME, 0, nullptr, 0) < 0)
    trace_start_error_with_name (("ptrace"));

  /* If GDBserver is connected to gdb via stdio, redirect the inferior's
     stdout to stderr so that inferior i/o doesn't corrupt the connection.
     Also, redirect stdin to /dev/null.  */
  if (remote_connection_is_stdio ())
    {
      if (close (0) < 0)
	trace_start_error_with_name (("close"));
      if (open ("/dev/null", O_RDONLY) < 0)
	trace_start_error_with_name (("open"));
      if (dup2 (2, 1) < 0)
	trace_start_error_with_name (("dup2"));
      if (write (2, "stdin/stdout redirected\n",
		 sizeof ("stdin/stdout redirected\n") - 1) < 0)
	{
	  /* Errors ignored.  */
	}
    }
}

/* Implement the create_inferior method of the target_ops vector.  */

int
mint_process_target::create_inferior (const char *program,
					const std::vector<char *> &program_args)
{
  DTRACE("%s\n", __func__);
  std::string str_program_args = construct_inferior_arguments (program_args);

  pid_t pid = fork_inferior (program, str_program_args.c_str (),
			     get_environ ()->envp (), mint_ptrace_fun,
			     nullptr, nullptr, nullptr, nullptr);

  add_process (pid, 0);

  post_fork_inferior (pid, program);

  return pid;
}

/* Implement the post_create_inferior target_ops method.  */

void
mint_process_target::post_create_inferior (void)
{
  DTRACE("%s\n", __func__);
  low_arch_setup ();
}

/* Implement the attach target_ops method.  */

/* Attach to PID.  If PID is the tgid, attach to it and all
   of its threads.  */
int
mint_process_target::attach (unsigned long pid)
{
  DTRACE("%s\n", __func__);
  /* Unimplemented.  */
  return -1;
}

/* Returns true if GDB is interested in any child syscalls.  */

static bool
gdb_catching_syscalls_p (pid_t pid)
{
  DTRACE("%s\n", __func__);
  struct process_info *proc = find_process_pid (pid);
  return !proc->syscalls_to_catch.empty ();
}

/* Implement the resume target_ops method.  */

void
mint_process_target::resume (struct thread_resume *resume_info, size_t n)
{
  DTRACE("%s\n", __func__);
  ptid_t resume_ptid = resume_info[0].thread;
  const int signal = resume_info[0].sig;
  const bool step = resume_info[0].kind == resume_step;

  if (resume_ptid == minus_one_ptid)
    resume_ptid = ptid_of (current_thread);

  const pid_t pid = resume_ptid.pid ();
  regcache_invalidate_pid (pid);

  if (ptrace (step ? PT_STEP : PT_CONTINUE, pid, (PTRACE_TYPE_ARG3) 1, signal) == -1)
    perror_with_name (("ptrace"));

  int request = gdb_catching_syscalls_p (pid) ? PT_CONTINUE : PT_SYSCALL;

  errno = 0;
  ptrace (request, pid, (PTRACE_TYPE_ARG3)1, signal);
  if (errno)
    perror_with_name (("ptrace"));
}

/* Returns true if GDB is interested in the reported SYSNO syscall.  */

#if USE_SIGTRAP_SIGINFO
static bool
mint_catch_this_syscall (int sysno)
{
  struct process_info *proc = current_process ();

  if (proc->syscalls_to_catch.empty ())
    return false;

  if (proc->syscalls_to_catch[0] == ANY_SYSCALL)
    return true;

  for (int iter : proc->syscalls_to_catch)
    if (iter == sysno)
      return true;

  return false;
}
#endif

/* Helper function for child_wait and the derivatives of child_wait.
   HOSTSTATUS is the waitstatus from wait() or the equivalent; store our
   translation of that in OURSTATUS.  */

static void
mint_store_waitstatus (struct target_waitstatus *ourstatus, int hoststatus)
{
  DTRACE("%s\n", __func__);
  if (WIFEXITED (hoststatus))
    ourstatus->set_exited (WEXITSTATUS (hoststatus));
  else if (!WIFSTOPPED (hoststatus))
    ourstatus->set_signalled (gdb_signal_from_host (WTERMSIG (hoststatus)));
  else
    ourstatus->set_stopped (gdb_signal_from_host (WSTOPSIG (hoststatus)));
}

/* Implement a safe wrapper around waitpid().  */

static pid_t
mint_waitpid (ptid_t ptid, struct target_waitstatus *ourstatus,
		target_wait_flags target_options)
{
  DTRACE("%s\n", __func__);
  int status;
  int options = (target_options & TARGET_WNOHANG) ? WNOHANG : 0;

  pid_t pid
    = gdb::handle_eintr (-1, ::waitpid, ptid.pid (), &status, options);

  if (pid == -1)
    perror_with_name (_("Child process unexpectedly missing"));

  mint_store_waitstatus (ourstatus, status);
  return pid;
}


/* Implement the wait target_ops method.

   Wait for the child specified by PTID to do something.  Return the
   process ID of the child, or MINUS_ONE_PTID in case of error; store
   the status in *OURSTATUS.  */

static ptid_t
mint_wait (ptid_t ptid, struct target_waitstatus *ourstatus,
	     target_wait_flags target_options)
{
  DTRACE("%s\n", __func__);
  pid_t pid = mint_waitpid (ptid, ourstatus, target_options);
  ptid_t wptid = ptid_t (pid);

  if (pid == 0)
    {
      gdb_assert (target_options & TARGET_WNOHANG);
      ourstatus->set_ignore ();
      return null_ptid;
    }

  gdb_assert (pid != -1);

  /* If the child stopped, keep investigating its status.  */
  if (ourstatus->kind () != TARGET_WAITKIND_STOPPED)
    return wptid;

  /* Extract the event and thread that received a signal.  */
#if USE_SIGTRAP_SIGINFO
  ptrace_siginfo_t psi;
  if (ptrace (PT_GET_SIGINFO, pid, &psi, sizeof (psi)) == -1)
    perror_with_name (("ptrace"));

  /* Pick child's siginfo_t.  */
  siginfo_t *si = &psi.psi_siginfo;

  int signo = si->si_signo;
  const int code = si->si_code;
#endif

  /* Construct PTID with a specified thread that received the event.
     If a signal was targeted to the whole process, lwp is 0.  */
  wptid = ptid_t (pid, 0, 0);

  /* Bail out on non-debugger oriented signals.  */
#if USE_SIGTRAP_SIGINFO
  if (signo != SIGTRAP)
    return wptid;
#else
  if (find_thread_ptid (wptid) == nullptr)
  {
    add_thread (wptid, NULL);
    ourstatus->set_thread_created ();
    return wptid;
  }
  if (ourstatus->sig () != GDB_SIGNAL_TRAP)
    return wptid;
#endif

#if USE_SIGTRAP_SIGINFO
  /* Stop examining non-debugger oriented SIGTRAP codes.  */
  if (code <= SI_USER || code == SI_NOINFO)
    return wptid;

  if (code == TRAP_EXEC)
    {
      ourstatus->set_execd
	(make_unique_xstrdup (mint_nat::pid_to_exec_file (pid)));
      return wptid;
    }

  if (code == TRAP_TRACE)
      return wptid;

  if (code == TRAP_SCE || code == TRAP_SCX)
    {
      int sysnum = si->si_sysnum;

      if (!mint_catch_this_syscall(sysnum))
	{
	  /* If the core isn't interested in this event, ignore it.  */
	  ourstatus->set_spurious ();
	  return wptid;
	}

      if (code == TRAP_SCE)
	ourstatus->set_syscall_entry (sysnum);
      else
	ourstatus->set_syscall_return (sysnum);

      return wptid;
    }

  if (code == TRAP_BRKPT)
    {
#ifdef PTRACE_BREAKPOINT_ADJ
      CORE_ADDR pc;
      struct reg r;
      ptrace (PT_GETREGS, pid, &r, psi.psi_lwpid);
      pc = PTRACE_REG_PC (&r);
      PTRACE_REG_SET_PC (&r, pc - PTRACE_BREAKPOINT_ADJ);
      ptrace (PT_SETREGS, pid, &r, psi.psi_lwpid);
#endif
      return wptid;
    }

  /* Unclassified SIGTRAP event.  */
  ourstatus->set_spurious ();
#else
#endif

  return wptid;
}

/* Implement the wait target_ops method.  */

ptid_t
mint_process_target::wait (ptid_t ptid, struct target_waitstatus *ourstatus,
			     target_wait_flags target_options)
{
  DTRACE("%s\n", __func__);
  while (true)
    {
      ptid_t wptid = mint_wait (ptid, ourstatus, target_options);

      /* Register thread in the gdbcore if a thread was not reported earlier.
	 This is required after ::create_inferior, when the gdbcore does not
	 know about the first internal thread.
	 This may also happen on attach, when an event is registered on a thread
	 that was not fully initialized during the attach stage.  */
      if (wptid.lwp () != 0 && !find_thread_ptid (wptid)
	  && ourstatus->kind () != TARGET_WAITKIND_THREAD_EXITED)
	add_thread (wptid, nullptr);

      switch (ourstatus->kind ())
	{
	case TARGET_WAITKIND_EXITED:
	case TARGET_WAITKIND_STOPPED:
	case TARGET_WAITKIND_SIGNALLED:
	case TARGET_WAITKIND_FORKED:
	case TARGET_WAITKIND_VFORKED:
	case TARGET_WAITKIND_EXECD:
	case TARGET_WAITKIND_VFORK_DONE:
	case TARGET_WAITKIND_SYSCALL_ENTRY:
	case TARGET_WAITKIND_SYSCALL_RETURN:
	  /* Pass the result to the generic code.  */
	  return wptid;
	case TARGET_WAITKIND_THREAD_CREATED:
	case TARGET_WAITKIND_THREAD_EXITED:
	  /* The core needlessly stops on these events.  */
	  /* FALLTHROUGH */
	case TARGET_WAITKIND_SPURIOUS:
	  /* Spurious events are unhandled by the gdbserver core.  */
	  if (ptrace (PT_CONTINUE, pid_of(current_process()), (PTRACE_TYPE_ARG3) 1, 0)
	      == -1)
	    perror_with_name (("ptrace"));
	  break;
	default:
	  error (("Unknown stopped status"));
	}
    }
}

/* Implement the kill target_ops method.  */

int
mint_process_target::kill (process_info *process)
{
  DTRACE("%s\n", __func__);
  pid_t pid = pid_of(process);
  if (ptrace (PT_KILL, pid, nullptr, 0) == -1)
    return -1;

  int status;
  if (gdb::handle_eintr (-1, ::waitpid, pid, &status, 0) == -1)
    return -1;
  mourn (process);
  return 0;
}

/* Implement the detach target_ops method.  */

int
mint_process_target::detach (process_info *process)
{
  DTRACE("%s\n", __func__);
  pid_t pid = process->pid;

  ptrace (PT_DETACH, pid, (PTRACE_TYPE_ARG3) 1, 0);
  mourn (process);
  return 0;
}

/* Implement the mourn target_ops method.  */

void
mint_process_target::mourn (struct process_info *proc)
{
  DTRACE("%s\n", __func__);
  for_each_thread (pid_of(proc), remove_thread);

  remove_process (proc);
}

/* Implement the join target_ops method.  */

void
mint_process_target::join (int pid)
{
  DTRACE("%s\n", __func__);
  /* The PT_DETACH is sufficient to detach from the process.
     So no need to do anything extra.  */
}

/* Implement the thread_alive target_ops method.  */

/* Return true iff the thread with thread ID TID is alive.  */
bool
mint_process_target::thread_alive (ptid_t ptid)
{
  DTRACE("%s\n", __func__);
  /* Our thread list is reliable; don't bother to poll target
     threads.  */
  return find_thread_ptid (ptid) != NULL;
}

/* Implement the fetch_registers target_ops method.  */

void
mint_process_target::fetch_registers (struct regcache *regcache, int regno)
{
  DTRACE("%s\n", __func__);
  if (current_thread == nullptr)
    return;
  const mint_regset_info *regset = get_regs_info ();
  ptid_t inferior_ptid = ptid_of (current_thread);

  while (regset->size >= 0)
    {
      std::vector<char> buf;
      buf.resize (regset->size);
      int res = ptrace (regset->get_request, inferior_ptid.pid (), buf.data (),
			inferior_ptid.lwp ());
      if (res == -1)
	perror_with_name (("ptrace"));
      regset->store_function (regcache, buf.data ());
      regset++;
    }
}

/* Implement the store_registers target_ops method.  */

void
mint_process_target::store_registers (struct regcache *regcache, int regno)
{
  DTRACE("%s\n", __func__);
  if (current_thread == nullptr)
    return;
  const mint_regset_info *regset = get_regs_info ();
  ptid_t inferior_ptid = ptid_of (current_thread);

  while (regset->size >= 0)
    {
      std::vector<char> buf;
      buf.resize (regset->size);
      int res = ptrace (regset->get_request, inferior_ptid.pid (), buf.data (),
			inferior_ptid.lwp ());
      if (res == -1)
	perror_with_name (("ptrace"));

      /* Then overlay our cached registers on that.  */
      regset->fill_function (regcache, buf.data ());
      /* Only now do we write the register set.  */
      res = ptrace (regset->set_request, inferior_ptid.pid (), buf. data (),
		    inferior_ptid.lwp ());
      if (res == -1)
	perror_with_name (("ptrace"));
      regset++;
    }
}

/* Implement the read_memory target_ops method.  */

int
mint_process_target::read_memory (CORE_ADDR memaddr, unsigned char *myaddr,
				    int size)
{
  DTRACE("%s\n", __func__);
  pid_t pid = pid_of(current_process());
  return mint_nat::read_memory (pid, myaddr, memaddr, size, nullptr);
}

/* Implement the write_memory target_ops method.  */

int
mint_process_target::write_memory (CORE_ADDR memaddr,
				     const unsigned char *myaddr, int size)
{
  DTRACE("%s\n", __func__);
  pid_t pid = pid_of(current_process());
  return mint_nat::write_memory (pid, myaddr, memaddr, size, nullptr);
}

/* Implement the request_interrupt target_ops method.  */

void
mint_process_target::request_interrupt (void)
{
  DTRACE("%s\n", __func__);
  ptid_t inferior_ptid = ptid_of (get_first_thread ());

  ::kill (inferior_ptid.pid (), SIGINT);
}

bool
mint_process_target::supports_z_point_type (char z_type)
{
  switch (z_type)
    {
    case Z_PACKET_SW_BP:
      return true;
    case Z_PACKET_HW_BP:
    case Z_PACKET_WRITE_WP:
    case Z_PACKET_READ_WP:
    case Z_PACKET_ACCESS_WP:
    default:
      return false; /* Not supported.  */
    }
}

/* Insert {break/watch}point at address ADDR.  SIZE is not used.  */

int
mint_process_target::insert_point (enum raw_bkpt_type type, CORE_ADDR addr,
		     int size, struct raw_breakpoint *bp)
{
  DTRACE("%s: type=%d\n", __func__, type);
  switch (type)
    {
    case raw_bkpt_type_sw:
      return insert_memory_breakpoint (bp);
    case raw_bkpt_type_hw:
    case raw_bkpt_type_write_wp:
    case raw_bkpt_type_read_wp:
    case raw_bkpt_type_access_wp:
    default:
      return 1; /* Not supported.  */
    }
}

/* Remove {break/watch}point at address ADDR.  SIZE is not used.  */

int
mint_process_target::remove_point (enum raw_bkpt_type type, CORE_ADDR addr,
				     int size, struct raw_breakpoint *bp)
{
  DTRACE("%s: type=%d\n", __func__, type);
  switch (type)
    {
    case raw_bkpt_type_sw:
      return remove_memory_breakpoint (bp);
    case raw_bkpt_type_hw:
    case raw_bkpt_type_write_wp:
    case raw_bkpt_type_read_wp:
    case raw_bkpt_type_access_wp:
    default:
      return 1; /* Not supported.  */
    }
}

/* Implement the stopped_by_sw_breakpoint target_ops method.  */

bool
mint_process_target::stopped_by_sw_breakpoint (void)
{
#if USE_SIGTRAP_SIGINFO
  ptrace_siginfo_t psi;
  pid_t pid = pid_of(current_process());

  if (ptrace (PT_GET_SIGINFO, pid, &psi, sizeof (psi)) == -1)
    perror_with_name (("ptrace"));

  return psi.psi_siginfo.si_signo == SIGTRAP &&
	 psi.psi_siginfo.si_code == TRAP_BRKPT;
#else
 /* XXX */
  return true;
#endif
}

/* Implement the supports_stopped_by_sw_breakpoint target_ops method.  */

bool
mint_process_target::supports_stopped_by_sw_breakpoint (void)
{
  return true;
}

/* This is only used for targets that define PT_TEXT_ADDR,
   PT_DATA_ADDR and PT_TEXT_END_ADDR.  If those are not defined, supposedly
   the target has different ways of acquiring this information, like
   loadmaps.  */

bool
mint_process_target::supports_read_offsets (void)
{
  return true;
}

int
mint_process_target::read_offsets (CORE_ADDR *text_p, CORE_ADDR *data_p)
{
  long offset;
  int pid = pid_of(current_process());

  if (ptrace (PT_BASEPAGE, pid, 0, (long) &offset))
    return 0;
  offset += 0x100;
  *text_p = offset;
  *data_p = offset;
  return 1;
}

#if USE_SIGTRAP_SIGINFO
/* Implement the supports_qxfer_siginfo target_ops method.  */

bool
mint_process_target::supports_qxfer_siginfo ()
{
  return true;
}

/* Implement the qxfer_siginfo target_ops method.  */

int
mint_process_target::qxfer_siginfo (const char *annex, unsigned char *readbuf,
				      unsigned const char *writebuf,
				      CORE_ADDR offset, int len)
{
  if (current_thread == nullptr)
    return -1;

  pid_t pid = pid_of(current_process());

  return mint_nat::qxfer_siginfo(pid, annex, readbuf, writebuf, offset, len);
}
#endif

/* Implement the supports_non_stop target_ops method.  */

bool
mint_process_target::supports_non_stop (void)
{
  return false;
}

/* Implement the supports_multi_process target_ops method.  */

bool
mint_process_target::supports_multi_process (void)
{
  return true;
}

/* Check if fork events are supported.  */

bool
mint_process_target::supports_fork_events (void)
{
  return false;
}

/* Check if vfork events are supported.  */

bool
mint_process_target::supports_vfork_events (void)
{
  return false;
}

/* Check if exec events are supported.  */

bool
mint_process_target::supports_exec_events (void)
{
  return true;
}

/* Return the name of a file that can be opened to get the symbols for
   the child process identified by PID.  */

const char *
mint_process_target::pid_to_exec_file (pid_t pid)
{
  return mint_nat::pid_to_exec_file (pid);
}

/* Implementation of the target_ops method "supports_pid_to_exec_file".  */

bool
mint_process_target::supports_pid_to_exec_file (void)
{
  return true;
}

/* Implement the supports_catch_syscall target_ops method.  */

bool
mint_process_target::supports_catch_syscall (void)
{
  return true;
}

void
initialize_low (void)
{
  set_target_ops (the_mint_target);
}
