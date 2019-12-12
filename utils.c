#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include "./data.h"
#include "./utils.h"
#include "./status.h"
#include "./strings.h"
#include "./files.h"

/*
  This file holds utilities functions used throught the program.
*/

/*
  A macro function constructor, it expects a head of a linked list and the type of each node
  Creates a function that takes a label(string) and searches if one of the nodes label matches
  the given parameter, and returns the node if matches, otherwise returns NULL.
*/
#define get_node_by_label(head, type)             \
  type *type##ByLabel(char *label)                \
  {                                               \
    type *cur = head;                             \
                                                  \
    while (cur && strcmp(label, cur->label) != 0) \
      cur = cur->next;                            \
                                                  \
    return cur;                                   \
}

get_node_by_label(symbolHead, symbolNode) /* Search function for the symbol table, function name result is symbolNodeByLabel */
get_node_by_label(dataHead, dataNode) /* Search function for the data table, function name result is dataNodeByLabel */

/*
  A macro function constructor, it expects a string that will be the first characters of the function name, a type and size multipler to allocate.
  Creates a function that allocates memory and checks if the memory allocated successfully.
  func_name with determine the name, if func_name is s then the result name will be salloc, concatenated with alloc.
  The amount of allocate space will be the sizeof the type inserted as the second argument multipled by 'size' argument.
  The resulted function expects no paramters
*/
#define create_malloc(func_name, type, size)  \
  type *func_name##alloc(void)                \
  {                                           \
    type *p;                                  \
    p = (type *)malloc(size * sizeof(type));  \
                                              \
    if (p == NULL)                            \
    {                                         \
      printf("Cannot allocate memory\n");     \
      exit(0);                                \
    }                                         \
                                              \
    return p;                                 \
  }

create_malloc(l, char, LINE_MAX * sizeof(char)) /* Creates lalloc, allocates enough memory for a source code line */
create_malloc(s, symbolNode, 1) /* Creates salloc, allocates memory for a symbolNode */
create_malloc(d, dataNode, 1) /* Creates dalloc, allocates memory for a dataNode */

/*
  A macro that expects the head of a linked list, a new node to be inserted, and the pointer to the type of the node.
  Loops through the list until the last argument and points the last one to the new parameter that was passed to this macro.
  If the head of the list is null then sets head to equal that new element.
*/
#define add_node(head, new, type) \
  {                               \
    type cur = head;              \
                                  \
    if (head == NULL)             \
    {                             \
      head = new;                 \
    }                             \
    else                          \
    {                             \
      while (cur->next != NULL)   \
        cur = cur->next;          \
      cur->next = new;            \
    }                             \
  }

/*
  A macro function constructor, expects a type of a linked list.
  The result function will loop through the linked list and free the memory allocated to all of the nodes.
  The result function expects the head of the list to be freed.
*/
#define free_list(type)             \
  void type##Free(type *head) {     \
    type *cur = head;               \
                                    \
    if (cur) {                      \
      type##Free(cur->next);        \
      free(cur);                    \
    }                               \
  }

free_list(symbolNode) /* Creates symbolNodeFree, that frees an entire symbolNode linked list */
free_list(dataNode) /* Creates dataNodeFree, that frees an entire dataNode linked list */

/*
  Initializes a symbol node for the symbol table.
  Adds it to the list.
*/
void addSymbolNode(char *label, int val, int type) {
  symbolNodePtr new = salloc();
  new->label = copyString(label); /* Copies the label because it was extracted from the source code line, once the source code is freed it may be deleited so we copy it */
  new->val = val;
  new->type = type;
  new->next = NULL;

  add_node(symbolHead, new, symbolNodePtr) /* Adds the new node to the list */
}

/*
  Initializes a data node for the data table.
  Adds it to the list.
*/
void addDataNode(char *label, int val) {
  dataNodePtr new = dalloc();

  new->label = label == NULL ? label : copyString(label); /* If the data node has a label we copy it */
  new->val = val;
  new->index = DC++;
  new->next = NULL;

  if (DC >= MEMORY_SIZE) { /* The computer has at most MEMORY_SIZE memory(4096 as stated in the exercise), we check if we exceed it */
    printe("Not enough memory in the hardware, maximum memory size is %d", 0, MEMORY_SIZE);
  }

  add_node(dataHead, new, dataNodePtr) /* Adds the node to the list */
}

/*
  Takes a number as a parameter and returns the amount of digits requried for its decimal representation.
*/
int getDigits(int num) {
  int digits = 1;

  if (num != 0) {
    digits += floor(log10(abs(num)));
  }

  return digits;
}

/*
  A variadic function.
  It is used in other variadic functions and is the pair of frees( just like vprintf is the pair of printf in the source code for <stdio.h> )
  Takes a va_list and an int that states the amount of argument passed and frees the memory allocated to each of them.
*/
void vfrees(int args, va_list ap) {
  while (args > 0) {
    free(va_arg(ap, void *)); /* void * is a generic pointer, this is why we can use this for every type */
    args--;
  }
}

/*
  A variadic function.
  This is the pair of vfrees, This function initializes the va_list and calls vfrees which handles the logic.
  Closes the list and returns.
*/
void frees(int args, ...) {
  va_list ap;

  va_start(ap, args);

  vfrees(args, ap);

  va_end(ap);
}

/*
  A variadic function.
  Triggers an error and sends a message.
  The first arguemnt is the message to be printed.
  The second argument is the amount of arguments to free, this functions calls vfrees
  The next arguments are the arguemnts to free, the amount of arguemnts it will attempt to free will be the value of 'args'.
  The next arguemnts are used to format the error message, Works just like printf does, with the '%'.
  If no arguemnts are to be freed args should equal 0.
*/
void printe(char *msg, int args, ...) {
  va_list ap;

  printf(PRED "%s" ASSEMBLY_EXT ":%d: Error: " PRES "%s", fileName, lineIndex, line); /* PRED is used to color the "Error" part of the string in red, next the file name is written with its extension, next the line where there is an error, PRES is used to restart the text color to its normal color(not red) */

  va_start(ap, args);

  vprintf(msg, ap); /* The first argument is a message explaning what is the error, we can format the error like we do in printf */
  putchar('\n');
  putchar('\n');

  vfrees(args, ap); /* Optionally free any arguments */

  va_end(ap);

  error = scanCount; /* Notify using a global variable that an error has occoured on the current scan(first or second) */
}

/*
  A variadic function.
  Works almost the same as 'printe' excepts here we don't free any variables and this warning message is printed in yellow.
  Also it doesn't notify any global variables about errors, the compilation continues as normal.
*/
void warning(char *msg, ...) {
  va_list ap;

  printf(PYEL "%s" ASSEMBLY_EXT ":%d: Warning: " PRES "%s", fileName, lineIndex, line);

  va_start(ap, msg);

  vprintf(msg, ap);
  putchar('\n');
  putchar('\n');

  va_end(ap);
}

/*
  This function is not used anywhere.
  I used it during development to print the symbol table.
  I left it here if you(whoever checks this code) would like to print the table.
*/
void printSymbols() {
  symbolNodePtr cur = symbolHead;

  while (cur) {
    printf("{ label: %s, val: %d, type: %d }\n", cur->label, cur->val, cur->type);
    cur = cur->next;
  }
}

/*
  This function is not used anywhere.
  I used it during development to print the data table.
  I left it here if you(whoever checks this code) would like to print the table.
*/
void printData() {
  dataNodePtr cur = dataHead;

  while (cur) {
    printf("{ label: %s, binaryString: %d, index: %d }\n", cur->label, cur->val, cur->index);
    cur = cur->next;
  }
}
