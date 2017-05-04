#ifndef BLOCK_HPP
#define BLOCK_HPP

#define MAX_SIZE 512
#define MIN_SIZE 128



struct Block{
  char* b_data;
  unsigned int block_size;
};

typedef struct Block Block_t;

void block_init(Block_t* block, unsigned int size){
  if(size > MAX_SIZE || size < MIN_SIZE){
    block = NULL;
    return;
  }

  if((size & (size-1)) > 0){
    block = NULL;
    return;
  }

  block->block_size = size;
  block->b_data = malloc(size * sizeof(char));
}



#endif
