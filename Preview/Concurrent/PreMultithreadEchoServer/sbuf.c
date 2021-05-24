#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include "sbuf.h"

#define P sem_wait
#define V sem_post

void sbuf_init(sbuf_t * sp, int n) {
	sp->buf = (int *)calloc(n, sizeof(int));
	sp->n = n;
	sp->front = sp->rear = 0;
	sem_init(&sp->mutex, 0, 1);
	sem_init(&sp->slots, 0, n);
	sem_init(&sp->items, 0, 0);
}

void sbuf_insert(sbuf_t * sp, int item) {
	P(&sp->slots);
	P(&sp->mutex);
	sp->buf[(++sp->rear) % (sp->n)] = item;
	V(&sp->mutex);
	V(&sp->items);
}

int sbuf_remove(sbuf_t *sp) {
	int item;
	P(&sp->items);
	P(&sp->mutex);
	item = sp->buf[(++sp->front) % (sp->n)]; 
	V(&sp->mutex);
	V(&sp->slots);
	return item;
}

