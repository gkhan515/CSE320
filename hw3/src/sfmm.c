/**
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "debug.h"
#include "sfmm.h"

#define WSIZE 2
#define RSIZE 8
#define M 32

#define MAX(x,y) ((x) > (y) ? (x) : (y))

#define ALIGN_TO_32B(x) (((x)+31) & ~31)

#define PACK(size, alloc, palloc) ((size)|(alloc << 4)|(palloc << 3))

#define GET(p) (*(unsigned long*)(p))
#define PUT(p, val) (*(unsigned long*)(p) = (val))

#define GET_SIZE(p) (GET(p) & ~0b11111)
#define GET_ALLOC(p) ((GET(p) & 0b10000) >> 4)
#define GET_PREV_ALLOC(p) ((GET(p) & 0b1000) >> 3)

#define HDRP(bp) ((char*)(bp) - RSIZE)
#define FTRP(bp) ((char*)(bp) + GET_SIZE(HDRP(bp)) - 2*RSIZE)

#define NEXT_BLKP(bp) ((char*)(bp) + GET_SIZE(HDRP(bp)))
#define PREV_BLKP(bp) ((char*)(bp) - GET_SIZE((char*)bp - 2*RSIZE))

int free_list_num (size_t size) {
    if (size <= M)
        return 0;
    else if (M < size && size <= 2*M)
        return 1;
    else if (2*M < size && size <= 3*M)
        return 2;
    else if (3*M < size && size <= 5*M)
        return 3;
    else if (5*M < size && size <= 8*M)
        return 4;
    else if (8*M < size && size <= 13*M)
        return 5;
    else if (13*M < size && size <= 21*M)
        return 6;
    else if (21*M < size && size <= 34*M)
        return 7;
    else
        return 8;
}

int valid_pointer(char *bp) {
    if (!bp)
        return 0;

    size_t size = GET_SIZE(HDRP(bp));
    size_t alloc = GET_ALLOC(HDRP(bp));
    // size_t palloc = GET_PREV_ALLOC(HDRP(bp));

    if ((unsigned long)bp % 32 ||
        size < 32 ||
        size % 32 ||
        (char*)HDRP(bp) < (char*)sf_mem_start() ||
        (char*)FTRP(bp) > (char*)sf_mem_end() ||
        !alloc)
        return 0;

    return 1;
}

int first_call = 1;
void sf_init() {
    // align start of heap
    char *heap_start = sf_mem_grow();

    char *bp = heap_start + RSIZE;
    while ((unsigned long)bp % M != 0)
        bp++;
    // At this point bp is one row in front of where prologue should go

    // prologue
    sf_block prologue = { .header = PACK(32, 1, 0) };
    *(sf_block*)HDRP(bp) = prologue;

    // epilogue
    sf_block epilogue = { .header = PACK(0, 1, 0) };
    *(sf_block*)HDRP(sf_mem_end()) = epilogue;

    // sentinel nodes
    for (int i = 0; i < NUM_FREE_LISTS; i++) {
        sf_block *node = &sf_free_list_heads[i];
        node->body.links.prev = node;
        node->body.links.next = node;
    }

    // make free block from remaining space
    bp = NEXT_BLKP(bp);
    size_t free_size = (char*)HDRP(sf_mem_end()) - (char*)HDRP(bp);
    size_t head = PACK(free_size, 0, 1);
    sf_block free_block = { .header = head };
    *(sf_block*)HDRP(bp) = free_block;
    PUT(FTRP(bp), head);

    // insert block into free list
    sf_block *sentinel = &sf_free_list_heads[8];
    sf_block *free_blockp = (sf_block*)HDRP(bp);
    sentinel->body.links.next = free_blockp;
    sentinel->body.links.prev = free_blockp;
    free_blockp->body.links.next = sentinel;
    free_blockp->body.links.prev = sentinel;
}

sf_block *find_free_block (size_t asize) {
    int list_num = free_list_num(asize);
    sf_block *ret = NULL;

    for (int i = list_num; i < NUM_FREE_LISTS && !ret; i++) {
        sf_block *sentinel = &sf_free_list_heads[i];
        sf_block *current_node = sentinel->body.links.next;

        while (current_node != sentinel) {
            if (GET_SIZE(current_node) >= asize){
                ret = current_node;
                break;
            }
            current_node = current_node->body.links.next;
        }
    }

    return ret;
}

void rm_from_free_list (sf_block *block) {
    sf_block *old_next = block->body.links.next;
    block->body.links.prev->body.links.next = old_next;
    old_next->body.links.prev = block->body.links.prev;
}

void coalesce (sf_block* block) {
    char *bp = (char*)block + RSIZE;

    //if previous block free
    if (!GET_PREV_ALLOC(HDRP(bp))) {

        // combine with previous block
        char *prev_bp = PREV_BLKP(bp);
        size_t new_size = GET_SIZE(HDRP(bp)) + GET_SIZE(HDRP(prev_bp));
        unsigned long head = PACK(new_size, 0, GET_PREV_ALLOC(HDRP(prev_bp)));
        PUT(HDRP(prev_bp), head);
        PUT(FTRP(prev_bp), head);

        // remove previous block from free list
        rm_from_free_list((sf_block*)HDRP(prev_bp));

        bp = prev_bp;
    }

    // if next block free
    if (!GET_ALLOC(HDRP(NEXT_BLKP(bp)))) {

        // combine with next block
        char *next_bp = NEXT_BLKP(bp);
        size_t new_size = GET_SIZE(HDRP(bp)) + GET_SIZE(HDRP(next_bp));
        unsigned long head = PACK(new_size, 0, GET_PREV_ALLOC(HDRP(bp)));
        PUT(HDRP(bp), head);
        PUT(FTRP(bp), head);

        // remove next block from free list
        rm_from_free_list((sf_block*)HDRP(next_bp));
    }

    // insert free block into free list
    block = (sf_block*)HDRP(bp);
    size_t size = GET_SIZE(HDRP(bp));
    sf_block *sentinel = &sf_free_list_heads[free_list_num(size)];
    sf_block *next = sentinel->body.links.next;

    sentinel->body.links.next = block;
    block->body.links.prev = sentinel;

    block->body.links.next = next;
    next->body.links.prev = block;
}

int extend_heap() {
    char *bp = sf_mem_grow();
    if (!bp)
        return 1;
    unsigned long head = PACK(PAGE_SZ, 0, GET_PREV_ALLOC(HDRP(bp)));
    PUT(HDRP(bp), head);
    PUT(FTRP(bp), head);

    sf_block epilogue = { .header = PACK(0, 1, 0) };
    *(sf_block*)HDRP(sf_mem_end()) = epilogue;

    coalesce((sf_block*)HDRP(bp));

    return 0;
}

void split_block (sf_block *free_block, size_t allocated) {
    char *bp = (char*)free_block + RSIZE;
    size_t free_leftover = GET_SIZE(HDRP(bp)) - allocated;

    char *new_free_block = bp + allocated;
    unsigned long head = PACK(free_leftover, 0, 1);
    PUT(HDRP(new_free_block), head);
    PUT(FTRP(new_free_block), head);

    coalesce((sf_block*)HDRP(new_free_block));
}

void allocate_block (sf_block* block, size_t size) {
    rm_from_free_list(block);

    char *bp = (char*)block + RSIZE;
    size_t free_size = GET_SIZE(HDRP(bp));

    if (free_size - size >= M)
        split_block(block, size);
    else if (free_size - size < M)
        size = free_size;

    unsigned long palloc = GET_PREV_ALLOC(HDRP(bp));
    unsigned long head = PACK(size, 1, palloc);
    PUT(HDRP(bp), head);

    // set the next block's prev_alloc to 1
    char *nb = NEXT_BLKP(bp);
    size_t nb_size = GET_SIZE(HDRP(nb));
    unsigned long alloc = GET_ALLOC(HDRP(nb));
    head = PACK(nb_size, alloc, 1);
    PUT(HDRP(nb), head);
}

void *sf_malloc(size_t size) {
    if (first_call) {
        sf_init();
        first_call = 0;
    }

    if (size == 0)
        return NULL;

    size_t asize = size + RSIZE; // adujst size to include header
    while (asize % M != 0)
        asize++;

    sf_block *ret = find_free_block(asize);
    while (!ret) {
        int err = extend_heap();
        if (err) {
            sf_errno = ENOMEM;
            return NULL;
        }
        ret = find_free_block(asize);
    }

    allocate_block(ret, asize);

    return ((char*)ret + RSIZE);
}

void sf_free(void *pp) {
    if (!valid_pointer(pp)) {
        fprintf(stderr, "Invalid pointer\n");
        abort();
    }

    char *bp = (char*)pp;

    // set allocated bit to 0
    unsigned long head = PACK(GET_SIZE(HDRP(bp)), 0, GET_PREV_ALLOC(HDRP(bp)));
    PUT(HDRP(bp), head);
    PUT(FTRP(bp), head);

    // set next block's prev_alloc to 0
    char *nb = NEXT_BLKP(bp);
    head = PACK(GET_SIZE(HDRP(nb)), GET_ALLOC(HDRP(nb)), 0);
    PUT(HDRP(nb), head);

    // coalesce
    coalesce((sf_block*)(pp - RSIZE));
}

void *sf_realloc(void *pp, size_t rsize) {
    if (!valid_pointer(pp)) {
        fprintf(stderr, "Invalid pointer\n");
        abort();
    }

    char *bp = (char*)pp;

    size_t orig_size = GET_SIZE(HDRP(bp));

    size_t asize = rsize + RSIZE; // adjust size to include header
    while (asize % M != 0)
        asize++;

    if (rsize == 0) {
        sf_free(pp);
        return NULL;
    }

    if (asize > orig_size) {
        char *nb = sf_malloc(rsize);
        if (!nb)
            return NULL;

        unsigned long head = PACK(GET_SIZE(HDRP(nb)), 1, GET_PREV_ALLOC(HDRP(nb)));
        PUT(HDRP(nb), head);
        memcpy(nb, pp, GET_SIZE(HDRP(pp)));
        sf_free(pp);
        return nb;
    }

    if (asize < orig_size) {
        if (orig_size - asize >= M){
            split_block((sf_block*)HDRP(bp), asize);
            unsigned long head = PACK(asize, 1, GET_PREV_ALLOC(HDRP(bp)));
            PUT(HDRP(bp), head);
        }
    }

    return bp;
}

void *sf_memalign(size_t size, size_t align) {
    if ((align < M) || (align & (align-1))) {
        sf_errno = EINVAL;
        return NULL;
    }

    char *large_bp = sf_malloc(size + align + M + RSIZE);
    if (!large_bp)
        return NULL;
    char *aligned_bp = large_bp;

    size_t amount_shifted = 0;
    while ((unsigned long)aligned_bp % align != 0) {
        aligned_bp++;
        amount_shifted++;
    }

    size_t asize = size + RSIZE;
    while (asize % M != 0)
        asize++;

    // if the malloc-ed block was already aligned
    if (!amount_shifted) {
        if ((GET_SIZE(HDRP(large_bp)) - asize) >= M)
            split_block((sf_block*)HDRP(large_bp), asize);
        unsigned long head = PACK(asize, 1, GET_PREV_ALLOC(HDRP(large_bp)));
        PUT(HDRP(large_bp), head);
        return large_bp;
    }

    // give aligned block a header
    size_t aligned_block_size = GET_SIZE(HDRP(large_bp)) - amount_shifted;
    unsigned long head = PACK(aligned_block_size, 1, 0);
    PUT(HDRP(aligned_bp), head);
    PUT(FTRP(aligned_bp), head);

    // free the previous block (large_bp)
    head = PACK(amount_shifted, 0, GET_PREV_ALLOC(HDRP(large_bp)));
    PUT(HDRP(large_bp), head);
    PUT(FTRP(large_bp), head);
    coalesce((sf_block*)HDRP(large_bp));

    // split the aligned block and give it a header
    split_block((sf_block*)HDRP(aligned_bp), asize);
    head = PACK(asize, 1, 0);
    PUT(HDRP(aligned_bp), head);

    return aligned_bp;
}