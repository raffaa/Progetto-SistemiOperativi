#pragma once
#include "bit_map.h"

#define MAX_LEVELS 16

typedef struct  {
  int num_levels;
  char* memory; // the memory area to be managed
  int mem_size;
  int min_bucket_size; // the minimum page of RAM that can be returned
  int* bitmap;
  int bitmap_size;
} BuddyAllocator;


// computes the size in bytes for the buffer of the allocator
//int BuddyAllocator_calcSize(int num_levels);


// initializes the buddy allocator, and checks that the buffer is large enough
void BuddyAllocator_init(BuddyAllocator* alloc,
                         int num_levels,
                         char* memory,
                         int mem_size,
                         int min_bucket_size,
                         int* bitmap,
                         int bitmap_size);

// returns (allocates) a buddy index at a given level.
// 0 id no memory available
int BuddyAllocator_getBuddy(BuddyAllocator* alloc, int level);


// releases an allocated buddy, performing the necessary joins
void BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, int idx);

//allocates memory
void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size);

//releases allocated memory
void BuddyAllocator_free(BuddyAllocator* alloc, void* mem);

//aux function to get the index of the smallest buddy available
int BuddyAllocator_minBuddy(BuddyAllocator* alloc);
