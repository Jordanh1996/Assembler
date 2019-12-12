#ifndef FILES_H
#define FILES_H

#include <stdio.h> /* Incldued here so I can use FILE in some functions prototypes */

#define MIN_ARGUMENTS 2/* The minimum of command line arguments the program expects including the program name */

/* String literals of the files extensions that are used within the project */
#define EXTERNAL_EXT ".ext" /* Externals file */
#define ENTRY_EXT ".ent"  /* Entries file */
#define OBJECT_EXT ".ob" /* Object file(machine code) */
#define ASSEMBLY_EXT ".as" /* Assembly file (source code) */

#define LINE_CHARS 4 /* In the entry, object and external output files lines are being written, some have leading zeros, this definition defines how many characters a line should have */
#define CPU_BIT_SIZE 14 /* The characters count of the binary representation of each word */

void setCurrentWorkingFile(char *fileName); /* Initializes variables in file.c and closes previously opened files */
FILE * openFile(char *fileName, char *mode); /* Takes a file name and a mode, opens the file with the given mode and returns a pointer to a FILE struct */
void deleteFiles(); /* Deletes previously compiled files for the current working file */

void writeObjectMeta(); /* Creates the object file and writes to it the instruction count and data count */
void writeObject(int bin); /* Writes a single word with the current line to the object file */
void writeExternal(char *ext, int line); /* Writes a label of an external and the line it was used to the external file */
void createEntries(); /* Creates the entries file if needed, loops through the symbol table and adds the entries to it and their usage line */

extern char *fileName; /* The current file that is being processed */

#endif
