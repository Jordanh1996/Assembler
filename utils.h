#ifndef UTILS_H
#define UTILS_H

#include <stdarg.h> /* Imported here so I can use va_list and '...' in some functions prototypes */

#define LINE_MAX 81 /* The maximum amount of chars a source code line may have */
#define MEMORY_SIZE 4096 /* The maximum amount of memory the data table can reach */

#define PRES "\033[0m" /* Turns output color to normal */
#define PRED "\x1B[31m" /* Turns output color to red */
#define PYEL "\x1B[33m" /* Turns output color to yellow */

struct symbolNode; /* Notifies that a declaration for this truct exists, defiend in data.h */
struct dataNode;   /* Notifies that a declaration for this truct exists, defiend in data.h */

char *lalloc(void); /* Allocates enough memory for a single line of source code and returns a pointer to it */
struct symbolNode * salloc(void); /* Allocates memory for a symbolNode and returns a pointer to it */
struct dataNode * dalloc(void); /* Allocates memory for a dataNode and returns a pointer to it */


void addSymbolNode(char *label, int val, int type); /* Creates a new symbolNode for the symbol table and adds it */
void addDataNode(char *label, int val); /* Creates a new dataNode for the data table and adds it */
struct symbolNode * symbolNodeByLabel(char *label); /* Loops through the symbol table and searches for a symbol that its label matches the one in the parameter, returns the node if found */
struct dataNode * dataNodeByLabel(char *label); /* Loops through the symbol table and searches for a symbol that its label matches the one in the parameter, returns the node if found */
void symbolNodeFree(struct symbolNode *head); /* Takes a head of a linked list of symbolNodes and frees all of the memory of all the nodes */
void dataNodeFree(struct dataNode *head);     /* Takes a head of a linked list of dataNodes and frees all of the memory of all the nodes */

int getDigits(int num); /* Takes an integer and returns the amount of digits required for its decimal representation */
void vfrees(int args, va_list ap); /* The pair of frees, Frees the memory of the first (args) arguments received from ap(va_list) */
void frees(int args, ...); /* The pair of vfrees, initializes the va_list and calls vfrees to handle the logic to free the memory */
void printe(char *msg, int args, ...); /* Prints an error message with the line it happend, A message explainig the error, arguments count and arguments to be freed, The explaning message can be formatted just like printf, the arguemnts to format will be placed after the arguemnts to free */
void warning(char *msg, ...); /* Prints a warning message of the current file and its line, and a message exaplning the warning, the next arguemnts are used to format the warning message */


/*
  These functions are not used anywhere, they print the current symbol and data tables.
  I left it here in case you want to use them.
*/
void printSymbols();
void printData();

#endif
