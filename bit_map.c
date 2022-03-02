#include <assert.h>
#include "bit_map.h"
#include <stdio.h>
#include <math.h>

// level of node idx
// N.B. levels start from 0
int levelIdx(int idx){
  return (int)floor(log2(idx+1));
}

// idx of 1st node of a level i
int firstIdx(int i){
  return (1<<i)-1;  /*{2^i}-1*/
}

// offset of node idx in his level
// N.B. starts from 0
int startIdx(int idx){
  return idx-(firstIdx(levelIdx(idx))); //((idx+1)-(1<<levelIdx(idx)));
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
  return (idx-1)/2;
}

// returns the number of bytes to store bits booleans
int BitMap_getBytes(int bits){
  return bits/8 + ((bits%8)!=0);
}

// initializes a bitmap on an external array
void BitMap_init(BitMap* bit_map, int num_bits, uint8_t* buffer){
  bit_map->buffer=buffer;
  bit_map->num_bits=num_bits;
  bit_map->buffer_size=BitMap_getBytes(num_bits);
}

// sets the bit bit_num in the bitmap
// status= 0 or 1
void BitMap_setBit(BitMap* bit_map, int bit_num, int status){
  assert(bit_num<bit_map->num_bits);
  int byte_num=bit_num/8;
  int bit_in_byte=bit_num%8;
  uint8_t mask=1<<bit_in_byte;
  if (status) {
    bit_map->buffer[byte_num]|=mask;
  } else {
    bit_map->buffer[byte_num]&=(~mask);
  }
}

// inspects the status of the bit bit_num
int BitMap_bit(const BitMap* bit_map, int bit_num){
  assert(bit_num<bit_map->num_bits);
  int byte_num=bit_num/8;
  int bit_in_byte=bit_num%8;
  return (bit_map->buffer[byte_num]&(1<<bit_in_byte))>0;
}

// prints bitmap structure
// 1=available, 0=unavailable/released
void BitMap_print(BitMap* bit_map) {
	int num_bits = bit_map->num_bits;
	int currentLevel = 1;
    int maxPerLevel = 1<<currentLevel;
    printf("Level %d:  ", currentLevel-1);
	for(int i = 0; i < num_bits-1; i++) {
        if(i == maxPerLevel-1) {
            printf("\n");
            currentLevel++;
            maxPerLevel = 1<<currentLevel;
            printf("Level %d:  ", currentLevel-1);
        }
        printf(" %d", BitMap_bit(bit_map, i));
    }
    printf("\n");
}

// recursively sets the "descendants" bits of bit_num in the bitmap
void BitMap_setBit_children(BitMap* bitmap, int bit_num, int status){	
  if (bit_num < bitmap->num_bits) {
	BitMap_setBit(bitmap, bit_num, status);
	BitMap_setBit_children(bitmap, bit_num*2+1, status); // left child of bit bit_num
	BitMap_setBit_children(bitmap, bit_num*2+2, status); // right child of bit bit_num
  }
}

// recursively sets the "ancestors" bits of bit_num in the bitmap
void BitMap_setBit_parents(BitMap* bitmap, int bit_num, int status){
  BitMap_setBit(bitmap, bit_num, status);
  if (bit_num != 0) BitMap_setBit_parents(bitmap, parentIdx(bit_num), status);
}

// recursive merge
void Bitmap_merge(BitMap *bitmap, int idx){ 
  if (idx == 0) return;
  
  if (!BitMap_bit(bitmap, buddyIdx(idx))){
    printf("\tmerging buddies %d and %d on level %d\n", idx, buddyIdx(idx), levelIdx(idx));
    BitMap_setBit(bitmap, parentIdx(idx), 0);
    Bitmap_merge(bitmap, parentIdx(idx));
  }
}

int leftChild(int idx) { return idx*2+1; }
int rightChild(int idx) { return idx*2+2; }

// returns 1 if all children of idx are free, 0 otherwise
int BitMap_check_children(BitMap* bit_map, int idx) {
  if (rightChild(idx) < bit_map->num_bits) {
	// check that both children are free	
    if (!(BitMap_bit(bit_map, leftChild(idx)) || BitMap_bit(bit_map, rightChild(idx)))) {
      BitMap_check_children(bit_map, leftChild(idx));
      BitMap_check_children(bit_map, rightChild(idx));
    }
    // at least one child is taken
    else return 0;
  } 
  //all children are free
  return 1;
}
