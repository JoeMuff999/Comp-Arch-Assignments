/*
    This is a simulated dynamic memory allocator which utilizes an explicit free list.
    The explicit free list is structured like a doubly-linked list, following the LIFO discipline,
    with each free block being structured as such ("[]" = one word or 4 bytes)::
        [HEADER] <- highest 29 bits are size, lowest 3 are allocate bits 
        [POINTER_TO_PREV] <-pointer to the previous free block in the list. NOTE:: for HEAD this will be 0x22222222
        [POINTER_TO_NEXT] <-pointer to the next free block in the list. NOTE:: for TAIL this will be 0xffffffff
        []*SIZE/4 - 8<- empty payload of size GET_SIZE(HEADER) - 8 (Header and Footer not included in size!)
        [FOOTER] <- mimics the Header, used as a boundary tag for coalescing
    explicit blocks are the same structure, expect that the pointers are replaced with whatever values the caller of mm_malloc chooses to give them

    Blocks will always be inserted at the front of the list (ie: where HEAD is) s.t. mm_free() will always be of constant time. 

    Allocation is based on the best fit discipline, prioritizing space utilization over throughput.

    Coalescing is only done during reallocation.


 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"
#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1 << 8)
#define ALIGNMENT 8

#define MAX(x, y) ((x) > (y) ? (x) : (y))

/* combines size and alloc into 32 bits (word/4 bytes)
 * can be used for creating block headers, where the lowest 3 bits are reserved for alloc, and the highest 29 are for representing the size of the chunk */
#define PACK(size, alloc) ((size) | (alloc))

#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

#define HDRP(bp) ((char *)(bp)-WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)))
#define NEXT(pp) ((char *)(pp) + WSIZE)
#define PREV(pp) ((char *)(pp))

#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp)-WSIZE)) + DSIZE)
#define PREV_BLKP(bp) ((char *)(bp)-GET_SIZE(((char *)(bp)-DSIZE)) - DSIZE)

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

void *HEAD = NULL; //points to payload_pointer of first block in linked list
void *TAIL = NULL; //points to pp of last block in linked list, tail.next == 0xffffffff
static void *allocate_block(size_t words);
static void *find_fit(size_t asize);
static void coalesce(void *pp);
int coalesce_free_list();

/* BELOW ARE ALL THE GLOBALS USED FOR HEAP CHECKING */
int mm_check();
int expected_free_list_size;

