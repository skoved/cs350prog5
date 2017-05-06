#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#define MAX_FNAME 32

struct instruction{
    int op;
    char ssfsFile[MAX_FNAME];
    char unixFile[MAX_FNAME];
    char letter;
    int startByte;
    int numBytes;
}

#endif
