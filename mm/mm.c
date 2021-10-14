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
	task_mm_node_t *node = NULL;
	addr = malloc(size);
	assert(addr);
	node = malloc(sizeof(*node));
	assert(node);
	node->tid = (unsigned long)pthread_self();
	node->pid = (unsigned long)gettid();
	node->func = func;
	node->line = line;
	node->size = size;
	node->addr = addr;

	task_mm_add(node->tid,node);
	
	return addr;
}


void mm_free(void *addr)
{
	unsigned long tid;  //thread ID
	if(addr)
	{
		tid = (unsigned long)pthread_self();
		task_mm_del(tid,addr);
		free(addr);
		addr = NULL;
	}
}


const char *task_name_get(unsigned long ppid,unsigned long pid,char *buf,unsigned int buf_size)
{
#define TASK_NAME_CMD "cat /proc/%d/task/%d/status | grep Name | xargs | sed 's/Name: \\(.*\\)/\\1/g'"
	char cmd[1024] = {0,};
	FILE *fp = NULL;
	char *p_ret = NULL;
	memset(cmd,0,sizeof(cmd));
    snprintf(cmd,sizeof(cmd),TASK_NAME_CMD,ppid,pid);
    //printf("+++ %s\n",cmd);
    fp = popen (cmd, "r");
    assert(fp);
	memset(buf,0,buf_size);
	p_ret = fgets (buf, buf_size, fp);
    assert(p_ret);
    fclose(fp);
    fp = NULL;
    buf[strlen(buf)-1] = 0;  //去掉最后换行符.
    //printf(">>> %s (%d)\n",buf,strlen(buf));
    return buf;
}


