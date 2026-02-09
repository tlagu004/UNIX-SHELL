#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>  
#include <sys/stat.h>

#define MAX_LINE 80  

int main(void)
{
    char *args[MAX_LINE/2 + 1];
    char input[MAX_LINE] = "";
    char history[MAX_LINE]; 
    int has_history = 0;  
    int should_run = 1; 

    
    while (should_run == 1) {
        printf("osh> ");
        fflush(stdout);
        
        /**
        * After reading user input, the steps are:
        * (1) fork a child process using fork()
        * (2) the child process will invoke execvp()
        * (3) parent will invoke wait() unless command included &
        */
        
        // USER INPUT
        if (fgets(input, MAX_LINE, stdin) == NULL) break;
        input[strcspn(input, "\n")] = '\0';

        // EXIT 
        if (strcmp(input, "exit") == 0) {
            fprintf(stderr, "(exit detected) Terminating shell...\n");
            should_run = 0;
            continue;
        }
        
        // HISTORY FEATURE
        if (strcmp(input, "!!") == 0) {
            printf("(!! detected)");
            if (has_history == 0) {
                printf("History is empty.\n");
                continue; 
            }
            strcpy(input, history);
            printf("%s\n", input);
        } else {
            strcpy(history, input);
            has_history = 1;
        } 

        // TOKENIZATION
        int i = 0;
        char temp_input[MAX_LINE];
        strcpy(temp_input, input);
        char *token = strtok(temp_input, " \n");
        while (token != NULL){
            args[i++] = token;
            token = strtok(NULL, " \n");
        }
        args[i] = NULL;
        if (i == 0) continue;
        
        fprintf(stderr, "Executing command: %s\n", args[0]);

        // Check Pipe and RUN PIPE
        int pipe_idx = -1;
        for (int j = 0; args[j] != NULL; j++) {
            if (strcmp(args[j], "|") == 0) {
                pipe_idx = j;
                break;
            }
        }

        if (pipe_idx != -1) {
            fprintf(stderr, "( | pipe detected)");
            args[pipe_idx] = NULL; 
            char **args2 = &args[pipe_idx + 1]; 
            int fd[2];
            if (pipe(fd) == -1) {
                perror("Pipe failed");
            }
        
            if (fork() == 0) {
                fprintf(stderr, "Child 1 (Left of Pipe) redirecting stdout...\n");
                dup2(fd[1], STDOUT_FILENO);
                close(fd[0]); 
                close(fd[1]);
                execvp(args[0], args);
                exit(1);
            }
        
            if (fork() == 0) {
                fprintf(stderr, "Child 2 (Right of Pipe) redirecting stdin...\n");
                dup2(fd[0], STDIN_FILENO); 
                close(fd[1]); 
                close(fd[0]);
                execvp(args2[0], args2);
                exit(1);
            }

            close(fd[0]);
            close(fd[1]);
            fprintf(stderr, "Parent waiting for piped processes to finish...\n");
            wait(NULL);
            wait(NULL);
        } else {
            // REGULAR COMMAND / REDIRECTION
            int background = 0;
            if (i > 0 && strcmp(args[i-1], "&") == 0) {
                fprintf(stderr, "(& detected) Background execution \n");
                background = 1;
                args[i-1] = NULL; 
            }
            pid_t pid = fork();
            
            if (pid < 0) { 
                fprintf(stderr, "Fork Failed");
                return 1;
            } else if (pid == 0) { 
                for (int j = 0; args[j] != NULL; j++) {
                    if (strcmp(args[j], ">") == 0) {
                        fprintf(stderr, "Redirecting output to %s\n", args[j+1]);
                        int fd = open(args[j+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        if (fd < 0) {
                            perror("Error opening file");
                            exit(1);
                        }
                        dup2(fd, STDOUT_FILENO); 
                        close(fd);  
                        args[j] = NULL;
                    }
                    
                else if (strcmp(args[j], "<") == 0) {
                    fprintf(stderr, "Redirecting input from %s\n", args[j+1]);
                    int fd = open(args[j+1], O_RDONLY);
                    if (fd < 0) {
                        perror("Error opening file");
                        exit(1);
                    }
                    dup2(fd, STDIN_FILENO);
                    close(fd);                        
                    args[j] = NULL;
                }
            }
            execvp(args[0], args);
            perror("Regular execution failed.\n");
            return 1;
        } else { 
            if (background == 0) {
                fprintf(stderr, "Parent waiting for child PID %d...\n", pid);
                waitpid(pid, NULL, 0);
            } else {
                fprintf(stderr, "Parent continuing (Child PID %d in background)\n", pid);
            }
        }
        }
    }
    printf("Program terminated\n");
    return 0;
}