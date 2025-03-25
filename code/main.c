#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <unistd.h>

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
    printf("Input a text:\n");

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
    return buffer;
}

// ./main.exe -f <file>
// ./main.exe -p
// ./main.exe -o <znaky>
// ./main.exe -?
int main(int argc, char *argv[]) {
    int opt;

    /* PO SPUŠTĚNÍ:
    * Chtít po uživatelovi slovo nebo znaky a pravděpodobnosti
    */
    bool isProbabilities = false; // Defaultní hodnota
    bool isSettingText = false; // Defaultní hodnota
    //TODO:
    bool saveToFile = false; // Defaultní hodnota
    char *fileName = NULL;
    
    bool customOutput = false; // Defaultní hodnota
    char *customOutputValues = NULL;

    /*
    -f <value> - Definování souboru
    -p - Budeme zadávat pravděpodobnosti
    -o <values> - Do kolika znaků budeme kódovat (01, 012, abc, ... bude záležet na pořadí v jakém se to tam zadá)
    -? - Help
    */
    while ((opt = getopt(argc, argv, "pf:o:t")) != -1) {
            switch (opt) {
                case 'p': // Probabilities
                    printf("Dal jsi -p\n");
                    isProbabilities = true;
                    break;
                case 'f': // File name
                    saveToFile = true;
                    fileName = optarg;
                    printf("Dal jsi -i, %s\n", fileName);
                    break;
                case 'o': // Výstupní kódování (znaky)
                    customOutput = true;
                    customOutputValues = optarg;
                    printf("Dal jsi -o, %s\n", customOutputValues);
                break;
                case 't':
                    isSettingText = true;
                    printf("Dal jsi -t\n");
                break;
                case '?':
                    printf("-f <file_name> -> Define a file name.\n");
                    printf("-o <value> -> TODO\n");
                    printf("-p -> Setting probabilities instead of text only\n");
                    printf("-t -> Only used with -p... You will define probabilities and text\n");
                break;
                default:
                    printf("Bad usage\n");
                break;
            }
    }
    //Zda bylo použito -p a -t
    if(!isProbabilities) { //pokud nebylo použito -p, tak má být defaultně zadáván text
        isSettingText = true;
    }

    /*
        INPUTS
    */
    
    char *inputText = NULL;
    if(isSettingText) {
        inputText = getInputText();
         if(inputText) {
            printf("You entered: %s\n", inputText);
            printf("Text length: %llu\n", strlen(inputText));
            printf("inputText[0] %c\n", inputText[0]);
            printf("inputText[max-2] %c\n", inputText[strlen(inputText)-2]);
            printf("inputText[max-1] %c\n", inputText[strlen(inputText)-1]);
            printf("inputText[max] %c\n", inputText[strlen(inputText)]);
            //printf("Text length: %llu\n", (sizeof(inputText)/sizeof(inputText[0])) );

         }
    }
    if(isProbabilities) {

        printf("HERE SET PROBABILITIES!");
    }
    

    printf("END\n");

    /*HUFFMAN:
    * Zesortit znaky podle pravděpodobností
    * Vytvořit skupiny po n-1 znaků a zjistit jak velká bude poslední skupina
        * Pokud bude poslední ksupina malá, tak budeme sčítat po n znacích
    * */

    /* VÝSTUP:
    * Znak - Pravděpodobnost- Kód
    * 
    * Popřípadě Slovo a jeho zakódování
    */

    if(inputText) // Uvolnění paměti
        free(inputText);
    return EXIT_SUCCESS;
}
