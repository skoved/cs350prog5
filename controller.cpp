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

  //Insert imap, insert dmap (in that order)
  fseek(fh, B_SIZE, SEEK_SET);
  
  
  
  
}

int Controller::create(string filename){
  return -1;
}

int Controller::import(string filename, string unix_filename){
  return -1;
}

int Controller::cat(string filename){
  return -1;
}

int Controller::remove(string filename){
  return -1;
}

int Controller::write(string fielname, char c, int startByte, int numByte){
  return -1;
}

vector<string> Controller::read(string filename, int startByte, int numByte){
  return vector<string>();
}

vector<string> Controller::list(string filename){
  return vector<string>();
}

int Controller::shutdown(struct superblock fileSys, string filename){
  return -1;
}

bool Controller::readBit(char* a, unsigned int bit_pos){
  
  return false;
}

void Controller::setBit(char* a, unsigned int bit_pos, bool set_value){
  return;
}


//Assume data points to a char array of size blockSize
int Controller::writeBlock(FILE* fh, unsigned int block_pos, char* data){
  if(data == NULL || fh == NULL){
    return -1;
  }
  
  //Rewind to start of ssfs
  rewind(fh);

  //Insert imap, insert dmap (in that order)
  fseek(fh, B_SIZE * block_pos, SEEK_SET);
  
  fwrite(data, B_SIZE, sizeof(char), fh);

  return 1;
}

int Controller::readBlock(FILE* fh, unsigned int block_pos, char* data){
  if(data == NULL || fh == NULL){
    return -1;
  }
  //Rewind to start of ssfs
  rewind(fh);

  //Insert imap, insert dmap (in that order)
  fseek(fh, B_SIZE * block_pos, SEEK_SET);
  
  fread(data, B_SIZE, sizeof(char), fh);
  return 1;
}
