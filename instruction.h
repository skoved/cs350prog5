#ifndef INSTRUCTION_H
#define INSTRUCTION_H

struct instruction{
    int op;
    char *ssfsFile;
    char *unixFile;
    char letter;
    int startByte;
    int numBytes;
}

#endif
