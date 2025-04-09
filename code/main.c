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

// ./main.exe -?
int main(int argc, char *argv[]) {
    int opt;

    bool compress = true; // true - compressing | false - decompressing

    /* PO SPUŠTĚNÍ:
    * Chtít po uživatelovi slovo nebo znaky a pravděpodobnosti
    */
    bool isProbabilities = false; // Defaultní hodnota
    bool isSettingText = false; // Defaultní hodnota

    bool importFromFile = false; // Defaultní hodnota
    //bool saveToFile = false; unused // Defaultní hodnota
    char *inputFileName = NULL;
    char *outputFileName = NULL;
    
    //bool customOutput = false; unused // Defaultní hodnota
    //char *customOutputValues = NULL;
    
    /*
        INPUTS
    */
   char *inputText = NULL;
   size_t inputTextSize = 0;

    /*
    -i <value> - Definovani vstupniho souboru
    -o <value> - Definovani vystupniho souboru
    -m - Bude po uživatelovi potřeba zadat vstupní text
    Navíc:
    -p - Budeme zadávat pravděpodobnosti
    NEBUDE ASI... -h --huff <values> - Do kolika znaků budeme kódovat (01, 012, abc, ... bude záležet na pořadí v jakém se to tam zadá)
    
    -c
    -d

    -? - Help
    */
    static struct  option long_options[] = {
        {"input", required_argument, 0, 'i'},
        {"output", required_argument, 0, 'o'},
        {"msg", required_argument, 0, 'm'},
        //{"huff", required_argument, 0, 'h'},
        {"probabilities", no_argument, 0, 'p'},
        {"compress", no_argument, 0, 'c'},
        {"decompress", no_argument, 0, 'd'},
        {0, 0, 0, 0}
    };
    //while ((opt = getopt_long(argc, argv, "i:o:m:h:pcd", long_options, NULL)) != -1) {
    while ((opt = getopt_long(argc, argv, "i:o:m:pcd", long_options, NULL)) != -1) {
            switch (opt) {
                case 'i': // Input file name
                    importFromFile = true;
                    inputFileName = optarg;
                    printf("Dal jsi -i, %s\n", inputFileName);
                    break;
                case 'o': // Output file name
                    outputFileName = optarg;
                    printf("Dal jsi -o, %s\n", outputFileName);
                    break;
                case 'm': // message
                    isSettingText = true;
                    inputText = optarg;
                    printf("Dal jsi -m\n");
                    break;
                // case 'h': // Výstupní kódování (znaky)
                //     customOutputValues = optarg;
                //     printf("Dal jsi -h, %s\n", customOutputValues);
                //     break;
                case 'p': // Probabilities
                    printf("Dal jsi -p\n");
                    isProbabilities = true;
                    break;
                case 'c':
                    compress = true; // trochu placebo, když true je to i default hodnota xd
                    break;
                case 'd':
                    compress = false;
                    break;
                case '?':
                    printf("-f <file_name> -> Define a file name.\n");
                    printf("-o <value> -> TODO\n");
                    printf("-p -> Setting probabilities instead of text only\n");
                    printf("-t -> Only used with -p... You will define probabilities and text\n");
                    printf("TODO\n"); // TODO
                break;
                default:
                    printf("Bad usage\n");
                break;
            }
    }

    if(compress) {
        if(importFromFile && isProbabilities) //Pokud budeme importovat ze souboru, tak nebudeme zadávat vlastní pravděpodobnosti... mohlo by být moc náročné
            isProbabilities = false;
        if(!isProbabilities && !isSettingText && !importFromFile) // Pokud neimportujeme ze souboru a ani nezádáváme čistě pravděpodobnosti, tak bude text
            isSettingText = true;
        if(importFromFile && isSettingText) // Pokud importujeme ze souboru, tak nebudeme zadávat text ručně
            isSettingText = false;

        if(isSettingText && inputText == NULL) {
            inputText = getInputText();
                if(inputText) {
                inputTextSize = strlen(inputText);
                printf("Text length: %llu\n", inputTextSize);
            }
        }
        if(isProbabilities) {
            printf("HERE SET PROBABILITIES!");
        }
        Huffman* huff;
        if(importFromFile) {
            huff = initHuffmanFromFile(inputFileName, outputFileName);
            huff->process(huff);

            printf("--- END HUFF FROM A FILE ---\n");
        } else {
            //Huffman huff = createHuffman();
            huff = initHuffmanFromText(inputText, outputFileName);
            huff->process(huff);

            printf("--- END HUFF FROM A TEXT ---\n");
        }

        if(huff->count)
            free(huff->count);
    }
    if(!compress) {
        Huffman *huff;
        huff = initHuffmanFromBinary(inputFileName, outputFileName);
        huff->process(huff);
        printf("--- END HUFF FROM BINARY (decompression) ---\n");
    }
    /*HUFFMAN:
    * Zesortit znaky podle pravděpodobností
    * Vytvořit skupiny po n-1 znaků a zjistit jak velká bude poslední skupina
        * Pokud bude poslední ksupina malá, tak budeme sčítat po n znacích
    * */

    if(inputText) // Uvolnění paměti
        free(inputText);
    return EXIT_SUCCESS;
}
