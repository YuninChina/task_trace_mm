#ifndef __MM__H
#define __MM__H


void *mm_malloc(const char *func,unsigned long line,unsigned long size);
void mm_free(void *addr);

#define MALLOC(size)	mm_malloc(__func__,__LINE__,size)
#define FREE(addr)		mm_free(addr)

#endif
