/* Xtensa GNU/Linux native support.

   Copyright (C) 2007-2015 Free Software Foundation, Inc.

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
#include "frame.h"
#include "inferior.h"
#include "gdbcore.h"
#include "regcache.h"
#include "target.h"
#include "linux-nat.h"
#include <sys/types.h>
#include <signal.h>
#include <sys/user.h>
#include <sys/ioctl.h>
#include "gdb_wait.h"
#include <fcntl.h>
#include <sys/procfs.h>
#include <sys/ptrace.h>
#include <asm/ptrace.h>

#include "gregset.h"
#include "xtensa-tdep.h"

/* Extended register set depends on hardware configs.
   Keeping these definitions separately allows to introduce
   hardware-specific overlays.  */
#include "xtensa-xtregs.c"

static int
get_thread_id (ptid_t ptid)
{
  int tid = ptid_get_lwp (ptid);
  if (0 == tid)
    tid = ptid_get_pid (ptid);
  return tid;
}
#define GET_THREAD_ID(PTID)	get_thread_id (PTID)

void
fill_gregset (const struct regcache *regcache,
	      gdb_gregset_t *gregsetp, int regnum)
{
  int i;
  xtensa_elf_gregset_t *regs = (xtensa_elf_gregset_t *) gregsetp;
  struct gdbarch *gdbarch = get_regcache_arch (regcache);

  if (regnum == gdbarch_pc_regnum (gdbarch) || regnum == -1)
    regcache_raw_collect (regcache, gdbarch_pc_regnum (gdbarch), &regs->pc);
  if (regnum == gdbarch_ps_regnum (gdbarch) || regnum == -1)
    regcache_raw_collect (regcache, gdbarch_ps_regnum (gdbarch), &regs->ps);

  if (regnum == gdbarch_tdep (gdbarch)->wb_regnum || regnum == -1)
    regcache_raw_collect (regcache,
			  gdbarch_tdep (gdbarch)->wb_regnum,
			  &regs->windowbase);
  if (regnum == gdbarch_tdep (gdbarch)->ws_regnum || regnum == -1)
    regcache_raw_collect (regcache,
			  gdbarch_tdep (gdbarch)->ws_regnum,
			  &regs->windowstart);
  if (regnum == gdbarch_tdep (gdbarch)->lbeg_regnum || regnum == -1)
    regcache_raw_collect (regcache,
			  gdbarch_tdep (gdbarch)->lbeg_regnum,
			  &regs->lbeg);
  if (regnum == gdbarch_tdep (gdbarch)->lend_regnum || regnum == -1)
    regcache_raw_collect (regcache,
			  gdbarch_tdep (gdbarch)->lend_regnum,
			  &regs->lend);
  if (regnum == gdbarch_tdep (gdbarch)->lcount_regnum || regnum == -1)
    regcache_raw_collect (regcache,
			  gdbarch_tdep (gdbarch)->lcount_regnum,
			  &regs->lcount);
  if (regnum == gdbarch_tdep (gdbarch)->sar_regnum || regnum == -1)
    regcache_raw_collect (regcache,
			  gdbarch_tdep (gdbarch)->sar_regnum,
			  &regs->sar);
  if (regnum >=gdbarch_tdep (gdbarch)->ar_base
      && regnum < gdbarch_tdep (gdbarch)->ar_base
		    + gdbarch_tdep (gdbarch)->num_aregs)
    regcache_raw_collect (regcache,regnum,
			  &regs->ar[regnum - gdbarch_tdep (gdbarch)->ar_base]);
  else if (regnum == -1)
    {
      for (i = 0; i < gdbarch_tdep (gdbarch)->num_aregs; ++i)
	regcache_raw_collect (regcache,
			      gdbarch_tdep (gdbarch)->ar_base + i,
			      &regs->ar[i]);
    }
}

