#ifndef SUPERBLOCK_H
#define SUPERBLOCK_H

using namespace std;
/**
 * Super block class
 */

class Superblock{
    public:
        int blockSize;
        int numBlocks;
        int inodeMap;
        int freeBlockList;

        Superblock(int, int);
};

#endif
