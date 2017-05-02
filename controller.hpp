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
#include <vector>

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
  FILE * fh;
  superblock sb;
  
public:
  Controller(std::string filename, unsigned int buffer_size);

  /*
    Disk Operations

    Returns: 
    int to indicate success or fail
  */
  int create(std::string filename);

  int import(std::string filename, std::string unix_filename);

  int cat(std::string filename);

  int remove(std::string filename);

  int write(std::string fielname, char c, int startByte, int numByte);

  std::vector<std::string> read(std::string filename, int startByte, int numByte);

  std::vector<std::string> list(std::string filename);

  int shutdown(struct superblock fileSys, std::string filename);
};


#endif
