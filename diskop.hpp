#ifndef DISKOP_HPP
#define DISKOP_HPP

#include "superblock.h"
#include <iostream>
#include <string>

//Helper function
/*
INODE_T findINode(std::string filename);
*/

struct superblock initSystem(char *fileName);

void create(std::string filename);

void import(std::string filename, std::string unix_filename);

void cat(std::string filename);

//Instruction name "DELETE"
void remove(std::string filename);

void write(std::string filename, char c, int startByte, int numByte);

void read(std::string filename, int startByte, int numByte);

void list(std::string filename);

void shutdown();


#endif
