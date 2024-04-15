#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>

/* print msg, followed by the usage message, and exit the program */
void print_usage(char *msg)
{
	printf(msg);
	printf("\n\n%s\n%s\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n",
			"Usage: wc [-l] [-w] [-c] [FILES...]",
			"where:",
			"-l prints the number of lines",
			"-w prints the number of words",
			"-c prints the number of characters",
			"FILES if no files are given, then read", 
			"from standard input");
	exit(0);
}

/* return an array that has the number of lines, words, and characters in filename */
/* should pass in a filename of "" to indicate to read from stdin */
int *get_counts(char *filename)
{
	int reading;
	int fd;
	int word_count;
	int line_count;
	int char_count;
	int white_space = 1;
	char *characters = (char*)malloc(sizeof(char));
	if (characters == NULL || filename == NULL)
	{       
		return NULL;
	}
	else if (strcmp(filename, "") == 0)
	{
		fd = 0;
	}
	else
	{
		fd = open (filename, O_RDONLY);
		if (fd == -1)
		{
			perror("open failed");
			exit(1);
		}
	}
	reading = read(fd, characters, 1);
	if (reading < 0)
	{
		perror("read");
		exit(1);
	}
	if (reading == 0)
	{
		char_count = line_count = word_count = 0;	
	}
	else{
		char_count = 0;
		line_count = -1;
		word_count = 0;		
		while(reading != 0)
		{
			reading = read(fd, characters,1);
			if (reading < 0)
			{
				perror("read");
				exit(1);
			}
			char_count += 1;
			int result = isspace(*characters);
			int newline = strcmp(characters, "\n");
			if (newline == 0)
			{
				line_count += 1;
			}
			if(result != 0 && white_space == 0)
			{
				word_count += 1;
				white_space = 1;			
			}
			if (result == 0 && white_space == 1)
			{
				white_space = 0;
			}
		}
		if (white_space == 0)
		{
			word_count += 1;
		}
	}
	free(characters);
	int *counts = (int*)malloc(3*sizeof(int));
	if (counts == NULL)
	{
		return NULL;
	}
	counts[2] = char_count;
	counts[1] = word_count;
	counts[0] = line_count;
	return counts;
}

/* print the indicated counts for file name */
/* show should be an array of three ints that indicates if the number of lines,
 *  * words, and characters should be printed */
/* count is an array of the three counts */
/* name is the name to print after the counts */
void print_counts(int *show, int *count, char *name)
{
	for (int i = 0; i < 3; i++)
	{
		if (show[i] == 1)
		{
			printf("%8d ", count[i]);
		}
	}

	printf("%s\n",name);
}

int main(int argc, char **argv)
{
	int show[3] = {0,0,0};
	int index = 4;
	int nofile = 0;
	for (int i = 1; i < argc; i++)
	{
		if (i == argc)
		{
			nofile = 1;
			break;
		}
		if (strcmp(argv[i], "-l") != 0 && strcmp(argv[i], "-w") != 0 && strcmp(argv[i], "-c") != 0)
		{
			char first = argv[i][0];
			if(first == '-')
			{
				print_usage("invalid input");
			}
			index = i;
			break;
		}
		else if (strcmp(argv[i], "-c") == 0)
		{
			show [2] = 1;
		}
		else if (strcmp(argv[i], "-w") == 0)
		{
			show [1] = 1;
		}
		else if (strcmp(argv[i], "-l") == 0)
		{
			show [0] = 1;
		}
		if (i == argc)
		{
			break;
		}
	}
	if (show [0] == 0 && show[1] == 0 && show [2] == 0)
	{
		show[0] = show [1] = show[2] = 1;
	}
	int totals[3] = {0,0,0};
	for (int j = index; j < argc; j++)
	{
		int fd;
		fd = open (argv[j], O_RDONLY);
		if (fd == -1)
		{
			perror(argv[j]);
			continue;			
		}
		else
		{
			int *counts = get_counts(argv[j]);
			if (counts == NULL)
			{
				return -1;
			} 
			print_counts(show, counts, argv[j]);
			totals[2]  += counts[2];
			totals[1]  += counts[1];
			totals[0]  += counts[0];
			free(counts);
		}
		if (close(fd) < 0)
		{
			perror("close");
			exit(1);
		}
	}
	if  (nofile == 1)
	{
		print_counts(show, get_counts(""), "");
		return 0;
	}

	if (index != argc-1)
	{
		print_counts(show, totals, "total");
	}
	return 0;
}
