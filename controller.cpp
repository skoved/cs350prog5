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
  
  char* imap = (char*)malloc(sizeof(char) * B_SIZE);
  char* dmap = (char*)malloc(sizeof(char) * B_SIZE);
  for(unsigned int i = 0; i < B_SIZE; i++){
    imap[i] = 0;
    dmap[i] = 0;
  }

  fseek(fh, B_SIZE * IMAP_POS, SEEK_SET);
  fread(&iMap, sizeof(char), B_SIZE, fh);
  fseek(fh, B_SIZE * DMAP_POS, SEEK_SET);
  fread(&dMap, sizeof(char), B_SIZE, fh);
	
  this->writeBlock(IMAP_POS, imap);
  this->writeBlock(DMAP_POS, dmap); 
  this->iMap = imap;
  this->dMap = dmap;
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
  	this->writeBlock(IMAP_POS,this->iMap);	
	
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
  unsigned int start_byte = 0;
  unsigned int num_block = inode.fileSize;

  if(num_block == 0){
    return 1;
  }
  int readPos = 0;
  for(unsigned int i = cal_start; i < 12; i++){
    if(this->readBit(this->dMap, inode.ptrs[i])){
      this->readBlock(inode.ptrs[i], data_block);
	  for(; readPos<inode.fileSize && readPos<sb.blockSize; readPos++)
      	cout << data_block[readPos%sb.blockSize];
		
    }else{
      break;
    }
  }  
  
  return 1;
}

int Controller::remove(string filename){
  return -1;
}

int Controller::write(string filename, char c, int startByte, int numByte){
	int filePos = this->findPosition(filename);
	inode_t currentBlock;
	fseek(fh, filePos * B_SIZE, SEEK_SET);	
  	fread(&currentBlock, sizeof(inode_t), 1, fh);
	vector<int> allocatedDataBlocks;
	
	//if data not allocated
	for(int i = 0; i < 12 ; i++){
		if(currentBlock.ptrs[i] != 0){
			allocatedDataBlocks.push_back(i);
		}	
	}
	
	if(allocatedDataBlocks.size() == 0 && currentBlock.indirect == 0 && startByte == 0){
		int bytesWritten = 0;
		int dBlock;
		unsigned int index = 0;
		int ptrsIndex = 0;
		while(bytesWritten < numByte){
			dBlock = this->findBlock();
			if(dBlock == -1){
				return -1;			
			}
			char* data = (char*) malloc(BYTE*B_SIZE);
			this->readBlock(dBlock, data);
			while(index < B_SIZE && bytesWritten < numByte){
				data[index] = c;
				bytesWritten++;
				index++;
				currentBlock.fileSize++;
					
			} 
			this->setBit(dMap, dBlock, 1);
			this->writeBlock(DMAP_POS,this->dMap);
			fseek(fh, dBlock * B_SIZE, SEEK_SET);
 			fwrite(data, sizeof(char), numByte, fh);
			currentBlock.ptrs[ptrsIndex] = dBlock;
			fseek(fh, B_SIZE*filePos, SEEK_SET);
			cout << "FILE SIZE: " << currentBlock.fileSize;
			fwrite(&currentBlock, sizeof(currentBlock), 1, fh);
			index = 0;		
		}
	}
	else{
		cout << "FILE SIZE: " << currentBlock.fileSize;
		if(startByte > currentBlock.fileSize){
			cout << "Exit" << endl;
			return -1;			
		}
		int offset = startByte % sb.blockSize;
		int blockNumber = startByte/sb.blockSize;
		int ptrsIndex = -1;
		if(blockNumber < 12){
			blockNumber = allocatedDataBlocks[blockNumber];
			int bytesWritten = 0;
			int dBlock = blockNumber;
			unsigned int index = offset;
			while(bytesWritten < numByte){
				char* data = (char*) malloc(BYTE*B_SIZE);
				this->readBlock(dBlock, data);
				while(index < B_SIZE && bytesWritten < numByte){
				
					data[index] = c;
					bytesWritten++;
					index++;
					currentBlock.fileSize++;
					
				}
				this->setBit(iMap, dBlock, 1);
				this->writeBlock(DMAP_POS,this->dMap);
				fseek(fh, dBlock * B_SIZE, SEEK_SET);
	 			fwrite(data, sizeof(char), numByte, fh);	
				for(int i = 0; i < 12; i++){
					if(currentBlock.ptrs[i] == 0){
						ptrsIndex = i;					
					}
				}
				if(index >= B_SIZE && bytesWritten < numByte && ptrsIndex != -1){
					dBlock = this->findBlock();
					if(dBlock == -1){
						cout << "Exit2" << endl;
						return -1;			
					}
					offset = 0;
					currentBlock.ptrs[ptrsIndex] = dBlock; 	
				}
				else{
									
				}	
			}			
		}
	}
	//if data is allocated
	
	
	
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

int Controller::findBlock(){
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

  unsigned int cal_start = (unsigned int)startByte/B_SIZE;
  unsigned int start_byte = (unsigned int)startByte % B_SIZE;
  unsigned int num_block = (unsigned int)numByte/B_SIZE;

  
  if((numByte % B_SIZE) > 0){
    num_block++;
  }

  if(cal_start > D_POINTER || inode.fileSize < (int)(cal_start + num_block)){
    return -1;
  }
  
  for(unsigned int i = cal_start; i < cal_start + num_block && i < D_POINTER; i++){
    if(this->readBit(this->dMap, inode.ptrs[i])){
      this->readBlock(inode.ptrs[i], data_block);
      cout << data_block[start_byte++];
      start_byte %= B_SIZE;
    }else{
      break;
    }
  }  
  
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
int Controller::writeBlock(unsigned int block_pos, char* data){
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

int Controller::readBlock(unsigned int block_pos, char* data){
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
