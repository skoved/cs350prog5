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
    string butt = "butts";
    int status = disk.create(butt);
    if (status == -1){
        perror("Create is bad\n");
    }
	
	cout << "FINISHED CREATE" << endl;
	status = disk.write(butt, '5', 2, 1);
	if (status == -1){
        perror("Write is bad\n");
    }
	disk.cat(butt);
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
