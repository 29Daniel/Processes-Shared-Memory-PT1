#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <grep argument>\n", argv[0]);
        exit(1);
    }

    int pipefd1[2];  // Pipe between cat and grep
    int pipefd2[2];  // Pipe between grep and sort
    int pid1, pid2;

    // Arguments for each command
    char *cat_args[] = {"cat", "scores", NULL};
    char *grep_args[] = {"grep", argv[1], NULL};
    char *sort_args[] = {"sort", NULL};

    // Create first pipe (cat -> grep)
    if (pipe(pipefd1) == -1) {
        perror("Pipe1 failed");
        exit(1);
    }

    pid1 = fork();
    if (pid1 < 0) {
        perror("Fork failed");
        exit(1);
    }

    if (pid1 == 0) {  // Child process (P2) - Executes grep
        // Create second pipe (grep -> sort)
        if (pipe(pipefd2) == -1) {
            perror("Pipe2 failed");
            exit(1);
        }

        pid2 = fork();
        if (pid2 < 0) {
            perror("Fork failed");
            exit(1);
        }

        if (pid2 == 0) {  // Child's Child process (P3) - Executes sort
            // Replace stdin with the input part of pipe2
            dup2(pipefd2[0], STDIN_FILENO);
            // Close unused ends of both pipes
            close(pipefd1[0]);
            close(pipefd1[1]);
            close(pipefd2[1]);
            close(pipefd2[0]);

            // Execute sort
            execvp("sort", sort_args);
            perror("execvp failed for sort");
            exit(1);
        } else {  // Child process (P2) - Executes grep
            // Replace stdin with the input part of pipe1
            dup2(pipefd1[0], STDIN_FILENO);
            // Replace stdout with the output part of pipe2
            dup2(pipefd2[1], STDOUT_FILENO);
            // Close unused ends of both pipes
            close(pipefd1[1]);
            close(pipefd2[0]);
            close(pipefd2[1]);
            close(pipefd1[0]);

            // Execute grep <argument>
            execvp("grep", grep_args);
            perror("execvp failed for grep");
            exit(1);
        }
    } else {  // Parent process (P1) - Executes cat
        // Replace stdout with the output part of pipe1
        dup2(pipefd1[1], STDOUT_FILENO);
        // Close unused ends of pipe1
        close(pipefd1[0]);
        close(pipefd1[1]);

        // Execute cat scores
        execvp("cat", cat_args);
        perror("execvp failed for cat");
        exit(1);
    }

    // Wait for both child processes to finish
    wait(NULL);
    wait(NULL);

    return 0;
}
