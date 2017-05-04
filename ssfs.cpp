#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <vector>
#include <cstring>
#include <cstdint>
#include "user_model.hpp"
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

    //open file
    FILE *ssfs = fopen(argv[1], "rb+");
    if(ssfs == NULL){
        perror("ABORT\n");
        return 0;
    }

    superblock_t sb;
    fread(&sb, sizeof(superblock_t), 1, ssfs);
    

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
