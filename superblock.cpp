#include "superblock.hpp"
/** 
 * For all methods relating to the super block.
 */

Superblock::Superblock(int size, int blocks){
    blockSize = size;
    numBlocks = blocks;
    inodeMap = 0;
    freeBlockList = 0;
}


