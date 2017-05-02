#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>

#include "superblock.h"

#define handle_error(msg)			\
  do{						\
    perror(msg);				\
    exit(EXIT_FAILURE);				\
  } while(0)

#define SIZE (sizeof(unsigned int) * buffer_size)

class Controller{
private:
  //Current instruction pointer [][][][][][x][][][]
  unsigned int current_instr;
  unsigned int buffer_size;
  int* ptr;
  File* fh;
  superblock sb;
  
public:
  Controller(std::string filename; unsigned int buffer_size){
    this->buffer_size = buffer_size;

    //initialize shared buffer
    ptr = (int*)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    
    //Memory not allocated
    if(ptr < 0){
      handle_error("shm open");
    }

    fh = fopen(filename, "wb+");
    if(fh == NULL){
      handle_error("file open");
    }

    //Rewind to locate 0, not necessary here, but may be necessary elsewhere
    frewind();
    fread(&sb, sizeof(superblock_t), 1, fh);

  };
};


#endif
