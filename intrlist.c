/**
 * intrlist.c - Intrusive doubly-linked list.
 * Written by Guilherme Nemeth. Public Domain.
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

// Intrusive doubly-linked list, must always be the first member of a structure.
typedef struct list {
    size_t       count; // Only considered in the first node.
    struct list* prev;  // Points to the tail in case it's the first node
    struct list* next;  // Points to NULL in case it's the last node
} list_t;

typedef int (*list_comp_func_t)(const void*, const void*);

#define list_init(list) (list_init_(&list->list_node))

#define list_foreach(item, first)                                                                                      \
    for (typeof(first) item = first; item != NULL; item = (typeof(first))item->list_node.next)

#define list_foreachi(item, index, first)                                                                        \
    for (int index = 0; index == 0; index++)                                                                           \
        for (typeof(first) item = first; item != NULL; (item = (typeof(first))item->list_node.next), index++)

// Appends an item to the end of the list and returns the new head pointer
#define list_append(list, item) ((typeof(list))list_append_(&(list)->list_node, &(item)->list_node))

// Remove an item from the list and returns the new head pointer
#define list_remove(list, item) ((typeof(list))list_remove_(&(list)->list_node, &(item)->list_node))

// Sorts the list and returns the new head pointer
#define list_sort(list, f) ((typeof(list))list_sort_(&(list)->list_node, (list_comp_func_t)(f), false))

// Sorts the list in reverse and returns the new head pointer
#define list_rsort(list, f) ((typeof(list))list_sort_(&(list)->list_node, (list_comp_func_t)(f), true))

// Returns the list element count, the passed node MUST be the head of the list
#define list_count(list) (((list) == NULL) ? 0 : list_count_(&(list)->list_node))

static inline void list_init_(list_t* list) {
    list->count = 1;
    list->prev = list;
    list->next = NULL;
}

static inline size_t list_count_(list_t* list) {
    if (list == NULL) {
        return 0;
    } else {
        return list->count;
    }
}

static inline list_t* list_append_(list_t* list, list_t* item) {
    if (!list) {
        list_init_(item);
        return item;
    }

    list_t* last = list->prev;
    list->prev = item;

    last->next = item;
    item->prev = last;

    item->next = NULL;

    list->count++;
    return list;
}

static inline list_t* list_remove_(list_t* list, list_t* item) {
    if (item == list) {
        // first item
        list_t* new_first = list->next;
        if (!new_first) {
            return NULL;
        }

        new_first->prev = list->prev;
        new_first->count = list->count - 1;
        return new_first;
    } else {
        item->prev->next = item->next;
        if (item->next) {
            item->next->prev = item->prev;
        }

        // is it the last item?
        if (item == list->prev) {
            list->prev = item->prev;
        }

        list->count--;
        return list;
    }
}

static inline list_t* list_sort_(list_t* list, list_comp_func_t compfunc, bool rev) {
    list_t** sortbuf = malloc(sizeof(list_t*) * list->count);
    size_t   i = 0;
    for (list_t* item = list; item != NULL; item = item->next) {
        sortbuf[i++] = item;
    }

    qsort(sortbuf, list->count, sizeof(list_t*), compfunc);

    int start = 0;
    int end = list->count - 1;
    int inc = 1;
    if (rev) {
        start = list->count - 1;
        end = 0;
        inc = -1;
    }

    list_t* newlist = sortbuf[start];
    newlist->count = 1;
    newlist->next = NULL;
    newlist->prev = newlist;

    for (int i = start + inc; i != (end + inc); i += inc) {
        newlist = list_append_(newlist, sortbuf[i]);
    }
    sortbuf[end]->next = NULL;

    free(sortbuf);
    return newlist;
}

#ifdef intrlist_unittest

#include <stdarg.h>
#include "test_common.h"

typedef struct data {
    struct list list_node;
    int         x;
} data_t;

data_t* makeData(int x) {
    data_t* d = calloc(1, sizeof(data_t));
    d->x = x;
    return d;
}

int sortData(const data_t* const* a, const data_t* const* b) { return (*a)->x - (*b)->x; }

static void printList(data_t* data_list) {
    list_foreach (entry, data_list) { printf("%d ", entry->x); }
    printf("\n");
}

static bool listEqual(data_t* a, data_t* b) {
    if (a->list_node.count != b->list_node.count) {
        return false;
    }

    data_t* it_b = b;
    list_foreach (it_a, a) {
        if (!it_b) {
            return false;
        }

        if (it_a->x != it_b->x) {
            return false;
        }
        it_b = (data_t*)it_b->list_node.next;
    }
    return true;
}

static data_t* makeList(int n, ...) {
    data_t* list = NULL;
    va_list args;
    va_start(args, n);
    for (int i = 0; i < n; i++) {
        list = list_append(list, va_arg(args, data_t*));
    }
    va_end(args);
    return list;
}

static bool test_listAppendRemove() {
    data_t* data_list = NULL;

    data_list = list_append(data_list, makeData(1));
    printList(data_list);
    TEST_ASSERT(listEqual(data_list, makeList(1, makeData(1))), "list should be equal");

    data_list = list_append(data_list, makeData(2));
    printList(data_list);
    TEST_ASSERT(listEqual(data_list, makeList(2, makeData(1), makeData(2))), "list should be equal");

    data_t* node = (data_t*)data_list->list_node.next;
    data_list = list_remove(data_list, node);
    printList(data_list);
    TEST_ASSERT(listEqual(data_list, makeList(1, makeData(1))), "list should be equal");

    data_list = list_append(data_list, makeData(3));
    printList(data_list);
    TEST_ASSERT(listEqual(data_list, makeList(2, makeData(1), makeData(3))), "list should be equal");

    data_list = list_append(data_list, makeData(2));
    printList(data_list);
    TEST_ASSERT(listEqual(data_list, makeList(3, makeData(1), makeData(3), makeData(2))), "list should be equal");

    node = (data_t*)data_list->list_node.next;
    data_list = list_remove(data_list, node);
    printList(data_list);
    TEST_ASSERT(listEqual(data_list, makeList(2, makeData(1), makeData(2))), "list should be equal");

    data_list = list_append(data_list, makeData(5));
    printList(data_list);
    TEST_ASSERT(listEqual(data_list, makeList(3, makeData(1), makeData(2), makeData(5))), "list should be equal");

    node = data_list;
    data_list = list_remove(data_list, node);
    printList(data_list);
    TEST_ASSERT(listEqual(data_list, makeList(2, makeData(2), makeData(5))), "list should be equal");

    data_list = list_append(data_list, makeData(7));
    printList(data_list);
    TEST_ASSERT(listEqual(data_list, makeList(3, makeData(2), makeData(5), makeData(7))), "list should be equal");

    return true;
}

static bool test_listSort() {
    data_t* data_list = makeList(5, makeData(1), makeData(5), makeData(4), makeData(7), makeData(2));
    printList(data_list);

    data_list = list_sort(data_list, sortData);
    printList(data_list);
    TEST_ASSERT(listEqual(data_list, makeList(5, makeData(1), makeData(2), makeData(4), makeData(5), makeData(7))),
                "list should be sorted");

    return true;
}

static bool test_listSortReverse() {
    data_t* data_list = makeList(5, makeData(1), makeData(5), makeData(4), makeData(7), makeData(2));
    printList(data_list);

    data_list = list_rsort(data_list, sortData);
    printList(data_list);
    TEST_ASSERT(listEqual(data_list, makeList(5, makeData(7), makeData(5), makeData(4), makeData(2), makeData(1))),
                "list should be sorted in reverse");

    return true;
}

#endif
