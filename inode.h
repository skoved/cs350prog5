#ifndef INODE_H
#define INODE_H

/*
 * Inode struct
 */

struct inode{
    char fileName[33];
    int fileSize;
    int ptrs[12];
    int indirect;
    int dindirect;
}

typedef struct inode inode_t;

void inode_init(inode_t *node, char *name){
    node->fileName = name;
    node->fileSize = 0;
    for(int i = 0; i<12; i++){
        node->ptrs[i] = 0;
    }
    node->indirect = 0;
    node->dindirect = 0;
}

#endif
