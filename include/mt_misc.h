#ifndef __MT_MISC_H
#define __MT_MISC_H

#ifdef  __cplusplus
extern "C" {
#endif

void time2str(char *buf,int size);
void thread_name_get(char *task_name);
void thread_name_set(const char *task_name);


#ifdef  __cplusplus
}
#endif

#endif  /* __MT_MISC_H */

