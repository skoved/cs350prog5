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
}


int Controller::create(string filename){

    if(filename.length() > 32){
        return -1;
    }

    inode_t currentBlock;
    int empty_inode = 0;


    //Start after DMAP blocks
    for(int i = INODE_POS; i < INODE_MAX; i++){
        fseek(fh, i * B_SIZE, SEEK_SET);

        if(this->readBit(this->iMap, i)){
            fread(&currentBlock, sizeof(inode_t), 1, fh);
            string s(currentBlock.fileName);
            if(s.compare(filename) == 0){
                return -1;
            }
        }else if(empty_inode == 0){
            empty_inode = i;
        }
    }		

    if(empty_inode <= 2){
        cout << "No more inode space" << endl;
        return -1;
    }

    createInode(filename.c_str(), empty_inode);
    this->setBit(iMap, empty_inode, 1);
    this->writeBlock(this->iMap, IMAP_POS);

    return 1;
}

int Controller::import(string filename, string unix_filename){
    int file_pos = this->findPosition(filename);
    if(file_pos <= 0){
        return -1;
    }

    inode_t inode;
    fseek(fh, file_pos * B_SIZE, SEEK_SET);
    fread(&inode, BYTE, B_SIZE, fh);

    return 1;
}


int Controller::cat(string filename){
  int inode_pos = this->findPosition(filename);
  if(inode_pos == -1){
    return -1;
  }
    inode_t inode;

    fseek(fh, inode_pos * B_SIZE, SEEK_SET);
    fread(&inode, sizeof(inode_t), 1, fh);
    
    this->read(filename, 0, inode.fileSize);
    
    return 1;
}

int Controller::remove(string filename){
    int filePos = this->findPosition(filename);
    if(filePos <= 0){
        return -1;
    }

    inode_t inode;
    fseek(fh, filePos * B_SIZE, SEEK_SET);
    fread(&inode, sizeof(inode_t), 1, fh);

    char* empty_block = (char*)malloc(BYTE * B_SIZE);
    for(unsigned int i = 0; i < B_SIZE; i++){
        empty_block[i] = 0;
    }

    for(unsigned int i = 0; i < D_POINTER; i++){
        if(inode.ptrs[i] != 0){
            this->setBit(this->dMap, inode.ptrs[i], 0);
            this->writeBlock(empty_block, inode.ptrs[i]); 
        }
    }

    this->setBit(this->iMap, filePos, 0);
    this->writeBlock(this->iMap, IMAP_POS);

    free(empty_block);
    return 1;
}

int Controller::write(string filename, char c, int startByte, int numByte){
    int filePos = this->findPosition(filename);
    if(filePos <= 0){
        return -1;
    }

    inode_t inode;
    fseek(fh, filePos * B_SIZE, SEEK_SET);	
    fread(&inode, sizeof(inode_t), 1, fh);
    
    if(startByte > inode.fileSize)
        return -1;

    unsigned int index = (unsigned int) startByte/B_SIZE;
    unsigned int end_index = (unsigned int) (startByte + numByte)/B_SIZE;
    unsigned int byte_index = (unsigned int) startByte % B_SIZE;
    unsigned int end_byte_index = (unsigned int) (startByte + numByte) % B_SIZE;

    if((end_index) >= D_POINTER){
        cerr << "OUT OF MEMORY" << endl;
        return -1; 
    }

    unsigned int offset = 0;
    if(end_byte_index > 0){
      offset = 1;
    }
    
    for(unsigned int i = index; i < (end_index + offset); i++){
        //Assume position 0 in dMap is always 0
        if(!this->readBit(this->dMap, inode.ptrs[i])){
            inode.ptrs[i] = findEmptyBlock();
            this->setBit(this->dMap, inode.ptrs[i], 1);
        }
    }

    //front
    char* f_block = (char*)malloc(B_SIZE * sizeof(*f_block));
    if(byte_index > 0){
        this->readBlock(f_block,inode.ptrs[index]);
    }
    unsigned int stop = B_SIZE;
    if(end_index == index){
        stop = numByte + byte_index;
    }
    for(unsigned int i = byte_index; i < stop; i++){
        f_block[i] = c;
    }
    for(unsigned int i = stop; i < B_SIZE; i++){
        f_block[i] = '\0';
    }
    //regular
    char* d_block = (char*)malloc(B_SIZE * sizeof(*d_block));
    for(unsigned int i = 0; i < B_SIZE; i++){
        d_block[i] = c;
    }
    
    //end
    char* e_block = (char*)malloc(BYTE * B_SIZE);
    if(end_byte_index > 0){
        this->readBlock(e_block,inode.ptrs[end_index]);
    }
    for(unsigned int i = 0; i < end_byte_index; i++){
        e_block[i] = c;
    }
    for(unsigned int i = end_byte_index; i < B_SIZE; i++){
      e_block[i] = '\0';
    }

    for(unsigned int i = index; i < (end_index + offset); i++){
      char* type_block = d_block;
      if(i == index && byte_index > 0){
            type_block = f_block;
        }else if(i == end_index){
            type_block = e_block;
        }
        if(inode.ptrs[i] != 0){
            this->writeBlock(type_block, inode.ptrs[i]);
        }
    }

    inode.fileSize = ((startByte + numByte) > inode.fileSize)?startByte+numByte:inode.fileSize;

    free(f_block);
    free(d_block);
    free(e_block);
    
    fseek(fh, filePos * B_SIZE, SEEK_SET);
    fwrite(&inode, sizeof(inode_t), 1, fh);

    return 1;
}