/*
    Resets the doubly linked free list (HEAD == TAIL == NULL)
    initializes a 1 word padding in order to maintain 8 byte alignment
    why? --> (each "[]" is 1 word) :: [PADDING][first_header][first_payload]  <-- necessary so that first_payload is 8 byte aligned
*/
int mm_init(void)
{
    HEAD = NULL;
    TAIL = NULL;
    void * caution;
    if((caution = mem_sbrk(WSIZE)) == (void*)-1) //1 word padding
        return -1;
    return 0;
}
/*
    Allocates a block of memory based the given size which is aligned to 8 bytes
    Follows one of two paths:
        1. If a block is found based on the find_fit() function (best_fit), allocate this block
        2. If a block is not found based on find_fit(), allocate a new block of memory of Max(aligned_size, CHUNKSIZE) 
    returns the pointer to the paylodd of the allocated block (payload_pointer = header_pointer+WSIZE)
*/
void *mm_malloc(size_t size)
{
    if (size == 0)
        return NULL;

    char *payload_pointer;
    size_t extendsize;

    size_t newsize = ALIGN(size + SIZE_T_SIZE);
    //searches for a free block that satisfies the size requirements. if not found, will move past
    if (HEAD != NULL && ((payload_pointer = find_fit(newsize)) != NULL))
    {

        //allocate block
        //three cases, either its head or its tail or its in the middle
        size = GET_SIZE(HDRP(payload_pointer));
        PUT(HDRP(payload_pointer), PACK(size, 1));
        PUT(FTRP(payload_pointer), PACK(size, 1));
        if (payload_pointer == HEAD) //if head
        {
            //assert(GET(PREV(payload_pointer)) == 0x22222222);

            if (GET(NEXT(payload_pointer)) == 0xffffffff)
            {
                //no next
                HEAD = NULL;
                TAIL = NULL;
            }
            else
            {
                //has a next
                HEAD = GET(NEXT(payload_pointer));
                PUT(PREV(HEAD), 0x22222222);
            }
        }
        else if (payload_pointer == TAIL) //if tail
        {
            //assert(GET(NEXT(payload_pointer)) == 0xffffffff);

            if (GET(PREV(payload_pointer)) == 0x22222222)
            {
                //no previous
                HEAD = NULL;
                TAIL = NULL;
            }
            else
            {
                //has a prev
                TAIL = GET(PREV(payload_pointer));
                PUT(NEXT(TAIL), 0xffffffff);
            }
        }
        else //general case
        {

            char *ptr_to_prev = GET(PREV(payload_pointer));
            char *ptr_to_next = GET(NEXT(payload_pointer));
            PUT(NEXT(ptr_to_prev), ptr_to_next);
            PUT(PREV(ptr_to_next), ptr_to_prev);
        }
        return payload_pointer;
    }
    //standard block allocation utilizing mem_sbrk. see allocate_block for more details
    extendsize = MAX(newsize, CHUNKSIZE);
    if ((payload_pointer = allocate_block(extendsize / WSIZE)) == NULL)
    {
        return NULL;
    }
    return payload_pointer;
}
/*
    A bit confusing. 
    Coalescing will only be done if the block to be coalesced is at least 2 blocks away from HEAD and TAIL. 
    This is done to prevent convoluting the code for minimal performance increase
    Returns nothing. 
*/
static void coalesce(void *pp)
{
    void *prev_block_ptr = PREV_BLKP(pp); 
    void *next_block_ptr = NEXT_BLKP(pp);

    if (prev_block_ptr == HEAD || prev_block_ptr == TAIL || next_block_ptr == HEAD || next_block_ptr == TAIL)
        return;
    
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(pp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(pp)));
    //use these variables to make sure next or prev are not out of bounds (necessary if free list is explicit)
    int next_exists = 1; 
    if (next_block_ptr >= mem_heap_hi())
        next_exists = 0;
    int prev_exists = 1;
    if (prev_block_ptr <= mem_heap_lo())
        prev_exists = 0;

    size_t size = GET_SIZE(HDRP(pp));
    // in the case that the physically next block is not allocated, coalesce
    if (prev_alloc && !next_alloc && next_exists)
    {
        //break nexts link in free list chain
        //set next of prev to next of curr
        PUT(NEXT(GET(PREV(next_block_ptr))), GET(NEXT(next_block_ptr)));
        //set prev of next to prev of curr
        PUT(PREV(GET(NEXT(next_block_ptr))), GET(PREV(next_block_ptr)));
        //adjust size to combine both + 8, the + 8 is from the header of next and the footer of pp being wiped out (remember, size doesn't include headers or footers)
        size += GET_SIZE(HDRP(next_block_ptr)) + 8;
        PUT(HDRP(pp), PACK(size, 0));
        PUT(FTRP(pp), PACK(size, 0));
    }
    // in the case that prev is not allocated but next is
    else if (!prev_alloc && next_alloc && prev_exists)
    { 
        //break pp's link in free list chain
        PUT(NEXT(GET(PREV(pp))), GET(NEXT(pp)));
        PUT(PREV(GET(NEXT(pp))), GET(PREV(pp)));
        //adjust size to combine both + 8, the + 8 is from the header of pp and the footer of prev being wiped out (remember, size doesn't include headers or footers)
        size += GET_SIZE(HDRP(prev_block_ptr)) + 8;
        //let prev serve as the header.
        PUT(HDRP(prev_block_ptr), PACK(size, 0));
        PUT(FTRP(prev_block_ptr), PACK(size, 0));
    }
    //in the case that prev and next are not allocated.
    else if (!prev_alloc && !next_alloc && next_exists && prev_exists)
    { 
        //combines the ideas from both functions above, taking pp and next out of the free list chain
        PUT(NEXT(GET(PREV(next_block_ptr))), GET(NEXT(next_block_ptr)));
        PUT(PREV(GET(NEXT(next_block_ptr))), GET(PREV(next_block_ptr)));
        PUT(NEXT(GET(PREV(pp))), GET(NEXT(pp)));
        PUT(PREV(GET(NEXT(pp))), GET(PREV(pp)));
        size += GET_SIZE(HDRP(next_block_ptr)) + GET_SIZE(HDRP(prev_block_ptr)) + 16;
        PUT(HDRP(prev_block_ptr), PACK(size, 0));
        PUT(FTRP(prev_block_ptr), PACK(size, 0));
    }
}
/*
    straightfoward heap extension
    allocates the new block 
    aligns the size to 8 bytes
    returns the pointer to the paylaod of the new block
*/
static void *allocate_block(size_t words)
{
    char *header_pointer;
    char *payload_pointer;
    size_t size;

    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
    if ((long)(header_pointer = mem_sbrk(size + DSIZE)) == -1)
    {
        return NULL;
    }
    payload_pointer = (char *)(header_pointer) + WSIZE;
    PUT(HDRP(payload_pointer), PACK(size, 1)); //header
    PUT(FTRP(payload_pointer), PACK(size, 1)); //footer

    //assert(TAIL == NULL || (GET(NEXT(TAIL))) == 0xffffffff);
    return payload_pointer;
}
/*
    finds the fit based on best fit discipline.
    iterates through the free list until it finds a block with the exact size we want or until the list runs out.
    returns a pointer to the best fit block or NULL if nothing was found.
*/
static void *find_fit(size_t asize)
{
    void *payload_pointer;
    void *best_fit_so_far = 0xffffffff;
    for (payload_pointer = HEAD; payload_pointer != TAIL; payload_pointer = GET(NEXT(payload_pointer)))
    {
        size_t curr_size = GET_SIZE(HDRP(payload_pointer));
        if ((curr_size > asize) && (curr_size < best_fit_so_far))
        {
            best_fit_so_far = payload_pointer;
        }
        else if (curr_size == asize)
        {
            return payload_pointer;
        }
    }
    if (best_fit_so_far == 0xffffffff)
    {
        return NULL;
    }
    else
    {
        //expected_free_list_size--;
        return best_fit_so_far;
    }
}
/*
    Frees the block at ptr and adds it to the free list.
    If the free list is empty, "instantiate" it.
    Else, add to the HEAD of the list (LIFO)
    returns nothing.
*/
void mm_free(void *ptr)
{

    size_t size;
    if (HEAD == NULL) // list not allocated yet
    {
        // set HEAD to the payload of the block
        // free the block
        // set tail to the payload of the newest freed block (this one)
        HEAD = ptr;
        TAIL = ptr;
        size = GET_SIZE(HDRP(HEAD));
        //header and footer
        PUT(HDRP(HEAD), PACK(size, 0));
        PUT(FTRP(HEAD), PACK(size, 0));
        //prev and next
        PUT(PREV(HEAD), 0x22222222);
        PUT(NEXT(HEAD), 0xffffffff); //end of list
    }
    else //general case
    {
        //set values of current head
        PUT(HEAD, ptr); //set curr.prev
        //set values of new head
        size = GET_SIZE(HDRP(ptr));
        PUT(HDRP(ptr), PACK(size, 0)); //free up
        PUT(FTRP(ptr), PACK(size, 0)); //free up
        PUT(PREV(ptr), 0x22222222);    //set new_head.prev as 0x22222222, use for assertions
        PUT(NEXT(ptr), HEAD);
        HEAD = ptr;
    }
    //assert(TAIL == NULL || (GET(NEXT(TAIL))) == 0xffffffff);

    //expected_free_list_size++;
    //mm_check();
}
/*
    Very simple mm_realloc implementation.
    if ptr is null, treats it as a malloc of size size, returns the pointer to the block
    if size is 0, treats it as wanting to free up the block and does exactly that.
    coalesces whenever it gets past initial checks.
*/
void *mm_realloc(void *ptr, size_t size)
{
    size_t oldsize;
    void *newptr;

    if (ptr == NULL)
        return mm_malloc(size);
    if (size == 0)
    {
        mm_free(ptr);
        return 0;
    }

    newptr = mm_malloc(size);
    
    if (HEAD != NULL)
        coalesce_free_list();

    // if realloc() fails the original block is left untouched 
    if (!newptr)
    {
        return 0;
    }
    // copy the old data
    oldsize = GET_SIZE(HDRP(ptr));
    if (size < oldsize)
        oldsize = size;
    memcpy(newptr, ptr, oldsize);

    // free the old block
    mm_free(ptr);

    return newptr;
}
/*
    used for coalescing the entire free list.
    calls coalesce on every block in free list
    returns 0 -> some reason this is faster than a static void return type so I left it this way.
*/
int coalesce_free_list()
{
    for (void *payload_pointer = GET(NEXT(HEAD)); payload_pointer != 0xffffffff; payload_pointer = GET(NEXT(payload_pointer)))
    {
        coalesce(payload_pointer);
    }
    return 0;
}