static void
supply_gregset_reg (struct regcache *regcache,
		    const gdb_gregset_t *gregsetp, int regnum)
{
  int i;
  xtensa_elf_gregset_t *regs = (xtensa_elf_gregset_t *) gregsetp;

  struct gdbarch *gdbarch = get_regcache_arch (regcache);

  if (regnum == gdbarch_pc_regnum (gdbarch) || regnum == -1)
    regcache_raw_supply (regcache, gdbarch_pc_regnum (gdbarch), &regs->pc);
  if (regnum == gdbarch_ps_regnum (gdbarch) || regnum == -1)
    regcache_raw_supply (regcache, gdbarch_ps_regnum (gdbarch), &regs->ps);

  if (regnum == gdbarch_tdep (gdbarch)->wb_regnum || regnum == -1)
    regcache_raw_supply (regcache,
			  gdbarch_tdep (gdbarch)->wb_regnum,
			  &regs->windowbase);
  if (regnum == gdbarch_tdep (gdbarch)->ws_regnum || regnum == -1)
    regcache_raw_supply (regcache,
			  gdbarch_tdep (gdbarch)->ws_regnum,
			  &regs->windowstart);
  if (regnum == gdbarch_tdep (gdbarch)->lbeg_regnum || regnum == -1)
    regcache_raw_supply (regcache,
			  gdbarch_tdep (gdbarch)->lbeg_regnum,
			  &regs->lbeg);
  if (regnum == gdbarch_tdep (gdbarch)->lend_regnum || regnum == -1)
    regcache_raw_supply (regcache,
			  gdbarch_tdep (gdbarch)->lend_regnum,
			  &regs->lend);
  if (regnum == gdbarch_tdep (gdbarch)->lcount_regnum || regnum == -1)
    regcache_raw_supply (regcache,
			  gdbarch_tdep (gdbarch)->lcount_regnum,
			  &regs->lcount);
  if (regnum == gdbarch_tdep (gdbarch)->sar_regnum || regnum == -1)
    regcache_raw_supply (regcache,
			  gdbarch_tdep (gdbarch)->sar_regnum,
			  &regs->sar);
  if (regnum >=gdbarch_tdep (gdbarch)->ar_base
      && regnum < gdbarch_tdep (gdbarch)->ar_base
		    + gdbarch_tdep (gdbarch)->num_aregs)
    regcache_raw_supply (regcache,regnum,
			  &regs->ar[regnum - gdbarch_tdep (gdbarch)->ar_base]);
  else if (regnum == -1)
    {
      for (i = 0; i < gdbarch_tdep (gdbarch)->num_aregs; ++i)
	regcache_raw_supply (regcache,
			      gdbarch_tdep (gdbarch)->ar_base + i,
			      &regs->ar[i]);
    }
}

void
supply_gregset (struct regcache *regcache, const gdb_gregset_t *gregsetp)
{
  supply_gregset_reg (regcache, gregsetp, -1);
}

void
fill_fpregset (const struct regcache *regcache,
	       gdb_fpregset_t *fpregsetp, int regnum)
{
  return;
}

void 
supply_fpregset (struct regcache *regcache,
		 const gdb_fpregset_t *fpregsetp)
{
  return;
}

/* Fetch greg-register(s) from process/thread TID
   and store value(s) in GDB's register array.  */

static void
fetch_gregs (struct regcache *regcache, int regnum)
{
  int tid = GET_THREAD_ID (inferior_ptid);
  const gdb_gregset_t regs;
  int areg;
  
  if (ptrace (PTRACE_GETREGS, tid, 0, (long) &regs) < 0)
    {
      perror_with_name (_("Couldn't get registers"));
      return;
    }
 
  supply_gregset_reg (regcache, &regs, regnum);
}

/* Store greg-register(s) in GDB's register 
   array into the process/thread specified by TID.  */

