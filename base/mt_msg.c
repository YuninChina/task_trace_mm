#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "mt_msg.h"
#include "mt_log.h"
#include "mt_async_queue.h"
#include "mm.h"


int mt_msg_send(mt_msg_t *msg)
{
	RETURN_VAL_IF_FAIL(msg, -1);
	mt_async_queue_t *q;
	q = task_aq_get(msg->dst);
	RETURN_VAL_IF_FAIL(q, -1);
	mt_async_queue_push_tail(q, msg);
	return 0;
}


void *mt_msg_recv(void)
{
	mt_msg_t *msg = NULL;
	void *data = NULL;
	mt_async_queue_t *q;
	q = task_aq_self();
	RETURN_VAL_IF_FAIL(q, -1);
	msg = mt_async_queue_pop_head(q);
	RETURN_VAL_IF_FAIL(msg, -1);
	msg = msg->data;
	FREE(msg);
	return data;
}


