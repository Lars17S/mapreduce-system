#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORD_SIZE 50
#define UNIT_SIZE 54


int binarySearch(FILE* f, int l, int r, char target[50]) {
    int mid;
    char word[MAX_WORD_SIZE];
    while(l <= r){
        mid = (l + r)/2;
        fseek(f, mid * UNIT_SIZE, SEEK_SET);
        fread(&word, sizeof(char), MAX_WORD_SIZE, f);
        //printf("l: %d | r: %d | m: %d | word: %s\n", l, r, mid, word);
        if (strcmp(word, target) == 0)
            return mid;
        else if (strcmp(word, target) > 0)
            r = mid - 1;
        else if (strcmp(word, target) < 0)
            l = mid + 1;
    }
    return -1;
}


int main() {
    char word[MAX_WORD_SIZE];
    printf("Palabra que deseas buscar: ");
    scanf("%s", word);

    for (int i = 0; i < strlen(word); ++i)
        if (islower(word[i])) word[i] = toupper(word[i]);

    FILE * rOne = fopen("fileOne", "rb+");
    if (rOne == NULL) {
        fprintf(stderr, "ERROR");
        return EXIT_FAILURE;
    }

    FILE * rTwo = fopen("fileTwo", "rb+");
    if (rTwo == NULL) {
        fprintf(stderr, "ERROR");
        return EXIT_FAILURE;
    }

    fseek(rOne, 0, SEEK_END);
    int lengthUnits = ftell(rOne) / UNIT_SIZE;
    fseek(rOne, 0, SEEK_SET);

    int sol = binarySearch(rOne, 0, lengthUnits, word);
    if (sol == -1) {
        printf("ERROR 404 | Not found!\n");
        return EXIT_SUCCESS;
    }

    int regFileOne;
    fseek(rOne, (sol * UNIT_SIZE) + (sizeof(char) * MAX_WORD_SIZE), SEEK_SET);
    fread(&regFileOne, sizeof(int), 1, rOne);
    printf("Palabra [%s] en primer registro: %d\n", word, regFileOne + 1);

    int posLine;
    int nextReg;
    fseek(rTwo, regFileOne * (2 * sizeof(int)), SEEK_SET);
    fread(&posLine, sizeof(int), 1, rTwo);
    fread(&nextReg, sizeof(int), 1, rTwo);


    printf("Lineas: ");
    printf("%d ", posLine);
    while (nextReg != -1) {
        fseek(rTwo, nextReg * (2 * sizeof(int)), SEEK_SET);
        fread(&posLine, sizeof(int), 1, rTwo);
        fread(&nextReg, sizeof(int), 1, rTwo);
        printf("%d ", posLine);
    }

    fclose(rOne);
    fclose(rTwo);    
    return EXIT_SUCCESS;
}