static void
store_gregs (struct regcache *regcache, int regnum)
{
  int tid = GET_THREAD_ID (inferior_ptid);
  gdb_gregset_t regs;
  int areg;

  if (ptrace (PTRACE_GETREGS, tid, 0, (long) &regs) < 0)
    {
      perror_with_name (_("Couldn't get registers"));
      return;
    }

  fill_gregset (regcache, &regs, regnum);

  if (ptrace (PTRACE_SETREGS, tid, 0, (long) &regs) < 0)
    {
      perror_with_name (_("Couldn't write registers"));
      return;
    }
}

static int xtreg_lo;
static int xtreg_high;

/* Fetch/Store Xtensa TIE registers.  Xtensa GNU/Linux PTRACE
   interface provides special requests for this.  */

static void
fetch_xtregs (struct regcache *regcache, int regnum)
{
  int tid = GET_THREAD_ID (inferior_ptid);
  const xtensa_regtable_t *ptr;
  char xtregs [XTENSA_ELF_XTREG_SIZE];

  if (ptrace (PTRACE_GETXTREGS, tid, 0, (long)&xtregs) < 0)
    perror_with_name (_("Couldn't get extended registers"));

  for (ptr = xtensa_regmap_table; ptr->name; ptr++)
    if (regnum == ptr->gdb_regnum || regnum == -1)
      regcache_raw_supply (regcache, ptr->gdb_regnum,
			   xtregs + ptr->ptrace_offset);
}

static void
store_xtregs (struct regcache *regcache, int regnum)
{
  int tid = GET_THREAD_ID (inferior_ptid);
  const xtensa_regtable_t *ptr;
  char xtregs [XTENSA_ELF_XTREG_SIZE];

  if (ptrace (PTRACE_GETXTREGS, tid, 0, (long)&xtregs) < 0)
    perror_with_name (_("Couldn't get extended registers"));

  for (ptr = xtensa_regmap_table; ptr->name; ptr++)
    if (regnum == ptr->gdb_regnum || regnum == -1)
      regcache_raw_collect (regcache, ptr->gdb_regnum,
			    xtregs + ptr->ptrace_offset);

  if (ptrace (PTRACE_SETXTREGS, tid, 0, (long)&xtregs) < 0)
    perror_with_name (_("Couldn't write extended registers"));
}

static void
xtensa_linux_fetch_inferior_registers (struct target_ops *ops,
				       struct regcache *regcache, int regnum)
{
  if (regnum == -1)
    {
      fetch_gregs (regcache, regnum);
      fetch_xtregs (regcache, regnum);
    }
  else if ((regnum < xtreg_lo) || (regnum > xtreg_high))
    fetch_gregs (regcache, regnum);
  else
    fetch_xtregs (regcache, regnum);
}

static void
xtensa_linux_store_inferior_registers (struct target_ops *ops,
				       struct regcache *regcache, int regnum)
{
  if (regnum == -1)
    {
      store_gregs (regcache, regnum);
      store_xtregs (regcache, regnum);
    }
  else if ((regnum < xtreg_lo) || (regnum > xtreg_high))
    store_gregs (regcache, regnum);
  else
    store_xtregs (regcache, regnum);
}

#ifndef PTRACE_GETHBPREGS
#define PTRACE_GETHBPREGS   20
#endif
#ifndef PTRACE_SETHBPREGS
#define PTRACE_SETHBPREGS   21
#endif

#define XTENSA_MAX_WP_LENGTH 64
#define MAX_BPTS 2
#define MAX_WPTS 2

#define DBREAKC_LOAD_MASK 0x40000000
#define DBREAKC_STOR_MASK 0x80000000

/* Information describing the hardware breakpoint capabilities.  */
struct xtensa_linux_hwbp_cap
{
  gdb_byte wp_count;
  gdb_byte bp_count;
};

struct xtensa_linux_hw_point
{
  uint32_t addr;
  uint32_t type;
};

