/*
 * Copyright (c) 2024, <Diaconescu Stefania Clara 313CA>
 */

#include <stdio.h>
#include "server.h"
#include "lru_cache.h"
#include "hashtable.h"
#include "queue.h"
#include "utils.h"
#include "constants.h"

void
free_request_data(void *data)
{
	if (!data)
		return;

	request *inf = (request *)data;
	if (inf->doc_name)
		free(inf->doc_name);
	if (inf->doc_content)
		free(inf->doc_content);
}

response *
malloc_response()
{
	response *resp = malloc(sizeof(response));
	DIE(!resp, "malloc failed");

	resp->server_log = malloc(SERVER_LOG_LENGTH);
	DIE(!(resp->server_log), "malloc failed");
	resp->server_response = malloc(SERVER_RESPONSE_LENGTH);
	DIE(!(resp->server_response), "malloc failed");
	resp->server_id = 0;

	return resp;
}

/**
 * Functia realizeaza modificarile specifice comenzii "EDIT"
 */
static response *
server_edit_document(server *s, char *doc_name, char *doc_content)
{
	response *resp = malloc_response();
	void *evicted_key = NULL;
	void *doc = lru_cache_get(s->cache, doc_name);

	// verifica daca documentul se afla in cache
	if (!doc) {
		// daca nu este in cache, verifica daca documentul se afla in database
		dll_node_t *doc_db = dll_get_node(s->database, doc_name);

		// daca nu se afla nici in database, este adaugat in cache si database
		if (!doc_db) {
			lru_cache_put(s->cache, doc_name, doc_content, &evicted_key);

			info_t *node_data = create_order_data(doc_name, doc_content);
			dll_add_nth_node(s->database, s->database->size, node_data,
							 sizeof(info_t));
			free(node_data);

			if (!evicted_key) {
				sprintf(resp->server_log, LOG_MISS, doc_name);
			} else {
				sprintf(resp->server_log, LOG_EVICT, doc_name,
						(char *)((info_t *)(evicted_key))->key);
				key_val_free_function(evicted_key);
				free(evicted_key);
			}
			sprintf(resp->server_response, MSG_C, doc_name);
		// daca este in database, este adaugat in cache si modficat
		} else {
			lru_cache_put(s->cache, doc_name, doc_content, &evicted_key);
			void *doc_db_value = ((info_t *)(doc_db->data))->value;
			memcpy(doc_db_value, doc_content, DOC_CONTENT_LENGTH);

			if (!evicted_key) {
				sprintf(resp->server_log, LOG_MISS, doc_name);
			} else {
				sprintf(resp->server_log, LOG_EVICT, doc_name,
						(char *)((info_t *)(evicted_key))->key);
				key_val_free_function(evicted_key);
				free(evicted_key);
			}
			sprintf(resp->server_response, MSG_B, doc_name);
		}
	// daca se afla in cache, este modificat
	} else {
		int i = 0;
		dll_node_t *current = s->cache->order->head;

		// daca docmentul se afla deja in lista de ordine, este eliminat si
		// adaugat la inceputul acesteia
		while (current) {
			void *data = ((info_t *)(current->data))->key;
			if (!compare_function_strings(data, doc_name)) {
				dll_node_t *node = dll_remove_nth_node(s->cache->order, i);
				lru_cache_put(s->cache, doc_name, doc_content,
							  &evicted_key);
				key_val_free_function(node->data);
				free(node->data);
				free(node);
				break;
			}
			i++;
			current = current->next;
		}

		info_t *node_data = create_order_data(doc_name, doc_content);
		dll_add_nth_node(s->cache->order, 0, node_data, sizeof(info_t));
		free(node_data);

		doc = lru_cache_get(s->cache, doc_name);
		memcpy(doc, doc_content, DOC_CONTENT_LENGTH);
		dll_node_t *doc_db = dll_get_node(s->database, doc_name);
		void *doc_db_value = ((info_t *)(doc_db->data))->value;
		memcpy(doc_db_value, doc_content, DOC_CONTENT_LENGTH);
		sprintf(resp->server_log, LOG_HIT, doc_name);
		sprintf(resp->server_response, MSG_B, doc_name);
	}
	resp->server_id = s->id;

	return resp;
}

/**
 * Functia realizeaza modificarile specifice comenzii "GET"
 */
