#include <assert.h>
#include "bit_map.h"
#include <stdio.h>
#include <math.h>

// N.B. levels start from 0
// level of node idx
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
  // get byte
  int byte_num=bit_num>>3;
  assert(byte_num<bit_map->buffer_size);
  int bit_in_byte=byte_num&0x03;
  if (status) {
    bit_map->buffer[byte_num] |= (1<<bit_in_byte);
  } else {
    bit_map->buffer[byte_num] &= ~(1<<bit_in_byte);
  }
}

// inspects the status of the bit bit_num
int BitMap_bit(const BitMap* bit_map, int bit_num){
  int byte_num=bit_num>>3; 
  assert(byte_num<bit_map->buffer_size);
  int bit_in_byte=byte_num&0x03;
  return (bit_map->buffer[byte_num] & (1<<bit_in_byte))!=0;
}

void setBit(BitMap* bit_map, int bitIndex) //sets the bit at index bitIndex
{
	int intIndex = bitIndex / (sizeof(int)*8);
	int pos = bitIndex % (sizeof(int)*8);
	unsigned int flag = 1;
	flag = flag << pos;
	bit_map->buffer[intIndex] = bit_map->buffer[intIndex] | flag;
}

// prints bitmap structure
// 0: at list one of the children is free  
// 1: is taken (or both children are taken)
void BitMap_print(BitMap* bitmap) {
	int num_bits = bitmap->num_bits;
	int currentLevel = 1;
    int maxPerLevel = 1<<currentLevel;
	for(int i = 0; i < num_bits-1; i++) {
        
        if(i == maxPerLevel-1) {
            printf("\n");
            currentLevel++;
            maxPerLevel = 1<<currentLevel;
        }
        printf(" %d", BitMap_bit(bitmap, i));
    }
    printf("\n");
}
