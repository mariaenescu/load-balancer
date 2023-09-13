/* Copyright 2023 <Enescu Maria> */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "load_balancer.h"
#include "utils.h"

#define REQUEST_LENGTH 1024
#define KEY_LENGTH 128
#define VALUE_LENGTH 65536

void get_key_value(char* key, char* value, char* request) {
	int key_start = 0, value_start = 0;
	int key_finish = 0, value_finish = 0;
	int key_index = 0, value_index = 0;

	for (unsigned int i = 0; i < strlen(request); ++i) {
		if (request[i] == '"' && value_start != 1) {
			if (key_start == 0) {
				key_start = 1;
			} else if (key_finish == 0) {
				key_finish = 1;
			} else if (value_start == 0) {
				value_start = 1;
			}
		} else {
			if (key_start == 1 && key_finish == 0) {
				key[key_index++] = request[i];
			} else if (value_start == 1 && value_finish == 0) {
				value[value_index++] = request[i];
			}
		}
	}

	value[value_index - 1] = 0;
}

void get_key(char* key, char* request) {
	int key_start = 0, key_index = 0;

	for (unsigned int i = 0; i < strlen(request); ++i) {
		if (request[i] == '"') {
			key_start = 1;
		} else if (key_start == 1) {
			key[key_index++] = request[i];
		}
	}
}

void apply_requests(FILE* input_file) {
	char request[REQUEST_LENGTH] = {0};
	char key[KEY_LENGTH] = {0};
	char value[VALUE_LENGTH] = {0};
	load_balancer* main_server = init_load_balancer();

	while (fgets(request, REQUEST_LENGTH, input_file)) {
		request[strlen(request) - 1] = 0;
		if (!strncmp(request, "store", sizeof("store") - 1)) {
			get_key_value(key, value, request);

			int index_server = 0;
			loader_store(main_server, key, value, &index_server);
			printf("Stored %s on server %d.\n", value, index_server);

			memset(key, 0, sizeof(key));
			memset(value, 0, sizeof(value));
		} else if (!strncmp(request, "retrieve", sizeof("retrieve") - 1)) {
			get_key(key, request);

			int index_server = 0;
			char *retrieved_value = loader_retrieve(main_server,
											key, &index_server);
			if (retrieved_value) {
				printf("Retrieved %s from server %d.\n",
						retrieved_value, index_server);
			} else {
				printf("Key %s not present.\n", key);
			}

			memset(key, 0, sizeof(key));
		} else if (!strncmp(request, "add_server", sizeof("add_server") - 1)) {
			int server_id = atoi(request + sizeof("add_server"));

			loader_add_server(main_server, server_id);
		} else if (!strncmp(request, "remove_server",
					sizeof("remove_server") - 1)) {
			int server_id = atoi(request + sizeof("remove_server"));

			loader_remove_server(main_server, server_id);
		} else {
			DIE(1, "unknown function call");
		}
	}

	free_load_balancer(main_server);
}

int main(int argc, char* argv[]) {
	FILE *input;

	if (argc != 2) {
		printf("Usage:%s input_file \n", argv[0]);
		return -1;
	}

	input = fopen(argv[1], "rt");
	DIE(input == NULL, "missing input file");

	apply_requests(input);

	fclose(input);

	return 0;
}
