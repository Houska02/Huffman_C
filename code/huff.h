#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ASCII_SIZE 256

typedef struct Huffman {
    char *inputText;
    int *count; //Tabulka četností/pravděpodobností

    
    void (*process)(struct Huffman*);

} Huffman;

Huffman* initHuffmanFromText(char *inputText);
Huffman* initHuffmanFromBinary(char *inputValues); //TODO
Huffman* initHuffmanFromTable(int *count); // Tabulka četností/pravděpodobnostní tabulka

int* countCharacters(const char *inputText);

void compress(Huffman* self); //Jen vypíše do konzole
void compressIntoFile(Huffman* self); //TODO - Uloží do souboru
void decompress(Huffman* self);
void simpleProcess(Huffman* self); // Jen zakóduje znaky a vypíše jejich hodnoty


//Jak se dostat ke keyTabulce?
//initHUffmanFromText -> Tabulak četností -> Strom?


typedef struct {
    int count; // Četnost tohoto nodu
    
    int charCount;
    char* znaky;
    //Nějak ty znaky?
} Node;

void createTable(int *count);