/* Per-process arch-specific data we want to keep.  */
struct xtensa_linux_process_info
{
  /* Linked list.  */
  struct xtensa_linux_process_info *next;
  /* The process identifier.  */
  pid_t pid;
  /* Hardware breakpoints state information.  */
  struct xtensa_linux_hw_point bpt[MAX_BPTS];
  /* Hardware watchpoints state information.  */
  struct xtensa_linux_hw_point wpt[MAX_WPTS];
};

/* Per-thread arch-specific data we want to keep.  */
struct arch_lwp_info
{
  /* Non-zero if our copy differs from what's recorded in the thread.  */
  char bpts_changed[MAX_BPTS];
  char wpts_changed[MAX_WPTS];
};

static struct xtensa_linux_process_info *xtensa_linux_process_list = NULL;

/* Find process data for process PID.  */

static struct xtensa_linux_process_info *
xtensa_linux_find_process_pid (pid_t pid)
{
  struct xtensa_linux_process_info *proc;

  for (proc = xtensa_linux_process_list; proc; proc = proc->next)
    if (proc->pid == pid)
      return proc;

  return NULL;
}

/* Add process data for process PID.  Returns newly allocated info
   object.  */

static struct xtensa_linux_process_info *
xtensa_linux_add_process (pid_t pid)
{
  struct xtensa_linux_process_info *proc;

  proc = xcalloc (1, sizeof (*proc));
  proc->pid = pid;

  proc->next = xtensa_linux_process_list;
  xtensa_linux_process_list = proc;

  return proc;
}

/* Get data specific info for process PID, creating it if necessary.
   Never returns NULL.  */

static struct xtensa_linux_process_info *
xtensa_linux_process_info_get (pid_t pid)
{
  struct xtensa_linux_process_info *proc;

  proc = xtensa_linux_find_process_pid (pid);
  if (proc == NULL)
    proc = xtensa_linux_add_process (pid);

  return proc;
}

/* Called whenever GDB is no longer debugging process PID.  It deletes
   data structures that keep track of debug register state.  */

static void
xtensa_linux_forget_process (pid_t pid)
{
  struct xtensa_linux_process_info *proc, **proc_link;

  proc = xtensa_linux_process_list;
  proc_link = &xtensa_linux_process_list;

  while (proc != NULL)
    {
      if (proc->pid == pid)
	{
	  *proc_link = proc->next;

	  xfree (proc);
	  return;
	}

      proc_link = &proc->next;
      proc = *proc_link;
    }
}

/* Get hold of the Hardware Breakpoint information for the target we are
   attached to.  Returns NULL if the kernel doesn't support Hardware
   breakpoints at all, or a pointer to the information structure.  */
static const struct xtensa_linux_hwbp_cap *
xtensa_linux_get_hwbp_cap (void)
{
  /* The info structure we return.  */
  static struct xtensa_linux_hwbp_cap info;

  /* Is INFO in a good state?  0 means that no attempt has been made to
     initialize INFO; 1 means INFO is in an initialized state.  */
  static int available;

  if (!available)
    {
      int i;
      int tid;
      uint32_t v[2];

      tid = GET_THREAD_ID (inferior_ptid);
      for (i = 0; i < MAX_WPTS; ++i)
	{
	  if (ptrace (PTRACE_GETHBPREGS, tid, i << 1 | 1, v) < 0)
	    break;
	}
      info.wp_count = i;

      for (i = 0; i < MAX_BPTS; ++i)
	{
	  if (ptrace (PTRACE_GETHBPREGS, tid, i << 1, v) < 0)
	    break;
	}
      info.bp_count = i;

      available = 1;
    }

  return &info;
}

static int
xtensa_linux_get_hw_breakpoint_count (void)
{
  const struct xtensa_linux_hwbp_cap *cap = xtensa_linux_get_hwbp_cap ();
  return cap != NULL ? cap->bp_count : 0;
}

static int
xtensa_linux_get_hw_watchpoint_count (void)
{
  const struct xtensa_linux_hwbp_cap *cap = xtensa_linux_get_hwbp_cap ();
  return cap != NULL ? cap->wp_count : 0;
}

