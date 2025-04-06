#define _POSIX_C_SOURCE 200809L // required for strdup() on cslab
#define _DEFAULT_SOURCE // required for strsep() on cslab
#define _BSD_SOURCE // required for strsep() on cslab

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define MAX_ARGS 32

int execution(char **command_line_words, size_t num_args);
void error_message(char *type);
int arrows(char **command_line_words, size_t num_args);
void arrow_directing(char **words, size_t num);

char **get_next_command(size_t *num_args)
{
	// print the prompt
	printf("cssh$ ");
	// get the next line of input
	char *line = NULL;
	size_t len = 0;
	getline(&line, &len, stdin);
	if (ferror(stdin))
	{
		perror("getline");
		exit(1);
	}
	if (feof(stdin))
	{
		return NULL;
	}
	// turn the line into an array of words
	char **words = (char **)malloc(MAX_ARGS*sizeof(char *));
	int i=0;
	char *parse = line;
	while (parse != NULL)
	{
		char *word = strsep(&parse, " \t\r\f\n");
		if (strlen(word) != 0)
		{
			words[i++] = strdup(word);
		}
	}
	*num_args = i;
	for (; i<MAX_ARGS; ++i)
	{
		words[i] = NULL;
	}
	// all the words are in the array now, so free the original line
	free(line);

	return words;
}

void free_command(char **words)
{
	for (int i=0; i<MAX_ARGS; ++i)
	{
		if (words[i] == NULL)
		{
			break;
		}
		free(words[i]);
	}
	free(words);
}

int execution(char **command_line_words, size_t num_args)
{
	if (command_line_words == NULL)
	{
		printf("Memory Error\n");
		exit(0);
	}
	int a = arrows(command_line_words, num_args);
        if (a == 0)
        {
                return 0;
        }	

	pid_t fork_rv = fork();
	if (fork_rv == -1)
	{
		perror("fork");
		exit(1);
	}
	//0 if in child, positive if in parent
	if (fork_rv != 0)
	{
		int status;
		pid_t wpid = wait(&status);
		if (wpid == -1)
		{
			perror("wait");
			exit(1);
		}
	}
	if (fork_rv == 0)
	{
		if(a == 1)
		{
			arrow_directing(command_line_words, num_args);
		}
		execvp(command_line_words[0], command_line_words);
		perror(command_line_words[0]);

		exit(0);	
	}
	return 1;
}

void error_message(char *type)
{
	if(type == NULL)
	{
		printf("Memory Error!\n");
		exit(1);
	}
	if (strcmp(type, ">") == 0)
	{
		printf("Error! Can't have two >'s or >>'s!\n");
	}
	else if (strcmp(type, "<") == 0)
	{
		printf("Error! Can't have two <'s!\n");
	}
}

int arrows(char **command_line_words, size_t num_args)
{
	if (command_line_words == NULL)
	{
		printf("Memory Error!\n");
		exit(0);
	}
	int flag1 = 0;
	int flag2 = 0;
	
	for(int i = 0; i< num_args; i++)
	{
		if (strcmp(command_line_words[i], ">") == 0|| strcmp(command_line_words[i], ">>") == 0)
		{
			if (flag1 == 1)
			{
				error_message(">");
				return 0;
			}
			flag1 = 1;
		}
		else if (strcmp(command_line_words[i], "<") == 0) 
		{
			if (flag2 == 1)
			{
				error_message("<");
				return 0;
			}
			flag2 = 1;
		}
	}
	return 1;
}

void arrow_directing(char **words, size_t num)
{
	if (words == NULL)
	{
		printf("Memory Error!\n");
		exit(0);
	}
	int fd;
	for (int i = 0; i < num; i++)
	{
		if(strcmp(words[i], "<")==0)
		{
			fd = open(words[i+1], O_RDONLY);
			if (fd == -1)
			{
				perror("open");
				exit(0);
			}
			int d2 = dup2(fd, STDIN_FILENO);
			if (d2 == -1)
			{
				perror("dup2");
				exit(1);
			}
			if (close(fd) == -1)
			{
				perror("close");
				exit(0);
			}
			words[i] = NULL;
		}
		else if(strcmp(words[i], ">") == 0)  
		{
			fd = open(words[i+1], O_WRONLY|O_TRUNC|O_CREAT, 0644);
			if (fd == -1)
			{
				perror("open");
				exit(0);
			}
			int d2 = dup2(fd, STDOUT_FILENO);
			if (d2 == -1)
			{
				perror("dup2");
				exit(1);
			}
			if (close(fd) == -1)
			{
				perror("close");
				exit(0);
			}
			words[i] = NULL;
		}
		else if(strcmp(words[i], ">>") == 0)
                {
                        fd = open(words[i+1], O_WRONLY|O_APPEND|O_CREAT, 0644);
                        if (fd == -1)
                        {
                                perror("open");
                                exit(0);
                        }
                        int d2 = dup2(fd, STDOUT_FILENO);
                        if (d2 == -1)
                        {
                                perror("dup2");
                                exit(1);
                        }
                        if (close(fd) == -1)
                        {
                                perror("close");
                                exit(0);
                        }
                        words[i] = NULL;
                }
	}
}

int main()
{
	size_t num_args;

	// get the next command
	char **command_line_words = get_next_command(&num_args);
	while (command_line_words != NULL)
	{
		if (num_args == 0)
		{
			command_line_words = get_next_command(&num_args);
			continue;
		}
		if (strcmp(command_line_words[0], "exit") == 0)
		{
			free_command(command_line_words);
			exit(0);
		}
		
		int k = execution(command_line_words, num_args);
		if (k==0)
		{
			free_command(command_line_words);
			command_line_words = get_next_command(&num_args);
			continue;
		}
		//arrows(command_line_words);	
		// run the command here
		// don't forget to skip blank commands
		// and add something to stop the loop if the user 
		// runs "exit"

		// free the memory for this command
		free_command(command_line_words);
		// get the next command
		command_line_words = get_next_command(&num_args);
	}

	// free the memory for the last command
	free_command(command_line_words);

	return 0;
}
