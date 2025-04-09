#include "huff.h"

Huffman* initHuffmanFromText(char *inputText, char  *outputFileName) {
    Huffman *huff = (Huffman*)malloc(sizeof(Huffman));

    huff->inputText = inputText;
    huff->fromFile = false;
    
    huff->count = countCharacters(inputText);
    if(outputFileName != NULL) {
        huff->outputFileName = outputFileName;
        huff->process = compressIntoFile;
    } else
        huff->process = compress;

    return huff;
}

Huffman* initHuffmanFromFile(char *inputFile, char  *outputFileName) {
    Huffman *huff = (Huffman*)malloc(sizeof(Huffman));

    huff->fromFile = true;
    huff->importFileName = inputFile;

    huff->count = processFile(inputFile);
    if(outputFileName != NULL) {
        huff->outputFileName = outputFileName;
        huff->process = compressIntoFile;
    } else
        huff->process = compress;

    return huff;
}

/*Huffman* initHuffmanFromTable(int *count) {
     Huffman *huff = (Huffman*)malloc(sizeof(Huffman));

     huff->count = count;
     huff->process = simpleProcess;
     return huff;
}*/

Huffman* initHuffmanFromBinary(char *importFileName, char *outputFileName) {
    Huffman *huff = (Huffman*)malloc(sizeof(Huffman));

    huff->fromFile = true;
    huff->importFileName = importFileName;
    huff->outputFileName = outputFileName;

    huff->process = decompress;
    return huff;
}

void compress(Huffman* self) {
    // Vytvořit kód z tabulky četností
    char **finalCode = createTable(self->count);
    self->table = finalCode;

    printResults(self);
    // Získat znak-kód
    // Zakódovat
    // Print
    
    for(int i = 0; i < ASCII_SIZE; i++)
        free(self->table[i]);
    free(self->table);
}
void compressIntoFile(Huffman* self) {
    // Vytvořit kód z tabulky četností
    char **finalCode = createTable(self->count);
    self->table = finalCode;
    printResults(self);
    // Získat znak-kód
    // Zakódovat s cílem uložit
    saveTo(self);
    // Nějak uložit
}
void decompress(Huffman* self) {
    BitReader br;
    initBitReader(&br, self->importFileName);

    self->table = importTable(&br);
    unsigned char ch;
    decodeNext(self, &br, &ch);

    closeBitReader(&br);
    
    for(int i = 0; i < ASCII_SIZE; i++)
        free(self->table[i]);
    free(self->table);
}

void simpleProcess(Huffman* self) {
    // Vytvořit kód z tabulky četností
    char **finalCode = createTable(self->count);
    self->table = finalCode;
    // Získat znak - kód
    // Print
}

