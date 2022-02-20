#include <stdio.h>
#include <assert.h>
#include <math.h> // for floor and log2
#include "buddy_allocator.h"

// these are trivial helpers to support you in case you want
// to do a bitmap implementation

// level of node idx
int levelIdx(size_t idx){
  return (int)floor(log2(idx));
};

// idx of 1st node of a level i
int firstIdx(int i){
  return 1<<i;  /*{2^i}*/
}

// offset of node idx in his level
int startIdx(int idx){
  return idx-firstIdx(levelIdx(idx)); //(idx-(1<<levelIdx(idx)));
}

// index of the buddy of node idx
int buddyIdx(int idx){
  if (idx&0x1){
    return idx-1;
  }
  return idx+1;
}

// parent of the node idx
int parentIdx(int idx){
  return idx/2;
}

// computes the size in bytes for the allocator
int BuddyAllocator_calcSize(int num_levels) {
  int buddies=1<<(num_levels+1); // maximum number of allocations
  int bits = BitMap_getBytes(buddies);
  return bits;
}

// initializes the buddy allocator, and checks that the buffer is large enough
void BuddyAllocator_init(BuddyAllocator* alloc,
                         int num_levels,
                         char* memory,
                         int mem_size,
                         uint8_t *buffer, // to store the bitmap
                         int buf_size,
                         int min_bucket_size) {
 
  // initializing fiels of the struct
  alloc->num_levels=num_levels;
  alloc->memory=memory;
  alloc->mem_size=mem_size;
  alloc->min_bucket_size=min_bucket_size;
  
  // the minimum page of RAM that can be returned has to contain at least an int
  assert ("Minimum bucket size too small!" && min_bucket_size<8);
  // we need room for level 0 (root)
  assert ("Number of levels bigger than maximum number of levels allowed!" && num_levels<MAX_LEVELS);
  // we need room to handle internal structures
  assert ("Not enough memory to handle internal structures!" && buf_size>=BuddyAllocator_calcSize(num_levels));
  // sanity check
  assert (mem_size==(1<<num_levels)*min_bucket_size);

  printf("BUDDY INITIALIZING at address %p\n", memory);
  printf("\tlevels: %d", num_levels);
  printf("\tmax list entries %d bytes\n", buddies);
  printf("\tbucket size:%d\n", min_bucket_size);
  printf("\tmanaged memory %d bytes\n", mem_size); //(1<<num_levels)*min_bucket_size
  
  int n_bits=1<<(num_levels+1); // maximum number of allocations, used to size the bitmap
  int n_bytes=BitMap_getBytes(n_bits);
  assert ("Not enough memory fo the bitmap!" && bitmap_size >= n_bytes);
  
  printf("BITMAP INITIALIZING at address %p\n", memory);
  printf("\tbits: %d", n_bits);
  printf("\tsize %d bytes\n", n_bytes);
  BitMap_init(&alloc->bitmap, buddies, buffer);
  printf("Bitmap initialized successfully!");
  
  printf("SUCCESS!");
}

//reverse loop to find smallest buddy
int BuddyAllocator_minBuddy(BuddyAllocator* alloc, int num_levels){
  int idx=-1;
  for (int i=(1<<num_levels+1); i>=0; --i) {
	if (!BitMap_getBit(alloc->bitmap, i)) // if not taken
	  idx = i;  
  }
  printf("ERROR! No more buddies available.\n");
  return idx;
}

void BitMap_setBit_children(BitMap* bitmap, int idx, int status){
  // TO DO	
}
void BitMap_setBit_children(BitMap* bitmap, int idx, int status){
  // TO DO		
}

int BuddyAllocator_getBuddy(BuddyAllocator* alloc, int level){
  assert(level < 0);
  assert(level <= alloc->num_levels);

  int idx=BuddyAllocator_minBuddy(alloc, level);   
  if (idx<0) {
    printf("ERROR! No more buddies available.\n");
    return idx;
  }
  
  BitMap_setBit(alloc->bitmap, idx, 1);
  BitMap_setBit(alloc->bitmap, buddyIdx(idx), 1);
  BuddyAllocator_getBuddy(alloc, level-1);
  
  return idx;
}

void BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, int idx){

  printf("Free memory at %d\n", idx);
  assert("ERROR! Double free." && !BitMap_bit(&alloc->bitmap, idx)); // trying to free a block that's already free

  //join
  //1. we destroy the two buddies in the free list;
  printf("merge %d\n", levelIdx(idx));
  // set children status recursively
  BitMap_setBit_children(&alloc->bitmap, idx, 0); // TO DO
   
  //2. we release the parent recursively
  BitMap_setBit_parents(&alloc->bitmap, idx, 0); // TO DO

  printf("Block at index %d released succesfully!\n", idx);
  
  return;
}

//allocates memory
void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size) {
  printf("Allocating requested size of %d bytes...\n", mem);
  
  // the size+8 is rounded up to the size of the smallest partition capable to contain it
  size += 8;
  // calculate max mem
  int mem_size=(1<<alloc->num_levels)*alloc->min_bucket_size;
  assert("Not enough memory available!" && size > mem_size);
  
  // calculate level for page
  int level=floor(log2(mem_size/(size+8))); // +8 of bookkeping
  // if the level is too small, we pad it to max
  if (level>alloc->num_levels)
    level=alloc->num_levels;

  printf("...looking for a buddy at level %d \n", level);
  
  int block_size = (1<<(alloc->num_levels-level))*alloc->min_bucket_size;
  printf("block size: %d\n", block_size);
  
  // we get a buddy of that size on that level
  int buddyIdx=BuddyAllocator_getBuddy(alloc, level);
  if (!buddyIdx) // no available buddies found
    return NULL;

  // we write in the memory region managed the buddy address
  char* target=alloc->memory+(startIdx(buddyIdx)*block_size);
  ((int*)target)[0]=buddyIdx; // bookkeeping
  
  printf("Allocated block at %p\n", target+sizeof(int)); 
  return (void*)(target + sizeof(int)); // to hide bookkeeping info
}

//releases allocated memory
void BuddyAllocator_free(BuddyAllocator* alloc, void* mem) {
  printf("Freeing %p\n", mem);
  
  assert("Cannot free NULL memory area." && mem);
  
  // we retrieve the buddy from the system
  mem=mem-8;
  char* mem_ptr=(char*) mem;
  int buddy_to_free=*((int*)mem_ptr); 
  
  assert("Index not valid." && buddy_to_free<((1<<alloc->num_levels)));
  
  printf("Releasing buddy %d...\n", buddy_to_free);
  
  BuddyAllocator_releaseBuddy(alloc, buddy_to_free);
}
