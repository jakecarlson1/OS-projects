/**
 * words.c
 * By: Jake Carlson
 * 2017-01-27
 * This program prints all of the command line arguments to console separated
 * by newlines.
 */

// includes
#include <stdio.h>

// main driver
int main(int argc, char **argv)
{
    // for-loop
    for(int i = 1; i < argc; i++)
    {
        printf("%s\n", argv[i]);
    }
    
    return 0;
}
