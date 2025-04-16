#include "huff.h"

/* INIT FUNCTIONS */

Huffman* initHuffmanFromText(char *inputText, char  *outputFileName) {
    Huffman *huff = (Huffman*)malloc(sizeof(Huffman));

    huff->inputText = inputText;
    huff->fromFile = false;
    huff->strLen = 0;

    huff->count = countCharacters(inputText, &(huff->strLen));
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
    huff->strLen = 0;

    huff->count = processFile(inputFile, &(huff->strLen));
    if(outputFileName != NULL) {
        huff->outputFileName = outputFileName;
        huff->process = compressIntoFile;
    } else
        huff->process = compress;

    return huff;
}

Huffman* initHuffmanFromBinary(char *importFileName, char *outputFileName) {
    Huffman *huff = (Huffman*)malloc(sizeof(Huffman));

    huff->fromFile = true;
    huff->importFileName = importFileName;
    huff->outputFileName = outputFileName;

    huff->process = decompress;
    return huff;
}

/* process FUNCTIONS */

void compress(Huffman* self) {
    self->table = createTable(self->count); // Creating a coding table
    printResults(self);
    
    for(int i = 0; i < ASCII_SIZE; i++)
        free(self->table[i]);
    free(self->table);
}
void compressIntoFile(Huffman* self) {
    self->table = createTable(self->count); // Creating a coding table
    saveTo(self);

    for(int i = 0; i < ASCII_SIZE; i++)
        free(self->table[i]);
    free(self->table);
}
void decompress(Huffman* self) {
    BitReader br;
    initBitReader(&br, self->importFileName);

    int uniqueCharCount = 0; // Počet importovaných znaků z tabulky, které mají nějaký kód
    self->table = importTable(&br, &uniqueCharCount, &(self->strLen));

    decode(self, &br, uniqueCharCount);

    closeBitReader(&br);
    printf("... imported from %s ... saved to %s\n", self->importFileName, self->outputFileName);
    
    for(int i = 0; i < ASCII_SIZE; i++)
        free(self->table[i]);
    free(self->table);
}


/* Help function for inserting newNode into a nodes array.
    Array is autoamtically sorted with this function in ascending order.
*/
void insertNode(Node *nodes, int *count, Node newNode) {
    for (int j = 0; j <= *count; j++) {
        if (j == *count) {
            nodes[j] = newNode;
            break;
        }
        if (nodes[j].count > newNode.count) {
            Node temp = nodes[j];
            nodes[j] = newNode;
            newNode = temp;
        }
    }
}