/* Have we got a free break-/watch-point available for use?  Returns -1 if
   there is not an appropriate resource available, otherwise returns 1.  */
static int
xtensa_linux_can_use_hw_breakpoint (struct target_ops *self, int type,
				    int cnt, int ot)
{
  if (type == bp_hardware_watchpoint || type == bp_read_watchpoint
      || type == bp_access_watchpoint || type == bp_watchpoint)
    {
      int count = xtensa_linux_get_hw_watchpoint_count ();

      if (count == 0)
	return 0;
      else if (cnt > count)
	return -1;
    }
  else if (type == bp_hardware_breakpoint)
    {
      int count = xtensa_linux_get_hw_breakpoint_count ();

      if (count == 0)
	return 0;
      else if (cnt > count)
	return -1;
    }
  else
    gdb_assert (FALSE);

  return 1;
}

/* Initialise the hardware breakpoint structure P.  The breakpoint will be
   enabled, and will point to the placed address of BP_TGT.  */
static void
xtensa_linux_hw_point_initialize (struct gdbarch *gdbarch,
				  struct bp_target_info *bp_tgt,
				  struct xtensa_linux_hw_point *p)
{
  CORE_ADDR address = bp_tgt->placed_address = bp_tgt->reqstd_address;

  p->addr = (unsigned int) address;
  p->type = 1;
}

/* Initialize the hardware breakpoint structure P for a watchpoint at ADDR
   to LEN.  The type of watchpoint is given in RW.  */
static void
xtensa_linux_hw_watchpoint_initialize (CORE_ADDR addr, int len, int rw,
				       struct xtensa_linux_hw_point *p)
{
  p->addr = (unsigned int) addr;
  p->type = len;
  if (rw == hw_read)
    p->type |= DBREAKC_LOAD_MASK;
  else if (rw == hw_write)
    p->type |= DBREAKC_STOR_MASK;
  else
    p->type |= DBREAKC_LOAD_MASK | DBREAKC_STOR_MASK;
}

/* Callback to mark a watch-/breakpoint to be updated in all threads of
   the current process.  */

struct update_registers_data
{
  int watch;
  int index;
};

static int
update_registers_callback (struct lwp_info *lwp, void *arg)
{
  struct update_registers_data *data = (struct update_registers_data *) arg;

  if (lwp->arch_private == NULL)
    lwp->arch_private = XCNEW (struct arch_lwp_info);

  /* The actual update is done later just before resuming the lwp,
     we just mark that the registers need updating.  */
  if (data->watch)
    lwp->arch_private->wpts_changed[data->index] = 1;
  else
    lwp->arch_private->bpts_changed[data->index] = 1;

  /* If the lwp isn't stopped, force it to momentarily pause, so
     we can update its breakpoint registers.  */
  if (!lwp->stopped)
    linux_stop_lwp (lwp);

  return 0;
}

/* Insert the hardware breakpoint (WATCHPOINT = 0) or watchpoint (WATCHPOINT
   =1) BPT for the process.  */
static int
xtensa_linux_insert_process_hw_point (const struct xtensa_linux_hw_point *bpt,
				      int watchpoint)
{
  int pid;
  ptid_t pid_ptid;
  struct xtensa_linux_hw_point *pt;
  int max_slot_count;
  int slot;

  pid = ptid_get_pid (inferior_ptid);
  pid_ptid = pid_to_ptid (pid);

  if (watchpoint)
    {
      max_slot_count = xtensa_linux_get_hw_watchpoint_count();
      pt = xtensa_linux_process_info_get (pid)->wpt;
    }
  else
    {
      max_slot_count = xtensa_linux_get_hw_breakpoint_count();
      pt = xtensa_linux_process_info_get (pid)->bpt;
    }

  for (slot = 0; slot < max_slot_count; ++slot)
    if (pt[slot].type == 0)
      {
	struct update_registers_data data =
	  {
	    .watch = watchpoint,
	    .index = slot,
	  };
	pt[slot] = *bpt;
	iterate_over_lwps (pid_ptid, update_registers_callback, &data);
	return 0;
      }
  return -1;
}

