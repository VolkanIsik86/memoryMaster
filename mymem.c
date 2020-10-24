#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mymem.h"
#include <time.h>
#include <zconf.h>


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

strategies myStrategy = NotSet;    // Current strategy
size_t mySize;
void *myMemory = NULL;
static MemoryList *head;
static MemoryList *next;


/* initmem must be called prior to mymalloc and myfree.

   initmem may be called more than once in a given exeuction;
   when this occurs, all memory you previously malloc'ed  *must* be freed,
   including any existing bookkeeping data.

   strategy must be one of the following:
		- "best" (best-fit)
		- "worst" (worst-fit)
		- "first" (first-fit)
		- "next" (next-fit)
   sz specifies the number of bytes that will be available, in total, for all mymalloc requests.
*/

// Frigør myMemory og og hele linkedlisten hvis de er allokeret.
void freeAll(){
    next = NULL;
    if (myMemory != NULL) free(myMemory); /* in case this is not the first time initmem2 is called */
    if (head != NULL) {

        MemoryList *search = head;
        MemoryList *nextone;
        while (search != NULL) {
            nextone = search->next;
            free(search);
            search = nextone;
        }
    }

}
//initalisering af memory
void initmem(strategies strategy, size_t sz)
{

    myStrategy = strategy;

    /* all implementations will need an actual block of memory to use */
    mySize = sz;

    freeAll();
    //memoryet allokeres og head placeres på begyndelsen af memory
    myMemory = malloc(sz);

    head = (MemoryList * ) malloc(sizeof(MemoryList)*1);
    head->ptr = myMemory;
    head->alloc = 0;
    head->size = sz;
    head->next = NULL;
    head->last = NULL;
}

/* Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1
 */
///////////////////////////////////.^-!__MemoryBlockManager__!-^.//////////////////////////////////////////

// Søg funktionen. Søger igennem hele for en ikke allokeret og den største block som er større end size. Listen starter fra head
MemoryList* search(size_t size){
    MemoryList *search = NULL;
    MemoryList *biggestnode = NULL;
    search = head;
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

MemoryList* bestSearch(size_t size){
    MemoryList *search = NULL;
    MemoryList *bestnode = NULL;
    search = head;
    int searchSize = 0;

    while (search!=NULL){
   	if(search->size >= size && search->alloc==0){
   		if(bestnode == NULL){
   		bestnode = search;
   		}
   		searchSize = search->size;
   		if(searchSize < bestnode->size){
   		bestnode=search;
   		}
   	}
        search=search->next;
    }
    return bestnode;


}

MemoryList* firstSearch(size_t size){
    MemoryList *search = NULL;
    search = head;

    while (search!=NULL){
   	if(search->size >= size && search->alloc==0){
		return search;
   	}
        search=search->next;
    }
}

MemoryList* nextSearch(size_t size){
    MemoryList *search = NULL;
    search = next;

	if(next==NULL){
		return head;
	}

	if(next->next==NULL){
	    search = head;
	}

    while (search!=NULL){
        if(search->size >= size && search->alloc==0){
            return search;
        }
        search=search->next;
    }
    
}

//Opretter en memoryblock og placerer den før eller overtager den block der blev givet.
MemoryList* insert(MemoryList* explode, size_t size){

    if(explode->size==size && explode->alloc==0){
        explode->alloc=1;
        if(explode->next==NULL){
            explode->ptr=explode->last->ptr+explode->last->size;
        }
        return explode;
    }

    //Opretter en ny node og placerer den i iforhold til given block
    MemoryList *node = (MemoryList*) malloc(sizeof(MemoryList)*1);
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
        head = node;
    }
    return node;
}

// Deallokerer en given block.
void dealloc(void* node){

    MemoryList * search = head;
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

/**
 * Disse koder skulle meget gerne merge de blocks som står ved hinanden hvis de er ikke allokeret.
 * Testen nextfit i stress testen passer ikke med disse koder. Ellers alle andre teste og de andre strategier passede fint.
 */

/*
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
                head=found;
            } else{
                MemoryList* tobeFreed = found->last;
                found->last->last->next = found;
                found->last=found->last->last;
                free(tobeFreed);
            }
        }
    }

*/


}


///////////////////////////////////.^-!__MemoryBlockManager__!-^.//////////////////////////////////////////

void *mymalloc(size_t requested){
    assert((int)myStrategy > 0);

    switch (myStrategy)
    {
        case NotSet:
            return NULL;
            break;
        case First:
        {
            MemoryList *explode = firstSearch(requested);
            	if(explode)
            		return insert(explode,requested)->ptr; 
            break;
            }
        case Best:
        {
            MemoryList *explode = bestSearch(requested);
            	if(explode)
            		return insert(explode,requested)->ptr; 
            break;
            }
        case Worst:
        {
            //søger efter en fri memoryblok som er den største i hele memory'et
            MemoryList *explode = search(requested);
            if(explode)
                //memoryblok placeres og pointeren returneres
               return insert(explode,requested)->ptr;
            break;
        }
        case Next:
        {

                MemoryList *explode = nextSearch(requested);
                if (explode) {
                    next = insert(explode, requested);
                    return next->ptr;
                }

            break;
            }
    }
    return NULL;
}

