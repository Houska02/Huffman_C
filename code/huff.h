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
    bool customFrequencies; // Zda byly ručně upraveny četnosti znaků

    int strLen; // Dělka textu (ukládá se a poté se používá při dekomrpesi na kontrolu zda je program už nakonci)

    char *importFileName;
    char *outputFileName;

    int *count; //Tabulka četností znaků v textu
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
void compressIntoFile(Huffman* self);

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

void modifyCountTable(Huffman* self);

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
/* Function that will import saved huffman table from a file.
    uniqueCharCount - Returns value of unique characters in table (basically number of rows in saved table)
    strLen - Returns a length of saved text
    Returns Array with size of ASCII characters and values that represents binary codes of key character
*/
char** importTable(BitReader *br, int *uniqueCharCount, int *strLen);
void saveTo(Huffman *self);

/* Function taht will decode text from .huff file.
    This function will also save text into a .txt file if necessary.
*/
void decode(Huffman *self, BitReader *br, int uniqueCharCount);