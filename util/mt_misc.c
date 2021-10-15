#include <stdarg.h>
#include <stdio.h>
#include <sys/prctl.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include <strings.h>
#include <string.h>

void time2str(char *buf,int size)
{
    assert(buf && size > 0);
    struct timeval tv;
    struct tm tm;
    int pos = 0;
    long millisecond = 0;
    bzero(buf,size);
    gettimeofday(&tv,NULL);
    localtime_r(&tv.tv_sec,&tm);
    //格式化化为字符串输出.
    strftime(buf,size,"%F %T",&tm);
    pos = strlen(buf);
    millisecond = tv.tv_usec / 1000;
    snprintf(buf+pos,size-pos,".%lu",millisecond);
}

void thread_name_get(char *task_name)
{
	prctl(PR_GET_NAME,task_name);
}

void thread_name_set(const char *task_name)
{
	prctl(PR_SET_NAME,task_name);
}


