#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#define DEBUG 1

struct ListNode{
    char val[100];
    struct ListNode* next;
};

struct ListNode* insert(struct ListNode* head, char* val);
void cleanList(struct ListNode* head);
void printList();
void interruptHandler(int signalNumber);