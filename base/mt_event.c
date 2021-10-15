#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <errno.h>
#include <fcntl.h>

#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <sys/select.h>		/* for select() workaround */
#include <sys/signalfd.h>	/* struct signalfd_siginfo */
#include <unistd.h>		/* close(), read() */


#include "mt_event.h"
#include "mt_log.h"



struct mt_ev_ctx_s {
	int running;
	int fd;	    	/* For epoll() */
	int maxevents;  /* For epoll() */
};


static int _init(mt_ev_ctx_t *ctx, int close_old)
{
	int fd;

	fd = epoll_create1(EPOLL_CLOEXEC);
	if (fd < 0)
		return -1;

	if (close_old)
		close(ctx->fd);

	ctx->fd = fd;

	return 0;
}


mt_ev_ctx_t *mt_ev_create(void)
{
	mt_ev_ctx_t *c = NULL;
	c = malloc(sizeof(*c));
	RETURN_VAL_IF_FAIL(c, NULL);
	memset(c,0,sizeof(*c));
	return mt_ev_create2(MT_EV_MAX_EVENTS);
}

mt_ev_ctx_t *mt_ev_create2(unsigned int maxevents)
{
	mt_ev_ctx_t *c = NULL;
	c = malloc(sizeof(*c));
	RETURN_VAL_IF_FAIL(c, NULL);
	memset(c,0,sizeof(*c));
	c->maxevents = maxevents;
	_init(c,0);
	return c;
}


void mt_ev_destroy(mt_ev_ctx_t *c)
{

}

int mt_ev_run(mt_ev_ctx_t *c)
{
	return 0;
}

int mt_ev_io_add(mt_ev_ctx_t *c,mt_ev_callback_t cb,void *arg,int fd, int events)
{
	return 0;
}

int mt_ev_timer_add(mt_ev_ctx_t *c,mt_ev_callback_t cb,void *arg,int timeout, int period)
{
	return 0;
}

int mt_ev_signal_add(mt_ev_ctx_t *c,mt_ev_callback_t cb,void *arg,int signo)
{
	return 0;
}