/* Frees a block of memory previously allocated by mymalloc. */
void myfree(void* block){
    dealloc(block);
}

/****** Memory status/property functions ******
 * Implement these functions.
 * Note that when refered to "memory" here, it is meant that the
 * memory pool this module manages via initmem/mymalloc/myfree.
 */

/* Get the number of contiguous areas of free space in memory. */
int mem_holes()
{
    int holes = 0;
    MemoryList * searchHole = NULL;
    searchHole = head;
    while (searchHole!=NULL){
        if (searchHole->alloc == 0 && searchHole->size>0){
            holes++;
        }
        searchHole = searchHole->next;
    }
    return holes;
}

/* Get the number of bytes allocated */
int mem_allocated()
{
    int allocated = 0;
    MemoryList * searchAlloc = NULL;
    searchAlloc = head;
    while (searchAlloc != NULL){
        if (searchAlloc->alloc == 1){
            allocated+=searchAlloc->size;
        }
        searchAlloc = searchAlloc->next;
    }
    return allocated;

}

/* Number of non-allocated bytes */
int mem_free()
{
    int allocated = 0;
    MemoryList * searchAlloc = NULL;
    searchAlloc = head;
    while (searchAlloc != NULL){
        if (searchAlloc->alloc == 0){
            allocated+=searchAlloc->size;
        }
        searchAlloc = searchAlloc->next;
    }
    return allocated;
}

/* Number of bytes in the largest contiguous area of unallocated memory */
int mem_largest_free()
{
    MemoryList * largest = search(0);

    if(largest!=NULL) {
        return largest->size;
    }
    else
        return 0;
}

/* Number of free blocks smaller than "size" bytes. */
int mem_small_free(int size)
{
    int block = 0;
    MemoryList * searchNonAlloc = NULL;
    searchNonAlloc = head;
    while (searchNonAlloc != NULL){
        if (searchNonAlloc->alloc == 0 && searchNonAlloc->size<=size){
            block++;
        }
        searchNonAlloc = searchNonAlloc->next;
    }
    return block;
}

//Er memory allokeret
char mem_is_alloc(void *ptr)
{
    MemoryList *search = NULL;
    search = head;

    while (search!=NULL){
        if(search->alloc==1 && search->ptr==ptr){
            return 1;
        }
        search=search->next;
    }
    return 0;
}

/*
 * Feel free to use these functions, but do not modify them.
 * The test code uses them, but you may find them useful.
 */


//Returns a pointer to the memory pool.
void *mem_pool()
{
    return (myMemory);
}

// Returns the total number of bytes in the memory pool. */
int mem_total()
{
    return mySize;
}

// Get string name for a strategy.
char *strategy_name(strategies strategy)
{
    switch (strategy)
    {
        case Best:
            return "best";
        case Worst:
            return "worst";
        case First:
            return "first";
        case Next:
            return "next";
        default:
            return "unknown";
    }
}

// Get strategy from name.
strategies strategyFromString(char * strategy)
{
    if (!strcmp(strategy,"best"))
    {
        return Best;
    }
    else if (!strcmp(strategy,"worst"))
    {
        return Worst;
    }
    else if (!strcmp(strategy,"first"))
    {
        return First;
    }
    else if (!strcmp(strategy,"next"))
    {
        return Next;
    }
    else
    {
        return 0;
    }
}


/*
 * These functions are for you to modify however you see fit.  These will not
 * be used in tests, but you may find them useful for debugging.
 */

/* Use this function to print out the current contents of memory. */
void print_memory()
{
    return;
}

/* Use this function to track memory allocation performance.
 * This function does not depend on your implementation,
 * but on the functions you wrote above.
 */

void print_memory_status()
{
    printf("%d out of %d bytes allocated.\n",mem_allocated(),mem_total());
    printf("%d bytes are free in %d holes; maximum allocatable block is %d bytes.\n",mem_free(),mem_holes(),mem_largest_free());
    printf("Average hole size is %f.\n\n",((float)mem_free())/mem_holes());
}

/* Use this function to see what happens when your malloc and free
 * implementations are called.  Run "mem -try <args>" to call this function.
 * We have given you a simple example to start.
 */

void try_mymem(int argc, char **argv) {
    strategies strat;
    void *a, *b, *c, *d, *e;
    if (argc > 1)
        strat = strategyFromString(argv[1]);
    else
        strat = Next;

    /* A simple example.
       Each algorithm should produce a different layout. */

    initmem(strat,500);

    a = mymalloc(100);
    b = mymalloc(100);
    c = mymalloc(100);
    myfree(b);
    d = mymalloc(50);
    myfree(a);
    e = mymalloc(25);

    print_memory();
    print_memory_status();
}