// inputCount by mělo mít délku ASCII_SIZE
char** createTable(int *inputCount) {
    Node nodes[ASCII_SIZE]; // Pořadí v poli neodpovídá ASCII IDčku (ta velikost je jen tak prozatím xd)

    int inputs = 0; // Number of elements in the nodes array

    // Získáme charaktery z poli četností. Vlkládáme do nodes pole a rovnou seřadíme  od nejmenšího po největší
    for(int i = 0; i < ASCII_SIZE; i++) {
        if(inputCount[i] > 0) {
            Node newNode = {.count = inputCount[i], .charLength = 1, .znaky =  calloc(1, sizeof(int))};
            newNode.znaky[newNode.charLength-1] = (unsigned char) i;

            if(inputs == 0) // If it is the first Node in the array
                nodes[inputs] = newNode;  
            else // Or we will add it and sort it in ascending order
                insertNode(nodes, &inputs, newNode);
            inputs++;
        }
    }
    
    // Provedeme huffmanovské sčítání... nové nody vkládáme do nodes pole podle počtu charakterů (dále použita na sčítání)
    for(int i = 0; i < inputs-1; i = i + 2) {
        Node first = nodes[i];
        Node second = nodes[i+1];
        // New summed node:
        Node newNode = {.count = (first.count + second.count),
                        .charLength = 0, 
                        .znaky = calloc((first.charLength + second.charLength), sizeof(int))};
        //combining characters:
        for(int m = 0; m < first.charLength; m++) { // characters from first node
            newNode.znaky[(newNode.charLength++)] = first.znaky[m];
        }
        for(int m = 0; m < second.charLength; m++) { // characters from second node
            newNode.znaky[(newNode.charLength++)] = second.znaky[m];
        }
        // Adding newNode and sorting it in ascending order
        insertNode(nodes, &inputs, newNode);
        inputs++;
    }

    /* Code creation */

    int freePrefixesSize = -1;
    char freePrefixes[FREE_PREFIX_ARRAY_SIZE][CODE_SIZE]; // free prefix is sequence of zeros and ones that will be used later ... for example from prefix 001 we will create 0010 and 0011 (these could be again free prefixes)
    memset(freePrefixes, 0, FREE_PREFIX_ARRAY_SIZE*sizeof(char));
    for(int i = 0; i < FREE_PREFIX_ARRAY_SIZE; i++)
        memset(freePrefixes[i], 0, CODE_SIZE*sizeof(char));

    char **finalCode; // finální kódovací tabulka
    finalCode = (char**)calloc(ASCII_SIZE, sizeof(char*));

    for(int ii = (inputs-2); ii >= 0; ii=ii-2) { //(inputs-2)... because when starting with -1 we will also include the summation node... and we do not want that
        /* PREFIX */
        char prefix[CODE_SIZE];
        if(freePrefixesSize >= 0) { // Pokud máme volný prefix
            sprintf(prefix, "%s", freePrefixes[0]); // Tak vezneme první prvek z pole volných prefixů
            for(int pref = 0; pref <= freePrefixesSize; pref++) { // Cyklus na posunutí všech prvků v poli volných prefixů
                if(pref != 0)
                    sprintf(freePrefixes[pref-1], "%s", freePrefixes[pref]);
                memset(freePrefixes[pref], 0, CODE_SIZE*sizeof(char));
            }
            freePrefixesSize--;
        } else
            memset(prefix, 0, CODE_SIZE*sizeof(char));
        
        /* Assigning a code to two nodes in queue: */
        bool one = false;
        for(int j = 0; j <=1; j++) {
            Node node = nodes[ii-j];
            char sequence = one ? '1': '0';
            char code[CODE_SIZE];
            memset(code, 0, CODE_SIZE*sizeof(char));

            sprintf(code, "%s%c", prefix, sequence); // <prefix> + <new_bit>

            if(node.charLength == 1) { // If it is just one character we do not want to use that as a prefix ---> final code
                int asciiID = (unsigned char)node.znaky[0];
                finalCode[asciiID] = (char*) calloc(strlen(code), sizeof(char));
                strcat(finalCode[asciiID], code);
            } else { // else --> this new code is a free prefix that will be used later (or not)
                strcat(freePrefixes[++freePrefixesSize], code); // Vložíme do freePrefixes
            }
            one = !one;
        }
    }

    // FREE nodes
    for(int i = 0; i < inputs; i++)
        free(nodes[i].znaky);
    
    return finalCode;
}

void printResults(Huffman *self) {
    printf("Compressed text: \n");

    if(self->fromFile == true) {
        FILE *file = fopen(self->importFileName, "r");
        if (file == NULL) {
            perror("Error opening file");
            return;
        }
        char ch;
        while ((ch = fgetc(file)) != EOF) { // charakter po charakteru
            printf("%s", self->table[(unsigned char)ch]);
        }
        printf("\n");
        fclose(file);
        return;
    }
    for(int i = 0; i< strlen(self->inputText); i++) {
        char c = self->inputText[i];
        printf("%s", self->table[(unsigned char)c]);
    }
    printf("\n");
}

int* countCharacters(const char *inputText, int *strLen) {
    int *count = malloc(ASCII_SIZE*sizeof(int));
    if(!count) {
        return NULL; //Alokace se nezdařila
    }
    memset(count, 0, ASCII_SIZE*sizeof(int));

    for(int i = 0; inputText[i] != '\0'; i++) {
        count[(unsigned char)inputText[i]]++;
        (*strLen)++;
    }
    return count;
}

int* processFile(char *fileName, int *strLen) {
    FILE *file = fopen(fileName, "r"); // Opening file, read-only
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    //Pole četností
    int *count = malloc(ASCII_SIZE*sizeof(int));
    if(!count) {
        return NULL; // Allocation failed
    }
    memset(count, 0, ASCII_SIZE*sizeof(int));

    // Reading characters from file:
    char ch;
    while ((ch = fgetc(file)) != EOF) { //charakter po charakteru
        count[(unsigned char)ch]++;
        (*strLen)++;
    }
    fclose(file);
    return count;
}

