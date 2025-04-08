#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char character;
    unsigned int frequency;
} HuffmanNode;

int main() {
    // Open file in write-binary mode
    FILE *file = fopen("compressed.huff", "wb");
    if (!file) {
        perror("File opening failed");
        return 1;
    }

    // Example Huffman nodes (binary data)
    HuffmanNode nodes[] = {
        {'A', 10},
        {'B', 5},
        {'C', 2},
        {'D', 5},
        {'E', 5},
    };
    int nodeCount = sizeof(nodes) / sizeof(nodes[0]);

    // Write binary data to file
    fwrite(&nodeCount, sizeof(int), 1, file);  // Save number of nodes
    fwrite(nodes, sizeof(HuffmanNode), nodeCount, file);

    fclose(file);  // Close file

    // Reading the binary file
    file = fopen("compressed.huff", "rb");
    if (!file) {
        perror("File opening failed");
        return 1;
    }

    // Read number of nodes
    int readCount;
    fread(&readCount, sizeof(int), 1, file);

    HuffmanNode *readNodes = malloc(readCount * sizeof(HuffmanNode));
    fread(readNodes, sizeof(HuffmanNode), readCount, file);

    printf("Read Huffman Nodes:\n");
    for (int i = 0; i < readCount; i++) {
        printf("Char: %c, Freq: %u\n", readNodes[i].character, readNodes[i].frequency);
    }

    free(readNodes);
    fclose(file);

    return 0;
}
