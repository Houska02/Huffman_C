#include "bitHandler.h"

void initBitWriter(BitWriter *bitWriter, char *outputFileName) {
    bitWriter->file = fopen(outputFileName, "wb"); // Binary file
    bitWriter->buffer = 0;
    bitWriter->bitCount = 0;
}

void writeCharCount(BitWriter *bitWriter, int count) {
    fwrite(&count, sizeof(int), 1, bitWriter->file);
    char c2 = '|';
    fwrite(&c2, sizeof(char), 1, bitWriter->file);
}

void writeTable(BitWriter *bitWriter, unsigned char character, char *code, int codeLength) {
    char c1 = '-';
    char c2 = '|';
    // |<uChar>-<code>||<uChar>-<code>|
    fwrite(&c2, sizeof(char), 1, bitWriter->file);
    fwrite(&character, sizeof(unsigned char), 1, bitWriter->file);
    fwrite(&c1, sizeof(char), 1, bitWriter->file);
    fwrite(code, sizeof(char), codeLength, bitWriter->file);
    fwrite(&c2, sizeof(char), 1, bitWriter->file);
}

void writeBit(BitWriter *bw, int bit) {
    bw->buffer <<= 1;  // Shifting bits left
    bw->buffer |= (bit & 1);  // Setting a bit value
    bw->bitCount++;

    if (bw->bitCount == 8) { // If all 8 bits were modified then save buffer and reset
        fwrite(&bw->buffer, 1, 1, bw->file);  // Writing buffer into a file
        bw->bitCount = 0;
        bw->buffer = 0; // Setting bac to 00000000
    }
}

void writeBitFromChar(BitWriter *bitWriter, char bit) {
    writeBit(bitWriter, (bit=='1' ? 1 : 0));
}

void flushBits(BitWriter *bitWriter) {
    if (bitWriter->bitCount > 0) {
        bitWriter->buffer <<= (8 - bitWriter->bitCount);  // Shifting bits left if unless all 8 bits have been modified (the rest is 000...)
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

int readTable(BitReader *br, unsigned char *character, char **code) {
    int codeLength = 0;
    *code = calloc(1, sizeof(char));

    int state = 0; 
    char ch;

    while ((ch = fgetc(br->file)) != EOF) {
        switch (state) {
            case 0: // Expecting '|'
                if (ch == '|') {
                    state = 2;
                } else {
                    ungetc(ch, br->file);
                    return 0;
                }
                break;

            case 2: // Reading character
                *character = (unsigned char) ch;
                state = 4;
                break;

            case 4: // Expecting '-'
                if (ch == '-') {
                    state = 6;
                } else {
                    return 0;
                }
                break;

            case 6: // Reading bits until next '|'
                if (ch == '|') {
                    (*code)[codeLength] = '\0';  // Null-terminate
                    return 1;
                }

                // Grow buffer by 1 char + room for null terminator
                char *temp = realloc(*code, codeLength + 2);
                if (!temp) {
                    free(*code);
                    return 0;
                }

                *code = temp;
                (*code)[codeLength] = ch;
                codeLength++;
                (*code)[codeLength] = '\0';  // Always null-terminate after append
                break;

            default:
                return 0;
        }
    }

    return 0;
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
