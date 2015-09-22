/****************************
 *        Homework #2       *
 *      by Sean McGunigal   *
 *         9/20/2015        *
 ****************************/

// Libraries
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

// Function to split the lines into commands and args
// Returns an array of strings
char** tokenize(char* line, int flag);

// Function to spawn a new process with the commands
void execute(char*** argv);

// Main function
int main() {
	// String to read in commands
	char *line = NULL;
	// Array of commands and args (Array of Arrays of Strings)
	char ***argv;
	// An command and its args
	char **command;
	// Required buffer size for getline
	ssize_t buffer_size = 0;
	// Index variable for for-loops
	int i;

	// Main Program Loop
	while (1) {
		// Instantiate array w/ space for 20 commands
		argv = malloc(20 * sizeof(char**)); // Instantiate
		printf("Sean's Shell> "); // My own personalized prompt ;)
		fflush(stdout); // Make sure the prompt is shown
		getline(&line, &buffer_size, stdin); // Read the commands
		command = tokenize(line, 0); // Tokenize the commands for pipes
		i = 0;
		while (command[i] != NULL) {
			argv[i] = tokenize(command[i], 1); // Tokenize each command for args
			i++;
		}
		if (strcmp(argv[0][0], "exit") == 0) // If the user wants to exit
			exit(0);
		execute(argv); // Execute the commands
	}
}

// Function to spawn each of the commands
void execute(char*** argv) {
	// Variable declarations
	int* pipeArray, status, commands = 1, i, j;

	while (argv[commands++] != NULL); // Count the number of commands in the array
	commands--; // Correct the number after the while loop

	// Allocate memory for two ints for every pipe in the command (one for read and one for write)
	pipeArray = malloc(2*(commands-1)*sizeof(int));

	for (i = 0; i < commands-1; i++)
		pipe(pipeArray + 2*i); // Run pipe on every set of two in the array

	// For every command, fork and execute
	for (i = 0; i < commands; i++) {
		if (fork() == 0) {
			// The child executes this:
			if (i != 0)
				dup2(pipeArray[2*(i-1)], 0); // If not the first command, read from a pipe
			if (i != commands-1)
				dup2(pipeArray[(2*(i-1))+3], 1); // If not the last command, write to a pipe
			
			for (j = 0; j < 2*(commands-1); j++)
				close(pipeArray[j]); // close all the pipes.

			execvp(argv[i][0], argv[i]); // Execute the command
		}
		// The parent does nothing but executes the next step in the loop
	}
	for (j = 0; j < 2*(commands-1); j++)
		close(pipeArray[j]); // The last parent still needs to close the pipes
	for (j = 0; j < commands; j++)
		wait(&status); // The last parents needs to wait for all the children.	
}

// Function to tokenize for both pipes and args (denoted by int flag)
char** tokenize(char* line, int flag) {
	// Instantiate array w/ space for 20 args
	char** output = malloc(20*sizeof(char*));
	int index = 0; // Index variable for for-loops
	char* token; // String for token

	if (flag == 1) // if parsing for args
		token = strtok(line, " \t\n\r");
	else
		token = strtok(line, "|"); // if parsing for pipes

	// While there is another token
	while (token != NULL) {
		output[index++] = token;
		if (flag == 1)
			token = strtok(NULL, " \t\n\r"); // Pick up where the args left off
		else
			token = strtok(NULL, "|"); // Pick up where the pipe left off
	}

	return output; // Return the processed array of tokens.
}
