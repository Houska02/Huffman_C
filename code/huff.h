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

Huffman* initHuffmanFromText(char *inputText, char *outputFileName);
Huffman* initHuffmanFromFile(char *inputFile, char *outputFileName);

Huffman* initHuffmanFromBinary(char *inputValues); //TODO
Huffman* initHuffmanFromTable(int *count); // Tabulka četností/pravděpodobnostní tabulka

int* countCharacters(const char *inputText);
int* processFile(char *fileName);

void compress(Huffman* self); //Jen vypíše do konzole
void compressIntoFile(Huffman* self); //TODO - Uloží do souboru
void decompress(Huffman* self);
void simpleProcess(Huffman* self); // Jen zakóduje znaky a vypíše jejich hodnoty

/* Vypisování/Ukládání výsledků: */
// Simple print of input text, char-code and compressed text into a command window
void printResults(Huffman* self);

//Jak se dostat ke keyTabulce?
//initHUffmanFromText -> Tabulak četností -> Strom?

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

void saveTo(Huffman *self);