// inputCount by mělo mít délku ASCII_SIZE
char** createTable(int *inputCount) {
    Node nodes[ASCII_SIZE]; // Pořadí v poli neodpovídá ASCII IDčku (ta velikost je jen tak prozatím xd)

    int inputs = 0; // Počet prvků v poli nodes

    // Získáme charaktery z poli charakterů.... vkládáme do nodes pole a rovnou seřadíme od nejmenšího po největší
    for(int i = 0; i < ASCII_SIZE; i++) {
        if(inputCount[i] > 0) {
            //Node newNode = {.count = inputCount[i], .charLength = 1, .znaky = malloc(1*sizeof(int))};
            //memset(newNode.znaky, 0, 1*sizeof(int));
            Node newNode = {.count = inputCount[i], .charLength = 1, .znaky =  calloc(1, sizeof(int))};
            
            newNode.znaky[newNode.charLength-1] = (unsigned char) i;
            if(inputs == 0) // Pokud je to první node v poli, tak přidáme
                nodes[inputs] = newNode;
            else // Jinak provedeme přidání na možná existující pozici a posunutí prvků v poli
                for(int j = 0; j <= inputs; j++) {
                    if(j == inputs) { // Nový poslední prvek
                        nodes[j] = newNode;
                        continue;
                    }
                    int value = nodes[j].count;
                    if(value > newNode.count) {
                        //Posunutí nodů
                        Node temp = nodes[j];
                        nodes[j] = newNode;
                        newNode = temp;
                    }
                }
            inputs++;
        }
    }
    printf("Sorted characters:\n");
    for (int i = 0; i < inputs; i++) {
        Node node = nodes[i];
        printf("'%s' : %d\n", node.znaky, node.count);
    }
    
    // Provedeme huffmanovské sčítání... nové nody vkládáme do nodes pole podle počtu charakterů (mohou být dále použita na sčítání)
    for(int i = 0; i < inputs-1; i = i + 2) {
        Node first = nodes[i];
        Node second = nodes[i+1];
        // Nový sečtený node:
        /*Node newNode = {.count = (first.count + second.count),
                        .charLength = 0, 
                        .znaky = malloc((first.charLength + second.charLength)*sizeof(int))};
        memset(newNode.znaky, 0, (first.charLength + second.charLength)*sizeof(int));*/
        Node newNode = {.count = (first.count + second.count),
                        .charLength = 0, 
                        .znaky = calloc((first.charLength + second.charLength), sizeof(int))};
        //Vložení znaků
        for(int m = 0; m < first.charLength; m++){
            newNode.znaky[(newNode.charLength++)] = first.znaky[m];
        }
        for(int m = 0; m < second.charLength; m++) {
            newNode.znaky[(newNode.charLength++)] = second.znaky[m];
        }
        //Cyklus na vkládání newNode do pole podle charLength:
        for(int j = 0; j <= inputs; j++) {
            if(j == inputs) {
                nodes[j] = newNode;
                continue;
            }
            int value = nodes[j].count;
            if(value >= newNode.count) {
                //Posunutí nodů
                Node temp = nodes[j];
                nodes[j] = newNode;
                newNode = temp;
            }
        }
        inputs++;
    }
    printf("New nodes:\n");
    for (int i = 0; i < inputs; i++) {
        Node node = nodes[i];
        printf("'%s' : %d : %d\n", node.znaky, node.count, node.charLength);
    }

    // Přiřazení 0 a 1 k prvkům pole
    printf("DEBUG:\n");
    char freePrefixes[20][20];
    memset(freePrefixes, 0, 20*sizeof(char));
    for(int i = 0; i < 20; i++)
        memset(freePrefixes[i], 0, 20*sizeof(char));
    int freePrefixesSize = -1;
    
    char **finalCode;
    finalCode = (char**)calloc(ASCII_SIZE, sizeof(char*));

    for(int ii = (inputs-2); ii >= 0; ii=ii-2) { //(inputs-2)... protože s -1 jdeme od posledního, ale protože chceme vynechat sumační součet všech, tak -2  
        /* PREFIX */
        char prefix[20];
        if(freePrefixesSize >= 0) { // Pokud máme volný prefix
            sprintf(prefix, "%s", freePrefixes[0]); // Tak vezneme první prvek z pole volných prefixů
            for(int pref = 0; pref <= freePrefixesSize; pref++) { // Cyklus na posunutí všech prvků v poli volných prefixů
                if(pref != 0)
                    sprintf(freePrefixes[pref-1], "%s", freePrefixes[pref]);
                memset(freePrefixes[pref], 0, 20*sizeof(char));
            }
            freePrefixesSize--;
        } else
            memset(prefix, 0, 20*sizeof(char));
        
        /* Přiřazení kódu dvěma prvkům v pořadí: */
        bool jednicka = false;
        for(int j = 0; j <=1; j++) {
            Node node = nodes[ii-j];
            char sequence = jednicka ? '1': '0';
            char code[21];
            memset(code, 0, 21*sizeof(char));

            sprintf(code, "%s%c", prefix, sequence);

            if(node.charLength == 1) { // Tak je to jenom jeden znak a kód nebudeme dávat do freePrefixes
                int asciiID = (unsigned char)node.znaky[0];
                finalCode[asciiID] = (char*) calloc(strlen(code), sizeof(char));
                strcat(finalCode[asciiID], code);
                printf("'%s' : %d : %d : %s\n", node.znaky, node.count, node.charLength, finalCode[asciiID]);
            } else {
                strcat(freePrefixes[++freePrefixesSize], code); // Vložíme do freePrefixes
                printf("'%s' : %d : %d : %s\n", node.znaky, node.count, node.charLength, code);
            }
            jednicka = !jednicka;
        }
    }

    // FREE - for now
    for(int i = 0; i < inputs; i++)
        free(nodes[i].znaky);
    
    return finalCode;
}

