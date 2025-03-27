#include "huff.h"

Huffman* initHuffmanFromText(char *inputText) {
    Huffman *huff = (Huffman*)malloc(sizeof(Huffman));

    huff->inputText = inputText;
    huff->count = countCharacters(inputText);

    huff->process = compress;

    return huff;
}

Huffman* initHuffmanFromTable(int *count) {
    Huffman *huff = (Huffman*)malloc(sizeof(Huffman));

    huff->count = count;
    huff->process = simpleProcess;

    return huff;
}
Huffman* initHuffmanFromBinary(char *inputValues) {
    Huffman *huff = (Huffman*)malloc(sizeof(Huffman));
    // TODO

    huff->process = decompress;

    return huff;
}

void compress(Huffman* self) {
    createTable(self->count);
    // Vytvořit strom z tabulky četností
    // Získat znak-kód
    // Zakódovat
    // Print
}
void compressIntoFile(Huffman* self) {
    // Vytvořit strom z tabulky četností
    // Získat znak-kód
    // Zakódovat s cílem uložit
    // Nějak uložit
}
void decompress(Huffman* self) {

}
void simpleProcess(Huffman* self) {
    createTable(self->count);
    // Vytvořit strom z tabulky četností
    // Získat znak - kód
    // Print
}

void createTable(int *inputCount) {
    Node nodes[ASCII_SIZE];

    int inputs = 0;

    for(int i = 0; i < ASCII_SIZE; i++) {
        if(inputCount[i] > 0) {
            //Node newNode = {.count = inputCount[i], .znaky = (unsigned char)i};
            Node newNode = {.count = inputCount[i], .charCount = 1, .znaky = malloc(1*sizeof(int))};
            memset(newNode.znaky, 0, 1*sizeof(int));
            newNode.znaky[newNode.charCount-1] = (unsigned char) i;
            if(inputs == 0)
                nodes[inputs] = newNode;
            else
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
    
    for(int i = 0; i < inputs-1; i = i + 2) {
        Node first = nodes[i];
        Node second = nodes[i+1];

        Node newNode = {.count = (first.count + second.count), 
                        .charCount = 0, 
                        .znaky = malloc((first.charCount + second.charCount)*sizeof(int))};
        memset(newNode.znaky, 0, (first.charCount + second.charCount)*sizeof(int));
        
        for(int m = 0; m < first.charCount; m++){
            newNode.znaky[(newNode.charCount++)] = first.znaky[m];
        }
        for(int m = 0; m < second.charCount; m++) {
            newNode.znaky[(newNode.charCount++)] = second.znaky[m];
        }

        //printf("Dohromady: '%s' : %d; %d\n", newNode.znaky, newNode.count, newNode.charCount);

        for(int j = 0; j <= inputs; j++){
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
        printf("'%s' : %d : %d\n", node.znaky, node.count, node.charCount);
    }

    printf("DEBUG:\n");

    char freePrefixes[10][10];
    memset(freePrefixes, 0, 10*sizeof(char));
    int freePrefixesSize = -1;

    for(int ii = (inputs-2); ii >= 0; ii=ii-2) { //(inputs-2)... protože -1 jdeme od posledního, ale protože chceme vynechat sumační součet všech, tak -2  
        bool jednicka = false;

        char prefix[10];
        if(freePrefixesSize >= 0) {
            sprintf(prefix, "%s", freePrefixes[0]);
            //todo - POSUNUTÍ?
            for(int pref = 0; pref <= freePrefixesSize; pref++){
                if(pref != 0)
                    sprintf(freePrefixes[pref-1], "%s", freePrefixes[pref]);
                memset(freePrefixes[pref], 0, 10*sizeof(char));
            }
            freePrefixesSize--;
        } else
            memset(prefix, 0, 10*sizeof(char));

        for(int j = 0; j <=1; j++) {
            Node node = nodes[ii-j];
            char sequence = jednicka ? '1': '0';
            char code[10];

            sprintf(code, "%s%c", prefix, sequence);

            if(node.charCount == 1) { // Tak je to jenom jeden znak a kód nebudeme dávat do freePrefixes
                printf("'%s' : %d : %d : %s\n", node.znaky, node.count, node.charCount, code);
            } else {
                strcat(freePrefixes[++freePrefixesSize], code); // Vložíme do freePrefixes
                printf("'%s' : %d : %d : %s\n", node.znaky, node.count, node.charCount, code);
            }
            jednicka = !jednicka;
        }
    }

    // FREE - for now
    for(int i = 0; i < inputs; i++)
        free(nodes[i].znaky);

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