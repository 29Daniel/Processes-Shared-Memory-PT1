#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 
  
int main() 
{ 
    // We use two pipes: 
    // First pipe to send input string from parent to child 
    // Second pipe to send input string from child to parent
  
    int fd1[2];  // Used to store two ends of first pipe 
    int fd2[2];  // Used to store two ends of second pipe 
  
    char fixed_str1[] = "howard.edu"; 
    char input_str[100]; 
    pid_t p; 
  
    // Create the pipes
    if (pipe(fd1) == -1 || pipe(fd2) == -1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 
  
    // Get initial input from parent
    printf("Enter a string to concatenate: ");
    scanf("%s", input_str); 
  
    // Create the child process
    p = fork(); 
  
    if (p < 0) 
    { 
        fprintf(stderr, "fork Failed" ); 
        return 1; 
    } 
  
    // Parent process
    else if (p > 0) 
    { 
        char concat_str[100];

        // Close the reading end of fd1 and writing end of fd2
        close(fd1[0]); 
        close(fd2[1]); 
  
        // Write input string to child via fd1
        write(fd1[1], input_str, strlen(input_str) + 1); 
        close(fd1[1]); // Close the writing end of fd1
  
        // Wait for child to send second string back via fd2
        wait(NULL); 
  
        // Read second input from child via fd2
        read(fd2[0], concat_str, 100); 
        close(fd2[0]); // Close the reading end of fd2
  
        // Concatenate "gobison.org" and print the final string
        strcat(input_str, fixed_str1); // Parent uses the original input + howard.edu
        strcat(input_str, concat_str); // Concatenate with the string from child
        printf("Final string from parent: %s\n", input_str); 
    } 
  
    // Child process
    else
    { 
        char concat_str[100];
        char second_input[100];
  
        // Close the writing end of fd1 and the reading end of fd2
        close(fd1[1]); 
        close(fd2[0]); 
  
        // Read string from parent via fd1
        read(fd1[0], concat_str, 100); 
        close(fd1[0]); // Close the reading end of fd1
  
        // Concatenate "howard.edu" and print the result
        strcat(concat_str, fixed_str1); 
        printf("Concatenated string from child: %s\n", concat_str); 
  
        // Prompt for second input from user
        printf("Enter another string: ");
        scanf("%s", second_input);
  
        // Send second input back to parent via fd2
        write(fd2[1], second_input, strlen(second_input) + 1); 
        close(fd2[1]); // Close the writing end of fd2
  
        exit(0); 
    } 
} 
