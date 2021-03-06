#include "superblock.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>

using namespace std;

int main(int agrc, char **argv){
    
  //check to make sure there are enough aruments
  int i;
  for(i=0; argv[i] != nullptr; i++)
    ;
  if(i != 4){
    cerr << "Run as:" << endl;
    cerr << "./ssfs_mkdsk <num blocks> <block size> <disk file name>" << endl;
    return 0;
  }
    
  //intialize values from args
  int numBlocks = atoi(argv[1]);
  int blockSize = atoi(argv[2]);

  //check that inputs are valid
  if(blockSize < 128 || blockSize > 512){
    cerr << "block size must be greater than 128 bytes and less than512 bytes" << endl;
    return 0;
  }
  if(numBlocks < 1024 || numBlocks > 128*1024){
    cerr << "block size must be greater than 128 bytes and less than512 bytes" << endl;
    return 0;
  }

  //create super block and create file system
  struct superblock dumb;
  dumb.blockSize = blockSize;
  dumb.numBlocks = numBlocks;
  /*
    superblock no long contains imap and dmap, must include elsewise
      
   
  */
  //create file and write out to it
  FILE * ssfs;
  ssfs = fopen(argv[3], "wb");
  

  //determine remaining size of the file
  int fremain = blockSize * numBlocks;
  char *data = (char*) malloc(fremain);
  for(int i=0; i<fremain; i++){
    data[i] = 0;
  }
  if( ssfs != NULL){
	fseek(ssfs, 0, SEEK_SET);
    fwrite(data, sizeof(char), fremain, ssfs);
	fseek(ssfs, 0, SEEK_SET);
    fwrite(&dumb, sizeof(dumb), 1, ssfs);
    fclose(ssfs);
  }
  else{
    cerr << "cannot create file system :(" << endl;
  }

  /*In Sang added code*/
    
    
  return 0;
}
