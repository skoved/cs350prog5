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

  fh = fopen(filename.c_str(), "rb+");
  if(fh == NULL){
    handle_error("file open");
  }

  //Rewind to locate 0, not necessary here, but may be necessary elsewhere
  rewind(fh);

  
  fread(&sb, sizeof(superblock_t), 1, fh);

  //Insert imap, insert dmap (in that order)
  fseek(fh, B_SIZE, SEEK_SET);
  
  this->iMap = (char*)malloc(BYTE * B_SIZE);
  this->dMap = (char*)malloc(BYTE * B_SIZE);

  this->readBlock(this->iMap, IMAP_POS);
  this->readBlock(this->dMap, DMAP_POS);

  char* dup_iMap = (char*)malloc(BYTE * B_SIZE);
  this->readBlock(dup_iMap, IMAP_POS);
  cout << "Check saved on recreation " << this->readBit(dup_iMap, 3)  << endl;
}

int Controller::create(string filename){
  if(filename.length() > 32)
    return -1;
  inode_t currentBlock;
  int firstEmptyINode = 0;
  bool isEqual = false;
  for(int i = 3; i < INODE_MAX; i++){
    fseek(fh, i * B_SIZE, SEEK_SET);
    if(this->readBit(this->iMap, i)){
      fread(&currentBlock, sizeof(inode_t), 1, fh);
      string s(currentBlock.fileName);
      if(s.compare(filename) == 0)
	isEqual = true;
    }
    if(isEqual){		
      return -1;	
    }
    
    if(this->readBit(this->iMap, i) == 0 && firstEmptyINode == 0){
      firstEmptyINode = i;		
    }
  }		
  if(firstEmptyINode == 0){
    return -1;
  }
  createInode(filename.c_str(), firstEmptyINode);
  this->setBit(iMap, firstEmptyINode, 1);
  this->writeBlock(this->iMap, IMAP_POS);

  
  return 1;
}

int Controller::import(string filename, string unix_filename){
  return -1;
}


int Controller::cat(string filename){
  int inode_pos = this->findPosition(filename);
  inode_t inode;

  fseek(fh, inode_pos * B_SIZE, SEEK_SET);
  fread(&inode, sizeof(inode_t), 1, fh);
  rewind(fh);

  char* data_block = (char*)malloc(BYTE * B_SIZE);

  unsigned int cal_start = 0;
  unsigned int num_block = inode.fileSize;


  if(num_block == 0){
    return 1;
  }
  
  int readPos = 0;
  for(unsigned int i = cal_start; i < 12; i++){
    if(this->readBit(this->dMap, inode.ptrs[i])){
      this->readBlock(data_block, inode.ptrs[i]);
      for(; readPos<inode.fileSize && readPos<sb.blockSize; readPos++)
      	cout << data_block[readPos%sb.blockSize];
		
    }else{
      break;
    }
    cout << endl;
  }
  cout << endl;

  
  char* dup_iMap = (char*)malloc(BYTE * B_SIZE);
  this->readBlock(dup_iMap, IMAP_POS);
  cout << "Check formation on cat " << this->readBit(dup_iMap, 3)  << endl;
  
  return 1;
}

int Controller::remove(string filename){
  int filePos = this->findPosition(filename);
  if(filePos == -1){
    return -1;
  }

  inode_t inode;
  fseek(fh, filePos, SEEK_SET);
  fread(&inode, sizeof(inode_t), 1, fh);

  char* empty_block = (char*)malloc(BYTE * B_SIZE);
  for(unsigned int i = 0; i < B_SIZE; i++){
    empty_block[i] = 0;
  }
  
  for(unsigned int i = 0; i < D_POINTER; i++){
    this->setBit(this->dMap, inode.ptrs[i], 0);
    this->writeBlock(empty_block, inode.ptrs[i]);
  }

  this->setBit(this->iMap, filePos, 0);
  return 1;
}

