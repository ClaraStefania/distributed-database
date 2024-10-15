/*
 * Copyright (c) 2024, <Diaconescu Stefania Clara 313CA>
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "queue.h"
#include "utils.h"
#include "doubly_list.h"
#include "server.h"

queue_t *
q_create(unsigned int data_size, unsigned int max_size)
{
	queue_t *queue = malloc(sizeof(queue_t));
	DIE(!queue, "malloc failed");

	queue->max_size = max_size;
	queue->size = 0;
	queue->data_size = data_size;
	queue->read_idx = 0;
	queue->write_idx = 0;

	queue->buff = malloc((max_size + 1) * sizeof(void *));
	DIE(!queue->buff, "malloc failed");

	for (unsigned int i = 0; i < max_size; i++) {
		queue->buff[i] = malloc(queue->data_size);
		DIE(!queue->buff[i], "malloc failed");
	}

	return queue;
}

unsigned int
q_get_size(queue_t *q)
{
	return q->size;
}

unsigned int
q_is_empty(queue_t *q)
{
	if (q->size == 0)
		return 1;
	return 0;
}

void *
q_front(queue_t *q)
{
	return q->buff[q->read_idx];
}

int
q_dequeue(queue_t *q)
{
	if (q->size == 0)
		return 0;

	if (q->buff[q->read_idx]) {
		free(q->buff[q->read_idx]);
		q->buff[q->read_idx] = NULL;
		q->read_idx = (q->read_idx + 1) % q->max_size;
		q->size--;
	}
	return 1;
}

int
q_enqueue(queue_t *q, void *new_data)
{
	if (q->size == q->max_size)
		return 0;

	if (!(q->buff[q->write_idx])) {
		q->buff[q->write_idx] = malloc(q->data_size);
		DIE(!q->buff[q->write_idx], "malloc failed");
	}

	// este adaugat new_data in coada
	memcpy(q->buff[q->write_idx], new_data, q->data_size);
	q->write_idx = (q->write_idx + 1) % q->max_size;
	q->size++;
	return 1;
}

void
q_clear(queue_t *q)
{
	for (unsigned int i = 0; i < q->max_size; i++) {
		if (q->buff[i]) {
			free(q->buff[i]);
			q->buff[i] = NULL;
		}
	}
	q->size = 0;
	q->write_idx = 0;
	q->read_idx = 0;
}

void
q_free(queue_t *q)
{
	q_clear(q);
	free(q->buff);
	free(q);
}
