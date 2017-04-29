/**
 * fact.c
 * By: Jake Carlson
 * 2017-01-27
 * This program takes the factorial of the provided command line argument if it
 * is a positive integer. Otherwise "Huh?" is printed to console
 */

// includes
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

// function prototype
int factorial(int);
bool isPosInt(char*);

// main driver
int main(int argc, char** argv)
{
	if(argc == 2 && isPosInt(argv[1]))
	{
		int num = atoi(argv[1]);
		if(num <= 12)
		{
			printf("%i\n", factorial(num));
		}
		else
		{
			printf("Overflow\n");
		}
	}
	else
	{
		printf("Huh?\n");
	}
	return 0;
}

// recursive factorial function
int factorial(int n)
{
	return (n == 0) ? 1 : n * factorial(n - 1);
}

// boolean function to check if the provided input is a positive integer
bool isPosInt(char* input)
{
	for(int i = 0; input[i] != 0; i++)
	{
		if(!isdigit(input[i]))
			return 0;
	}
	
	return 1;
}
