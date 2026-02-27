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
    // 第一次分配：要了 80 bytes (20 * 4) 的記憶體，交給 ptr
    int *ptr = malloc(20 * sizeof(int)); 
    
    // 問題發生在這裡！我們沒有 free(ptr)，就直接讓 ptr 指向新的記憶體
    ptr = malloc(10 * sizeof(int)); // 第二次分配：要了 40 bytes
    
    // 我們雖然乖乖寫了 free，但這只會釋放「第二次」要到的 40 bytes
    // 第一次要到的 80 bytes 已經徹底失去了指標，變成絕對的 Memory Leak (Lost Pointer)
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