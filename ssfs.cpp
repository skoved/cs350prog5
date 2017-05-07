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
#include "parse.hpp"


using namespace std;

int main(int argc, char **argv){
    if(argc != 3){
        printf("Run as ./ssfs <disk file> <threadop file>\n");
        return 0;
    }
    ifstream infile(argv[2]);
    if(infile.is_open()){
        string line;
        while(getline(infile, line){
            vector<string> cmd = parse::split(line, ' ');
            if(cmd[0] == "create"){
                disk.create(cmd[1]);
            }
            else if(cmd[0] == "write"){
                disk.write(cmd[1], cmd[2][0], stoi(cmd[3]), stoi(cmd[4])); 
            }
            else if(cmd[0] == "read"){
                disk.read(cmd[1], stoi(cmd[2]), stoi(cmd[3]));
            }
            else if(cmd[0] == "cat"){
                disk.cat(cmd[1]);
            }
            else if(cmd[0] == "delete"){
                disk.remove(cmd[1]);
            }
            else if(cmd[0] == "list"){
                disk.list()
            }
            else if(cmd[0] == "shutdown"){
                disk.shutdown();
            }
        }
        infile.close();
    }
    return 0;
}

