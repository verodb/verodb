#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *aofFile = NULL;

void initAof(const char *filename) {
    aofFile = fopen(filename, "a");
    if (!aofFile) {
        perror("Failed to open AOF file");
        exit(EXIT_FAILURE);
    }
}

void appendToAof(const char *command) {
    if (aofFile) {
        fprintf(aofFile, "%s\n", command);
        fflush(aofFile);
    }
}

void closeAof() {
    if (aofFile) {
        fclose(aofFile);
        aofFile = NULL;
    }
}
close(file);

