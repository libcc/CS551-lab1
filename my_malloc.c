// THIS CODE IS MY OWN WORK, IT WAS WRITTEN WITHOUT CONSULTING
// A TUTOR OR CODE WRITTEN BY OTHER STUDENTS - NAVID AZIMI

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include "my_malloc.h"
#include "my_malloc_aux.h"

MyErrorNo my_errno;
void * heap_t = NULL;

void *my_malloc(size_t size) {
    // If the heap is not initialized, initialize it.
    if (heap_t == NULL) 
        heap_t = sbrk(0);
    
    // Find a free node that is large enough to hold the requested allocation.
    FreeListNode usable_node = NULL; 
    void * chunk_header = NULL;
    size_t actual_size = 0;

    // Round up the requested size to the nearest multiple of CHUNK_MU.
    if (size % CHUNK_MU) size += (CHUNK_MU - size % CHUNK_MU);
        // Set the actual size to the maximum of the requested size, the minimum chunk size, and the rounded-up size.
        size = max(size, max(CHUNK_MIN_SIZE, Size_roundup())-CHUNKHEADERSIZE);

    // If no free node is found, allocate a new chunk from the heap.
    if ((usable_node = Free_Node_Search(size)) == NULL) {

        // If the requested size is less than or equal to the default sbrk size minus the chunk header size, allocate a new chunk of the default size.
        if (size <= DEFAULT_SBRK_SIZE - CHUNKHEADERSIZE) {

            // If the sbrk call fails, set the error code to MYENOMEM and return NULL
            if ((chunk_header = sbrk(DEFAULT_SBRK_SIZE)) == (void *)-1) {
                my_errno = MYENOMEM;
                return NULL;
            }

            // Split the new chunk into two chunks: one for the requested allocation and one for the free list.
            actual_size = ChunkSplit(chunk_header, DEFAULT_SBRK_SIZE, size);
        } 
        
        else {

            // If the requested size is greater than the default sbrk size minus the chunk header size, allocate a new chunk of the requested size plus the chunk header size.
            // If the sbrk call fails, set the error code to MYENOMEM and return NULL.
            if ((chunk_header = sbrk(size + CHUNKHEADERSIZE)) == (void *)-1) {
                my_errno = MYENOMEM;
                return NULL;
            }

            // Set the actual size to the requested size plus the chunk header size.
            actual_size = size + CHUNKHEADERSIZE;
        }

    } 
    
    else {
        // If a free node was found, use it for the requested allocation.
        chunk_header = (void *) usable_node;

        // If a free node was found, use it for the requested allocation.
        actual_size = ChunkSplit(chunk_header, usable_node->size, size);
    }

    // Mark the chunk header as valid.
    ChunkHeader_sign(chunk_header, actual_size);
    // Set the error code to success.
    my_errno = MYNOERROR;
    // Return the pointer to the allocated memory.
    return chunk_header + CHUNKHEADERSIZE;
}

void my_free(void *ptr) {

    // Check if the pointer is null.
    if (ptr == NULL) {
        my_errno = MYBADFREEPTR;
        fprintf(stderr, " MYBADFREEPTR: 0x%p is not valid\n", ptr);
        // exit(1);
    }

    // Check if the pointer is within the heap.
    if (heap_t == NULL || ptr - CHUNKHEADERSIZE < heap_t || ptr > sbrk(0)) {
        my_errno = MYBADFREEPTR;
        return;
    }

    ptr -= CHUNKHEADERSIZE;
    // Get the pointer to the chunk header.
    uint32_t * chunk_header = (uint32_t *) ptr;

    // Check if the chunk header is valid.
    if (*++chunk_header != 0xDEADBEEF) {
        my_errno = MYBADFREEPTR;
        return;
    }
    // Insert the chunk into the free list.
    Node_toFList(ptr, *--chunk_header);
    // Set the error code to success.
    my_errno = MYNOERROR;
}

FreeListNode free_list_begin(void) {
  // Return the head of the free list.
  return FREE_NODES_LIST;
}

void coalesce_free_list(void) {
    FreeListNode node = FREE_NODES_LIST;

    // Traverse the free list
    while (node != NULL) {
        // Check if the current node can be coalesced with its next node
        if ((void *)node + node->size == node->flink) {
            // Merge the current node with its next node
            node->size += node->flink->size;
            node->flink = node->flink->flink;
        } 
        
        else {
            // Move to the next node in the free list
            node = node->flink;
        }
    }
}

