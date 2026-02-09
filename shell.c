/***
 Author: Thierry Laguerre
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 80  /* The maximum length command */

int main(void)
{
    char *args[MAX_LINE/2 + 1];
    char input[MAX_LINE];
    char history[MAX_LINE]; 
    int has_history = 0;  
    int should_run = 1; 

    
    while (should_run == 1) {
        printf("osh>");
        fflush(stdout);
        
        /**
        * After reading user input, the steps are:
        * (1) fork a child process using fork()
        * (2) the child process will invoke execvp()
        * (3) parent will invoke wait() unless command included &
        */
        
        if (fgets(input, MAX_LINE, stdin) == NULL) break;
        
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0) {
            should_run = 0;
            continue;
        }
        
        if (strcmp(input, "!!") == 0) {
            if (has_history == 0) {
                printf("No commands in history.\n");
                continue; 
            }
            strcpy(input, history);
            printf("%s\n", input);
        } else {
            strcpy(history, input);
            has_history = 1;
        }

        int i = 0;
        char *token = strtok(input, " \n");
        while (token != NULL){
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;
        
        if (i > 0){
            int background = 0;
            if (strcmp(args[i-1], "&") == 0) {
                background = 1;
                args[i-1] = NULL; // Remove '&' so the command doesn't see it
            }
            pid_t pid = fork();
            
            if (pid < 0) { // Error
                fprintf(stderr, "Fork Failed");
                return 1;
            } 
            else if (pid == 0) { // Child Process
                execvp(args[0], args);
                printf("Invalid Command\n"); // Only runs if execvp fails
                return 1;
            } 
            else { // Parent Process
                if (background == 0) {wait(NULL);}
                else {printf("Background process started\n");} 
            }
        }
    }

    printf("Program terminated\n");
    return 0;
}