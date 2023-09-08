# Kernel Module Programming

**Description -**  

#### `mymodule.c`
We create a loadable kernel module which takes in the `pid` as a command line argument and fetches the `task struct`. It then prints out all the details of the task struct using the fields in the `task struct`. We use `printk` to print out the details to the console, which can be seen using `dmesg | tail`.

Reference - [The Linux Kernel Module Programming Guide](https://sysprog21.github.io/lkmpg/)
