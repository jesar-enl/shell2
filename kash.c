#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

struct builtin
{
	char *name;
	void (*func)(char **args);
};

/* to read the input from the user, we use the getline() function */

/**
 * kash_read_line - reads the input from the user
 * Return: the line
 */
char *kash_read_line()
{
	char *line = NULL;
	size_t buflen = 0;

	getline(&line, &buflen, stdin);
	return (line);
}

/* split the input into tokens */

/**
 * the strtok() is used to split the string on instances of whitespaces.
 * the leading and trailing whitespace is ignored and consecutive
 * whitespace chars
 * are treated as a sing;e delimiter.
 * This function returns a NULL terminated array of string pointers.
 */
/**
 * kash_split_line - split the input into tokens
 * @line: input from the user
 * Return: tokens
 */
char **kash_split_line(char *line)
{
	int length = 0;
	int capacity = 16;
	char **tokens = malloc(sizeof(char) * capacity);

	char *delimiters = " \t\n\r";
	char *token = strtok(line, delimiters);

	while (token != NULL)
	{
		tokens[length] = token;
		length++;

		if (length >= capacity)
		{
			capacity = (int) (capacity * 1.5);
			tokens = realloc(tokens, capacity * sizeof(char *));
		}

		token = strtok(NULL, delimiters);
	}

	tokens[length] = NULL;
	return (tokens);
}

/**
 * the kash_exec() is t execute the command provided.
 * To accomplish this, we introduce the fork() to clone our inital
 * process, leaving us with identical parent and child processes.
 * The exec() command is used in the child process to replace the running
 * program with the program we want to run.
 * These are introduced in the header files <sys/types.h>, <sys/wait.h>
 * <unistd.h>
 */

/**
 * kash_exit - exit the program
 * @args: arguments
 */
void kash_exit(char **args)
{
	exit(0);
}

/**
 * kash_cd - change directory
 * @args: arguments
 */
void kash_cd(char **args)
{
	if (args[1] == NULL)
		fprintf(stderr, "kash: cd: missing argument\n");
	else
	{
		if (chdir(args[1]) != 0)
		{
			perror("kash: cd");
		}
	}
}

/**
 * kash_help - help for our shell
 * @args: arguments
 */
void kash_help(char **args)
{
	char *helptext = 
		"Kash - the Kinda Aimless Shell. "
		"The following commands are available:\n"
		"   cd		Change the working directory.\n"
		"   exit	Exit the shell.\n"
		"   help	Print this help text.\n";
	printf("%s", helptext);
}

struct builtin builtins[] = {
	{"help", kash_help},
	{"cd", kash_cd},
	{"exit", kash_exit},
};

int kash_num_builtins()
{
	return (sizeof(builtins) / sizeof(struct builtin));
}

/**
 * kash_exec - execute the commands given
 * @args: the arguments that are to be executed.
 */
void kash_exec(char **args)
{
	for (int i = 0; i < kash_num_builtins(); i++)
	{
		if (strcmp(args[0], builtins[i].name) == 0)
		{
			builtins[i].func(args);
			return;
		}
	}

	pid_t child = fork();

	if (child == 0)
	{
		execvp(args[0], args);
		perror("kash");
		exit(1);
	}
	else if (child > 0)
	{
		int status;

		do {
			waitpid(child, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	else
	{
		perror("kash");
	}
}

/**
 * main - check the code
 *
 * Return: 0
 */
int main(void)
{
	while (true)
	{
		printf(">> "); /* prompt the user */

		char *line = kash_read_line(); /* read the input from the user */
		/**
		 * split the input into an array of tokens
		 * tokens - interpreted as a command and its arguments
		 */
		char **tokens = kash_split_line(line);

		/* execute the command */
		if (tokens[0] != NULL)
		{
			kash_exec(tokens);
		}

		free(tokens);
		free(line);
	}
}
