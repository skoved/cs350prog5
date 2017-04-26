#include "diskop.hpp"

#define MAX_INODE 256

using namespace std;

//Me
void create(std::string filename){
  //Check that filename already exist
  if(filename.length() > 32){
    cerr << "File name is too long" << endl;
  }



  if( 1 /* inode_number == MAX_INODE */){
    cerr << "Not enough inodes to create new files" << endl;
  }


  
  bool index[MAX_INODE];
  int inode_map_value = 0;
  /*  
      inode_map_value = ssfs_inode_map_value;
   */

  for(unsigned int i = 0; i < MAX_INODE; i++){
    index[i] = ((((unsigned int)1) << i) & inode_map_value > 0)?true:false;
  }


  /*
    Some code to get the index of in-use inodes
  */

  
  
}

void import(std::string filename, std::string unix_filename){
  
}


//Me
void cat(std::string filename){
  
}

void remove(std::string filename){
  
}


//Me
void write(std::string filename, char c, int startBytes, int numBytes){

}
void read(std::string filename, int startBytes, int numBytes){

}

//Me
void list(){
  //Step through the inodes, and read out name and size
}

void shutdown(){

}
