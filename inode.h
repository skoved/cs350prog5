#ifndef INODE_H
#define INODE_H

/*
 * Inode struct
 */

struct inode{
    char fileName[33];
    int fileSize;
    int ptrs[12];
    unsigned int indirect;
    unsigned int dindirect;
};

typedef struct inode inode_t;
void inode_init(inode_t *node, const char *name);

#endif
