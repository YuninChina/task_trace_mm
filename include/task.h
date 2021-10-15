#ifndef __TASK__H
#define __TASK__H
#include "klist.h"

#ifdef  __cplusplus
extern "C" {
#endif

typedef struct task_mm_node_s{
	struct list_head list;
	/////////////////////
	unsigned long pid;  //process ID
	unsigned long tid;  //thread ID
	const char *func;
	unsigned long line;
	unsigned long size;
	void *addr;
}task_mm_node_t;


typedef struct task_self_s{
	int exit;
}task_self_t;


typedef void *(*task_func_t)(task_self_t *self,void *user_data);
typedef struct task_s task_t;


task_t *task_create(const char *name,unsigned long stack_size,int priority,task_func_t func,void *arg);
void task_destroy(task_t *task);
void task_exit(task_t *task);

void task_mm_show(void);
void task_mm_add(unsigned long tid,task_mm_node_t *mnode);
void task_mm_del(unsigned long tid,void *addr);

#ifdef  __cplusplus
}
#endif


#endif
