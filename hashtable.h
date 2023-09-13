/* Copyright 2023 <Enescu Maria> */
#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include "LinkedList.h"

typedef struct info info;
struct info {
	void *key;
	void *value;
};

typedef struct hashtable_t hashtable_t;
struct hashtable_t {
	linked_list_t **buckets; /* Array de liste simplu-inlantuite. */
	/* Nr. total de noduri existente curent in toate bucket-urile. */
	unsigned int size;
	unsigned int hmax;/* Nr. de bucket-uri. */
	/* (Pointer la) Functie pentru a calcula valoarea hash asociata cheilor. */
	unsigned int (*hash_function)(void*);
    /* (Pointer la) Functie pentru a compara doua chei. */
	int (*compare_function)(void*, void*);
};

hashtable_t *ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		               int (*compare_function)(void*, void*));
int ht_has_key(hashtable_t *ht, void *key);
void* ht_get(hashtable_t *ht, void *key);
void ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	        void *value, unsigned int value_size);
void ht_remove_entry(hashtable_t *ht, void *key);
void ht_free(hashtable_t *ht);
unsigned int ht_get_size(hashtable_t *ht);
unsigned int ht_get_hmax(hashtable_t *ht);
unsigned int hash_function_int(void *a);
unsigned int hash_function_string(void *a);
int compare_function_strings(void *a, void *b);
int compare_function_ints(void *a, void *b);

info** ht_get_all(hashtable_t *ht, int *no_items);

#endif /* HASHTABLE_H_ */
