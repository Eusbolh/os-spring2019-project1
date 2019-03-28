#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/moduleparam.h>

#include <linux/sched/signal.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Emre Yavuz");

int processID=0; 
static int llamas_init(void)
{
	printk(KERN_ALERT " \n");
	printk(KERN_ALERT "Llamas Kernel Module is loaded.\n");
	struct task_struct* task;
	bool pidFlag = true;
	bool childrenFlag = true;
	u64 start_time_var = -1;
	struct task_struct* oldest_child = NULL;
	for_each_process(task){
		if (task->pid == processID) {
			printk(KERN_ALERT "Process with id %d is found: %s", processID, task->comm);
			pidFlag = false;
			struct task_struct* child;
			struct list_head* children;
			list_for_each(children, &task->children) {
				childrenFlag = false;
				child = list_entry(children, struct task_struct, sibling);
				if (start_time_var == -1) {
					start_time_var = child->real_start_time;
					oldest_child = child;
				}
				else if (child->real_start_time < start_time_var) {
					start_time_var = child->real_start_time;
					oldest_child = child;
				} 
				printk(KERN_ALERT "Child Info: %d/%llu/%s\n", child->pid, child->real_start_time, child->comm);
			}
		}
	}
	if (pidFlag) {
		printk(KERN_ALERT "No process with id %d is found.", processID);
	}
	if (childrenFlag) {
		printk(KERN_ALERT "No child is found for process %d", processID);
	}
	else {
		if (oldest_child != NULL) {
			printk(KERN_ALERT "Oldest child: %d/%s", oldest_child->pid, oldest_child->comm);
		}
	}
	printk(KERN_ALERT " \n");
	return 0;
}
 
static void llamas_cleanup(void)
{
	printk(KERN_ALERT "Llamas Kernel Module is unloaded.\n");
}

module_init(llamas_init);
module_exit(llamas_cleanup);
module_param(processID, int, 0);
