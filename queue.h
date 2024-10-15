/*
 * Copyright (c) 2024, <Diaconescu Stefania Clara 313CA>
 */

#ifndef QUEUE_H
#define QUEUE_H

typedef struct queue_t queue_t;
struct queue_t
{
	unsigned int max_size;
	unsigned int size;
	unsigned int data_size;
	unsigned int read_idx;
	unsigned int write_idx;
	void **buff;
};

/**
 * Functia aloca si initializeaza o coada
 */
queue_t *
q_create(unsigned int data_size, unsigned int max_size);

/**
 * Functia intoarce numarul de elemente din coada al carei pointer este trimis
 * ca parametru
 */
unsigned int
q_get_size(queue_t *q);

/**
 *  Functia intoarce 1 daca coada este goala si 0 in caz contrar
 */
unsigned int
q_is_empty(queue_t *q);

/**
 * Functia intoarce primul element din coada, fara sa il elimine
 */
void *
q_front(queue_t *q);

/**
 * Functia elimina ultimul element din coada
 */
int
q_dequeue(queue_t *q);

/**
 * Functia audauga un nou element in coada
 */
int
q_enqueue(queue_t *q, void *new_data);

/**
 * Functia elimina toate elementele din coada primita ca parametru
 */
void
q_clear(queue_t *q);

/**
 * Functia elibereaza memoria ocupata de coada
 */
void
q_free(queue_t *q);

#endif
