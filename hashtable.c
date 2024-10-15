/*
 * Copyright (c) 2024, <Diaconescu Stefania Clara 313CA>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "utils.h"
#include "hashtable.h"
#include "doubly_list.h"

int
compare_function_ints(void *a, void *b)
{
	int int_a = *((int *)a);
	int int_b = *((int *)b);

	if (int_a == int_b) {
		return 0;
	} else if (int_a < int_b) {
		return -1;
	} else {
		return 1;
	}
}

int
compare_function_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	return strcmp(str_a, str_b);
}

void
key_val_free_function(void *data)
{
	if (!data)
		return;

	info_t *inf = (info_t *)data;
	if (inf->key)
		free(inf->key);
	if (inf->value)
		free(inf->value);
}

hashtable_t*
ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		  int (*compare_function)(void*, void*),
		  void (*key_val_free_function)(void*))
{
	hashtable_t *hashtable = malloc(sizeof(hashtable_t));
	DIE(!hashtable, "malloc failed");
	hashtable->buckets = malloc(hmax * sizeof(doubly_linked_list_t*));
	DIE(!hashtable->buckets, "malloc failed");

	for (unsigned int i = 0; i < hmax; i++) {
		hashtable->buckets[i] = dll_create(sizeof(info_t));

		if (!hashtable->buckets[i]) {
			for (unsigned int j = 0; j < i; j++) {
				free(hashtable->buckets[j]);
				free(hashtable->buckets);
				free(hashtable);
				exit(-1);
			}
			hashtable->buckets[i]->head = NULL;
		}
	}

	hashtable->size = 0;  // toate nodurile din hashtable
	hashtable->hmax = hmax;
	hashtable->compare_function = compare_function;
	hashtable->hash_function = hash_function;
	hashtable->key_val_free_function = key_val_free_function;

	return hashtable;
}

int
ht_has_key(hashtable_t *ht, void *key)
{
	if (!ht || !key)
		return -1;

	int index = ht->hash_function(key) % ht->hmax;

	if (!(ht->buckets[index]))
		return 0;
	if (!(ht->buckets[index]->head))
		return 0;

	dll_node_t *current = ht->buckets[index]->head;

	while (current) {
		if (ht->compare_function(key, ((info_t*)current->data)->key) == 0)
			return 1;
		current = current->next;
	}

	return 0;
}

void*
ht_get(hashtable_t *ht, void *key)
{
	if (!ht || !key || ht_has_key(ht, key) != 1)
		return NULL;

	int index = ht->hash_function(key) % ht->hmax;

	if (!(ht->buckets[index]))
		return 0;
	if (!(ht->buckets[index]->head))
		return 0;

	dll_node_t *current = ht->buckets[index]->head;

	// Caut cheia. Daca o gasesc, returnez valoarea
	while (current) {
		if (ht->compare_function(key, ((info_t*)current->data)->key) == 0)
			return ((info_t*)current->data)->value;
		current = current->next;
	}

	return NULL;
}

void
ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	   void *value, unsigned int value_size)
{
	if (!ht || !key || !value)
		return;

	int index = ht->hash_function(key) % ht->hmax;

	dll_node_t *current = ht->buckets[index]->head;

	while (current) {
		// daca este gasita cheia, se suprascrie valoarea
		if (ht->compare_function(key, ((info_t *)current->data)->key) == 0) {
			free(((info_t *)current->data)->value);
			((info_t *)current->data)->value = malloc(value_size);
			DIE(!((info_t *)current->data)->value, "malloc failed");
			memcpy(((info_t *)current->data)->value, value, value_size);
			return;
		}
		current = current->next;
	}
	info_t *node_data = malloc(sizeof(info_t));
	DIE(!node_data, "malloc failed");

	node_data->key = malloc(key_size);
	DIE(!node_data->key, "malloc failed");
	memcpy(node_data->key, key, key_size);
	node_data->value = malloc(value_size);
	DIE(!node_data->value, "malloc failed");
	memcpy(node_data->value, value, value_size);


	dll_add_nth_node(ht->buckets[index], ht->buckets[index]->size, node_data,
						sizeof(info_t));
	ht->size++;
	free(node_data);
}

void
ht_remove_entry(hashtable_t *ht, void *key)
{
	if (!ht || !key || ht_has_key(ht, key) != 1)
		return;

	int index = ht->hash_function(key) % ht->hmax;
	dll_node_t *current = ht->buckets[index]->head;
	int i = 0;
	while (current != NULL) {
		if (ht->compare_function(key, ((info_t *)current->data)->key) == 0)
			break;
		i++;
		current = current->next;
	}
	dll_node_t *node = dll_remove_nth_node(ht->buckets[index], i);
	ht->key_val_free_function(node->data);
	free(node->data);
	free(node);
	ht->size--;
}

void
ht_free(hashtable_t *ht)
{
	if (!ht)
		return;

	for (unsigned int i = 0; i < ht->hmax; i++) {
		if(ht->buckets[i]->head) {
			dll_node_t *current = ht->buckets[i]->head;
			while (current != NULL)
			{
				dll_node_t *aux = current;
				current = current->next;
				key_val_free_function(aux->data);
				free(aux->data);
				free(aux);
			}
		}
		free(ht->buckets[i]);
	}
	free(ht->buckets);
	free(ht);
}

unsigned int
ht_get_size(hashtable_t *ht)
{
	if (!ht)
		return 0;

	return ht->size;
}

unsigned int
ht_get_hmax(hashtable_t *ht)
{
	if (!ht)
		return 0;

	return ht->hmax;
}
