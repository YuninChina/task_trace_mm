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
	int exit;
	sem_t sem;
};


pthread_mutex_t task_mutex = PTHREAD_MUTEX_INITIALIZER;
static LIST_HEAD(task_list);


static void *__task_routine(void *arg)
{
	pthread_detach(pthread_self());
	task_t *task = (task_t *)arg;
	assert(task);
	task->node.info.tid = (unsigned long)pthread_self();
	task->node.info.pid = (unsigned long)gettid();
	prctl(PR_SET_NAME,task->node.info.name);
	
	if(task->node.info.func) 
		task->node.info.func(task->node.info.arg);
	return NULL;
}

task_t *task_create(const char *name,unsigned long stack_size,int priority,task_func_t func,void *arg)
{
	task_t *task = NULL;
	pthread_t thread_id;
	task = malloc(sizeof(*task));
	assert(task);
	task->node.info.name = name;
	task->node.info.stack_size = stack_size;
	task->node.info.priority = priority;
	task->node.info.func = func;
	task->node.info.arg = arg;
	task->exit = 1;
	sem_init(&task->sem,0,0);
	
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
	task->exit = 0;
	return task;
}


void task_destroy(task_t *task)
{
	task_t *node = NULL,*tmp = NULL;
	task_mm_node_t *mnode = NULL,*tmnode = NULL;
	
	if(task)
	{
		task->exit = 1;
		/* waiting task exit */
		sem_wait(&task->sem);
		sem_destroy(&task->sem);
		list_for_each_entry_safe(node, tmp,&task_list, list) {
			if(node == task)
			{
				list_del(&node->list);
				break;
			}
		}
		
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


void task_mm_add(unsigned long tid,task_mm_node_t *mnode)
{
	task_t *node = NULL,*tmp = NULL;
	list_for_each_entry_safe(node, tmp,&task_list, list) {
		if(tid == node->node.info.tid)
		{
			list_add_tail(&mnode->list, &node->node.list);
			break;
		}
	}
}


void task_mm_del(unsigned long tid,void *addr)
{
	task_t *node = NULL,*tmp = NULL;
	list_for_each_entry_safe(node, tmp,&task_list, list) {
		if(tid == node->node.info.tid)
		{
			break;
		}
	}
	task_mm_node_t *mnode = NULL,*tmnode = NULL;
	if(node)
	{
		list_for_each_entry_safe(mnode, tmnode,&node->node.list, list) {
			if(addr == mnode->addr)
			{
				list_del(&mnode->list);
				free(mnode);
				break;
			}
		}
	}
}















