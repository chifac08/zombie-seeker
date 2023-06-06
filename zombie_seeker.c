#include <linux/module.h>
#include <linux/sched/signal.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>

//defines
#define MODULE_NAME "zombie-seeker"
#define FORMAT "#COMM:PPID:CPID:EXIT_STATE"
/** MSG Max Length 
 * Process name -> 16 bytes
 * PPID -> 5 bytes
 * CPID -> 5 bytes
 * EXIT_STATE -> 2 bytes (can be only 32) 
 */
#define MSG_MAX_LENGTH 28
#define BUFFER_SIZE 35

MODULE_LICENSE("GPL-2.0");
MODULE_AUTHOR("Fabian Lichtenegger-Lukas <github.com/chifac08>");
MODULE_DESCRIPTION("Kernel module to seek zombie processes");
MODULE_VERSION("0.01");

static struct proc_dir_entry *ent;

static ssize_t seek_zombie(struct file *file, char __user *ubuf, size_t count, loff_t *ppos)
{    
    struct task_struct *task;
    char buf[BUFFER_SIZE];
    int len=strlen(FORMAT)+1;

    memset(&buf[0], 0, sizeof(buf));

    if(*ppos > 0 || count < BUFFER_SIZE)
        return 0;

    printk(KERN_DEBUG "%s: seeking for zombies...\n", MODULE_NAME);
    
    sprintf(buf, "%s\n", FORMAT);

    for_each_process(task)
    {
        //Exclude scheduler with PID 0
        if(task->exit_state == EXIT_ZOMBIE && task->pid > 0)
        {
            printk(KERN_DEBUG "%s: identified a zombie process: [%d]\n", MODULE_NAME, task->pid);
            //check if buffer has enough memory for a new message
            if(len < (BUFFER_SIZE-MSG_MAX_LENGTH))
                len += sprintf(buf+len, "%s:%d:%d:%d\n", task->comm, task->group_leader->real_parent->pid, task->pid, task->exit_state);
            else
                printk(KERN_WARNING "%s: could not log zombie process due to insufficient buffer size!", MODULE_NAME);
        }
    }

    if(copy_to_user(ubuf, buf, len))
        return -EFAULT;

    *ppos = len;

    return len;
}

static const struct proc_ops proc_ops = 
{
    .proc_read = seek_zombie,
};

static int simple_init(void)
{
    printk(KERN_DEBUG "%s: started module\n", MODULE_NAME);
    ent = proc_create("zombie_seeker", 0600, NULL, &proc_ops);
    return 0;
}

static void simple_cleanup(void)
{
    printk(KERN_DEBUG "%s: module cleaned up!\n", MODULE_NAME);
    proc_remove(ent);
}

module_init(simple_init);
module_exit(simple_cleanup);