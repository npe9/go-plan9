// Copyright 2009 The Go Authors.  All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <pthread.h>
#include "libcgo.h"

static void* threadentry(void*);

static pthread_key_t km, kg;

void
initcgo(void)
{
	if(pthread_key_create(&km, nil) < 0) {
		fprintf(stderr, "libcgo: pthread_key_create failed\n");
		abort();
	}
	if(pthread_key_create(&kg, nil) < 0) {
		fprintf(stderr, "libcgo: pthread_key_create failed\n");
		abort();
	}
}

void
libcgo_sys_thread_start(ThreadStart *ts)
{
	pthread_attr_t attr;
	pthread_t p;
	size_t size;

	pthread_attr_init(&attr);
	pthread_attr_getstacksize(&attr, &size);
	ts->g->stackguard = size;
	pthread_create(&p, &attr, threadentry, ts);
}

static void*
threadentry(void *v)
{
	ThreadStart ts;

	ts = *(ThreadStart*)v;
	free(v);

	ts.g->stackbase = (uintptr)&ts;

	/*
	 * libcgo_sys_thread_start set stackguard to stack size;
	 * change to actual guard pointer.
	 */
	ts.g->stackguard = (uintptr)&ts - ts.g->stackguard + 4096;

	crosscall_amd64(ts.m, ts.g, ts.fn);
	return nil;
}

void
libcgo_set_scheduler(void *m, void *g)
{
	pthread_setspecific(km, m);
	pthread_setspecific(kg, g);
}

struct get_scheduler_args {
	void *m;
	void *g;
};

void libcgo_get_scheduler(struct get_scheduler_args *)
  __attribute__ ((visibility("hidden")));

void
libcgo_get_scheduler(struct get_scheduler_args *p)
{
	p->m = pthread_getspecific(km);
	p->g = pthread_getspecific(kg);
}
