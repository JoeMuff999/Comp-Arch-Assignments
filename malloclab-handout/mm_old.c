// static char * heap_listp; //var to point to prologue block
// static void * extend_heap(size_t);
// static void allocate(void *, size_t);
// static void * find_fit(size_t);
// /* 
//  * mm_init - initialize the malloc package.
//  */
// int mm_init(void)
// {
//     /*free list initialization: give 4 bytes of heap to this implementation (next line) */
//     if((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1) //initializes the heap (empty), heap_listp points to first byte of heap area (ret. val. of mem_sbrk())
//         return -1;
//     PUT(heap_listp, 0); //start the pointer at 0, leave first 4 bytes of heap untouched (8 byte alignment is reason why)
//     PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1)); //set second 4 bytes of heap to being allocated and set to value = 8(heap prologue header)
//     PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1)); //set third 4 bytes of heap to being allocated and set to value = 8(prologue footer)
//     PUT(heap_listp + (3*WSIZE), PACK(0, 1)); //set fourth 4 bytes of heap to being allocated (epilogue header) -> this will always be at the end of the heap
//     heap_listp += (2*WSIZE); //advance heap pointer by 8 bytes (not 12) -> means that the epilogue header will always be at the end of the heap
    
//     if (extend_heap(CHUNKSIZE/WSIZE) == NULL)
//     {
//         return -1;
//     }
    
//     return 0;
// }

// /* 
//  * mm_malloc - Allocate a block by incrementing the brk pointer.
//  *     Always allocate a block whose size is a multiple of the alignment.
//  */
// // void *mm_malloc(size_t size)
// // {
// //     if (size == 0){
// //         return NULL;
// //     }
    
// //     int newsize = ALIGN(size + SIZE_T_SIZE);
// //     void *p = mem_sbrk(newsize);
// //     if (p == (void *)-1)
// // 	return NULL;
// //     else {
// //         *(size_t *)p = size;
// //         return (void *)((char *)p + SIZE_T_SIZE);
// //     }
// // }

// void * mm_malloc(size_t size)
// {
//     if(size == 0){
//         return NULL;
//     }

//     char * block_pointer;
//     size_t extendsize;

    

//     size_t newsize = ALIGN(size + SIZE_T_SIZE);
    
//     if((block_pointer = find_fit(newsize)) != NULL)
//     {
        
//       //  printf("FIND_FIT_BP:: %x \n", block_pointer);
//         allocate(block_pointer, newsize);
//         return block_pointer;
//     }

//     /* if block_pointer == NULL, then must extend heap ORRRR you can coalesce */
//     extendsize = MAX(newsize, CHUNKSIZE);
    
//     if((block_pointer = extend_heap(extendsize/WSIZE)) == NULL)
//     {
//         return NULL;
//     }
//     //start = f699e020
//     //x 0xf699f020-4
//     //x 0xf699f020-8
//    // printf("EXTEND_HEAP_BP:: %x \n", block_pointer);
//     allocate(block_pointer, newsize);
//     return block_pointer;

// }

// static void *extend_heap(size_t words)
// {
//     char *block_pointer;
//     size_t size;

//     size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
//     if((long)(block_pointer = mem_sbrk(size)) == -1)
//     {
//         return NULL;
//     }
//     //printf("mem_sbrk call :: %x \n ", block_pointer);
//     size_t debug = HDRP(NEXT_BLKP(block_pointer));
//     PUT(HDRP(block_pointer), PACK(size, 0));
//     PUT(FTRP(block_pointer), PACK(size, 0));
//     PUT(HDRP(NEXT_BLKP(block_pointer)), PACK(0,1));
    
//     return block_pointer;
// }

// static void allocate(void * block_pointer, size_t asize)
// {
//     size_t block_header = PACK(GET_SIZE(HDRP(block_pointer)),1);
//     size_t debug = FTRP(block_pointer);
//     PUT(HDRP(block_pointer), block_header);
//     PUT(FTRP(block_pointer), block_header);
// }

// static void *find_fit(size_t asize)
// {
//     void * block_pointer;

//     for(block_pointer = heap_listp; GET_SIZE(HDRP(block_pointer)) > 0; block_pointer = NEXT_BLKP(block_pointer))
//     {
//         if(!GET_ALLOC(HDRP(block_pointer)) && (GET_SIZE(HDRP(block_pointer)) >= asize))
//         {
//             return block_pointer;
//         }
//     }
//     return NULL;
// }

// /*
//  * mm_free - Freeing a block does nothing.
//  */
// void mm_free(void *ptr)
// {
//     size_t size = GET_SIZE(HDRP(ptr));

//     PUT(HDRP(ptr), PACK(size, 0));
//     PUT(FTRP(ptr), PACK(size, 0));
    
// }

// static void * coalesce(void *break_point){
    
// }

// /*
//  * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
//  */
// void *mm_realloc(void *ptr, size_t size)
// {
//     void *oldptr = ptr;
//     void *newptr;
//     size_t copySize;
    
//     newptr = mm_malloc(size);
//     if (newptr == NULL)
//       return NULL;
//     copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
//     if (size < copySize)
//       copySize = size;
//     memcpy(newptr, oldptr, copySize);
//     mm_free(oldptr);
//     return newptr;
// }


