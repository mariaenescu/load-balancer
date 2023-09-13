/* Copyright 2023 <Enescu Maria> */
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"
#include "server.h"
#include "utils.h"

#define MAX_MEMORY 1024

struct server_memory {
	hashtable_t *memory;
};

server_memory *init_server_memory()
{
	server_memory* root = malloc(sizeof(server_memory));
	DIE(!root, "Error");

	root->memory = ht_create(MAX_MEMORY, hash_function_string,
							 compare_function_ints);
	return root;
}

void server_store(server_memory *server, char *key, char *value)
{
	unsigned int key_size = (strlen(key) + 1)*sizeof(char);
	unsigned int value_size = (strlen(value) + 1)*sizeof(char);

	key[strlen(key)] = '\0';
	value[strlen(value)] = '\0';

	ht_put(server->memory, key, key_size, value, value_size);
}

char *server_retrieve(server_memory *server, char *key)
{
	return ht_get(server->memory, key);
}

void server_remove(server_memory *server, char *key)
{
	ht_remove_entry(server->memory, key);
}

void free_server_memory(server_memory *server)
{
	ht_free(server->memory);
	free(server);
}

void **server_retrieve_all(server_memory *server, int *no_items)
{
	return (void**)ht_get_all(server->memory, no_items);
}
