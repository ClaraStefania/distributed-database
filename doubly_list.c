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

doubly_linked_list_t*
dll_create(unsigned int data_size)
{
	doubly_linked_list_t *list = malloc(1 * sizeof(doubly_linked_list_t));
	DIE(!list, "malloc failed");

	list->head = NULL;
	list->tail = NULL;
	list->size = 0;
	list->data_size = data_size;

	return list;
}

void
dll_add_nth_node(doubly_linked_list_t *list, unsigned int n, void *data,
				 unsigned int data_size)
{
	dll_node_t *node = malloc(sizeof(dll_node_t));
	DIE(!node, "malloc failed");
	node->data = malloc(data_size);
	DIE(!node->data, "malloc failed");
	memcpy(node->data, data, data_size);

	if (!list->head) {
		node->next = NULL;
		node->prev = NULL;
		list->head = node;
		list->tail = node;
	} else if (n == 0) {
		node->next = list->head;
		node->prev = NULL;
		node->next->prev = node;
		list->head = node;
	} else if (n >= list->size) {
		dll_node_t *current = list->tail;

		current->next = node;
		node->next = NULL;
		node->prev = current;
		list->tail = node;
	} else {
		dll_node_t *current = list->head;
		unsigned int i = 0;

		while (i < n - 1 && current->next) {
			current = current->next;
			i++;
		}

		node->next = current->next;
		node->prev = current;
		current->next->prev = node;
		current->next = node;
	}

	list->size++;
}

dll_node_t*
dll_remove_nth_node(doubly_linked_list_t *list, unsigned int n)
{
	if (!list || !list->head)
		return NULL;

	if (n == 0) {
		dll_node_t *current = list->head;

		if (list->head->next)
			list->head->next->prev = NULL;

		list->head = list->head->next;
		list->size--;

		if (list->size == 0)
			list->tail = NULL;

		return current;
	}

	if (n >= list->size - 1) {
		dll_node_t *current = list->tail;

		if (list->tail->prev)
			list->tail->prev->next = NULL;

		list->tail = list->tail->prev;
		list->size--;

		if (list->size == 0)
			list->head = NULL;

		return current;
	}

	dll_node_t *current = list->head;
	unsigned int i = 0;

	while (i < n) {
		current = current->next;
		i++;
	}

	current->prev->next = current->next;
	current->next->prev = current->prev;
	list->size--;

	return current;
}

void
dll_free(doubly_linked_list_t **pp_list)
{
	if (!(*pp_list))
		return;
	if (!(*pp_list)->head) {
		free(*pp_list);
		(*pp_list) = NULL;
		return;
	}

	dll_node_t *current = (*pp_list)->head;

	while (current) {
		dll_node_t *next = current->next;

		if (current->data)
			free(current->data);
		free(current);
		current = next;
	}
	free(*pp_list);
	(*pp_list) = NULL;
}

dll_node_t *
dll_get_node(doubly_linked_list_t *list, void *key)
{
	if (list->head) {
		dll_node_t *current = list->head;
		while (current) {
			void *data = ((info_t *)(current->data))->key;
			if (!compare_function_strings(data, key))
				return current;
			current = current->next;
		}
	}
	return NULL;
}
