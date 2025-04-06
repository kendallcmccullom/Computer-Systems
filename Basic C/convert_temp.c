#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

int checkFirst(char first[])
{
	int len = strlen(first);
	char valid1[] = "F";
	char valid2[] = "C";

	if (len != 1)
	{       
		return 0;
	}
	else if (strcmp(first, valid1) == 0)
	{       
		return 1;
	}
	else if (strcmp(first, valid2) == 0)
	{       
		return 2;
	}
	return 0;
}

int isDouble(char input[])
{       
	int first = isdigit(input[0]);
	bool decimalUsed = false;
	if (input[0] == '-' || first != 0)
	{       
		for (int i = 1; i < strlen(input); i++)
		{       
			int following = isdigit(input[i]);
			if (input[i] == '.')
			{       
				if (!decimalUsed)
				{       
					decimalUsed = true;
					continue;
				}
				return 0;
			}
			else if (following == 0)
			{       
				return 0;
			}
		}        
		return 1;
	}
	return 0;
}

double FtoC(double fdegree)
{
	return (fdegree - 32) * (5)/9;
}

double CtoF(double cdegree)
{
	return 1.8 * cdegree + 32;
}

void format(char inputNum[], int type)
{
	double in = atof(inputNum);
	
	if (type == 1)
	{
		printf("%g%c = %g%c\n", in, 'F', FtoC(in), 'C');
	}
	else
	{
		printf("%g%c = %g%c\n", in, 'C', CtoF(in), 'F');
	}

}

int main (int argc, char *argv[])
{
	if (argc >= 3)
	{
		int first = checkFirst(argv[1]);
		if (first != 0)
		{
			for (int i = 2; i < argc; i++)
			{
				if (isDouble(argv[i]) == 0)
				{
					printf("%s %s\n", argv[i], "is not a valid temperature.");
				}
				else
				{
					format(argv[i], first);
				}
			}
			return 0;
		}

	}
	printf("usage: convert_temp F|C temp (temp ...)\n");
}


