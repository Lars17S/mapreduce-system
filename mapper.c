#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MAX_WORD_SIZE 50
#define MAX_LINE_SIZE 100
#define P1 1
#define P2 2
#define START_LINE_A 1801
#define START_LINE_B 1

char delimit[] = " \n\t'.!`?-,:;\"()30*[]_";
pid_t pid, ppid;

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

void mapper(FILE* f, int p_id) {
    int num_line = (p_id == P1) ? START_LINE_A : START_LINE_B;
    char line[MAX_LINE_SIZE];
    while (fgets(line, MAX_LINE_SIZE, f) != NULL) {
        char* rest = line;
        char* token;
        while ((token = strtok_r(rest, delimit, &rest))) {
            for (int k = 0; k < strlen(token); ++k)
                if (islower(token[k])) token[k] = toupper(token[k]);
            
            printf("%s;%d\n", token, num_line);
            fflush(stdout);
        }
        ++num_line;
    }
}

void wToFiles(FILE* fOne, FILE* fTwo, tableOne *head) {
    tableOne * node = head;
    int lim = -1;
    while (node != NULL) {
        fwrite(node->word, sizeof(char), MAX_WORD_SIZE, fOne);
        int linesFilePos = ftell(fTwo) / (2 * sizeof(int));
        // printf("word: %s, reg: %d\n", node->word, linesFilePos);
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
    FILE * fptr;
    FILE * fOne;
    FILE * fTwo;
    char name_file_a[] = "alice_a.txt";
    char name_file_b[] = "alice_b.txt";

    ppid = getpid();
    if ( (pid = fork()) == 0) {
        FILE* f_a;
        if ( (f_a = fopen("alice_a.txt", "r")) == NULL) {
            fprintf(stderr, "ERROR Reading file");
            return EXIT_FAILURE;
        }
        mapper(f_a, P2);
        fclose(f_a);
    }
    else {
        FILE* f_b;
        if ( (f_b = fopen("alice_b.txt", "r")) == NULL) {
            fprintf(stderr, "ERROR Reading file");
            return EXIT_FAILURE;
        }
        mapper(f_b, P1);
        fclose(f_b);
        wait(&pid);
    }
    
    return EXIT_SUCCESS;
}