void printResults(Huffman *self) {
    if(self->fromFile == true) {
        printf("Cannot print results... importing from a FILE\n");
        printf("TODO LATER....\n");
        return;
    }
    printf("\nResults...\n");
    

    for(int i = 0; i< strlen(self->inputText); i++) {
        char c = self->inputText[i];
        printf("%c - ", c);
        printf("%s \n", self->table[(unsigned char)c]);
    }
    printf("Zakodovano: ");
    for(int i = 0; i< strlen(self->inputText); i++) {
        char c = self->inputText[i];
        printf("%s", self->table[(unsigned char)c]);
    }
    printf("\n");
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

// Spočítá četnosti znaků v souboru
int* processFile(char *fileName) {
    FILE *file = fopen(fileName, "r"); //Otevření filu na read-only
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    //Pole četností
    int *count = malloc(ASCII_SIZE*sizeof(int));
    if(!count) {
        return NULL; //Alokace se nezdařila
    }
    memset(count, 0, ASCII_SIZE*sizeof(int));

    // Čtení charakterů:
    char ch;
    while ((ch = fgetc(file)) != EOF) { //Bereme charakter po charakteru
        count[(unsigned char)ch]++;
    }
    fclose(file);

    // Print character counts
    printf("Character frequencies in a file:\n");
    for (int i = 0; i < ASCII_SIZE; i++) {
        if (count[i] > 0) {
            printf("'%c' : %d\n", i, count[i]);
        }
    }
    return count;
}

void saveTo(Huffman *self) {
    BitWriter bw;
    initBitWriter(&bw, self->outputFileName);

    printf("Ulozeno   : ");
    if(self->fromFile) {
        FILE *file = fopen(self->importFileName, "r"); //Otevření filu na read-only
        if (file == NULL) {
            printf("Error opening file");
            return;
        }
        // Zapíšeme kódovací tabulku
        for(int i = 0; i < ASCII_SIZE; i++) {
            if(self->count[(unsigned char)i] > 0) {
                writeTable(&bw, (unsigned char)i, self->table[i], strlen(self->table[i]));
            }
        }
        // Čtení charakterů:
        char ch;
        unsigned char chU;
        while ((ch = fgetc(file)) != EOF) { //Bereme charakter po charakteru
            chU = (unsigned char) ch;
            for(int r = 0; r < strlen(self->table[chU]); r++){
                writeBit(&bw, self->table[chU][r]);
                printf("%c", self->table[chU][r]);
            }

        }
        fclose(file);
    } else {
        // Zapíšeme kódovací tabulku
        for(int i = 0; i < ASCII_SIZE; i++) {
            if(self->count[(unsigned char)i] > 0) {
                writeTable(&bw, (unsigned char)i, self->table[i], strlen(self->table[i]));
            }
        }
        // Jdeme postupně znak po znaku z self.inputText
        for(int i = 0; i < strlen(self->inputText); i++) {
            // Čtení charakterů:
            char ch = self->inputText[i];
            unsigned char chU = (unsigned char) ch;
            for(int r = 0; r < strlen(self->table[chU]); r++) {
                writeBit(&bw, self->table[chU][r]);
                printf("%c", self->table[chU][r]);
            }
        }
    }
    printf("\n");
    closeBitWriter(&bw);
}

char** importTable(BitReader *br) {
    char **importedTable; // [ASCII][kód]
    importedTable = (char**)calloc(ASCII_SIZE, sizeof(char*));

    unsigned char ch;
    char *code = NULL;
    while(readTable(br, &ch, &code)) {
        importedTable[ch] = code;
        //free(code);
        code = NULL;
    }
    printf("Read table: \n");
    for(int i = 0; i <ASCII_SIZE; i++) {
        //printf("DEBUG: %c - %s \n", (unsigned char)i, importedTable[(unsigned char)i]);
        if(importedTable[(unsigned char)i] != NULL)
           printf("| Obtained character: %c - %s \n", (unsigned char)i, importedTable[(unsigned char)i]);
    }
    return importedTable;
}

bool decodeNext(Huffman *self, BitReader *br, unsigned char *ch) {
    int bit;
    printf("Read bits : ");
    while (readBit(br, &bit)) {
        printf("%d", bit);
        for(int i = 0; i < ASCII_SIZE ; i++) {
            if(self->table[(unsigned char)i] == NULL)
                continue;
            if(self->table[i][0] == (bit==1?'1':'0'))
                printf("");
        }

    }
    printf("\n");

    return false;
}

