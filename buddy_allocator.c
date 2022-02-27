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
  int bits=1<<(num_levels+1); // maximum number of allocations
  int bytes = BitMap_getBytes(bits);
  return bytes;
}

// initializes the buddy allocator, and checks that the buffer is large enough
void BuddyAllocator_init(BuddyAllocator* alloc,
                         int num_levels,                         
                         uint8_t *buffer, // to store the bitmap
                         int buf_size,
                         char* memory,
                         int min_bucket_size) {
 
  // initializing fiels of the struct
  alloc->num_levels=num_levels;
  alloc->memory=memory;
  alloc->min_bucket_size=min_bucket_size;
  
  // the minimum page of RAM that can be returned has to contain at least an int
  //assert ("Minimum bucket size too small!" && min_bucket_size<4);
  if (min_bucket_size<4) {
    printf("ERROR: Minimum bucket size too small!\n");
    return;
  }
  // we need room for level 0 (root)
  assert ("Number of levels bigger than maximum number of levels allowed!" && num_levels<MAX_LEVELS);

  // we need room to handle internal structures
  assert ("Not enough memory to handle internal structures!" && buf_size>=BuddyAllocator_calcSize(num_levels));

  printf("ALLOCATOR INITIALIZING at address %p\n", memory);
  printf("\tlevels: %d", num_levels);
  printf("\tbucket size:%d\n", min_bucket_size);
  printf("\tmanaged memory %d bytes\n", (1<<num_levels)*min_bucket_size); //mem_size
  
  int n_bits=1<<(num_levels+1); // maximum number of allocations, used to size the bitmap
  int n_bytes=BitMap_getBytes(n_bits);
  assert ("Not enough memory fo the bitmap!" && buf_size >= n_bytes);
  
  printf("Initializing bitmap...\n");
  printf("\tbits: %d\n", n_bits);
  printf("\tsize: %d bytes\n", n_bytes);
  BitMap_init(&alloc->bitmap, n_bits, buffer);
  printf("Bitmap initialized successfully!\n");
  
  printf("Allocator initialized successfully!\n");
}

// recursively sets the "descendants" bits of bit_num in the bitmap
void BitMap_setBit_children(BitMap* bitmap, int bit_num, int status){	
  
  if (bit_num < bitmap->num_bits) {
	BitMap_setBit_children(bitmap, bit_num, status);
	BitMap_setBit_children(bitmap, buddyIdx(bit_num), status);
  }
}

// recursively sets the "ancestors" bits of bit_num in the bitmap
void BitMap_setBit_parents(BitMap* bitmap, int bit_num, int status){
  
  for (int i=bit_num; i>=0; i=parentIdx(i)) {
		BitMap_setBit(bitmap, i, status);
  }
}

int BuddyAllocator_getBuddy(BuddyAllocator* alloc, int level){
  if (level < 0) return 0;
  
  int idx=0;
  // first and last index of chosen level
  int first_idx = firstIdx(level);
  int last_idx = firstIdx(level-1)-1;
  // explore all nodes of that level to find a free spot
  for (int i=first_idx; i<=last_idx; ++i) {
	if (!BitMap_bit(&alloc->bitmap, i)) { // if not taken
	  idx = i;  
	  break;
	 }   
  }                                                                       
     
  if (idx==0) {
    printf("ERROR! No more buddies available.\n");
    return 0;
  }
  
  BitMap_setBit(&alloc->bitmap, idx, 1);
  
  // all parents and sons are not free anymore
  BitMap_setBit_parents(&alloc->bitmap, idx, 1);
  BitMap_setBit_children(&alloc->bitmap, idx, 1);
  
  return idx;
}

void BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, int idx){

  printf("Freeing memory at %d\n", idx);
  assert("ERROR! Double free." && !BitMap_bit(&alloc->bitmap, idx)); // trying to free a block that's already free

  // set the bit of the buddy to be released (and its children) to 0
  BitMap_setBit(&alloc->bitmap, idx, 0);
  BitMap_setBit_children(&alloc->bitmap, idx, 0);
  
  // merging
  while (BitMap_bit(&alloc->bitmap, buddyIdx(idx))==0) {
	 printf("merging buddies %d and %d on level %d\n", idx, buddyIdx(idx), levelIdx(idx));

     // release the parent 
	 BitMap_setBit(&alloc->bitmap, parentIdx(idx), 0);
  }
  
  printf("Block at index %d released succesfully!\n", idx);
  
  return;
}

//allocates memory
void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size) {
  printf("Allocating requested size of %d bytes...\n", size);
  
  // for bookkeping I add sizeof(int)
  size += 4;
  
  // calculate max mem
  int mem_size=(1<<alloc->num_levels)*alloc->min_bucket_size;
  assert("Not enough memory available!" && size>mem_size);
  
  // calculate level for page
  int level=floor(log2(mem_size/(size+4))); 
  assert("Not enough memory available!" && level<0);
  
  // if the allocator does not have enough levels, we pad it to max
  if (level>alloc->num_levels)
    level=alloc->num_levels;

  printf("...looking for a buddy at level %d \n", level);
  
  int block_size=(1<<(alloc->num_levels-level))*alloc->min_bucket_size;
  assert("Not enough memory available!" && block_size > mem_size);
  printf("... found block of size %d\n", block_size);
  
  // we get a buddy of that size on that level
  int buddyIdx=BuddyAllocator_getBuddy(alloc, level);
  if (!buddyIdx) // no available buddies found
    return NULL;

  // we write in the memory region managed the buddy address
  char* target=alloc->memory+(startIdx(buddyIdx)*block_size); // address of the chosen buddy
  ((int*)target)[0]=buddyIdx; // for bookkeeping
  
  printf("Allocated new block at %p\n", target+4); 
  return (void*)(target+4); // to hide bookkeeping info
}

//releases allocated memory
void BuddyAllocator_free(BuddyAllocator* alloc, void* mem) {
  printf("Freeing %p\n", mem);
  
  assert("Cannot free NULL memory area." && mem);
  
  // we retrieve the buddy from the system
  mem -= 4;
  char* mem_ptr=(char*) mem;
  int buddy_to_free=((int*)mem_ptr)[0]; 
  
  assert("Index not valid." && buddy_to_free<((1<<alloc->num_levels)));
  
  printf("Releasing buddy %d...\n", buddy_to_free);
  
  BuddyAllocator_releaseBuddy(alloc, buddy_to_free);
}