/* Remove the hardware breakpoint (WATCHPOINT = 0) or watchpoint
   (WATCHPOINT = 1) BPT for the process.  */
static int
xtensa_linux_remove_process_hw_point (const struct xtensa_linux_hw_point *bpt,
				      int watchpoint)
{
  int pid;
  ptid_t pid_ptid;
  struct xtensa_linux_hw_point *pt;
  int max_slot_count;
  int slot;

  pid = ptid_get_pid (inferior_ptid);
  pid_ptid = pid_to_ptid (pid);

  if (watchpoint)
    {
      max_slot_count = xtensa_linux_get_hw_watchpoint_count();
      pt = xtensa_linux_process_info_get (pid)->wpt;
    }
  else
    {
      max_slot_count = xtensa_linux_get_hw_breakpoint_count();
      pt = xtensa_linux_process_info_get (pid)->bpt;
    }

  for (slot = 0; slot < max_slot_count; ++slot)
    if (pt[slot].addr == bpt->addr &&
	pt[slot].type == bpt->type)
      {
	struct update_registers_data data =
	  {
	    .watch = watchpoint,
	    .index = slot,
	  };
	pt[slot].type = 0;
	iterate_over_lwps (pid_ptid, update_registers_callback, &data);
	return 0;
      }
  return -1;
}

/* Insert a Hardware breakpoint.  */
static int
xtensa_linux_insert_hw_breakpoint (struct target_ops *self,
				   struct gdbarch *gdbarch,
				   struct bp_target_info *bp_tgt)
{
  struct xtensa_linux_hw_point p;

  if (xtensa_linux_get_hw_breakpoint_count () == 0)
    return -1;

  xtensa_linux_hw_point_initialize (gdbarch, bp_tgt, &p);
  return xtensa_linux_insert_process_hw_point (&p, 0);
}

/* Remove a hardware breakpoint.  */
static int
xtensa_linux_remove_hw_breakpoint (struct target_ops *self,
				   struct gdbarch *gdbarch,
				   struct bp_target_info *bp_tgt)
{
  struct xtensa_linux_hw_point p;

  if (xtensa_linux_get_hw_breakpoint_count () == 0)
    return -1;

  xtensa_linux_hw_point_initialize (gdbarch, bp_tgt, &p);
  return xtensa_linux_remove_process_hw_point (&p, 0);
}

/* Are we able to use a hardware watchpoint for the LEN bytes starting at
   ADDR?  */
static int
xtensa_linux_region_ok_for_hw_watchpoint (struct target_ops *self,
					  CORE_ADDR addr, int len)
{
  CORE_ADDR aligned_addr;

  /* Can not set watchpoints for zero or negative lengths.  */
  if (len <= 0)
    return 0;

  /* Test that the range [ADDR, ADDR + LEN) fits into the largest address
     range covered by a watchpoint.  */
  aligned_addr = addr & ~(XTENSA_MAX_WP_LENGTH - 1);

  if (aligned_addr + XTENSA_MAX_WP_LENGTH < addr + len)
    return 0;

  /* The current ptrace interface can only handle watchpoint lengths that
     are a power of 2.  */
  if ((len & (len - 1)) != 0)
    return 0;

  /* All tests passed so we must be able to set a watchpoint.  */
  return 1;
}

/* Insert a Hardware watchpoint.  */
static int
xtensa_linux_insert_watchpoint (struct target_ops *self,
				CORE_ADDR addr, int len, int rw,
				struct expression *cond)
{
  struct xtensa_linux_hw_point p;
  int slot;

