#ifndef SCAN_H
#define SCAN_H

#include <stdio.h> /* Incldued here so I can use FILE in some functions prototypes */

#define LABEL_MAX 31 /* The maximum characters a label can have not including the null terminator */

/* States that these struct exists, they are defiened in data.h */
struct dataNode;
struct symbolNode;

void scan(FILE *fp, int func(char *)); /* Receives a file pointer, and a function, iterates through all of the lines in the file pointer and for each triggers the function parameter */
int scanFirst(char *line); /* A funciton to treat a single line of code in the first scan */
int scanSecond(char *line); /* A function to treat a single line of code in the second scan */

#endif
