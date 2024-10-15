/*
 * Copyright (c) 2024, <Diaconescu Stefania Clara 313CA>
 */

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "doubly_list.h"

typedef struct hashtable_t {
	doubly_linked_list_t **buckets;
	unsigned int size;
	unsigned int hmax;
	unsigned int (*hash_function)(void*);
	int (*compare_function)(void*, void*);
	void (*key_val_free_function)(void*);
} hashtable_t;

int
compare_function_ints(void *a, void *b);

int
compare_function_strings(void *a, void *b);

/**
 * Functia elibereaza data dintr-o structura de tip info_t
 */
void
key_val_free_function(void *data);

/**
 * Functia aloca si initializeaza un hashtable
 */
hashtable_t*
ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		  int (*compare_function)(void*, void*),
		  void (*key_val_free_function)(void*));

/**
 * Functia verifica daca pentru cheia key a fost asociata anterior o valoare
 * in hashtable
 */
int
ht_has_key(hashtable_t *ht, void *key);

void*
ht_get(hashtable_t *ht, void *key);

/**
 * Functia adauga un element in hashtable
 */
void
ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	   void *value, unsigned int value_size);

/**
 * Functia elimina din hashtable intrarea asociata cheii key
 */
void
ht_remove_entry(hashtable_t *ht, void *key);

/**
 * Functia elibereaza memoria folosita de toate intrarile din hashtable si pe
 * cea folosita pentru a stoca structura hashtable
 */
void
ht_free(hashtable_t *ht);

unsigned int
ht_get_size(hashtable_t *ht);

unsigned int
ht_get_hmax(hashtable_t *ht);

#endif