void saveTo(Huffman *self) {
    BitWriter bw;
    initBitWriter(&bw, self->outputFileName);

    if(self->fromFile) {
        FILE *file = fopen(self->importFileName, "r"); //Opening import file, read-only
        if (file == NULL) {
            printf("Error opening file");
            return;
        }
        // Zapíšeme kódovací tabulku + počet znaků
        writeCharCount(&bw, self->strLen);
        for(int i = 0; i < ASCII_SIZE; i++) {
            if(self->count[(unsigned char)i] > 0) {
                writeTable(&bw, (unsigned char)i, self->table[i], strlen(self->table[i]));
            }
        }
        // Character compression:
        char ch;
        unsigned char chU;
        while ((ch = fgetc(file)) != EOF) { //charakter po charakteru
            chU = (unsigned char) ch;
            for(int r = 0; r < strlen(self->table[chU]); r++) {
                writeBit(&bw, self->table[chU][r]);
            }

        }
        fclose(file);
    } else {
        // Zapíšeme kódovací tabulku + počet znaků
        writeCharCount(&bw, self->strLen);
        for(int i = 0; i < ASCII_SIZE; i++) {
            if(self->count[(unsigned char)i] > 0) {
                writeTable(&bw, (unsigned char)i, self->table[i], strlen(self->table[i]));
            }
        }
        // Character compression:
        for(int i = 0; i < strlen(self->inputText); i++) { // znak po znaku z self.inputText
            char ch = self->inputText[i];
            unsigned char chU = (unsigned char) ch;
            for(int r = 0; r < strlen(self->table[chU]); r++) {
                writeBit(&bw, self->table[chU][r]);
            }
        }
    }
    closeBitWriter(&bw);
    printf("... saved into %s", self->outputFileName);
}

char** importTable(BitReader *br, int *uniqueCharCount, int *strlen) {
    readCharCount(br, strlen);
    // Table:
    char **importedTable; // [ASCII][kód]
    importedTable = (char**)calloc(ASCII_SIZE, sizeof(char*));

    unsigned char ch;
    char *code = NULL;
    while(readTable(br, &ch, &code)) {
        importedTable[ch] = code;
        *uniqueCharCount = *uniqueCharCount + 1;
        //free(code);
        code = NULL;
    }
    return importedTable;
}

void decode(Huffman *self, BitReader *br, int uniqueCharCount) {
    FILE *file = fopen(self->outputFileName, "w"); //Otevření souboru do kterého budeme ukládat

    char *match = calloc(uniqueCharCount, sizeof(char)); // Pole charakterů, se kterými se kód zatím shoduje při čtení
    int matchCount = 0;

    int decodedCharLen = 0;

    int bit;
    int readingBit= 0;
    while (readBit(br, &bit) && decodedCharLen < self->strLen) { //Reading next bit in queue
        if(readingBit == 0) { // Looping through ASCII self.table array if we are matching first bit
            for(int i = 0; i < ASCII_SIZE ; i++) {
                if(self->table[(unsigned char)i] == NULL)
                    continue;
                if(self->table[i][readingBit] == (bit==1?'1':'0')) { // Pokud máme shodu (přičteme readingBit)
                    // tak přiřadíme shodu do match pole
                    match[matchCount++] = (unsigned char) i;
                }
            }
            readingBit++;
        } else {
            for(int i = 0; i < matchCount; i++) {
                if(self->table[(unsigned char)match[i]][readingBit] != (bit==1?'1':'0')) {
                    // Odstraníme nechtěný prvek a posuneme seznam
                    for(int j = i+1; j <= matchCount; j++) {
                        match[j-1] = match[j];
                    }
                    i--;
                    matchCount--;
                }
            }
            readingBit++;
        }
        // Pokud je v match poli jen jeden prvek, tak máme shodu
        if(matchCount == 1) {
            printf("%c", match[matchCount-1]);
            fwrite(&match[matchCount-1], sizeof(char), 1, file);

            matchCount = 0; //Reset counteru, aby jsem začali opět od začátku
            readingBit = 0;
            decodedCharLen++;
        }
    }
    free(match);
    
    printf("\n");
    fclose(file);
}

