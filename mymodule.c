#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/moduleparam.h>
#include <linux/pid.h>
#include <linux/init.h>
#include <linux/cred.h>
#include <linux/unistd.h>
#include <asm/uaccess.h>
#include <linux/delay.h>
#include <linux/kprobes.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ROOT");
MODULE_DESCRIPTION("Hello World LKM");

int pidv = 0;
struct task_struct *t = NULL;
//module_param(t,task_struct,S_IRUSR);
//module_param(t, *task_struct, 0);
module_param(pidv, int, 0);


static void printer(void){
        printk(KERN_INFO "HERE WE ARE\n");
}

static int __init hello_init(void)
{
    printk(KERN_INFO "Hello World!\n");
    //printk(KERN_INFO "Current process is \"%s\" (pid %i)\n", current->comm, current->pid);
    //printer();
    //printk(KERN_INFO "ANSWER IS %d", pidv+10);
    t = pid_task(find_vpid(pidv), PIDTYPE_PID);
    int numb = pid_vnr(task_pgrp(t));
    printk(KERN_INFO "Process ID: %d User ID: %d Group ID: %d Command Path: %s",t->pid,t->cred->uid,numb,t->comm);
    return 0;
}


// static int __init ModuleInit(void)
// {
//         printk(KERN_INFO "Hello Kernel!\n");
//      /* A non 0 return means init_module failed; module can't be loaded. */
//         return 0;
// }

static void __exit ModuleExit(void){
        printk(KERN_INFO "Goodbye kernel!\n");
}

module_init(hello_init);
module_exit(ModuleExit);