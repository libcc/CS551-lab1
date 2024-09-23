// THIS CODE IS MY OWN WORK, IT WAS WRITTEN WITHOUT CONSULTING
// A TUTOR OR CODE WRITTEN BY OTHER STUDENTS - NAVID AZIMI

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include "my_malloc.h"
#include "my_malloc_aux.h"

FreeListNode FREE_NODES_LIST = NULL;

FreeListNode Free_Node_Search(size_t size) {

    FreeListNode previous = NULL;
    FreeListNode node = FREE_NODES_LIST;
    FreeListNode selected_node = NULL;

    if (FREE_NODES_LIST == NULL) {
        return NULL;
    }

    // Find the first node in the free list that is large enough to satisfy the request.
    while (node != NULL) {
        if (node->size >= CHUNKHEADERSIZE + size) {
            selected_node = node;
            break;
        } 

        else {
            previous = node;
            node = node->flink;
        }
    }

    // If a suitable node was found, remove it from the free list and return it.
    if (selected_node != NULL) {

        if (previous == NULL) {
            FREE_NODES_LIST = selected_node->flink;
        } 
        
        else {
            previous->flink = selected_node->flink;
        }
        
        return selected_node;
    }

    // Otherwise, return NULL.
    else {
        return NULL;
    }

}

void Node_toFList(void *chunk_header, size_t size) {

    // Cast the chunk head to a FreeListNode pointer.
    FreeListNode node = (FreeListNode)chunk_header;

    // Set the size of the free list node.
    node->size = size;

    // If the free list is empty, add the new node to the head of the list.
    if (FREE_NODES_LIST == NULL) {
        FREE_NODES_LIST = node;
        node->flink = NULL;
        return;
    }

    // If the new node is smaller than the head of the free list, add the new node to the head of the list.
    if (FREE_NODES_LIST > node) {
        node->flink = FREE_NODES_LIST;
        FREE_NODES_LIST = node;
        return;
    }

    // Find the insertion point for the new node in the free list.
    FreeListNode previous = FREE_NODES_LIST;
    while (previous->flink != NULL && previous->flink < node) {
        previous = previous->flink;
    }

    // Insert the new node into the free list.
    node->flink = previous->flink;
    previous->flink = node;
}

size_t Size_roundup() {

    // Get the size of a FreeListNode object.
    size_t freelistnode_size = sizeof(struct freelistnode);

    // If the size is not a multiple of CHUNK_MU, round it up to the nearest multiple of CHUNK_MU.
    if (freelistnode_size % CHUNK_MU != 0) {
        freelistnode_size = freelistnode_size + ((CHUNK_MU - freelistnode_size) % CHUNK_MU);
    }

    // Return the rounded size.
    return freelistnode_size;
}

void ChunkHeader_sign(uint32_t* chunk_header, size_t actual_size) {

    // Initialize the first element of the chunk header with the actual size
    *chunk_header = (uint32_t)actual_size;

    // Initialize the second element of the chunk header with the signature
    *(chunk_header + 1) = 0xDEADBEEF;
}

size_t ChunkSplit(void *chunk_header, size_t chunk_size, size_t request_size) {

    // Calculate the size of the remaining chunk after the allocation.
    size_t remaining_chunk_size = chunk_size - request_size - CHUNKHEADERSIZE;

    // If the remaining chunk is large enough, split the chunk.
    if (remaining_chunk_size >= max(CHUNK_MIN_SIZE, Size_roundup())) {
        
        // Calculate the address of the second chunk header.
        void *second_chunk_header = chunk_header + CHUNKHEADERSIZE + request_size;

        // Insert the second chunk into the free list.
        Node_toFList(second_chunk_header, remaining_chunk_size);

        // Return the size of the requested chunk.
        return request_size + CHUNKHEADERSIZE;
    }

    // The chunk cannot be split, so return the size of the original chunk.
    return chunk_size;
}

