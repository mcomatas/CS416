#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define DEBUG 1

struct ListNode{
    char val[100];
    struct ListNode* next;
};

struct ListNode* insert(struct ListNode* head, char* val);
void printList();