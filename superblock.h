#ifndef SUPERBLOCK_H
#define SUPERBLOCK_H

/*
 * Super block struct
 */

struct superblock{
  
  int blockSize;
  int numBlocks;
  
};

typedef struct superblock superblock_t;

#endif
