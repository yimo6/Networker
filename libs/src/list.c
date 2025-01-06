#include "../include/list.h"

int last = 0;

ListNode* list_node_create(size_t id, void* data){
    ListNode* node = (ListNode *)mem_malloc(sizeof(ListNode), NULL);
    node -> id = id;
    node -> data = data;
    node -> next = NULL;
    return node;
}

List* list_create(){
    List* list = (List *)mem_malloc(sizeof(List), NULL);
    list -> size = 0;
    list -> ptr = 0;
    list -> eventFree = NULL;
    list -> root = list_node_create(0, NULL);
    return list;
}

bool list_on(List* list, void (*event)(ListNode* node)){
    ListNode* node = list -> root -> next;
    int ptr = 0;
    while(node != NULL){
        if(event != NULL) (event)(node);
        ptr++;
        node = node -> next;
    }
    if(last != ptr){
        last = ptr;
    }
    return true;
}

bool list_del(List* list, void* data){
    ListNode* node = list -> root -> next, *last = list -> root;
    while(node != NULL){
        if(node -> data == data){
            last -> next = node -> next;
            if(list -> eventFree != NULL){
                (list -> eventFree)(node -> data);
            }
            list -> size--;
            mem_free(node, NULL); //只释放区块本身,data需要用event释放
            return true;
        }
        last = node;
        node = node -> next;
    }
    return false;
}

bool list_add(List* list, void* data){
    ListNode* node = list -> root;
    ListNode* new = list_node_create(++list -> size, data);
    if(list -> size == 0){
        list -> root = new;
        return true;
    }
    while(node -> next != NULL){
        node = node -> next;
    }
    node -> next = new;
    return true;
}
