#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    FILE *file;             // Output file
    unsigned char buffer;   // Bit buffer (1 byte = 8 bits)
    int bitCount;           // How many bits are currently stored in buffer
} BitWriter;
void initBitWriter(BitWriter *bw, const char *filename) {
    bw->file = fopen(filename, "wb");
    bw->buffer = 0;
    bw->bitCount = 0;
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

void flushBits(BitWriter *bw) {
    if (bw->bitCount > 0) {
        bw->buffer <<= (8 - bw->bitCount);  // Pad with 0s
        fwrite(&bw->buffer, 1, 1, bw->file);
    }
}
void closeBitWriter(BitWriter *bw) {
    flushBits(bw);
    fclose(bw->file);
}
int main() {
    BitWriter bw;
    initBitWriter(&bw, "example.huff");

    // Write bits: 00000000 → 1 byte
    writeBit(&bw, 1);
    writeBit(&bw, 0);
    writeBit(&bw, 1);
    writeBit(&bw, 1);
    writeBit(&bw, 0);
    writeBit(&bw, 0);
    writeBit(&bw, 1);
    writeBit(&bw, 1);

    // Add more bits (this will go into the next byte)
    writeBit(&bw, 1);
    writeBit(&bw, 0);

    closeBitWriter(&bw);

    printf("Bit stream written to example.huff\n");
    return 0;
}
