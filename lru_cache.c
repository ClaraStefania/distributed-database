/*
 * Copyright (c) 2024, <Diaconescu Stefania Clara 313CA>
 */

#include <stdio.h>
#include <string.h>
#include "lru_cache.h"
#include "utils.h"
#include "queue.h"
#include "hashtable.h"
#include "doubly_list.h"
#include "server.h"

info_t *
create_order_data(void *key, void* value)
{
	info_t *node_data = malloc(sizeof(info_t));
	DIE(!node_data, "malloc failed");

	node_data->key = malloc(DOC_NAME_LENGTH);
	DIE(!(node_data->key), "malloc failed");
	node_data->value = malloc(DOC_CONTENT_LENGTH);
	DIE(!(node_data->value), "malloc failed");

	strcpy((char *)node_data->key, (char *)key);
	strcpy((char *)node_data->value, (char *)value);

	return node_data;
}

lru_cache *
init_lru_cache(unsigned int cache_capacity)
{
	lru_cache *cache = malloc(sizeof(lru_cache));
	DIE(!cache, "malloc failed");

	cache->ht = ht_create(cache_capacity, hash_string,
						  compare_function_strings, key_val_free_function);
	cache->order = dll_create(sizeof(info_t));

	return cache;
}

bool
lru_cache_is_full(lru_cache *cache)
{
	if (cache->ht->size == cache->ht->hmax)
		return true;
	return false;
}

void
free_lru_cache(lru_cache **cache)
{
	if (!(*cache))
		return;
	if ((*cache)->order) {
		if ((*cache)->order->head) {
			dll_node_t *current = (*cache)->order->head;

			// se elibereaza campurile din structura info_t
			while (current) {
				if ((info_t *)current->data) {
					key_val_free_function((info_t *)current->data);
					current = current->next;
				}
			}
		}
		dll_free(&((*cache)->order));
	}
	if ((*cache)->ht)
		ht_free((*cache)->ht);
	free(*cache);
}

bool
lru_cache_put(lru_cache *cache, void *key, void *value, void **evicted_key)
{
	if (ht_has_key(cache->ht, key))
		return false;

	// verifica daca cache-ul este plin
	if (lru_cache_is_full(cache) == true && ht_has_key(cache->ht, key) == 0) {
		// se extrage cel mai vechi document si este eliminat din cache
		dll_node_t *node = dll_remove_nth_node(cache->order,
											   cache->order->size);
		*evicted_key = node->data;
		lru_cache_remove(cache, ((info_t *)(*evicted_key))->key);
		free(node);
	}

	// este adaugat noul document in hashtable si in lista de ordine
	info_t *node_data = create_order_data(key, value);
	dll_add_nth_node(cache->order, 0, node_data, sizeof(info_t));
	ht_put(cache->ht, key, DOC_NAME_LENGTH, value, DOC_CONTENT_LENGTH);
	free(node_data);
	return true;
}

void *
lru_cache_get(lru_cache *cache, void *key)
{
	void * data = ht_get(cache->ht, key);
	if (!data)
		return NULL;
	else
		return data;
}

void
lru_cache_remove(lru_cache *cache, void *key)
{
	int i = 0;
	ht_remove_entry(cache->ht, key);

	// elimina aparitiile documentului in lista de ordine
	if (cache->order->head) {
		dll_node_t *current = cache->order->head;

		while (current) {
			void *data = ((info_t *)(current->data))->key;
			if (!compare_function_strings(data, key))
				dll_remove_nth_node(cache->order, i);
			i++;
			current = current->next;
		}
	}
}
