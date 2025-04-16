#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*
    WRITING
*/
typedef struct BitWriter {
    FILE *file; // Output file
    unsigned char buffer; // Upravuejme 8 bitů u jednoho charakteru, který se potom ukládá so souboru
    int bitCount; // Kolik bitů bylo upraveno
} BitWriter;

void initBitWriter(BitWriter *bitWriter, char *outputFileName);

// Funkce na zapsání do souboru počet znaků
void writeCharCount(BitWriter *bitWriter, int count);
/* Funkce na zapsání znaku do kódovací tabulky
    bitWriter - instance BitWriter
    character - Znak
    code - Kód znaku
    codeLength - Délka kódu
*/
void writeTable(BitWriter *bitWriter, unsigned char character, char *code, int codeLength);
// Funcke pomocí které se zapisují bity
void writeBit(BitWriter *bitWriter, int bit);
// Funkce na zapsání bitu z charakteru ('0' neo '1')
void writeBitFromChar(BitWriter *bitWriter, char bit);
// Uloží aktuální buffer
void flushBits(BitWriter *bitWriter);
// Uloží a ukončí zapisování do souboru
void closeBitWriter(BitWriter *bitWriter);

/*
    READING
*/
typedef struct BitReader {
    FILE *file;
    unsigned char buffer;  // 8 bitový znak ze souboru
    int bitCount;          // Kolik bitů zbývá přečíst
} BitReader;

void initBitReader(BitReader *br, const char *filename);
/* Funkce na přečtení jak dlouhá je zakódovaná zpráva.
    @note Může se stát, že přebývají bity na konci, protože se ukládá 8 bitovej char.
*/
void readCharCount(BitReader *br, int *strLen);
/* Funkce na čtení bitů ze souboru.
    @return 1 pokud je co číst. Pokud již není, tak je vrácena 0 a čtení by mělo být zastaveno. 
*/
int readBit(BitReader *br, int *bit);
/* Funkce na přečtení dalšího znaku v pořadí z kódovací tabulky.
    @return 1 pokud je co číst. Pokud již není, tak je vrácena 0 a čtení by mělo být zastaveno. 
*/
int readTable(BitReader *br, unsigned char *character, char **code);
// Ukončí čtení ze souboru.
void closeBitReader(BitReader *br);