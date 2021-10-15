#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>

#include "mt_msg.h"
#include "mt_log.h"
#include "mt_async_queue.h"
#include "mm.h"
#include "task.h"


int mt_msg_send(mt_msg_t *msg)
{
	RETURN_VAL_IF_FAIL(msg, -1);
	mt_async_queue_t *q = NULL;
	q = (mt_async_queue_t *)task_aq_get(msg->dst);
	assert(q);
	RETURN_VAL_IF_FAIL(q, -1);
	mt_async_queue_push_tail(q, msg);
	return 0;
}


mt_msg_t *mt_msg_recv(void)
{
	mt_msg_t *msg = NULL;
	mt_async_queue_t *q;
	q = task_aq_self();
	RETURN_VAL_IF_FAIL(q, NULL);
	msg = mt_async_queue_pop_head(q);
	RETURN_VAL_IF_FAIL(msg, NULL);
	return msg;
}


