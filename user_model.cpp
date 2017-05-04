#include "user_model.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;
/*
	0. Create
	1. Import
	2. Cat
	3. Delete
	4. Write
 */
void *user_model(void * arg){
	char* argument = (char*) arg[1];
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

