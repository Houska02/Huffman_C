#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <getopt.h> // Knihovna na zpracování input argumentů při spouštění programu

#include "huff.h"

/* Funkce na získání textu z kozole. */
char *getInputText() {
    printf("Input a text: ");

    int portion_size = 10;
    char *buffer = NULL;
    size_t size = 0;
    char temp[portion_size];

    while(fgets(temp, portion_size, stdin)) {
        size_t temp_len = strlen(temp);
        char *new_buffer = realloc(buffer, size+temp_len+1);

        if(!new_buffer) {
            free(buffer);
            return NULL; // Nepodařila se alokace
        }

        // Přidání k již načtené části
        buffer = new_buffer;
        strcpy(buffer + size, temp);
        size += temp_len;

        if(temp[temp_len-1] == '\n') //Konec textu => konec cyklu
            break;
    }
    buffer[size-1] = '\0'; // Odstranění \n
    return buffer;
}

/* Program na vytvoření Huffmanova kódování z input textu. Také umožňuje zpětnou dekompresi .huff souboru.
    Možnosti:
    • Vytvoření huffmanova kódování pro zadaný text
    • Vytvoření huffmanova kódování z .txt souboru
    • Komprese textu do .huff souboru
    • Vypsání výsledku do konzole (pokud neukládáme do souboru)
    • Změna četnosti znaků (tip: pokud se ve zprávě definuje znak pouze jednou, tak se potom může změnit četnost a výsledkem bude kód pro každý charakter s custom četností)

    Arguments:
    -? » Help message
    -m --msg <inputText>
    -i --input <inputFileName> » Defines input file. For compression use .txt and for decompression use .huff.
    -o --output <outputFileName> » Defines output file. For compression use .huff and for decompression use .txt.
    -c --compress » Compressing text into a file or 
    -d --decompress » Decompressing a .huff file created by this program
    -p --probabilities » Set custom probability for each character in a mesasge
*/
int main(int argc, char *argv[]) {
    int opt;

    bool compress = true; // true - compressing | false - decompressing

    bool isProbabilities = false; // Defaultní hodnota
    bool isSettingText = false; // Defaultní hodnota

    bool importFromFile = false; // Defaultní hodnota

    char *inputFileName = NULL;
    char *outputFileName = NULL;
    char *inputText = NULL;

    static struct option long_options[] = {
        {"input", required_argument, 0, 'i'},
        {"output", required_argument, 0, 'o'},
        {"msg", required_argument, 0, 'm'},
        {"probabilities", no_argument, 0, 'p'},
        {"compress", no_argument, 0, 'c'},
        {"decompress", no_argument, 0, 'd'},
        {"help", no_argument, 0, '?'},
        {0, 0, 0, 0}
    };
    while ((opt = getopt_long(argc, argv, "i:o:m:cdp?", long_options, NULL)) != -1) {
            switch (opt) {
                case 'i': // Input file name
                    importFromFile = true;
                    inputFileName = optarg;
                    break;
                case 'o': // Output file name
                    outputFileName = optarg;
                    break;
                case 'm': // message
                    isSettingText = true;
                    inputText = optarg;
                    break;
                case 'p': // Probabilities
                    isProbabilities = true;
                    break;
                case 'c':
                    compress = true; // trochu placebo, protože true je i default hodnota xd
                    break;
                case 'd':
                    compress = false;
                    break;
                case '?':
                default:                    
                    printf("Help\n");
                    printf("  -m --msg <inputText>\n");
                    printf("  -i --input <inputFileName> -> Defines input file. For compression use .txt and for decompression use .huff.\n");
                    printf("  -o --output <outputFileName> -> Defines output file. For compression use .huff and for decompression use .txt.\n");
                    printf("  -c --compress -> Compressing a text\n");
                    printf("  -d --decompress -> Decompressing a .huff file created by this program\n");
                    printf("  -p --probabilities -> User will be able to define a custom probablities (frequencies because of ints... :D) for each character. (does not work when importing from a .txt file)\n");
                    printf("  -> If output file is not specified then output will be in command window. If input file or message is not specified then user will be asked for an additional input after launch.\n");
                    printf("  -> If action (-c or -d) is not specified then default action is compression.");
                    printf("  -> PRO TIP: You can use -m to set unique characters and then -p to define custom frequencies for each one of them. For example: -m ABCDEFG -p.");
                    return EXIT_FAILURE;
                break;
            }
    }

    if(compress) { // Komprese
        if(importFromFile && isProbabilities) //Pokud budeme importovat ze souboru, tak nebudeme zadávat vlastní pravděpodobnosti
            isProbabilities = false;
        if(!isSettingText && !importFromFile) // Pokud neimportujeme ze souboru tak bude text
            isSettingText = true;
        if(importFromFile && isSettingText) // Pokud importujeme ze souboru, tak nebudeme zadávat text ručně
            isSettingText = false;
        if(isSettingText && inputText == NULL) // Pokud již nebyl text zadán
            inputText = getInputText();

        Huffman* huff;
        if(importFromFile)
            huff = initHuffmanFromFile(inputFileName, outputFileName);
        else
            huff = initHuffmanFromText(inputText, outputFileName);

        if(isProbabilities)
            modifyCountTable(huff);

        huff->process(huff);
        if(huff->count)
            free(huff->count);
        free(huff);
        // TODO - free
    }
    if(!compress) { // Dekomprese
        if(!inputFileName){
            printf("You need to specifi input .huff file.\n");
            return EXIT_FAILURE;
        }
        Huffman *huff;
        huff = initHuffmanFromBinary(inputFileName, outputFileName);
        huff->process(huff);
        // TODO - free
        free(huff);
    }

    if(inputText) // Uvolnění paměti
        free(inputText);
    if(inputFileName)
        free(inputFileName);
    if(outputFileName)
        free(outputFileName);
    return EXIT_SUCCESS;
}
