#include "cshell.h"

char wdBuffer[512];
char inputBuffer[512];
char* delims = " ";
int id;

//please dont test multiple redirects or piping thank u

int main(int argc, char** argv){
    //set sig handler
    signal(SIGINT, interruptHandler);
    id = getpid();

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
    char* argBuffer[128];

    int newCommand = 1, count = 0;
    struct ListNode* traverser = tokenList;
    while(traverser != NULL){
        //; execute currently stored command
        if(strcmp(traverser->val, ";") == 0){
            argBuffer[count] = NULL;
            count++;
            if(strcmp(commandBuffer, "cd") == 0){
                struct ListNode* cdList = NULL;
                int i;
                for(i = 0; i < count-1; i++){
                    cdList = insert(cdList, argBuffer[i]);
                }
                dochdir(cdList);
                cleanList(cdList);
            }
            else{
                if(fork() == 0){
                    execvp(commandBuffer, argBuffer);
                }
                else{
                    wait(NULL);
                }
            }
            count = 0;
            newCommand = 1;
            traverser = traverser->next;
            if(traverser == NULL) {
                //if(DEBUG)printf("had to break after semicolon\n");
                break;
            }
        }

        //redirection >> >
        else if(strcmp(traverser->val, ">") == 0 || strcmp(traverser->val, ">>") == 0){ // > is make new, >> is append
            int mode = 0;
            //1 = rewrite, 2 = append
            if(strcmp(traverser->val, ">") == 0) mode = 1;
            else mode = 2;
            
            //get name of file by getting next token
            char fileBuffer[100];
            traverser = traverser->next;
            if(traverser == NULL) return;
            strcpy(fileBuffer, traverser->val);

            //open the file and duplicate the fds
            if(mode == 1){
                //erases whatever was in the file
                fclose(fopen(fileBuffer, "w"));
            }
            int fd = open(fileBuffer, O_CREAT | O_WRONLY | O_APPEND);
            //save current stdout location to restore it later
            int oldStdout = dup(1);

            dup2(fd, 1);

            //now run the previously stored command
            argBuffer[count] = NULL;
            count++;
            if(fork() == 0){
                execvp(commandBuffer, argBuffer);
            }
            else{
                wait(NULL);
            }
            //restore old stdout
            dup2(oldStdout, 1);
            close(oldStdout);
            close(fd);

            count = 0;
            newCommand = 1;
            traverser = traverser->next;
            if(traverser == NULL) {
                //if(DEBUG)printf("had to break after redirection\n");
                break;
            }
            else{ //have to go again, because if it didnt break already, then there's a semicolon after that we have to get through.
                traverser = traverser->next;
                if(traverser == NULL) { //check again if it reached null
                    //if(DEBUG)printf("had to break after redirection\n");
                    break;
                }
            }
        }

        //piping
        else if(strcmp(traverser->val, "|") == 0){
            char pipeCommandBuffer[50];
            char* pipeArgBuffer[128];
            int pipeArgCount = 0;
            //gather the second command and put it in the above buffers
            traverser = traverser->next;
            strcpy(pipeCommandBuffer, traverser->val);
            while(traverser->next != NULL){
                pipeArgBuffer[pipeArgCount] = traverser->val;
                pipeArgCount++;
                if(strcmp(traverser->next->val, ";") == 0) break;
                traverser = traverser->next;
            }
            pipeArgBuffer[pipeArgCount] = traverser->val;
            pipeArgCount++;
            // pipeArgBuffer[pipeArgCount] = NULL;
            // pipeArgCount++;

            //open a pipe of communication to be used between 2 children
            int pipeBuffer[128];
            int warp[2];
            char outputBuffer[512];
            //prep temp file
            fclose(fopen("tempforpiping123123", "w")); //cleans file
            int fd = open("tempforpiping123123", O_CREAT | O_WRONLY | O_APPEND);
            int oldStdout = dup(1);
            dup2(fd, 1);


            //execute first command
            argBuffer[count] = NULL;
            count++;
            if(fork() == 0){
                execvp(commandBuffer, argBuffer);
                exit(0);
            }
            else{
                wait(NULL);
            }
            dup2(oldStdout, 1);
            close(oldStdout);
            close(fd);
            
            //grab all the results from the first command's temp file, add them and keep forking to execvp
            FILE* fp = fopen("tempforpiping123123", "r");

            char lineBuffer[512];
            while(fgets(lineBuffer, 512, fp)){
                int length = strlen(lineBuffer);
                //strip the newline character
                lineBuffer[length-1] = '\0';
                //printf("%s", lineBuffer);
                if(fork() == 0){
                    strcpy(pipeArgBuffer[pipeArgCount], lineBuffer);
                    pipeArgBuffer[pipeArgCount+1] = NULL;
                    execvp(pipeCommandBuffer, pipeArgBuffer);
                }
                else{
                    wait(NULL);
                }
            }
            fclose(fp);
            remove("tempforpiping123123");
            count = 0;
            newCommand = 1;
            traverser = traverser->next;
            if(traverser == NULL) {
                //if(DEBUG)printf("had to break after redirection\n");
                break;
            }
            else{ //have to go again, because if it didnt break already, then there's a semicolon after that we have to get through.
                traverser = traverser->next;
                if(traverser == NULL) { //check again if it reached null
                    //if(DEBUG)printf("had to break after redirection\n");
                    break;
                }
            }
        }

        //just had a special character or initializing, bring in the new command name
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
        if(strcmp(commandBuffer, "cd") == 0){
            struct ListNode* cdList = NULL;
            int i;
            for(i = 0; i < count-1; i++){
                cdList = insert(cdList, argBuffer[i]);
            }
            dochdir(cdList);
            cleanList(cdList);
        }
        else{
            if(fork() == 0){
                execvp(commandBuffer, argBuffer);
            }
            else{
                wait(NULL);
            }
        }
    }
}

void dochdir(struct ListNode* tokenList)
{
    tokenList = tokenList->next;
    if(tokenList == NULL) chdir("/");

    char pathBuffer[256];
    memset(pathBuffer, 0, 256);
    strcat(pathBuffer, wdBuffer);
    int length = strlen(pathBuffer);
    pathBuffer[length-3] = '/';
    pathBuffer[length-2] = '\0';
    struct ListNode* traverser = tokenList;
    while(traverser != NULL){
        length = strlen(pathBuffer);
        //strcat(pathBuffer, traverser->val);
        if(strcmp(traverser->val, "..") == 0){
            int i;
            for(i = length-2; i >= 0; i--){
                if(pathBuffer[i] == '/'){
                    pathBuffer[i+1] = '\0';
                    break;
                }
            }
        }
        else{
            strcat(pathBuffer, traverser->val);
            strcat(pathBuffer, "/");
        }
        if(traverser == NULL) break;
        traverser = traverser->next;
    }
    //printf("%s\n", pathBuffer);

    chdir(pathBuffer);
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
    
    if( getpid() != id )
    {
        exit(0);

    }
    printf("%s", wdBuffer);
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

char* substr(char* str, char* sub , int start, int len){
    memcpy(sub, &str[start], len);
    sub[len] = '\0';
    return sub;
}
