#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched/signal.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/string.h>

static long state_filter = -1; // Filter processes by state (-1 for no filter)
static char* name_filter = ""; // Filter processes by name (empty for no filter)
module_param(state_filter, long, S_IRUGO); // Module parameter for state filtering
module_param(name_filter, charp, S_IRUGO); // Module parameter for name filtering
MODULE_PARM_DESC(state_filter, "Process state to filter by; -1 for no filter");
MODULE_PARM_DESC(name_filter, "Process name to filter by; empty for no filter");

#define BUFFER_SIZE 256
static char buffer[BUFFER_SIZE];

char *get_task_state(long state) {
    switch (state) {
        case TASK_RUNNING:
            return "TASK_RUNNING";
        case TASK_INTERRUPTIBLE:
            return "TASK_INTERRUPTIBLE";
        case TASK_UNINTERRUPTIBLE:
            return "TASK_UNINTERRUPTIBLE";
        case __TASK_STOPPED:
            return "__TASK_STOPPED";
        case __TASK_TRACED:
            return "__TASK_TRACED";
        default:
            snprintf(buffer, BUFFER_SIZE, "Unknown Type:%ld", state);
            return buffer;
    }
}

static int __init process_filter_init(void) {
    struct task_struct *task;
    unsigned int process_count = 0;
    printk(KERN_INFO "process_filter: Module loaded.\n");
    printk(KERN_INFO "process_filter: Filtering by state: %ld and name: %s\n", state_filter, name_filter);

    for_each_process(task) {
        if ((state_filter == -1 || task->__state == state_filter) &&
            (strlen(name_filter) == 0 || strncmp(task->comm, name_filter, TASK_COMM_LEN) == 0)) {
            printk(KERN_INFO "Process: %s\t PID:[%d]\t State:%s\n",
                   task->comm, task->pid, get_task_state(task->__state));
            process_count++;
        }
    }
    printk(KERN_INFO "process_filter: Total processes filtered: %u\n", process_count);
    return 0;
}

static void __exit process_filter_exit(void) {
    printk(KERN_INFO "process_filter: Module unloaded.\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Manav Jarial");
MODULE_DESCRIPTION("Enhanced process listing module with state and name filtering.");
MODULE_VERSION("0.1");

module_init(process_filter_init);
module_exit(process_filter_exit);
