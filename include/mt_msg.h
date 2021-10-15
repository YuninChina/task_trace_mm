#ifndef __MT_MSG_H
#define __MT_MSG_H

typedef struct mt_msg_s {
	const char *src;
	const char *dst;
	int priority;
	unsigned int size;
	unsigned char data[0];
}mt_msg_t;


int mt_msg_send(mt_msg_t *msg);
void *mt_msg_recv(void);


#endif
