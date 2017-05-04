#include "inode.h"


void inode_init(inode_t *node, const char *name){
	for(int i = 0; i < 33; i++){
		if(name[i] == '\0'){
			node->fileName[i] = '\0';	
			break;	
		}
		node->fileName[i] = name[i];	
	}
    node->fileSize = 0;
    for(int i = 0; i<12; i++){
        node->ptrs[i] = 0;
    }
    node->indirect = 0;
    node->dindirect = 0;
}
	
