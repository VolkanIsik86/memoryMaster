//
// Created by volkan on 14/10/2020.
//
#pragma once
#ifndef memoryMaster_MEMORYNODEMANAGER_H
#define memoryMaster_MEMORYNODEMANAGER_H

#endif //memoryMaster_MEMORYNODEMANAGER_H

#include <stdio.h>
#include <stdlib.h>

struct memoryList
{
    // doubly-linked list
    struct memoryList *last;
    struct memoryList *next;

    int size;            // How many bytes in this block?
    char alloc;          // 1 if this block is allocated,
    // 0 if this block is free.
    void *ptr;           // location of block in memory pool.
};
typedef struct memoryList MemoryList;


MemoryList* search(MemoryList *Head,size_t size){
    MemoryList *search = NULL;
    MemoryList *biggestnode = NULL;
    search = Head;
    int worstSize = 0;

    while (search!=NULL){
        if(search->size>=size && search->alloc==0){
            if(search->size > worstSize) {
                biggestnode = search;
                worstSize = search->size;
            }
        }
        search=search->next;
    }
    if(biggestnode != NULL)
    return biggestnode;
    else
        return NULL;


}

MemoryList* insert(MemoryList** head, MemoryList* explode, size_t size,MemoryList* lastnode){

    if(explode->size==size){
        explode->alloc=1;

        if(explode->next==NULL){
            explode->ptr=explode->last->ptr+lastnode->size;
        }
        return explode;
    }


    MemoryList *node = (MemoryList*) malloc(sizeof(MemoryList));
    node->size=size;
    node->alloc=1;
    node->ptr=(lastnode->ptr+lastnode->size);
    explode->size-=size;

    if(explode->last != NULL){
        explode->last->next=node;
        node->last=explode->last;
        node->next=explode;
        explode->last=node;
    } else{
        node->last=NULL;
        node->next=explode;
        explode->last=node;
        *head = node;
    }
    return node;
}

MemoryList* delete(MemoryList **Head, MemoryList* node){
    if(node->last != NULL){
        node->last->next=node->next;
    }else
        *Head = node->next;

    if(node->next != NULL){
        node->next->last=node->last;
    } else
        node->last=NULL;

    free(node);

    return *Head;

}

void freeAll(MemoryList *Head){
    MemoryList * search = Head;
    while (search!=NULL){
        search=search->next;
        free(search->last);
    }
    free(search);
}

void dealloc(MemoryList **Head, MemoryList* node){

    MemoryList * search = *Head;
    MemoryList * found = NULL;
    while (search!=NULL){
        if(search->ptr == node && search->alloc==1){
            found=search;
        }
        search=search->next;
    }





    found->alloc=0;

    if(found->next != NULL){
        if(found->next->alloc == 0){
            found->size+=found->next->size;
            if(found->next->next == NULL){
                free(found->next);
                found->next=NULL;
            }else{
                MemoryList* tobeFreed = found->next;
                found->next->next->last=found;
                found->next=found->next->next;
                free(tobeFreed);
            }
        }
    }
    if(found->last != NULL){
        if(found->last->alloc == 0){
            found->size+=found->last->size;
            if(found->last->last == NULL){
                free(found->last);
                found->last=NULL;
                *Head=found;
            } else{
                MemoryList* tobeFreed = found->last;
                found->last->last->next = found;
                found->last=found->last->last;
                free(tobeFreed);
            }
        }
    }
}



//int main() {
//    MemoryList *head = (MemoryList*) malloc(sizeof(MemoryList) * 1);
//    head->key=3;
//    head->last=NULL;
//    head->next=NULL;
//    MemoryList *a =(MemoryList*) malloc(sizeof(MemoryList) * 1);
//    a->key=5;
//    a->last=NULL;
//    a->next=NULL;
//    head = insert(head,a);
//    MemoryList *b =(MemoryList*) malloc(sizeof(MemoryList) * 1);
//    b->key=7;
//    b->last=NULL;
//    b->next=NULL;
//    head = insert(head,b);
//    printf("%d\n",search(head,7)->key);
//    head = delete(head,a);
//    printf("%d\n",search(head,5)->key);
//    return 0;
//}