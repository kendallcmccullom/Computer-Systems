#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>
#include <math.h>

void usage()
{
	printf("%s\n\t%s\n\t%s\n\t%s\n\t%s\n", "usage: stats samples population lowerbound upperbound",
			"samples: number of samples", 
			"population: number of random values to generate in each sample",
			"lowerbound: bottom of random number range",
			"upperbound: top of random number range");
	exit(0);
}

int *generate_population(int size, int lower, int upper)
{
	int *arr = (int *)malloc(sizeof(int)*size);
	if (arr == NULL)
	{
		printf("here");
		return NULL;
	}
	for (int i = 0; i<size; i++)
	{
		if(upper == lower)
		{
			arr[i] = upper;
			continue;
		}
		int num = rand() % (upper - lower + 1);
		num = lower + num;
		arr[i] = num;
	}
	return arr;
}

void get_stats(int *a, int size, int *min, int *max, double *mean, double *stddev)
{
	if (a == NULL || min == NULL || max == NULL || mean == NULL || stddev == NULL)
	{
		return;
	}
	*min = INT_MAX; 
	*max = 0;
	double sum = 0;
	for (int i = 0; i < size; i++)
	{
		if(a[i] > *max)
		{
			*max = a[i];
		}
		if (a[i] < *min)
		{
			*min = a[i];
		}
		sum = sum + a[i];	
	}


	*mean = (double) sum/size;
	sum = 0;
	for (int j = 0; j < size; j++)
	{
		sum = sum + pow((a[j] - *mean), 2);	
	}
	*stddev = (sqrt(sum/size));
}

int is_valid_int(char *s)
{
	if (s == NULL)
	{
		return -1;    
	}

	if (*s == '-')
	{
		++s;
	}
	while (*s != '\0')
	{
		if (!isdigit(*s))
		{
			return 0;
		}
		++s;
	}
	return 1;
}

int main(int argc, char *argv[])
{

	srand(time(NULL));

	int min;
	int max;
	double mean;
	double stddev;

	if(argc == 5)
	{
		int breaking;
		for(int i = 1; i<5; i++)
		{
			breaking = 0;
			if (is_valid_int(argv[i]) != 1)
			{	
				printf("%s\n\n","all arguments must be integers");
				break;
			}
			if(atoi(argv[i]) <= 0 && i<3)
			{
				if (i == 1)
				{
					printf("%s\n\n","samples must be a positive integer");	
				}
				if(i == 2)
				{
					printf("%s\n\n","population must be a positive integer");
				}
				break;
			}	
			breaking = 1;

		}

		if(atoi(argv[3]) <= atoi(argv[4]) && breaking == 1)
		{
			for (int j = 1; j <= atoi(argv[1]); j++){

				int *population = generate_population(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
				get_stats(population, atoi(argv[2]), &min, &max, &mean, &stddev); 
				printf("Sample %i: min=%i, max=%i, mean=%g, stddev=%g\n",j, min, max, mean, stddev);
				free(population);
			}
			return 0;
		}
		else if (atoi(argv[3]) >= atoi(argv[4]) && breaking == 1)
		{
			printf("%s\n\n","upperbound must be >= lowerbound");
		}
	}
	else{
		printf("%s\n\n", "incorrect number of arguments");
	}
	usage();
	return 0;
}
