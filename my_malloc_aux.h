//
//  my_malloc_aux.h
//  Lab1: Malloc
//
//  Created by Dorian Arnold on 7/17/20.
//  Copyright © 2020 Dorian Arnold. All rights reserved.
//
// THIS CODE IS MY OWN WORK, IT WAS WRITTEN WITHOUT CONSULTING
// A TUTOR OR CODE WRITTEN BY OTHER STUDENTS - NAVID AZIMI
//
 
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "my_malloc.h"

#ifndef my_malloc_aux_h
#define my_malloc_aux_h

// defining constants
#define CHUNK_MU 8
#define CHUNK_MIN_SIZE 16
#define DEFAULT_SBRK_SIZE 8192
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

extern FreeListNode FREE_NODES_LIST;

//search for a suitable free node in a linked list of FreeListNode structures based on the requested size
FreeListNode Free_Node_Search(size_t size);

//add a given chunk (represented by chunk_header) to a linked list of free nodes (FREE_NODES_LIST).
void Node_toFList(void *chunk_header, size_t size);

//calculate the size of a FreeListNode object and round it up to the nearest multiple of CHUNK_MU
size_t Size_roundup();

//add a signature and store the actual size of a chunk in its header
void ChunkHeader_sign(uint32_t* chunk_header, size_t actual_size);

//split a chunk into two parts if there's enough space for the split, and then insert the second chunk into the free list.
size_t ChunkSplit(void* chunk_header, size_t chunk_size, size_t request_size);

#endif /* my_malloc_aux_h */
