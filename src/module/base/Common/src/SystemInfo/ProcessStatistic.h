#ifndef PROCESS_STATSTIC_H
#define PROCESS_STATSTIC_H
#include "Base.h"
#include "jsoncpp/json.h"
/*
 * 统计进程信息
*/

class SystemInfo;
class ABSTRACT_EXPORT ProcessStatistic
{
    friend class SystemInfo;
public:
    Json::Value toJson();
private:
    // Whether the data is ok.
    bool ok = false;
    // The time in ms when sample.
    long long sample_time = 0;
    // The percent of usage. 0.153 is 15.3%.
    float percent = 0.0;

    // data of /proc/[pid]/stat
private:
    // pid %d      The process ID.
    int pid = 0;
    // comm %s     The  filename  of  the  executable,  in parentheses. This is visible whether or not the executable is
    //             swapped out.
    std::string comm;
    // state %c    One character from the string "RSDZTW" where R is running, S is sleeping in an interruptible  wait,  D
    //             is  waiting in uninterruptible disk sleep, Z is zombie, T is traced or stopped (on a signal), and W is
    //             paging.
    unsigned char state = 0;
    // ppid %d     The PID of the parent.
    int ppid = 0;
    // pgrp %d     The process group ID of the process.
    int pgrp = 0;
    // session %d  The session ID of the process.
    int session = 0;
    // tty_nr %d   The controlling terminal of the process.  (The minor device number is contained in the combination  of
    //             bits 31 to 20 and 7 to 0; the major device number is in bits 15 t0 8.)
    int tty_nr = 0;
    // tpgid %d    The ID of the foreground process group of the controlling terminal of the process.
    int tpgid = 0;
    // flags %u (%lu before Linux 2.6.22)
    //             The  kernel  flags  word  of  the process.  For bit meanings, see the PF_* defines in <linux/sched.h>.
    //             Details depend on the kernel version.
    unsigned int flags = 0;
    // minflt %lu  The number of minor faults the process has made which have not required loading  a  memory  page  from
    //             disk.
    unsigned long minflt = 0;
    // cminflt %lu The number of minor faults that the process's waited-for children have made.
    unsigned long cminflt = 0;
    // majflt %lu  The number of major faults the process has made which have required loading a memory page from disk.
    unsigned long majflt = 0;
    // cmajflt %lu The number of major faults that the process's waited-for children have made.
    unsigned long cmajflt = 0;
    // utime %lu   Amount  of  time that this process has been scheduled in user mode, measured in clock ticks (divide by
    //             sysconf(_SC_CLK_TCK).  This includes guest time, guest_time (time spent running  a  virtual  CPU,  see
    //             below),  so  that  applications  that are not aware of the guest time field do not lose that time from
    //             their calculations.
    unsigned long utime = 0;
    // stime %lu   Amount of time that this process has been scheduled in kernel mode, measured in clock ticks (divide by
    //             sysconf(_SC_CLK_TCK).
    unsigned long stime = 0;
    // cutime %ld  Amount  of  time that this process's waited-for children have been scheduled in user mode, measured in
    //             clock ticks (divide  by  sysconf(_SC_CLK_TCK).   (See  also  times(2).)   This  includes  guest  time,
    //             cguest_time (time spent running a virtual CPU, see below).
    long cutime = 0;
    // cstime %ld  Amount of time that this process's waited-for children have been scheduled in kernel mode, measured in
    //             clock ticks (divide by sysconf(_SC_CLK_TCK).
    long cstime = 0;
    // priority %ld
    //          (Explanation for Linux 2.6) For processes running a real-time scheduling  policy  (policy  below;  see
    //          sched_setscheduler(2)),  this  is the negated scheduling priority, minus one; that is, a number in the
    //          range -2 to -100, corresponding to real-time priorities 1 to 99.  For processes running under  a  non-
    //          real-time scheduling policy, this is the raw nice value (setpriority(2)) as represented in the kernel.
    //          The kernel stores nice values as numbers in the range 0 (high) to 39 (low), corresponding to the user-
    //          visible nice range of -20 to 19.
    //
    //          Before Linux 2.6, this was a scaled value based on the scheduler weighting given to this process.
    long priority = 0;
    // nice %ld    The nice value (see setpriority(2)), a value in the range 19 (low priority) to -20 (high priority).
    long nice = 0;
    // num_threads %ld
    //          Number  of threads in this process (since Linux 2.6).  Before kernel 2.6, this field was hard coded to
    //          0 as a placeholder for an earlier removed field.
    long num_threads = 0;
    // itrealvalue %ld
    //          The time in jiffies before the next SIGALRM is sent to the process due to an  interval  timer.   Since
    //          kernel 2.6.17, this field is no longer maintained, and is hard coded as 0.
    long itrealvalue = 0;
    // starttime %llu (was %lu before Linux 2.6)
    //          The time in jiffies the process started after system boot.
    long long starttime = 0;
    // vsize %lu   Virtual memory size in bytes.
    unsigned long vsize = 0;
    // rss %ld     Resident Set Size: number of pages the process has in real memory.  This is just the pages which count
    //             towards text, data, or stack space.  This does not include pages which have not been demand-loaded in,
    //             or which are swapped out.
    long rss = 0;
    // rsslim %lu  Current  soft limit in bytes on the rss of the process; see the description of RLIMIT_RSS in getprior-
    //             ity(2).
    unsigned long rsslim = 0;
    // startcode %lu
    //             The address above which program text can run.
    unsigned long startcode = 0;
    // endcode %lu The address below which program text can run.
    unsigned long endcode = 0;
    // startstack %lu
    //             The address of the start (i.e., bottom) of the stack.
    unsigned long startstack = 0;
    // kstkesp %lu The current value of ESP (stack pointer), as found in the kernel stack page for the process.
    unsigned long kstkesp = 0;
    // kstkeip %lu The current EIP (instruction pointer).
    unsigned long kstkeip = 0;
    // signal %lu  The bitmap of pending signals, displayed as a decimal number.  Obsolete, because it does  not  provide
    //             information on real-time signals; use /proc/[pid]/status instead.
    unsigned long signal = 0;
    // blocked %lu The  bitmap  of blocked signals, displayed as a decimal number.  Obsolete, because it does not provide
    //             information on real-time signals; use /proc/[pid]/status instead.
    unsigned long blocked = 0;
    // sigignore %lu
    //             The bitmap of ignored signals, displayed as a decimal number.  Obsolete, because it does  not  provide
    //             information on real-time signals; use /proc/[pid]/status instead.
    unsigned long sigignore = 0;
    // sigcatch %lu
    //             The  bitmap  of  caught signals, displayed as a decimal number.  Obsolete, because it does not provide
    //             information on real-time signals; use /proc/[pid]/status instead.
    unsigned long sigcatch = 0;
    // wchan %lu   This is the "channel" in which the process is waiting.  It is the address of a system call, and can be
    //             looked  up in a namelist if you need a textual name.  (If you have an up-to-date /etc/psdatabase, then
    //             try ps -l to see the WCHAN field in action.)
    unsigned long wchan = 0;
    // nswap %lu   Number of pages swapped (not maintained).
    unsigned long nswap = 0;
    // cnswap %lu  Cumulative nswap for child processes (not maintained).
    unsigned long cnswap = 0;
    // exit_signal %d (since Linux 2.1.22)
    //             Signal to be sent to parent when we die.
    int exit_signal = 0;
    // processor %d (since Linux 2.2.8)
    //             CPU number last executed on.
    int processor;
    // rt_priority %u (since Linux 2.5.19; was %lu before Linux 2.6.22)
    //             Real-time scheduling priority, a number in the range 1 to 99 for processes scheduled under a real-time
    //             policy, or 0, for non-real-time processes (see sched_setscheduler(2)).
    unsigned int rt_priority = 0;
    // policy %u (since Linux 2.5.19; was %lu before Linux 2.6.22)
    //             Scheduling policy (see sched_setscheduler(2)).  Decode using the SCHED_* constants in linux/sched.h.
    unsigned int policy = 0;
    // delayacct_blkio_ticks %llu (since Linux 2.6.18)
    //             Aggregated block I/O delays, measured in clock ticks (centiseconds).
    unsigned long long delayacct_blkio_ticks = 0;
    // guest_time %lu (since Linux 2.6.24)
    //             Guest time of the process (time spent running a virtual CPU for a guest operating system), measured in
    //             clock ticks (divide by sysconf(_SC_CLK_TCK).
    unsigned long guest_time = 0;
    // cguest_time %ld (since Linux 2.6.24)
    //             Guest time of the process's children, measured in clock ticks (divide by sysconf(_SC_CLK_TCK).
    long cguest_time = 0;
};
#endif
