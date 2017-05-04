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

#define B_SIZE (unsigned int)sb.blockSize
#define BYTE sizeof(char)
#define INODE_MAX 256
#define NAME_SIZE 32
#define handle_error(msg)			\
  do{						\
    perror(msg);				\
    exit(EXIT_FAILURE);				\
  } while(0)

#define SUPER_POS 0
#define IMAP_POS 1
#define DMAP_POS 2

#define SIZE (sizeof(unsigned int) * buffer_size)

class Controller{
private:
  //Current instruction pointer [][][][][][x][][][]
  unsigned int current_instr;
  unsigned int buffer_size;
  int* ptr;
  FILE * fh;
  superblock sb;
  char* iMap;
  char* dMap;
  
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

  bool readBit(char* a, unsigned int bit_pos);
  
  void setBit(char* a, unsigned int bit_pos, bool set_value);
  
  int writeBlock(unsigned int block_pos, char* data);
  
  int readBlock(unsigned int block_pos, char* data);
};
#endif
