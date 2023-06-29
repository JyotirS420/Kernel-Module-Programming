
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/unistd.h>
#include <linux/version.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/pid.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");

#define HAVE_KPROBES 1
#include <linux/kprobes.h>
#include <linux/kallsyms.h>

static unsigned long sym = 0;
module_param(sym, ulong, 0644);

static unsigned long **sys_call_table;


static int uid;
module_param(uid, int, 0644);


static asmlinkage int (*original_call)(const char *, int, int);


static asmlinkage int our_sys_open(const char *filename, int flags, int mode)
{
    int i = 0;
    struct task_struct *t = NULL;
    t = pid_task(find_vpid(uid), PIDTYPE_PID);
    int numb = pid_vnr(task_pgrp(t));
    pr_info("Process ID: %d ",t->pid);
    pr_info("User ID: %d ",t->cred->uid);
    pr_info("Group ID: %d ",numb);
    pr_info("Command Path: %s ",t->comm);


    return original_call(filename, flags, mode);
}

static unsigned long **aquire_sys_call_table(void)
{
    unsigned long (*kallsyms_lookup_name)(const char *name);
    struct kprobe kp = {
        .symbol_name = "kallsyms_lookup_name",
    };

    if (register_kprobe(&kp) < 0)
        return NULL;
    kallsyms_lookup_name = (unsigned long (*)(const char *name))kp.addr;
    unregister_kprobe(&kp);

    return (unsigned long **)kallsyms_lookup_name("sys_call_table");
}


static inline void __write_cr0(unsigned long cr0)
{
    asm volatile("mov %0,%%cr0" : "+r"(cr0) : : "memory");
}

static void enable_write_protection(void)
{
    unsigned long cr0 = read_cr0();
    set_bit(16, &cr0);
    __write_cr0(cr0);
}

static void disable_write_protection(void)
{
    unsigned long cr0 = read_cr0();
    clear_bit(16, &cr0);
    __write_cr0(cr0);
}

static int __init syscall_start(void)
{
    if (!(sys_call_table = aquire_sys_call_table()))
        return -1;

    disable_write_protection();

    original_call = (void *)sys_call_table[451];

    sys_call_table[451] = (unsigned long *)our_sys_open;

    enable_write_protection();


    return 0;
}

static void __exit syscall_end(void)
{
    if (!sys_call_table)
        return;

    if (sys_call_table[451] != (unsigned long *)our_sys_open) {
        pr_alert("Somebody else also played with the ");
        pr_alert("open system call\n");
        pr_alert("The system may be left in ");
        pr_alert("an unstable state.\n");
    }

    disable_write_protection();
    sys_call_table[451] = (unsigned long *)original_call;
    enable_write_protection();

    msleep(2000);
}

module_init(syscall_start);
module_exit(syscall_end);


