#ifndef SUPERBLOCK_H
#define SUPERBLOCK_H

using namespace std;
/**
 * Super block struct
 */

struct superblock{

    int blockSize;
    int numBlocks;
  
    int iMap;
    int dMap;
};
typedef struct superblock superblock_t;

#endif