/*
    heap checker:
    checks for if all free blocks are in free list
    if every block is marked as free in the free list
    returns -1 if violation.
*/
int mm_check()
{
    /* use this loop the check if:
     -> all free blocks are in free list 
     -> every block marked as free
     -> do pointers in the free list point to valid free blocks
     */
    int a_size = 0;
    for (void *payload_pointer = HEAD; payload_pointer != 0xffffffff; payload_pointer = GET(NEXT(payload_pointer)))
    {
        if (GET_ALLOC(HDRP(payload_pointer)))
        {
            printf("FREE BLOCK STILL ALLOCATED AT PP :: %x\n", payload_pointer);
            return -1;
        }
        if(payload_pointer != HEAD)
        {
            void * prev = GET(PREV(payload_pointer));
            void * next = GET(NEXT(payload_pointer));
            if(prev <= mem_heap_lo() || prev >= mem_heap_hi() || next <= mem_heap_lo() || next >= mem_heap_hi())
            {
                printf("INVALID POINTER IN FREE LIST");
                return -1;
            }
        }
        

        a_size++;
    }
    if (expected_free_list_size != a_size)
    {
        printf("Expected list size :: %i || Actual list size :: %i \n", expected_free_list_size, a_size);
        return -1;
    }
    return 0;
}