  if (xtensa_linux_get_hw_watchpoint_count () == 0)
    return -1;

  xtensa_linux_hw_watchpoint_initialize (addr, len, rw, &p);
  return xtensa_linux_insert_process_hw_point (&p, 1);
}

/* Remove a hardware watchpoint.  */
static int
xtensa_linux_remove_watchpoint (struct target_ops *self,
				CORE_ADDR addr, int len, int rw,
				struct expression *cond)
{
  struct xtensa_linux_hw_point p;
  int slot;

  if (xtensa_linux_get_hw_watchpoint_count () == 0)
    return -1;

  xtensa_linux_hw_watchpoint_initialize (addr, len, rw, &p);
  return xtensa_linux_remove_process_hw_point (&p, 1);
}

/* What was the data address the target was stopped on accessing.  */
static int
xtensa_linux_stopped_data_address (struct target_ops *target,
				   CORE_ADDR *addr_p)
{
  siginfo_t siginfo;
  int slot;

  if (!linux_nat_get_siginfo (inferior_ptid, &siginfo))
    return 0;

  /* This must be a hardware breakpoint.  */
  if (siginfo.si_signo != SIGTRAP
      || (siginfo.si_code & 0xffff) != 0x0004 /* TRAP_HWBKPT */)
    return 0;

  /* We must be able to set hardware watchpoints.  */
  if (xtensa_linux_get_hw_watchpoint_count () == 0)
    return 0;

  slot = siginfo.si_errno;

  /* If we are in an even slot then we're looking at a breakpoint and not
     a watchpoint.  */
  if ((slot & 0x1) == 0)
    return 0;

  *addr_p = (CORE_ADDR) (uintptr_t) siginfo.si_addr;
  return 1;
}

/* Has the target been stopped by hitting a watchpoint?  */
static int
xtensa_linux_stopped_by_watchpoint (struct target_ops *ops)
{
  CORE_ADDR addr;
  return xtensa_linux_stopped_data_address (ops, &addr);
}

static int
xtensa_linux_watchpoint_addr_within_range (struct target_ops *target,
					   CORE_ADDR addr,
					   CORE_ADDR start, int length)
{
  return start <= addr && start + length - 1 >= addr;
}

/* Handle thread creation.  We need to copy the breakpoints and watchpoints
   in the parent thread to the child thread.  */
static void
xtensa_linux_new_thread (struct lwp_info *lp)
{
  int i;
  struct arch_lwp_info *info = XCNEW (struct arch_lwp_info);

  /* Mark that all the hardware breakpoints/watchpoints
     for this thread need to be initialized.  */

  for (i = 0; i < MAX_BPTS; i++)
    info->bpts_changed[i] = 1;
  for (i = 0; i < MAX_WPTS; i++)
    info->wpts_changed[i] = 1;

  lp->arch_private = info;
}

static long
xtensa_linux_set_hw_point (const struct xtensa_linux_hw_point *bpt,
			   int pid, int slot, int watchpoint)
{
  uint32_t bp[2] = { bpt->addr, bpt->type };

  return ptrace (PTRACE_SETHBPREGS, pid, slot << 1 | watchpoint, bp);
}

/* Called when resuming a thread.
   The hardware debug registers are updated when there is any change.  */
static void
xtensa_linux_prepare_to_resume (struct lwp_info *lwp)
{
  int pid, i;
  struct xtensa_linux_hw_point *bpts, *wpts;
  struct arch_lwp_info *lwp_info = lwp->arch_private;

  pid = ptid_get_lwp (lwp->ptid);
  bpts = xtensa_linux_process_info_get (ptid_get_pid (lwp->ptid))->bpt;
  wpts = xtensa_linux_process_info_get (ptid_get_pid (lwp->ptid))->wpt;

  if (lwp_info == NULL)
    return;

  for (i = 0; i < xtensa_linux_get_hw_breakpoint_count (); i++)
    if (lwp_info->bpts_changed[i])
      {
	errno = 0;

	if (xtensa_linux_set_hw_point (bpts + i, pid, i, 0) < 0)
	  perror_with_name (_("Unexpected error setting breakpoint"));

	lwp_info->bpts_changed[i] = 0;
      }

  for (i = 0; i < xtensa_linux_get_hw_watchpoint_count (); i++)
    if (lwp_info->wpts_changed[i])
      {
	errno = 0;

	if (xtensa_linux_set_hw_point (wpts + i, pid, i, 1) < 0)
	  perror_with_name (_("Unexpected error setting watchpoint"));

	lwp_info->wpts_changed[i] = 0;
      }
}

