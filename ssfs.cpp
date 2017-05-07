#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <vector>
#include <cstring>
#include <cstdint>
#include <iostream>
#include <fstream>


#include "controller.hpp"
#include "diskop.hpp"
#include "superblock.h"


using namespace std;

void *user_model(void *arg);

int main(int argc, char **argv){
    if(argc < 3 || argc > 6){
        perror("Run as ./ssfs <disk file> <threadops files>\nCan enter up to four thread ops files\n");
        return 0;
    }
    
    string filename(argv[1]);
    Controller disk(filename, 1);
    string butt = "butt";
    int status = 0;
    int run_num = atoi(argv[3]);
    
    switch(run_num){
    case 0:
      status =  disk.create(butt);
      if (status == -1){
	perror("Create is bad\n");
      }
      status = disk.write(butt, 'H', 0, 5);
      if (status == -1){
        perror("Write is bad\n");
      }
      disk.cat(butt);
      disk.read(butt, 0, 5);
      disk.write(butt, 'G', 2, 5);
      
      break;
    case 1:
      butt = "ass";
      status =  disk.create(butt);
      if (status == -1){
	cerr << "Create is bad" << endl;
      }
      status = disk.write(butt, 'A', 0, 5);
      if (status == -1){
        cerr << "Write is bad" << endl;
      }
      disk.cat(butt);
      disk.read(butt, 2, 3);
      
      disk.write(butt, 'S', 2, 5);
      break;
    case 2:
      butt = "big_ass";
      status = 0;
      status = disk.create(butt);
      if(status == -1){
	cerr << "Create is bad" << endl;
      }
      status = 0;
      status = disk.write(butt, 'D', 0, 200);
      if(status == -1){
	cerr << "Write is bad" << endl;
      }
      disk.read(butt, 120, 10);
      break;
    case 3:
      butt = "amicable";
      status = disk.create("cunts");
      if(status == -1){
	cerr << "Create is bad" << endl;
      }
      status = disk.create("fick");
      if(status == -1){
	cerr << "Create is bad" << endl;
      }
      status = disk.create("fickus");
      if(status == -1){
	cerr << "Create is bad" << endl;
      }
      status = disk.create("amicable");
      if(status == -1){
	cerr << "Create is bad" << endl;
      }
      disk.list();
      cout << endl;
      disk.remove("fick");
      disk.remove("fickus");
      disk.remove("cunts");
      break;
    case 4:
      butt = "amicable";
      status = disk.create("cunts");
      if(status == -1){
	cerr << "Create is bad" << endl;
      }
      status = disk.create("fick");
      if(status == -1){
	cerr << "Create is bad" << endl;
      }
      status = disk.create("fickus");
      if(status == -1){
	cerr << "Create is bad" << endl;
      }
      status = disk.create("amicable");
      if(status == -1){
	cerr << "Create is bad" << endl;
      }
      disk.list();
      cout << endl;
      disk.remove("fick");
      disk.remove("fickus");
      disk.remove("cunts");
      disk.list();
      cout << endl;
      disk.create("pls");
      disk.create("cunts");
    }
    disk.cat(butt);    
    disk.list();

    disk.shutdown();
    

    
    
    

    return 0;
}

/*
  0. Create
  1. Import
  2. Cat
  3. Delete
  4. Write
*/
void *user_model(void * arg){
  char* argument = (char*) arg;
  string fileName(argument);
  ifstream inFile(fileName);
  if(inFile.is_open()){
    string cmd;
    vector<vector<string>> parsedCommands;
    vector<string> command;  
    char split_char = ' ';      
    while(getline(inFile, cmd)){
      //read in line and store in vector
      istringstream split(cmd);
      command.clear();
      for(string each; getline(split, each, split_char); command.push_back(each));
      parsedCommands.push_back(command);
    }
  }
  return NULL;
}
