#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


typedef struct BitWriter {
    FILE *file; // Output file with
    unsigned char buffer;
    int bitCount;
} BitWriter;

void initBitWriter(BitWriter *bitWriter, char *outputFileName);

void writeCharCount(BitWriter *bitWriter, int count); //TODO - předělat na uint8
void writeTable(BitWriter *bitWriter, unsigned char character, char *code, int codeLength);
void writeBit(BitWriter *bitWriter, int bit);
void writeBitFromChar(BitWriter *bitWriter, char bit);

void flushBits(BitWriter *bitWriter);
void closeBitWriter(BitWriter *bitWriter);

typedef struct BitReader {
    FILE *file;
    unsigned char buffer;  // 1 byte of data
    int bitCount;          // How many bits are left to read from buffer
} BitReader;

void initBitReader(BitReader *br, const char *filename);
/* Dokud je co číst, tak vrací 1. Pokud již není, tak je vrácena 0 a čtení by mělo být zastaveno. */
int readBit(BitReader *br, int *bit);
int readTable(BitReader *br, unsigned char *character, char **code);

void closeBitReader(BitReader *br);