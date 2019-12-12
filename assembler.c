/*
  Program Description
  Author: Jordan Hury
  Date: 06.08.19
  Maman 14
  This is a compiler for the assembly language.
  The program accepts as command line arguments the file names of the source files without the extension and compiles them to the
  machine code as specified in the exercise description.
  The program write the machine code to a .ob file in the current working directory and also creates .ent file for entries and
  .ext file for external usages.
  The output file names will be as the input file name supplied in the command line with the correct file extension.
  The compiler will search for errors and warnings and output them to the user.
  The compiler runs through the code 2 times.
  If an error has occoured in any of them the output files will not be generated.

  To create the program please use 'make' in the CLI at the correct working directory where a makefile is present.
  USAGE: 
  eg: assembler FILENAME1 FILENAME2

  NOTE: The assembly files to be compiled must be supplied without the '.as' file extension
*/
#include <stdio.h>
#include <stdlib.h>
#include "./files.h"
#include "./utils.h"
#include "./data.h"
#include "./scan.h"
#include "./strings.h"
#include "./output.h"
#include "./status.h"

/* Initialization of global variables for the project, in data.h they are initialized as extern for usage in other files of the project */
int DC;
int IC;
int error;
int scanCount;
symbolNodePtr symbolHead;
dataNodePtr dataHead;

/* 
  Prototypes for functions that are available only for this file.
*/
void updateSymbolIndex(void);
void writeData(void);
int compileFile(char *fileName);

/*
  The compiler begins execution here.
  This function loops through the command line arguments other than the program name and for
  each argument triggers the compile function supplying it the name of the file.
*/
int main(int argc, char *argv[]) {
  if (argc < MIN_ARGUMENTS) { /* When 0 files are been supplied it prints an instructional message to the user */
    printf("Please insert files to compile\n");
  } else {
    while (argc >= MIN_ARGUMENTS) {
      argc--;
      compileFile(argv[argc]);
    }
  }

  return OK_STATUS;
}

/*
  Triggers the entire compilation flow for a given file name.
  Initializes the global variables.
  Frees memory of symbols and data from previous files compilation.
  Triggers a function to open the given file name.
  Triggers the scan functions on that file.
  Prints messages to notify the user wether the file completed compilation.
  If scans completed successfully deletes old files.
  Returns a status wether file compiled successfully.
*/
int compileFile(char *fileName) {
  char *fileExt = addExtension(fileName, ASSEMBLY_EXT); /* The file name given doesn't include the '.as' extension. We create it here. */
  FILE *fp;

  /* Frees the symbol and data tables */
  symbolNodeFree(symbolHead);
  dataNodeFree(dataHead);

  /* Init of global variables */
  DC = DATA_BASE;
  IC = MEMORY_BASE;
  error = OK;
  scanCount = OK;
  symbolHead = NULL;
  dataHead = NULL;

  setCurrentWorkingFile(fileName); /* Initializes variables in files.c, closes previous opened files */
  fp = openFile(fileExt, "r");     /* Opens the '.as' file to be compiled */
  free(fileExt);

  if (fp == NULL) { /* Returns if file was not found or couldn't be opened */
    return BAD_STATUS;
  }

  scanCount = FIRST;
  scan(fp, scanFirst); /* Triggers first scan */

  if (error != OK) { /* If first scan had an error it returns */
    printf("An error has been found on the first scan, failed to compile %s\n", fileName);
    return BAD_STATUS;
  }

  updateSymbolIndex(); /* Increments each guidance symbol in the symbol table with the instruction count */
  initOutputVars(); /* Initializes variables that will store the words to be compiled until the second scan will be finished */

  IC = MEMORY_BASE;

  scanCount = SECOND;
  scan(fp, scanSecond); /* Triggers second scan */

  fclose(fp); /* Closes the source code file */

  if (error != OK) { /* If an error has occoured on the second scan notifies the user */
    freeOutputVars(); /* Frees the variables that stoered the output */
    printf("\nAn error has been found on second scan, failed to compile %s\n", fileName);
    return BAD_STATUS;
  } else {
    deleteFiles(); /* Delete files from previous compilations */
    createOutput();   /* Creates the compiled files */
    writeData();      /* Write the data from the data table to the object file */
    freeOutputVars(); /* Frees the variables that stoered the output */
    printf("\n%s Compiled successfully\n", fileName);
  }

  return OK_STATUS;
}

/*
  Should be called after the first scan once the instruction count is known.
  Loops thorugh all of the symbol table and for each symbol that is of type guidance increments it
  with the value of the instruction count.
  This is because in the result machine code the data is located after the instructions.
*/
void updateSymbolIndex() {
  symbolNodePtr cur = symbolHead;

  while (cur) {
    if (cur->type == GUIDANCE) {
      cur->val = cur->val + IC;
    }
    cur = cur->next;
  }
}

/*
  After all of the instructions in the program has been written to the object file the data variables of the assembly program
  also needs to be translated into machine code.
  Loops thorugh the data table and for each node in the table creates a line in the object file with the value of the node.
*/
void writeData() {
  dataNodePtr cur = dataHead;

  while (cur != NULL) {
    writeObject(cur->val); /* Writes to the object file a line with the encoded value of 'val' */
    IC++; /* The IC is used in the object file to write the correct corresponding line for each word. Even though this is data it still needs to be incremented */
    cur = cur->next;
  }
}