static void
xtensa_linux_new_fork (struct lwp_info *parent, pid_t child_pid)
{
  pid_t parent_pid;
  struct xtensa_linux_process_info *parent_state;
  struct xtensa_linux_process_info *child_state;

  /* NULL means no watchpoint has ever been set in the parent.  In
     that case, there's nothing to do.  */
  if (parent->arch_private == NULL)
    return;

  /* GDB core assumes the child inherits the watchpoints/hw
     breakpoints of the parent, and will remove them all from the
     forked off process.  Copy the debug registers mirrors into the
     new process so that all breakpoints and watchpoints can be
     removed together.  */

  parent_pid = ptid_get_pid (parent->ptid);
  parent_state = xtensa_linux_process_info_get (parent_pid);
  child_state = xtensa_linux_process_info_get (child_pid);
  memcpy (child_state->bpt, parent_state->bpt, sizeof (parent_state->bpt));
  memcpy (child_state->wpt, parent_state->wpt, sizeof (parent_state->wpt));
}

void _initialize_xtensa_linux_nat (void);

void
_initialize_xtensa_linux_nat (void)
{
  struct target_ops *t;
  const xtensa_regtable_t *ptr;

  /* Calculate the number range for extended registers.  */
  xtreg_lo = 1000000000;
  xtreg_high = -1;
  for (ptr = xtensa_regmap_table; ptr->name; ptr++)
    {
      if (ptr->gdb_regnum < xtreg_lo)
	xtreg_lo = ptr->gdb_regnum;
      if (ptr->gdb_regnum > xtreg_high)
	xtreg_high = ptr->gdb_regnum;
    }

  /* Fill in the generic GNU/Linux methods.  */
  t = linux_target ();

  /* Add our register access methods.  */
  t->to_fetch_registers = xtensa_linux_fetch_inferior_registers;
  t->to_store_registers = xtensa_linux_store_inferior_registers;

  /* Add our hardware breakpoint and watchpoint implementation.  */
  t->to_can_use_hw_breakpoint = xtensa_linux_can_use_hw_breakpoint;
  t->to_insert_hw_breakpoint = xtensa_linux_insert_hw_breakpoint;
  t->to_remove_hw_breakpoint = xtensa_linux_remove_hw_breakpoint;

  t->to_region_ok_for_hw_watchpoint = xtensa_linux_region_ok_for_hw_watchpoint;
  t->to_insert_watchpoint = xtensa_linux_insert_watchpoint;
  t->to_remove_watchpoint = xtensa_linux_remove_watchpoint;
  t->to_stopped_by_watchpoint = xtensa_linux_stopped_by_watchpoint;
  t->to_stopped_data_address = xtensa_linux_stopped_data_address;
  t->to_watchpoint_addr_within_range =
    xtensa_linux_watchpoint_addr_within_range;

  linux_nat_add_target (t);

  /* Handle thread creation and exit.  */
  linux_nat_set_new_thread (t, xtensa_linux_new_thread);
  linux_nat_set_prepare_to_resume (t, xtensa_linux_prepare_to_resume);

  /* Handle process creation and exit.  */
  linux_nat_set_new_fork (t, xtensa_linux_new_fork);
  linux_nat_set_forget_process (t, xtensa_linux_forget_process);
}
