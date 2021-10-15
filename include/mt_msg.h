#ifndef __MT_MSG_H
#define __MT_MSG_H

typedef struct mt_msg_s {
	unsigned int src;
	unsigned int dst;
	int priority;
	unsigned int size;
	unsigned char data[0];
}mt_msg_t;


int mt_msg_send(mt_msg_t *msg);


#endif
