#include "bitHandler.h"

void initBitWriter(BitWriter *bitWriter, char *outputFileName){
    bitWriter->file = fopen(outputFileName, "wb"); //Budeme zapisovat binárně
    bitWriter->buffer = 0;
    bitWriter->bitCount = 0;
}

void writeCharCount(BitWriter *bitWriter, int count) { //TODO - předělat na uint8
    fwrite(&count, sizeof(int), 1, bitWriter->file);
    char c2 = '|';
    fwrite(&c2, sizeof(char), 1, bitWriter->file);
}

void writeTable(BitWriter *bitWriter, unsigned char character, char *code, int codeLength) {
    char c1 = '-';
    char c2 = '|';
    // <char_count>|<uChar>-<code>|
    fwrite(&character, sizeof(unsigned char), 1, bitWriter->file);
    fwrite(&c1, sizeof(char), 1, bitWriter->file);
    fwrite(code, sizeof(char), codeLength, bitWriter->file);
    fwrite(&c2, sizeof(char), 1, bitWriter->file);
}

void writeBit(BitWriter *bw, int bit) {
    bw->buffer <<= 1;  // Shift left to make room
    bw->buffer |= (bit & 1);  // Add new bit (just in case it's not 0 or 1)
    bw->bitCount++;

    if (bw->bitCount == 8) {
        fwrite(&bw->buffer, 1, 1, bw->file);  // Write full byte
        bw->bitCount = 0;
        bw->buffer = 0;
    }
}

void writeBitFromChar(BitWriter *bitWriter, char bit) {
    if(bit == '1')
        writeBit(bitWriter, 1);
    else
        writeBit(bitWriter, 0);
}
void flushBits(BitWriter *bitWriter) {
    if (bitWriter->bitCount > 0) {
        bitWriter->buffer <<= (8 - bitWriter->bitCount);  // Pad with 0s
        fwrite(&bitWriter->buffer, 1, 1, bitWriter->file);
    }
}
void closeBitWriter(BitWriter *bitWriter) {
    flushBits(bitWriter);
    fclose(bitWriter->file);
}

/* 
    READER
*/

void initBitReader(BitReader *br, const char *filename) {
    br->file = fopen(filename, "rb");
    br->buffer = 0;
    br->bitCount = 0;
}

int readTable(BitReader *br ) {

}

int readBit(BitReader *br, int *bit) {
    if (br->bitCount == 0) {
        if (fread(&br->buffer, 1, 1, br->file) != 1) {
            return 0; // End of file or error
        }
        br->bitCount = 8;
    }

    *bit = (br->buffer >> 7) & 1; // get the top bit
    br->buffer <<= 1;             // shift left for next read
    br->bitCount--;
    return 1;
}

void closeBitReader(BitReader *br) {
    fclose(br->file);
}
