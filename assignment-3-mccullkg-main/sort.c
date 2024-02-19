#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void usage()
{
	printf("usage: sort [-r] num1 num2 ...\n");
	printf("\t -r: reverse sort\n");
}

void swap(double *x, double *y)
{
	if (x == NULL || y == NULL)
	{
		return;
	} 
	double valx = *x;
	double valy = *y;
	*x = valy;
	*y = valx;
}

void sort(double *a, int length)
{
	if (a == NULL)
	{
		return;
	}
	int i = 1;
	while (i < length)
	{
		int j = i;
		while (j > 0 && a[j-1] > a[j])
		{
			swap(&a[j-1], &a[j]);
			j = j - 1;
		}
		i = i + 1;
	}
}

int is_valid_num(char *word)
{
	if (word == NULL)
        {       
                return -1;
        }
	int found_dot = 0;

	for (int i=0; i < strlen(word); ++i)
	{
		if (word[i] == '-')
		{
			if (i != 0)
			{
				return 0;
			}
			continue;
		}

		if (word[i] == '.')
		{
			if (found_dot == 1)
			{
				return 0;
			}
			found_dot = 1;
			continue;
		}

		if (!isdigit(word[i]))
		{
			return 0;
		}
	}

	return 1;
}

int main(int argc, char *argv[])
{
	double doubleNums[argc];
	int shift = 1;
	if (argc < 2)
	{
		usage();		
		return 0;
	}
	if (strcmp(argv[1], "-r") == 0)
	{
		shift = 2;
		if (argv[2] == NULL)
		{
			usage();
			return 0;
		}
	}
	for(int i = shift; i < argc; i++)
	{
		if(!is_valid_num(argv[i]))
		{
			printf("%s %s\n", argv[i], "is not a valid number");
			usage();
			return 0;	
		}
		doubleNums[i - shift] = atof(argv[i]);	
	}
	
	sort(doubleNums, argc - shift);

	for (int k = 0; k < argc - shift; k++){
        	if(shift == 2)
		{
			printf("%g\n",doubleNums[argc - k - shift - 1]);
			continue;
		}        
		printf("%g\n", doubleNums[k]);
        }

	return 0;
}
