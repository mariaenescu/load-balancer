/* Copyright 2023 <Enescu Maria> */
#include "LinkedList.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

linked_list_t *ll_create()
{
    linked_list_t *ll;

    ll = malloc(sizeof(linked_list_t));

    ll->head = NULL;
    ll->size = 0;

    return ll;
}

void ll_add_nth_node(linked_list_t* list, unsigned int n, void* new_data)
{
    ll_node_t *prev, *curr;
    ll_node_t *new_node;

    if (!list) {
        return;
    }

    if (n > list->size) {
        n = list->size;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    new_node = malloc(sizeof(ll_node_t));
    new_node->data = new_data;

    new_node->next = curr;
    if (prev == NULL) {
        list->head = new_node;
    } else {
        prev->next = new_node;
    }

    list->size++;
}


ll_node_t *ll_remove_nth_node(linked_list_t* list, unsigned int n)
{
    ll_node_t *prev, *curr;

    if (!list || !list->head) {
        return NULL;
    }

    if (n > list->size - 1) {
        n = list->size - 1;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    if (prev == NULL) {
        list->head = curr->next;
    } else {
        prev->next = curr->next;
    }

    list->size--;

    return curr;
}

unsigned int ll_get_size(linked_list_t* list)
{
     if (!list) {
        return -1;
    }

    return list->size;
}

void ll_free(linked_list_t* pp_list)
{
    ll_node_t *currNode;

    if (!pp_list) {
        return;
    }

    while (pp_list->size > 0) {
        currNode = ll_remove_nth_node(pp_list, 0);
		if (currNode != NULL) {
			if (currNode->data != NULL) {
				free(currNode->data);
			}
			free(currNode);
			currNode = NULL;
		}
    }

    free(pp_list);
    pp_list = NULL;
}

void ll_print_int(linked_list_t* list)
{
    ll_node_t *curr;

    if (!list) {
        return;
    }

    curr = list->head;
    while (curr != NULL) {
        printf("%d ", *((int*)curr->data));
        curr = curr->next;
    }

    printf("\n");
}

void ll_print_string(linked_list_t* list)
{
    ll_node_t *curr;

    if (!list) {
        return;
    }

    curr = list->head;
    while (curr != NULL) {
        printf("%s ", (char*)curr->data);
        curr = curr->next;
    }

    printf("\n");
}
