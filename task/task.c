#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <sys/prctl.h>
#include <semaphore.h>

#include "task.h"
#include "klist.h"
#include "inits.h"


typedef struct task_info_s {
	const char *name;
	unsigned long stack_size;
	int priority;
	task_func_t func;
	void *arg;
	/* other */
	unsigned long tid;
	unsigned long pid;
}task_info_t;

struct task_s {
	struct list_head list;
	struct __task_node_s {
		struct list_head list;
		pthread_mutex_t mutex;
		task_info_t info;
	}node;
	task_self_t self;
};


pthread_mutex_t task_mutex = PTHREAD_MUTEX_INITIALIZER;
static LIST_HEAD(task_list);


static void task_exit(task_t *task)
{
	task_t *node = NULL,*tmp = NULL;
	task_mm_node_t *mnode = NULL,*tmnode = NULL;
	
	if(task)
	{
		printf("task_exit: %s(%u : %u)\n",task->node.info.name,
		task->node.info.pid,task->node.info.tid);
		mt_async_queue_free(task->self.q);
		pthread_mutex_lock(&task_mutex);
		list_for_each_entry_safe(node, tmp,&task_list, list) {
			if(node == task)
			{
				list_del(&node->list);
				break;
			}
		}
		pthread_mutex_unlock(&task_mutex);
		
		list_for_each_entry_safe(mnode, tmnode,&task->node.list, list) {
			list_del(&mnode->list);
			free(mnode);
			break;
		}
		pthread_mutex_destroy(&task->node.mutex);
		free(task);
		task = NULL;
	}
}

static void *__task_routine(void *arg)
{
	pthread_detach(pthread_self());
	task_t *task = (task_t *)arg;
	assert(task);
	task->node.info.tid = (unsigned long)pthread_self();
	task->node.info.pid = (unsigned long)gettid();
	prctl(PR_SET_NAME,task->node.info.name);
	if(task->node.info.func) 
		task->node.info.func(&task->self,task->node.info.arg);
	task_exit(task);
	return NULL;
}

task_t *task_create(const char *name,unsigned long stack_size,int priority,task_func_t func,void *arg)
{
	task_t *task = NULL;
	pthread_t thread_id;
	task = malloc(sizeof(*task));
	assert(task);
	memset(task,0,sizeof(*task));
	task->node.info.name = name;
	task->node.info.stack_size = stack_size;
	task->node.info.priority = priority;
	task->node.info.func = func;
	task->node.info.arg = arg;
	task->self.q = mt_async_queue_new();
	
	INIT_LIST_HEAD(&task->node.list);
	pthread_mutex_init(&task->node.mutex, NULL);
	pthread_mutex_lock(&task_mutex);
	list_add_tail(&task->list, &task_list);
	pthread_mutex_unlock(&task_mutex);
	
	if(pthread_create(&thread_id, NULL, __task_routine, (void *)task) != 0)
	{
		assert(0);
		free(task);
		return NULL;
	}
	return task;
}





void task_mm_add(unsigned long tid,task_mm_node_t *mnode)
{
	task_t *node = NULL,*tmp = NULL;
	pthread_mutex_lock(&task_mutex);
	list_for_each_entry_safe(node, tmp,&task_list, list) {
		if(tid == node->node.info.tid)
		{
			list_add_tail(&mnode->list, &node->node.list);
			break;
		}
	}
	pthread_mutex_unlock(&task_mutex);
}


void task_mm_del(unsigned long tid,void *addr)
{
	task_t *node = NULL,*tmp = NULL;
	pthread_mutex_lock(&task_mutex);
	list_for_each_entry_safe(node, tmp,&task_list, list) {
		if(tid == node->node.info.tid)
		{
			task_mm_node_t *mnode = NULL,*tmnode = NULL;
			list_for_each_entry_safe(mnode, tmnode,&node->node.list, list) {
				if(addr == mnode->addr)
				{
					list_del(&mnode->list);
					free(mnode);
					break;
				}
			}
			break;
		}
	}
	pthread_mutex_unlock(&task_mutex);
}


void task_mm_show(void)
{
	task_t *node = NULL,*tmp = NULL;

	char cmd[1024] = {0,};
	FILE *fp = NULL;
    char buf[1024] = {0,};
    char time[64] = {0,};
    char *pResult = NULL;
    unsigned long task_pid = 0,task_tid = 0;
	int i = 0;
	unsigned long mem_size = 0;
	const char *task_name = NULL;
	pid_t pid = getpid();
	memset(cmd,0,sizeof(cmd));
    snprintf(cmd,sizeof(cmd),"ls /proc/%d/task/ | xargs",pid);
    fp = popen (cmd, "r");
    assert(fp);
	memset(buf,0,sizeof(buf));
	pResult = fgets (buf, sizeof (buf), fp);
    assert(pResult);
    fclose(fp);
    fp = NULL;
    //printf("%s\n",buf);
    time2str(time,sizeof(time));
	printf("\n\n=========================================== task_mm_show [%s] ===========================================\n",time);
	printf("%-20s %-20s %-20s %-20s\n","[task]","[task_pid]","[task_tid]","[size]");
	
    char *sub = NULL,*str = NULL;
	str = buf;
	do {
		sub = strtok(str," ");
		if(NULL == sub)
			break;
		sscanf(str,"%u",&task_pid);
		//printf("pid=%d, task_pid: %d\n",pid,task_pid);
		str += (strlen(sub)+1);

		///////////////////////////////
		mem_size = 0;
		task_name = NULL;
		list_for_each_entry_safe(node, tmp,&task_list, list) {
			if(task_pid == node->node.info.pid)
			{
				task_name = node->node.info.name;
				task_tid = node->node.info.tid;
				///////////////////////////////
				task_mm_node_t *mnode = NULL,*tmnode = NULL;
				list_for_each_entry_safe(mnode, tmnode,&node->node.list, list) {
					mem_size += mnode->size;
				}
			}
		}
		printf("%-20s %-20u %-20u %-20u\n",task_name,task_pid,task_tid,mem_size);
	} while(1);
	
}


static task_t _task;
static int __task_init(void)
{
	//printf("enter __task_init()\n");
	static char _name[32] = {0,};
	memset(&_task,0,sizeof(_task));
	prctl(PR_GET_NAME,_name);
	_task.node.info.name = _name;
	_task.node.info.stack_size = 0;
	_task.node.info.priority = 0;
	_task.node.info.func = NULL;
	_task.node.info.arg = NULL;
	
	_task.node.info.tid = (unsigned long)pthread_self();
	_task.node.info.pid = (unsigned long)gettid();
	
	INIT_LIST_HEAD(&_task.node.list);
	pthread_mutex_init(&_task.node.mutex, NULL);
	pthread_mutex_lock(&task_mutex);
	list_add_tail(&_task.list, &task_list);
	pthread_mutex_unlock(&task_mutex);
}

static void __task_exit(void)
{
	pthread_mutex_destroy(&_task.node.mutex);
}

pure_init(__task_init);
module_exit(__task_exit);

