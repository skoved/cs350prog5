#include "controller.hpp"

using namespace std;

Controller::Controller(std::string filename, unsigned int buffer_size){
  this->buffer_size = buffer_size;

  //initialize shared buffer
  ptr = (int*)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    
  //Memory not allocated
  if(ptr < 0){
    handle_error("shm open");
  }

  fh = fopen(filename.c_str(), "wb+");
  if(fh == NULL){
    handle_error("file open");
  }

  //Rewind to locate 0, not necessary here, but may be necessary elsewhere
  rewind(fh);
  fread(&sb, sizeof(superblock_t), 1, fh);
}

int create(string filename);

int import(string filename, string unix_filename);

int cat(string filename);

int remove(string filename);

int write(string fielname, char c, int startByte, int numByte);

std::vector<std::string> read(string filename, int startByte, int numByte);

std::vector<std::string> list(string filename);

int shutdown(struct superblock fileSys, string filename);
