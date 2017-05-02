#include "user_model.hpp"
#include <iostream>
#include <fstream>
#include <string>

void *user_model(void *arg){
    String fileName(arg[0]);
    ifstream infile(fileName);
    if(infile.is_open()){
        string cmd;
        while(!infile.eof()){
            //read in line and store in vector
        }
    return NULL;
}

