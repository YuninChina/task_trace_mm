#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>

#include "mm.h"
#include "task.h"


void *mm_malloc(const char *func,unsigned long line,unsigned long size)
{
	void *addr = NULL;
	addr = malloc(size);
	assert(addr);

#if defined(CONFIG_TASK_MM)	
	task_mm_node_t *node = NULL;
	node = malloc(sizeof(*node));
	assert(node);
	node->tid = (unsigned long)pthread_self();
	node->pid = (unsigned long)gettid();
	node->func = func;
	node->line = line;
	node->size = size;
	node->addr = addr;

	task_mm_add(node->tid,node);
#endif

	return addr;
}


void mm_free(void *addr)
{
	if(addr)
	{
#if defined(CONFIG_TASK_MM)	
		unsigned long tid;	//thread ID
		tid = (unsigned long)pthread_self();
		task_mm_del(tid,addr);
#endif
		free(addr);
		addr = NULL;
	}
}