static response *
server_get_document(server *s, char *doc_name)
{
	response *resp = malloc_response();
	void *evicted_key = NULL;
	void *doc = lru_cache_get(s->cache, doc_name);

	// verifica daca documentul se afla in cache
	if (!doc) {
		// daca nu este in cache, verifica daca documentul se afla in database
		dll_node_t *doc_db = dll_get_node(s->database, doc_name);

		// daca nu se afla nici in database, este afisat mesaj de eroare
		if (!doc_db) {
			free(resp->server_response);
			resp->server_response = NULL;
			sprintf(resp->server_log, LOG_FAULT, doc_name);
		// daca este in database, este adaugat in cache
		} else {
			void *doc_db_value = ((info_t *)(doc_db->data))->value;
			lru_cache_put(s->cache, doc_name, doc_db_value, &evicted_key);
			strcpy(resp->server_response, doc_db_value);

			if (!evicted_key) {
				sprintf(resp->server_log, LOG_MISS, doc_name);
			} else {
				sprintf(resp->server_log, LOG_EVICT, doc_name,
						(char *)((info_t *)(evicted_key))->key);
				key_val_free_function(evicted_key);
				free(evicted_key);
			}
		}
	} else {
		int i = 0;
		dll_node_t *current = s->cache->order->head;

		// daca docmentul se afla deja in lista de ordine, este eliminat si
		// adaugat la inceputul acesteia
		while (current) {
			void *data = ((info_t *)(current->data))->key;
			if (!compare_function_strings(data, doc_name)) {
				dll_node_t * node = dll_remove_nth_node(s->cache->order, i);
				lru_cache_put(s->cache, doc_name, doc, &evicted_key);
				key_val_free_function(node->data);
				free(node->data);
				free(node);
				break;
			}
			i++;
			current = current->next;
		}

		info_t *node_data = create_order_data(doc_name, doc);
		dll_add_nth_node(s->cache->order, 0, node_data, sizeof(info_t));
		free(node_data);

		strcpy(resp->server_response, (char *)doc);
		sprintf(resp->server_log, LOG_HIT, doc_name);
		resp->server_id = s->id;
	}
	resp->server_id = s->id;

	return resp;
}

server *
init_server(unsigned int cache_size)
{
	server *serv = malloc(sizeof(server));
	DIE(!serv, "malloc failed");

	serv->nr_max_cache = cache_size;
	serv->id = 0;

	serv->cache = init_lru_cache(serv->nr_max_cache);
	serv->database = dll_create(sizeof(info_t));
	serv->tasks_queue = q_create(sizeof(request), TASK_QUEUE_SIZE);

	for (unsigned int i = 0; i <serv->tasks_queue->max_size; i++) {
		((request *)serv->tasks_queue->buff[i])->doc_name = NULL;
		((request *)serv->tasks_queue->buff[i])->doc_content = NULL;
	}

	return serv;
}

response *
server_handle_request(server *s, request *req)
{
	char *type = get_request_type_str(req->type);

	if (strcmp(type, EDIT_REQUEST) == 0) {
		// este adaugata comanda in coada
		request *req_copy = malloc(sizeof(request));
		DIE(!req_copy, "malloc failed");
		req_copy->doc_name = malloc(DOC_NAME_LENGTH);
		DIE(!req_copy->doc_name, "malloc failed");
		req_copy->doc_content = malloc(DOC_CONTENT_LENGTH);
		DIE(!req_copy->doc_content, "malloc failed");
		response *resp = malloc_response();

		strcpy(req_copy->doc_name, req->doc_name);
		strcpy(req_copy->doc_content, req->doc_content);
		req_copy->type = req->type;

		q_enqueue(s->tasks_queue, req_copy);

		sprintf(resp->server_response, MSG_A, "EDIT", req->doc_name);
		sprintf(resp->server_log, LOG_LAZY_EXEC, s->tasks_queue->size);
		resp->server_id = s->id;
		free(req_copy);
		return resp;
	} else if (strcmp(type, GET_REQUEST) == 0) {
		// se executa comenzile din coada
		while (q_is_empty(s->tasks_queue) == 0) {
			request *req_task = q_front(s->tasks_queue);
			response *resp = server_edit_document(s, req_task->doc_name,
												  req_task->doc_content);
			PRINT_RESPONSE(resp);
			int i = s->tasks_queue->read_idx;
			free_request_data((request*)s->tasks_queue->buff[i]);
			q_dequeue(s->tasks_queue);
		}
		response *resp = server_get_document(s, req->doc_name);
		return resp;
	}
	return NULL;
}

void
free_server(server **s)
{
	free_lru_cache(&((*s)->cache));

	while (q_is_empty((*s)->tasks_queue) == 0) {
		int i = (*s)->tasks_queue->read_idx;
		free_request_data((request*)(*s)->tasks_queue->buff[i]);
		q_dequeue((*s)->tasks_queue);
	}
	q_free((*s)->tasks_queue);

	if ((*s)->database) {
		if ((*s)->database->head) {
			dll_node_t *current = (*s)->database->head;
			while (current) {
				key_val_free_function(current->data);
				current = current->next;
			}
		}
		dll_free(&((*s)->database));
	}
	free((*s));
}
