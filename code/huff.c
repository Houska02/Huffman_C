#include "huff.h"

Huffman* initHuffmanFromText(char *inputText) {
    Huffman *huff = (Huffman*)malloc(sizeof(Huffman));

    huff->count = countCharacters(inputText);

    return huff;
}

// Spočítá četnosti znaků v textu
int* countCharacters(const char *inputText) {
    int *count = malloc(ASCII_SIZE*sizeof(int));
    if(!count) {
        return NULL; //Alokace se nezdařila
    }
    memset(count, 0, ASCII_SIZE*sizeof(int));

    for(int i = 0; inputText[i] != '\0'; i++) {
        count[(unsigned char)inputText[i]]++;
    }
    
    // Print character counts
    printf("Character frequencies:\n");
    for (int i = 0; i < ASCII_SIZE; i++) {
        if (count[i] > 0) {
            printf("'%c' : %d\n", i, count[i]);
        }
    }

    return count;
}