int Controller::write(string filename, char c, int startByte, int numByte){
  int filePos = this->findPosition(filename);
  if(filePos == -1){
    return -1;
  }

  cout << "Before bit set " << this->readBit(this->dMap, 3) << endl;
	
  inode_t inode;
  fseek(fh, filePos * B_SIZE, SEEK_SET);	
  fread(&inode, sizeof(inode_t), 1, fh);
  
  unsigned int index = (unsigned int) startByte/B_SIZE;
  unsigned int end_index = (unsigned int) (startByte + numByte)/B_SIZE;
  unsigned int byte_index = (unsigned int) startByte % B_SIZE;
  unsigned int end_byte_index = (unsigned int) (startByte + numByte) % B_SIZE;

  if(end_index > D_POINTER){
    cout << "OUT OF MEMORY" << endl;
    return -1; 
  }

  for(unsigned int i = index; i < (end_index + (end_byte_index > 0)); i++){
    //Assume position 0 in dMap is always 0
    if(!this->readBit(this->dMap, inode.ptrs[i])){
      inode.ptrs[i] = findEmptyBlock();
      this->setBit(this->dMap, inode.ptrs[i], 1);
    }
  }

  //front
  char* f_block = (char*)malloc(BYTE * B_SIZE);
  if(byte_index > 0){
    this->writeBlock(f_block,index);
  }
  for(unsigned int i = byte_index; i < B_SIZE; i++){
    f_block[i] = c;
  }

  //regular
  char* d_block = (char*)malloc(BYTE * B_SIZE);
  for(unsigned int i = 0; i < B_SIZE; i++){
    d_block[i] = c;
  }

  //end
  char* e_block = (char*)malloc(BYTE * B_SIZE);
  if(end_byte_index > 0){
    this->writeBlock(f_block,index);
  }
  for(unsigned int i = 0; i < end_byte_index; i++){
    e_block[i] = c;
  }

  for(unsigned int i = index; i < (end_index + (end_byte_index > 0)?1:0); i++){
    char* type_block = d_block;
    if(i == index){
      type_block = f_block;
    }else if(i == end_index){
      type_block = e_block;
    }
    this->writeBlock(type_block, inode.ptrs[i]);
  }

  inode.fileSize += numByte;

  fseek(fh, filePos * B_SIZE, SEEK_SET);
  fwrite(&inode, BYTE, B_SIZE, fh);

  cout << "After bit set " << this->readBit(this->dMap, 3) << endl;
  
  return 1;
}

int Controller::findPosition(string filename){
  inode_t currentBlock;
  fseek(fh, B_SIZE * 3, SEEK_SET);
  bool isEqual = false;
  for(int i = 3; i < INODE_MAX; i++){
    fread(&currentBlock, sizeof(inode_t), 1, fh);
    fseek(fh, i * B_SIZE, SEEK_SET);

    if(this->readBit(this->iMap, i)){
      string s(currentBlock.fileName);
      if(s.compare(filename) == 0){
	isEqual = true;
      }	
    }
    if(isEqual){		
      return i;	
    }    
  }	

  return -1;
}

int Controller::findEmptyBlock(){
  for(int i = 259; i < sb.numBlocks; i++){
    if(!this->readBit(this->dMap, i)){
      return i;		
    }
  }
  return -1;
}



int Controller::read(string filename, int startByte, int numByte){
  int inode_pos = this->findPosition(filename);
  inode_t inode;

  fseek(fh, inode_pos * B_SIZE, SEEK_SET);
  fread(&inode, sizeof(inode_t), 1, fh);
  rewind(fh);

  char* data_block = (char*)malloc(BYTE * B_SIZE);

  unsigned int index = (unsigned int)startByte/B_SIZE;
  unsigned int byte_index = (unsigned int)startByte % B_SIZE;
  unsigned int num_block = 1;

  this->readBlock(data_block, index);
  for(unsigned int i = startByte; i < startByte + numByte; i++){
    cout << data_block[i%B_SIZE];
    if((i % B_SIZE) == 0){
      this->readBlock(data_block, index + num_block++);
    }
  }
  cout << endl;
  
  return 1;

}

int Controller::list(){
  inode_t inode;
  for(unsigned int i = 0; i < INODE_MAX; i++){
    if(this->readBit(this->iMap, i)){
      fseek(fh, i * B_SIZE, SEEK_SET);
      fread(&inode, sizeof(inode_t), 1, fh);
      rewind(fh);
      string filename = "";
      for(unsigned int j = 0; j < 33 && inode.fileName[j] != '\0'; j++){
	filename.insert(filename.end(), inode.fileName[j]);
      }
      cout << "File " << filename << " of size: " << inode.fileSize << endl;
    }
  }
  
  return 1;
}

int Controller::shutdown(){
  this->writeBlock(this->iMap, IMAP_POS);
  this->writeBlock(this->dMap, DMAP_POS);
  fclose(fh);
  return -1;
}

void Controller::createInode(const char* name, int index){
  inode_t node;
  inode_init( &node, name);
  fseek(fh, index * B_SIZE, SEEK_SET);
  fwrite(&node, sizeof(node), 1, fh);
	 
}


bool Controller::readBit(char* a, unsigned int bit_pos){
  unsigned int index = (unsigned int) bit_pos/8;
  unsigned int bit_index = bit_pos % 8;
  if(index > B_SIZE){
    return false;
  }
  if(a[index] & (1 << bit_index)){
    return true;
  }
  return false;
}

void Controller::setBit(char* a, unsigned int bit_pos, bool set_value){
  unsigned int index = (unsigned int) bit_pos/8;
  unsigned int bit_index = bit_pos % 8;
  if(index > B_SIZE){
    return;
  }
  if(set_value){
    a[index] |= (1 << bit_index);
  }else{
    a[index] &= ~(1 << bit_index);
  }
}


//Assume data points to a char array of size blockSize
int Controller::writeBlock(char* data, unsigned int block_pos){
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

int Controller::readBlock(char* data, unsigned int block_pos){
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
