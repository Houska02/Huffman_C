#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "bitHandler.h"

#define ASCII_SIZE 256

#define FREE_PREFIX_ARRAY_SIZE 20
#define CODE_SIZE 20

typedef struct Huffman {

    bool fromFile;
    char *inputText;

    int strLen;

    char *importFileName;
    char *outputFileName;

    int *count; //Tabulka četností/pravděpodobností
    char **table; //ASCII charaktery | kód
    
    void (*process)(struct Huffman*);

} Huffman;

// Compression from a text
Huffman* initHuffmanFromText(char *inputText, char *outputFileName);
// Compression from a .txt file
Huffman* initHuffmanFromFile(char *inputFile, char *outputFileName);
// Decompression from a .huff file
Huffman* initHuffmanFromBinary(char *importFileName, char *outputFileName); //Decompression


/* COMPRESSION */

// Function that will compress a text and results will be in command window
void compress(Huffman* self);
// Function that will compress a text and results will be saved into a file
void compressIntoFile(Huffman* self); // Uloží do souboru

// Simple print of input text, char-code and compressed text into a command window
void printResults(Huffman* self);

/* Funkce na vytvoření tabulky četností znaků v textu.
    @return Int array with ASCII_SIZE size and text length in strLen variable
*/
int* countCharacters(const char *inputText, int *strLen);
/* Funkce na vytvoření tabulky četností znaků ze souboru .txt.
    @return Int array with ASCII_SIZE size and text length in strLen variable
*/
int* processFile(char *fileName, int *strLen);

/* DECOMPRESSION */
void decompress(Huffman* self);



typedef struct {
    /* Number of times the letter occurs in the text */
    int count;
    /*Odpoví délce charakterů.
    S číslováním se začíná od 1
    */
    int charLength;
    char* znaky;
} Node;

// Funkce na vytvoření kódovací tabulky z tabulky četností znaků
char** createTable(int *count);
// count - Počet znaků z ASCII, které mají nějaký kód
char** importTable(BitReader *br, int *uniqueCharCount, int *strlen);

void saveTo(Huffman *self);

/* Function taht will decode text from .huff file.
    This function will also save text into a .txt file if necessary.
*/
void decode(Huffman *self, BitReader *br, int uniqueCharCount);