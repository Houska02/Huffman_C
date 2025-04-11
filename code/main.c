#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <getopt.h>
#include <string.h>

#include "huff.h"

/*
* Zadat text nebo možnost zadat znaky a pravděpodobnosti
* Zesortovat znaky od největšího po nejmenší podle pravděpodobností (nebo opačně, záleží na dalším použití)
* Provést obecného Huffmana (takže mít možnost zadat výstupní znaky (0, 1 .... nebo 0, 1, 2... a, b....))
    * Na zadaných výstupních znacích bude záležet jak budu postupovat Huffmanem (kolik budu sčítat)

*/

/* SPOUŠTĚNÍ:
* Definování jestli budeme zadávat slovo nebo pravděpodobnosti znaků
* Definovat kolik bude výstupních znaků (0, 1 .... nebo 0, 1, 2... a, b....)

Default: slovo
Default: binární kódování
*/

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

/*

    Arguments:
    -? » Help message
    -m --msg <inputText>
    -i --input <inputFileName> » Defines input file. For compression use .txt and for decompression use .huff.
    -o --output <outputFileName> » Defines output file. For compression use .huff and for decompression use .txt.
    -c --compress » Compressing text into a file or 
    -d --decompress » Decompressing a .huff file created by this program
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

    static struct  option long_options[] = {
        {"input", required_argument, 0, 'i'},
        {"output", required_argument, 0, 'o'},
        {"msg", required_argument, 0, 'm'},
        //{"probabilities", no_argument, 0, 'p'},
        {"compress", no_argument, 0, 'c'},
        {"decompress", no_argument, 0, 'd'},
        {"help", no_argument, 0, '?'},
        {0, 0, 0, 0}
    };
    while ((opt = getopt_long(argc, argv, "i:o:m:cd?", long_options, NULL)) != -1) {
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
                /*case 'p': // Probabilities
                    printf("Dal jsi -p\n");
                    isProbabilities = true;
                    break;*/
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
                    printf("  -> If output file is not specified then output will be in command window. If input file or message is not specified then user will be asked for an additional input after launch.\n");
                    printf("  -> If action (-c or -d) is not specified then default action is compression.");
                    return EXIT_SUCCESS;
                break;
            }
    }

    if(compress) { // Komprese
        if(importFromFile && isProbabilities) //Pokud budeme importovat ze souboru, tak nebudeme zadávat vlastní pravděpodobnosti
            isProbabilities = false;
        if(!isProbabilities && !isSettingText && !importFromFile) // Pokud neimportujeme ze souboru a ani nezádáváme čistě pravděpodobnosti, tak bude text
            isSettingText = true;
        if(importFromFile && isSettingText) // Pokud importujeme ze souboru, tak nebudeme zadávat text ručně
            isSettingText = false;

        if(isSettingText && inputText == NULL)
            inputText = getInputText();

        /*if(isProbabilities) { // TODO
            printf("HERE SET PROBABILITIES!");
        }*/

        Huffman* huff;
        if(importFromFile) {
            huff = initHuffmanFromFile(inputFileName, outputFileName);
            huff->process(huff);

            printf("--- END HUFF FROM A FILE ---\n");
        } else {
            huff = initHuffmanFromText(inputText, outputFileName);
            huff->process(huff);

            printf("--- END HUFF FROM A TEXT ---\n");
        }

        if(huff->count)
            free(huff->count);
    }
    if(!compress) { // Dekomprese
        Huffman *huff;
        huff = initHuffmanFromBinary(inputFileName, outputFileName);
        huff->process(huff);
        printf("--- END HUFF FROM BINARY (decompression) ---\n");
    }

    if(inputText) // Uvolnění paměti
        free(inputText);
    return EXIT_SUCCESS;
}
