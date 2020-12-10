#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DEBUG 1

struct ListNode{
    char val[100];
    struct ListNode* next;
};

char* substr(char* str, char* sub , int start, int len);
void dochdir(struct ListNode* tokenList);
void doCommands(struct ListNode* tokenList);
struct ListNode* insert(struct ListNode* head, char* val);
void cleanList(struct ListNode* head);
void printList();
void interruptHandler(int signalNumber);