int Controller::read(string filename, int startByte, int numByte){
    int inode_pos = this->findPosition(filename);
    if(inode_pos == -1){
      return -1;
    }
    inode_t inode;

    fseek(fh, inode_pos * B_SIZE, SEEK_SET);
    fread(&inode, sizeof(inode_t), 1, fh);
    rewind(fh);
    

    unsigned int index = (unsigned int)startByte/B_SIZE;
    //unsigned int byte_index = (unsigned int)startByte % B_SIZE;
    unsigned int num_block = 0;

    if(startByte + numByte > inode.fileSize){
      return -1;
    }
    
    char* data_block = (char*)malloc(B_SIZE * sizeof(*data_block));

    if(inode.ptrs[index] == 0 || !this->readBit(this->dMap, inode.ptrs[index])){
      free(data_block);
      return -1;
    }
    this->readBlock(data_block, inode.ptrs[index]);
    for(unsigned int i = startByte; i < (unsigned int)startByte + numByte; i++){

      cout << data_block[i%B_SIZE];

      if((i % B_SIZE) == 0 && i != 0){
	if((inode.ptrs[index + num_block] == 0) || !this->readBit(this->dMap, inode.ptrs[index + num_block])){
	  free(data_block);
	  cout << "Prematurely reached end of file"  << endl;
	  return -1;
	}
	num_block++;
	this->readBlock(data_block, inode.ptrs[index + num_block]);
      }
    }
    cout << endl;


    free(data_block);
    
    return 1;

}

int Controller::list(){
    inode_t inode;
    for(unsigned int i = 0; i < INODE_MAX; i++){
        if(this->readBit(this->iMap, i)){
            fseek(fh, i * B_SIZE, SEEK_SET);
            fread(&inode, sizeof(inode_t), 1, fh);
            rewind(fh);
            string filename(inode.fileName);
            cout << "File " << filename << " of size: " << inode.fileSize << endl;
        }
    }

    return 1;
}

int Controller::shutdown(){
    this->writeBlock(this->iMap, IMAP_POS);
    this->writeBlock(this->dMap, DMAP_POS);
    free(this->iMap);
    free(this->dMap);
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
    if(bit_pos == 0)
        return false;
    unsigned int index = (unsigned int) bit_pos/8;
    unsigned int bit_index = (unsigned int) bit_pos % 8;
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
        char temp = 0;
        switch(bit_index){
            case 0:
                temp = a[index] | 0b00000001;
                a[index] = temp;
                break;
            case 1:
                temp = a[index] | 0b00000010;
                a[index] = temp;
                break;
            case 2:
                temp = a[index] | 0b00000100;
                a[index] = temp;
                break;
            case 3:
                temp = a[index] | 0b00001000;
                a[index] = temp;
                break;
            case 4:
                temp = a[index] | 0b00010000;
                a[index] = temp;
                break;
            case 5:
                temp = a[index] | 0b00100000;
                a[index] = temp;
                break;
            case 6:
                temp = a[index] | 0b01000000;
                a[index] = temp;
                break;
            case 7:
                temp = a[index] | 0b10000000;
                a[index] = temp;
                break;
        }


    }else{
        char temp = 0;
        switch(bit_index){
            case 0:
                temp = a[index] & 0b11111110;
                a[index] = temp;
                break;
            case 1:
                temp = a[index] & 0b11111101;
                a[index] = temp;
                break;
            case 2:
                temp = a[index] & 0b11111011;
                a[index] = temp;
                break;
            case 3:
                temp = a[index] & 0b11110111;
                a[index] = temp;
                break;
            case 4:
                temp = a[index] & 0b11101111;
                a[index] = temp;
                break;
            case 5:
                temp = a[index] & 0b11011111;
                a[index] = temp;
                break;
            case 6:
                temp = a[index] & 0b10111111;
                a[index] = temp;
                break;
            case 7:
                temp = a[index] & 0b01111111;
                a[index] = temp;
                break;
        }
    }
    return;
}


//Assume data points to a char array of size blockSize
int Controller::writeBlock(char* data, unsigned int block_pos){
    if(data == NULL || fh == NULL || block_pos == 0){
        return -1;
    }

    //Rewind to start of ssfs
    rewind(fh);

    //Insert imap, insert dmap (in that order)
    fseek(fh, B_SIZE * block_pos, SEEK_SET);

    fwrite(data, BYTE, B_SIZE, fh);

    return 1;
}

int Controller::readBlock(char* data, unsigned int block_pos){
    if(data == NULL || fh == NULL){
        return -1;
    }

    fseek(fh, B_SIZE * block_pos, SEEK_SET);
    fread(data, BYTE, B_SIZE, fh);
    
    return 1;
}

int Controller::findPosition(string filename){
    inode_t currentBlock;
    bool isEqual = false;
    for(int i = INODE_POS; i < ADDR_START; i++){
        fseek(fh, i * B_SIZE, SEEK_SET);
        fread(&currentBlock, sizeof(inode_t), 1, fh);

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
    for(int i = DBLOC_POS; i < MAX_DBLOCK; i++){
        if(!this->readBit(this->dMap, i)){
            return i;		
        }
    }
    return -1;
}
