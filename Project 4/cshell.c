#include "cshell.h"

char wdBuffer[500];
char inputBuffer[500];
char* delims = " ";

int main(int argc, char** argv){
    //set sig handler
    signal(SIGINT, interruptHandler);
    //command shell inf loop, will be broken if ctrl+c happens + "exit" is entered
    while(1){
        struct ListNode* tokenList = NULL;
        //setup prompt
        getcwd(wdBuffer, 500);
        strcat(wdBuffer, " $ ");
        printf("%s", wdBuffer);

        //get input - space at the beginning to accomodate for loop, [^\n] says to take input until a new line
        scanf(" %[^\n]s", inputBuffer);
        //printf("input: %s\n", inputBuffer);
        //tokenize input into LL
        char* token;

        //get first tok
        token = strtok(inputBuffer, delims);

        //if the command is exit get out
        if(strcmp(token, "exit") == 0){
            exit(0);
        }

        //get rest of tokens
        while(token != NULL){
            tokenList = insert(tokenList, token);
            token = strtok(NULL, delims);
        }
        //printList(tokenList);
        //printf("head: %s\n", tokenList->val);

        

        //then do the command here

        //clean list to prep for next loop
        cleanList(tokenList);
    }
    return 0;
}

//interrupt handler (ctrl+c)
void interruptHandler(int signalNumber){
    // char interruptBuffer[500];
    // //printf("\nSIGINT detected.  If you wish to exit, type in \"exit\": ");

    // scanf(" %[^\n]s", interruptBuffer);
    // //printf("%s\n", interruptBuffer);
    // if(strcmp(interruptBuffer, "exit") == 0){
    //     //shellOpen = 0; //this will make it so the loop dont run again
    //     exit(0);
    // }

    // //signal(SIGINT, interruptHandler);
}

//no need to worry about duplicates or anything just insert
struct ListNode* insert(struct ListNode* head, char* val){
    struct ListNode* toInsert = (struct ListNode*)malloc(sizeof(struct ListNode));
    toInsert->next = NULL;
    strcpy(toInsert->val, val);
    //empty
    if(head == NULL){
        head = toInsert;
        //printf("inserted\n");
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

//freees the nodes
void cleanList(struct ListNode* head){
    struct ListNode* traverser = head;
    struct ListNode* lagger = NULL;
    while(traverser != NULL){
        lagger = traverser;
        traverser = traverser->next;
        free(lagger);
    }
}

//prints the list
void printList(struct ListNode* head){
    struct ListNode* traverser = head;
    while(traverser != NULL){
        printf("%s ", traverser->val);
        traverser = traverser->next;
    }
}
