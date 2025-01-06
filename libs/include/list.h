#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "mem.h"

typedef struct ListNode {
    size_t id;
    void* data;
    struct ListNode* next;
} ListNode;

typedef struct List {
    size_t size;
    size_t ptr;
    void (*eventFree)(void* data);
    struct ListNode* root;
} List;

List* list_create();
bool list_on(List* list, void (*event)(ListNode* node));
bool list_del(List* list, void* data);
bool list_add(List* list, void* data);