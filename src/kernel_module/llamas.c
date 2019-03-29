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

static void recurOldestChild(struct task_struct* proc) {

	/* Iterating over children to find oldest child */
	u64 start_time_var = -1;																/* Variable to compare next children with current oldest child - default value is '-1' because start time of a process cannot be less than zero */
	struct task_struct* oldest_child = NULL;								/* Variable to keep oldest child to print its info at the end */
	struct task_struct* child;															/* Variable to keep children in list_for_each loop */
	struct list_head* children;															/* Linked list head of children of process */
	list_for_each(children, &proc->children) {
		/* 'child' is now current child in the linked list */
		child = list_entry(children, struct task_struct, sibling);
		
		/* Recursive call to child */
		recurOldestChild(child);

		/* Comparing child's start_time with current oldest child */
		if (start_time_var == -1) {
			start_time_var = child->start_time;
			oldest_child = child;
		}
		else if (child->start_time < start_time_var) {
			start_time_var = child->start_time;
			oldest_child = child;
		} 
	}

	/* If oldest_child is still null, it means that &proc->children is empty and code didn't react into 'list_for_each' loop */
	/* Otherwise, we found our oldest child */
	if (oldest_child != NULL) {
		printk(KERN_ALERT "Process(ID/Name): %d/%s - Oldest Child(ID/Name): %d/%s", proc->pid, proc->comm, oldest_child->pid, oldest_child->comm);
	}
}

static int llamas_init(void)
{
	struct task_struct* task;																/* Root of process tree */
	bool pid_flag = true;																		/* Boolean to keep track of whether we found a process with given id */

	/* Kernel alert to indicate that kernel module is loaded */
	printk(KERN_ALERT " \n");
	printk(KERN_ALERT "Llamas Kernel Module is loaded.\n");
	
	for_each_process(task){
		if (task->pid == processID) {

			/* We found a process with given id, now we can update the flag */
			pid_flag = false;

			/* Kernel alert to indicate that a process is found with given id */
			printk(KERN_ALERT "Process with id %d is found: %s", processID, task->comm);

			/* Calling 'recurOldestChild' to find oldest children of processes in subtree */
			recurOldestChild(task);
		}
	}

	/* If pid_flag is still true, it means that we couldn't find a process with given id */
	if (pid_flag) {
		printk(KERN_ALERT "No process with id %d is found.", processID);
	}
	printk(KERN_ALERT " \n");
	return 0;
}
 
static void llamas_cleanup(void)
{
	/* Kernel alert to indicate that kernel module is unloaded */
	printk(KERN_ALERT "Llamas Kernel Module is unloaded.\n");
}

module_init(llamas_init);
module_exit(llamas_cleanup);
module_param(processID, int, 0);
