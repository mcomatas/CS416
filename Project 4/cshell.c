#include "cshell.h"

char wdBuffer[500];
char inputBuffer[500];
char* delims = " ";

int main(int argc, char** argv){
    //set sig handler
    //signal(SIGINT, interruptHandler);

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
            cleanList(tokenList);
            exit(0);
        }

        //otherwise get rest of tokens
        while(token != NULL){
            tokenList = insert(tokenList, token);
            token = strtok(NULL, delims);
        }


        doCommands(tokenList);
        // //fork a child process to run the commands
        // if(fork() == 0){
        //     doCommands(tokenList);
        //     //stop forkbombing ilab
        //     exit(0);
        // }
        // //have the original parent process wait on it
        // else{
        //     wait(NULL);
        // }

        //clean list to prep for next loop iteration
        cleanList(tokenList);
    }
    return 0;
}

void doCommands(struct ListNode* tokenList){
    char commandBuffer[50];
    char* argBuffer[50];

    int newCommand = 1, count = 0;
    struct ListNode* traverser = tokenList;
    while(traverser != NULL){
        //deal with a semicolon by executing the currently stored command
        if(strcmp(traverser->val, ";") == 0){
            argBuffer[count] = NULL;
            count++;
            if(fork() == 0){
                execvp(commandBuffer, argBuffer);
            }
            else{
                wait(NULL);
            }
            count = 0;
            newCommand = 1;
            traverser = traverser->next;
            if(traverser == NULL) break;
        }

        //just had a ; or initializing, bring in the new command name
        if(newCommand){
            strcpy(commandBuffer, traverser->val);
            newCommand = 0;
        }

        argBuffer[count] = traverser->val;
        count++;
        traverser = traverser->next;
    }

    argBuffer[count] = NULL;
    count++;
    if(argBuffer[0] != NULL){
        if(fork() == 0){
            execvp(commandBuffer, argBuffer);
        }
        else{
            wait(NULL);
        }
    }
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
