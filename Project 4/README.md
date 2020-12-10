# Project 4: User-Level Shell
## Elijah Ongoco (edo16), Michael Comatas (mac776)
## 10 December 2020

# Shell Instructions:

Our shell works like a basic unix shell, except that everything is delmited with a space.
To compile it use the Makefile using 'make clean' and 'make all' commands.

# Function Descriptions:

## void doCommands(struct ListNode* tokenList)

The doCommands function parses the linked list in order to handle the commands respectively. Since the commands are taken care of using execvp, the doCommands function also takes care of special cases such as ";", ">", ">>", and "|". 
When the function has deemed it logically necessary to use execvp (whether it's because it has encountered a semicolon or it has reached the end of the token list), a child process is forked and is used to execute execvp with the appropriate arguments, as execvp functionally kills the process after completion.


## void dochdir(struct ListNode* tokenList)

The purpose of this function was to change directories, since it is handled differently than the rest of the commands. It takes in the token list and builds the directory path to successfully change into that directory.

## void interruptHandler(int signalNumber)

This function was used to handle the control c signal handler if a user did control c in the shell or if control c was done during a command.

## struct ListNode* insert(struct ListNode* head, char* val)

This function was used to insert a token into the linked list of arguments.

## void cleanList(struct ListNode* head)

This function was a way to free the tokens/list memory.

## void printList(struct ListNode* head)

This function was a way to print out the tokens/arguments for the commands. This was mainly used for testing/debugging.

# What we struggled with/did not implement

One of the main things we struggled with was implementing chdir to use quotation marks/spaces. For example our main project folder was named 'Project 4' and if we were to do the command "cd .." and then try "cd 'Project 4'" it would give us an error. 