#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "bitHandler.h"

#define ASCII_SIZE 256

typedef struct Huffman {

    bool fromFile;
    char *inputText;

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
//TODO Huffman* initHuffmanFromTable(int *count); // Tabulka četností/pravděpodobnostní tabulka

// Decompression from a .huff file
Huffman* initHuffmanFromBinary(char *importFileName, char *outputFileName); //Decompression

/* COMPRESSION */
void compress(Huffman* self); // Jen vypíše do konzole
void compressIntoFile(Huffman* self); // Uloží do souboru
void simpleProcess(Huffman* self); // Jen zakóduje znaky a vypíše jejich hodnoty

// Simple print of input text, char-code and compressed text into a command window
void printResults(Huffman* self);

int* countCharacters(const char *inputText);
int* processFile(char *fileName);

/* DECOMPRESSION */
void decompress(Huffman* self);





typedef struct {
    /* Informace o četnosti těchto charakterů v textu
    */
    int count;
    /*Odpoví délce charakterů.
    S číslováním se začíná od 1
    */
    int charLength;
    char* znaky;
} Node;

char** createTable(int *count);
// count - Počet znaků z ASCII, které mají nějaký kód
char** importTable(BitReader *br, int *count);

void saveTo(Huffman *self);

void decodeToFile(Huffman *self, BitReader *br, int count);