/* Copyright 2023 <Enescu Maria> */
#include <stdlib.h>
#include <string.h>

#include "load_balancer.h"
#include "server.h"
#include "hashtable.h"
#include "utils.h"

#define MAX_SERVER 99999
#define REPLICA 100000

struct server_t {
    server_memory *data;
    unsigned int id;
    unsigned int hash;
};

struct load_balancer {
    server_t **servers; /* Array of servers */
    int size;
};

unsigned int hash_function_servers(void *a)
{
    unsigned int uint_a = *((unsigned int *)a);

    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;
    return uint_a;
}

unsigned int hash_function_key(void *a)
{
    unsigned char *puchar_a = (unsigned char *)a;
    unsigned int hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c;

    return hash;
}

load_balancer *init_load_balancer()
{
    /**
     * Function to create load balancer which distributes the work.
     * Allocate memory foar the load balancer.
    */
    load_balancer *root = malloc(sizeof(load_balancer));
    DIE(!root, "Error");

    /* I initialize each element in the struct. */
    root->servers = NULL;
    root->size = 0;

    /* This function returns the load balancer*/
    return root;
}

void loader_add_server(load_balancer *main, int server_id)
{
    DIE(!main, "Error: load balancer not initialized!");

    /** It reallocates memory for the main->servers array
     * allowing it to accommodate additional elements.
    */
	main->servers = realloc(main->servers, (main->size + 3) * sizeof(server_t*));
    DIE(!main->servers, "Error");

    for (int i = 0; i < 3; i ++)
	{
		unsigned int r_id = i * REPLICA + server_id;
        unsigned int hash = hash_function_servers(&r_id);

        server_t* server = malloc(sizeof(server_t));
        DIE(!server, "Error");

        server->data = init_server_memory();
        server->id = server_id;
        server->hash = hash;

        if (main->size == 0) {
            main->servers[0] = server;
        } else {
            int index = 0;
			int j = 0;
			for(j = 0; j < main->size; j++) {
				if (main->servers[j]->hash >= hash) {
					if (main->servers[j]->hash == hash) {
						if (main->servers[j]->id >
                            (unsigned int)server_id && j > 0) {
							j--;
						}
					}

					for(int k = main->size - 1; k >= j; k--)
						main->servers[k+1] = main->servers[k];

					main->servers[j] = server;
					index = (j == 0)? 1: j + 1;
					break;
				}
			}

			if (j == main->size){
				main->servers[j] = server;
				index = 0;
			}

            int count = 0;
            server_memory *data = main->servers[index]->data;
            info **more_info = (info**)server_retrieve_all(data, &count);

            if (count > 0) {
                for (int k = 0; k < count; ++k)
                {
                    info *t = more_info[k];
                    unsigned int h_key = hash_function_key(t->key);
                    /** Moving the information in the following cases:
                     * 
                     *  1. Insert the server at position 0
                     *  2. Insert the server at the last position
                     *  3. Insert the at any position other than 0 and
                     *  last position
                    */
                    if ((h_key > main->servers[index]->hash &&
                        (h_key <= server->hash || index == 1)) ||
                        (h_key < main->servers[index]->hash &&
                        h_key <= hash && index != 0)) {
                        server_store(server->data, t->key, t->value);
                        server_remove(main->servers[index]->data, t->key);
                    }
                }
                free(more_info);
            }
        }
		main->size++;
    }
}

void loader_remove_server(load_balancer *main, int server_id)
{
    DIE(!main, "Error: load balancer not initialized!");

	for (int ri = 0; ri < 3; ri++) {
        if (main->servers[0]->id == (unsigned int)server_id && main->size == 1)
        {
            main->size = 0;
            free_server_memory(main->servers[0]->data);
			free(main->servers[0]);
            return;
        }

		server_t *nod = NULL; /* For the next server in the array*/
		server_t *deleted = NULL; /* To store the reference*/

		for(int i = 0; i < main->size; i++) {
			if (main->servers[i]->id == (unsigned int)server_id) {
				deleted = main->servers[i];
				if (i == main->size - 1) {
					nod = main->servers[0];
				} else {
					nod = main->servers[i + 1];
				}

                /** All elements in the array starting from the position
                 * of the deleted server are shifted one position to the
                 * left to fill the gap left by the deleted server.
                */
				for(int k = i; k < main->size - 1; k++)
					main->servers[k] = main->servers[k + 1];
				break;
			}
		}

        int count = 0;
        info **more_info = (info**)server_retrieve_all(deleted->data, &count);

        if (count > 0) {
            for (int j = 0; j < count; j++) {
                info* t = more_info[j];
                server_store(nod->data, (char*)t->key, (char*)t->value);
            }
            free(more_info);
        }
        free_server_memory(deleted->data);
        free(deleted);

        /* Decrement the seize of the array. */
        main->size--;

		main->servers = realloc(main->servers, main->size * sizeof(server_t*));
        DIE(!main->servers, "Error");
    }
}

void loader_store(load_balancer *main, char *key, char *value, int *server_id)
{
    DIE(!main, "Error: load balancer not initialized!");
    if (!key || !value)
        return;

    unsigned int h_key = hash_function_key(key);

	int index = 0;
	for(int i = 0; i< main->size; i++) {
		if (main->servers[i]->hash >= h_key) {
			index = i;
			break;
		}
	}

    *server_id = main->servers[index]->id;
    server_store(main->servers[index]->data, key, value);
}

char *loader_retrieve(load_balancer *main, char *key, int *server_id)
{
    DIE(!main, "Error: load balancer not initialized!");
    if (key == NULL) {
        return NULL;
    }

    unsigned int h_key = hash_function_key(key);
	int index = 0;
	for(int i = 0; i< main->size; i++) {
		if (main->servers[i]->hash >= h_key) {
			index = i;
			break;
		}
	}
    *server_id = main->servers[index]->id;

    return server_retrieve(main->servers[index]->data, key);
}

void free_load_balancer(load_balancer *main)
{
    if (main != NULL) {
		for(int i = 0; i < main->size; i++) {
			free_server_memory(main->servers[i]->data);
			free(main->servers[i]);
		}
        free(main->servers);
        free(main);
        main = NULL;
    }
}
