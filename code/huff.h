#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ASCII_SIZE 256

typedef struct {
    char *inputText;
    int *count;

} Huffman;

Huffman* initHuffmanFromText(char *inputText);

int* countCharacters(const char *inputText);