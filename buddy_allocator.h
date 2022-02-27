#pragma once
#include "bit_map.h"

#define MAX_LEVELS 16

typedef struct  {
  int num_levels;
  char* memory; // the memory area to be managed
  int min_bucket_size; // the minimum page of RAM that can be returned
  BitMap bitmap;
} BuddyAllocator;


// computes the size in bytes for the buffer of the allocator
int BuddyAllocator_calcSize(int num_levels);

// initializes the buddy allocator, and checks that the buffer is large enough
void BuddyAllocator_init(BuddyAllocator* alloc,
                         int num_levels,                         
                         uint8_t *buffer, // to store the bitmap
                         int buf_size,
                         char* memory,
                         int min_bucket_size);

// returns (allocates) a buddy index at a given level.
// 0 id no memory available
int BuddyAllocator_getBuddy(BuddyAllocator* alloc, int level);

// releases an allocated buddy, performing the necessary joins
void BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, int idx);

//allocates memory
void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size);

//releases allocated memory
void BuddyAllocator_free(BuddyAllocator* alloc, void* mem);

//aux functions
