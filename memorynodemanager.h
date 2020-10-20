//
// Created by volkan on 14/10/2020.
//
#pragma once
#ifndef memoryMaster_MEMORYNODEMANAGER_H
#define memoryMaster_MEMORYNODEMANAGER_H

#endif //memoryMaster_MEMORYNODEMANAGER_H

#include <stdio.h>
#include <stdlib.h>

/* The main structure for implementing memory allocation.
 * You may change this to fit your implementation.
 */
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
// Søg funktionen. Søger igennem hele for en ikke allokeret og den største block som er større end size. Listen starter fra head
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
//Opretter en memoryblock og placerer den før eller overtager den block der blev givet.
MemoryList* insert(MemoryList** head, MemoryList* explode, size_t size){
    //overtager blokken
    if(explode->size==size){
        explode->alloc=1;
        if(explode->next==NULL){
            explode->ptr=explode->last->ptr+explode->last->size;
        }
        return explode;
    }

    //Opretter en ny node og placerer den i iforhold til given block
    MemoryList *node = (MemoryList*) malloc(sizeof(MemoryList));
    node->size=size;
    node->alloc=1;
    if(explode->last!=NULL)
        node->ptr=(explode->last->ptr+explode->last->size);
    else
        node->ptr=explode->ptr;
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

// Deallokerer en given block.
void dealloc(MemoryList **Head, MemoryList* node){
    //Finder den node der skal deallokeres.
    MemoryList * search = *Head;
    MemoryList * found = NULL;
    while (search!=NULL){
        if(search->ptr == node && search->alloc==1){
            found=search;
        }
        search=search->next;
    }
    /** Deallokerer og frigør given node hvis der er en allerede deallokeret
     *  node ved siden af given node så vil den sættessammen.
     */

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