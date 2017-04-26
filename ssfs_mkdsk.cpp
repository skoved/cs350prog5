#include "superblock.hpp"
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

int main(int agrc, char **argv){
    
    //check to make sure there are enough aruments
    int i;
    for(i=0; argv[i] != nullptr; i++)
        ;
    if(i != 4){
        cerr << "ssfs_mkdsk <num blocks> <block size> <disk file name>" << endl;
        return 0;
    }
    
    //intialize values from args
    int numBlocks = atoi(argv[1]);
    int blockSize = atoi(argv[2]);
    string fileName(argv[3]);
    
    //create super block and create file system
    Superblock dumb(blockSize, numBlocks);
    fstream ssfs;
    ssfs.open(fileName);
    Superblock test(0,0);

    if(ssfs.is_open()){
        ssfs << dumb;
        ssfs >> test;
    }
    else{
        cerr << "cannot create file system :(" << endl;
    }
    return 0;
}
