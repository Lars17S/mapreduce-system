#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_WORD_SIZE 50

char input[MAX_WORD_SIZE];
char prev_word[MAX_WORD_SIZE];

typedef struct tableTwo {
    int line;
    struct tableTwo* next;
} tableTwo;

typedef struct tableOne {
    char word[MAX_WORD_SIZE];
    struct tableTwo* arch;
    struct tableOne* next;
} tableOne;

void insertTableTwo(tableTwo* node, tableTwo** head) {
    if (*head == NULL) {
        *head = node;
        return;
    }
    if (node->line == (*head)->line)
        return;
    insertTableTwo(node, &((*head)->next));
}

void insertTableOne(tableOne* node, tableOne** head) {
    if(*head == NULL) {
        *head = node;
        return; 
    }

    if(strcmp(node->word, (*head)->word) < 0) {
        node->next = *head;
        *head = node;
        return;
    }

    tableOne *nodeOne = *head;
    tableOne *nodeTwo = nodeOne->next;
    while(nodeTwo != NULL && strcmp(node->word, nodeTwo->word) > 0) {
        nodeOne = nodeTwo;
        nodeTwo = nodeTwo->next;
    }

    if (strcmp(node->word, nodeOne->word) == 0) {
        insertTableTwo(node->arch, &(nodeOne->arch));
        return;
    }

    if(nodeTwo != NULL) {
        if (strcmp(node->word, nodeTwo->word) == 0) {
            insertTableTwo(node->arch, &(nodeTwo->arch));
        }
        else {
            node->next = nodeTwo;
            nodeOne->next = node;
        }
        return;
    }

    nodeOne->next = node;
}

void print(tableOne *head) {
    tableOne * node = head;
    while (node != NULL) {
        printf("%s: ", node->word);
        tableTwo * node2 = node->arch;
        while (node2 != NULL) {
            printf("%d ", node2->line);
            node2 = node2->next;
        }
        printf("\n");
        node = node->next;
    }
}

void wToFiles(FILE* fOne, FILE* fTwo, tableOne *head) {
    tableOne * node = head;
    int lim = -1;
    while (node != NULL) {
        fwrite(node->word, sizeof(char), MAX_WORD_SIZE, fOne);
        int linesFilePos = ftell(fTwo) / (2 * sizeof(int));
        fwrite(&linesFilePos, sizeof(int), 1, fOne);

        tableTwo * node2 = node->arch;
        
        while (node2->next != NULL) {
            fwrite(&node2->line, sizeof(int), 1, fTwo);
            ++linesFilePos;
            fwrite(&linesFilePos, sizeof(int), 1, fTwo);
            node2 = node2->next;
        }
        fwrite(&node2->line, sizeof(int), 1, fTwo);
        fwrite(&lim, sizeof(int), 1, fTwo);
        node = node->next;
    }
}

int main() {
    
    tableOne * head = NULL;
    while (scanf("%s", input) != EOF) {
        char* word = strtok(input, ";");
        char* line_str = strtok(NULL,";");
        tableOne* newNodeOne = (tableOne *) malloc(sizeof(tableOne));
        tableTwo* newNodeTwo = (tableTwo *) malloc(sizeof(tableOne));
          
        int num_line = atoi(line_str);
        newNodeTwo->line = num_line;
        newNodeTwo->next = NULL;
        
        strcpy(newNodeOne->word, word);
        newNodeOne->arch = newNodeTwo;
        newNodeOne->next = NULL;

        insertTableOne(newNodeOne, &head);
    }
   
    FILE * fOne;
    FILE * fTwo;

    fOne = fopen("fileOne", "wb+");
    if (fOne == NULL) {
        fprintf(stderr, "ERROR");
        return EXIT_FAILURE;
    }

    fTwo = fopen("fileTwo", "wb+");
    if (fTwo == NULL) {
        fprintf(stderr, "ERROR");
        return EXIT_FAILURE;
    }

    wToFiles(fOne, fTwo, head);

    fclose(fOne);
    fclose(fTwo);

    return EXIT_SUCCESS;
}