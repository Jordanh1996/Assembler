#include <stdlib.h>
#include <stdio.h>
#include "./files.h"
#include "./data.h"
#include "./strings.h"

/*
  Prototypes for functions that are private to this file.
  The rest of the functions prototypes can be found in output.h
*/
void initObjOut(int lines);
void initExtOut(int maxLines);
void createObjectFile();
void createExternalFile();

/*
  Checks if a given pointer points somewhere and if so frees that memory allocated
*/
#define freeVarIfExists(p) {  \
  if (p != NULL) {            \
    free(p);                  \
  }                           \
}

/* objOut holds the values for the words that needs to be written to the .ob file */
static int *objOut, *extLines; /* extLines holds lines in their order that needs to be written to the .ext file */
static char **extLabels; /* Holds the labels in their order that needs to be written to the .ext file */
static int curWord, curExt; /* curWord is the current .ob file word count, and curExt is the current .ext file count */

/*
  After the first scan we want to initialize some variables that correspond to the current file being processed.
  We need to initialize objOut and extLines, extLabels with enough memory.
*/
void initOutputVars() {
  curWord = 0;
  curExt = 0;
  initObjOut(IC - MEMORY_BASE);
  initExtOut((IC - MEMORY_BASE) * 2); /* extOut can have at most IC - MEMORY_BASE * 2 lines, we multiple by 2 because each instruction may use 2 operands and both can be externals */
}

/*
  After each compilation of a file we want to free these variables if memory was allocated to them.
*/
void freeOutputVars() {
  freeVarIfExists(objOut);
  freeVarIfExists(extLines);
  freeVarIfExists(extLabels);
}

/*
  This function is called after the second scan if no error was found, it calls other functions to create the output files.
*/
void createOutput() {
  createObjectFile(); /* Creates the .ob file */
  createExternalFile(); /* Creates the .ext file */
  createEntries(); /* Loops through symbol table to create entry file */
}

/*
  Allocates memory to the objOut variable that will store all of the words before they will be written.
*/
void initObjOut(int lines) {
  objOut = (int *) malloc(sizeof(int) * lines);

  if (objOut == NULL) {
    printf("Cannot allocate memory\n");
    exit(0);
  }
}

/*
  Allocates memory to the extLines and extLabel that will hold the external labels and lines of usage before they will be written to the .ext file
*/
void initExtOut(int maxLines) {
  extLines = malloc (sizeof(int) * maxLines); /* The external file will have at most maxLines lines */
  extLabels = malloc (sizeof(char *) * maxLines);

  if (extLines == NULL || extLabels == NULL) {
    printf("Cannot allocate memory\n");
    exit(0);
  }
}

/*
  Accepts an array of words and an int that represents that array length to be considered.
  Loops throug the array and for each element adds its value to objOut.
*/
void addWords(int words[], int wordCount) {
  int i;

  for (i = 0; i < wordCount; i++, curWord++) {
    *(objOut + curWord) = words[i];
    IC++;
  }
}

/*
  Calls whenever an external is stumpled upon in the second scan.
  Adds the line of the external usage to extLines and the label of the external to extLabels.
  Increments curExt to count the amount of externals usage
*/
void addExternal(char *label, int line) {
  *(extLines + curExt) = line;
  *(extLabels + curExt) = label;
  curExt++;
}

/*
  Creates the .ob file and writes the instrction and data count to it.
  Loops through all of the words in objOut and write all of them to the .ob file.
*/
void createObjectFile() {
  int i;
  writeObjectMeta(); /* Write the instruction count and data count to the object file */

  for (i = 0; i < curWord; i++) { /* Loops trough all of the words in objOut and writes each of them */
    writeObject(*(objOut + i));
    IC++;
  }
}

/*
  Loops through all of the externals and writes each of them to the .ext file.
  Creates the external file if any externals were used.
*/
void createExternalFile() {
  int i;

  for (i = 0; i < curExt; i++) {
    writeExternal(*(extLabels + i), *(extLines + i));
  }
}
