#include "cshell.h"

char wdBuffer[500];
char inputBuffer[500];
char* delims = " ";
struct ListNode* tokenList = NULL;

int main(int argc, char** argv){
    //command shell inf loop
    while(1){
        tokenList = NULL;
        //setup prompt
        getcwd(wdBuffer, 500);
        strcat(wdBuffer, " $ ");
        printf("%s", wdBuffer);

        //get input - space at the beginning to accomodate for loop, [^\n] says to take input until a new line
        scanf(" %[^\n]s", inputBuffer);
        printf("input: %s\n", inputBuffer);
        //tokenize input into LL
        char* token;

        //get first tok
        token = strtok(inputBuffer, delims);

        //get rest of tokens
        while(token != NULL){
            tokenList = insert(tokenList, token);
            token = strtok(NULL, delims);
        }
        //printList(tokenList);
        //printf("head: %s\n", tokenList->val);

        //then do the command here
    }
    return 0;
}

//no need to worry about duplicates or anything just insert
struct ListNode* insert(struct ListNode* head, char* val){
    struct ListNode* toInsert = (struct ListNode*)malloc(sizeof(struct ListNode));
    toInsert->next = NULL;
    strcpy(toInsert->val, val);
    //empty
    if(head == NULL){
        head = toInsert;
        printf("inserted\n");
        return head;
    }
    //not empty
    struct ListNode* traverser = head;
    struct ListNode* lagger = NULL;
    while(traverser != NULL){
        lagger = traverser;
        traverser = traverser->next;
    }
    lagger->next = toInsert;
    //printf("inserted\n");
    return head;
}

void printList(struct ListNode* head){
    struct ListNode* traverser = head;
    while(traverser != NULL){
        printf("%s ", traverser->val);
        traverser = traverser->next;
    }
}
