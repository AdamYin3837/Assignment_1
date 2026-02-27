#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void leak_one() {
    int *ptr = malloc(10 * sizeof(int));
    // Problem: ptr is never freed before the function returns.
}

void overflow_example() {
    char buffer[5];
    strcpy(buffer, "This string is way too long!"); // Problem: Overflow
}

void leak_two() {
    int *ptr = malloc(20 * sizeof(int)); 
    
    ptr = malloc(10 * sizeof(int)); // 第二次分配：要了 40 bytes
    
    free(ptr); 
}

int main(int argc, char *argv[]) {
    if (argc < 3) return 1;
    
    if (strcmp(argv[1], "--LEAK") == 0) {
        int choice = atoi(argv[2]);
        if (choice == 1) leak_one();
        if (choice == 2) overflow_example();
        if (choice == 3) leak_two();
    }
    return 0;
}
