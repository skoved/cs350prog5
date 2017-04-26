#ifndef DISKOP_HPP
#define DISKOP_HPP
#include <string>

void create(std::string filename);

void import(std::string filename, std::string unix);

void cat(std::string filename);

void delete(std::string filename);

void write(std::string filename, char c, int startByte, int numByte);

void read(std::string filename, int startByte, int numByte);

void list(std::string filename);

void shutdown();


#endif
