/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/* single word (4) or double word (8) alignment */
#define WSIZE     4
#define DSIZE     8
#define CHUNKSIZE (1<<12)
#define ALIGNMENT 8

#define MAX(x,y) ((x) > (y)? (x) : (y))

/* combines size and alloc into 32 bits (word/4 bytes)
 * can be used for creating block headers, where the lowest 3 bits are reserved for alloc, and the highest 29 are for representing the size of the chunk */
#define PACK(size, alloc) ((size) | (alloc)) 


#define GET(p)       (*(unsigned int *)(p))
#define PUT(p, val)  (*(unsigned int *)(p) = (val))

#define GET_SIZE(p)   (GET(p) & ~0x7)
#define GET_ALLOC(p)  (GET(p) & 0x1)

#define HDRP(bp)      ((char *)(bp) - WSIZE)
#define FTRP(bp)      ((char *)(bp) + GET_SIZE(HDRP(bp))- DSIZE)

#define NEXT_BLKP(bp)   ((char *)(bp) + GET_SIZE(((char *)(bp)-WSIZE)))
#define PREV_BLKP(bp)   ((char *)(bp)- GET_SIZE(((char *)(bp)-DSIZE)))
/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

static char * heap_listp; //var to point to prologue block

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    /*free list initialization: give 4 bytes of heap to this implementation (next line) */
    if((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1) //initializes the heap (empty), heap_listp points to first byte of heap area (ret. val. of mem_sbrk())
        return -1;
    PUT(heap_listp, 0); //start the pointer at 0, leave first 4 bytes of heap untouched (8 byte alignment is reason why)
    PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1)); //set second 4 bytes of heap to being allocated and set to value = 8(heap prologue header)
    PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1)); //set third 4 bytes of heap to being allocated and set to value = 8(prologue footer)
    PUT(heap_listp + (3*WSIZE), PACK(0, 1)); //set fourth 4 bytes of heap to being allocated (epilogue header) -> this will always be at the end of the heap
    heap_listp += (2*WSIZE); //advance heap pointer by 8 bytes (not 12) -> means that the epilogue header will always be at the end of the heap

    return 0;
}

static void *extend_heap(size_t words)
{
    
}
/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    if (size == 0){
        return NULL;
    }

    int newsize = ALIGN(size + SIZE_T_SIZE);
    void *p = mem_sbrk(newsize);
    if (p == (void *)-1)
	return NULL;
    else {
        *(size_t *)p = size;
        return (void *)((char *)p + SIZE_T_SIZE);
    }
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}














