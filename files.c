#include <stdio.h>
#include <stdlib.h>
#include "./files.h"
#include "./strings.h"
#include "./utils.h"
#include "./data.h"

/*
  This file holds functions that helps deal with files, open, and write to them.
*/

/*
  This macro is a wrapper for function that write to files, when a functions that writes to a file is executed
  It usually returns the amount of characters written, or EOF(-1) in case of error at 'putc', if the value that returns
  from that function is less or equal to 0 it means that no characters were written.
*/
#define writeCheck(exp)                                                                           \
  if (exp <= 0) /* exp is the return value from the function that is wrapped with 'writeCheck' */ \
  {                                                                                               \
    printf("Cannot write to file\n");                                                             \
    exit(0);                                                                                      \
  }

/*
  Prototypes for functions that are only used within this file, the rest of the prototypes for the other functions of this file
  can be found on files.h so they can be used in other files.
*/
void writeLine(FILE *fp, int line);
void createFileIfNotExists(FILE **file, char *ext, char *mode);

static FILE *obFile, *entFile, *extFile; /* File pointers for each of the result compiled files (.ent, .ext, .ob). It is static so it can be accessed only within this file */
char *fileName; /* The name of the file that is currently being proccessed without the extension */

/*
  Points fileName to a string of the name of the current file that needs to be proccessed. 
  fileName will be used later to create the name of the compiled files.
  Also closes previously opened files if they were open.
*/
void setCurrentWorkingFile(char *name) {
  fileName = name;

  if (obFile != NULL) {
    fclose(obFile);
  }
  if (entFile != NULL) {
    fclose(entFile);
  }
  if (extFile != NULL) {
    fclose(extFile);
  }

  obFile = entFile = extFile = NULL;
}

/*
  Takes a name of a file and a mode and opens it(can be also used for creating files for writing).
  If failed to open the file it prints a message to the user and returns NULL.
  Upon success returns the FILE pointer to the file.
*/
FILE * openFile(char *fName, char *mode) {
  FILE *fp;
  
  fp = fopen(fName, mode);

  if (fp == NULL) {
    printf("Cannot open file %s\n", fName);
  }

  return fp;
}

/*
  Once the second scan has completed successfully we want to create new files to write to them.
  So first we delete the files from previous compilations if they exist, if they don't this will
  just do nothing.
  Also if for example a previous compilation had entries, but now it doesn't we want to no longer have
  an entry file, so this is required.
*/
void deleteFiles() {
  char *obFileName = addExtension(fileName, OBJECT_EXT), /* Takes the name of the file and returns a string that holds the same name but with the proper file extenstion */
  *extFileName = addExtension(fileName, EXTERNAL_EXT),
  *entFileName = addExtension(fileName, ENTRY_EXT);

  remove(obFileName); /* Deletes the old compiled files */
  remove(extFileName);
  remove(entFileName);
}

/*
  In the compiled files we specify line. eg: 0102. For example in the ext file we specify in which instruction word we used an external variable so later it will be replaced
  with the correct value by the linker/loader.
  This functions takes a pointer to a file and and int that represents a line and writes to that file the line.
  In the execise documentation the lines are written with 4 characters, and if the line is less than 1000 then trailing zero's are added before the line, so this is also handled here.
*/
void writeLine(FILE *fp, int line) {
  int zeros = LINE_CHARS - getDigits(line), i = 0; /* Zeros specifies how many zeros needs to be added before the number itself(If they acctully do need to be added) */

  for (i = 0; i < zeros; i++) { /* Writes the zeros */
    writeCheck(putc('0', fp))
  }

  writeCheck(fprintf(fp, "%d", line)) /* Prints the rest of the line */
}

/*
  The result object filed first line needs to hold the nubmer of words for instruction and number of words for the data.
  This function creates the .ob file and writes these numbers to the first line of it.
*/
void writeObjectMeta() {
  char *newFileName = addExtension(fileName, OBJECT_EXT); /* Creates a string of the .ob file name eg: FILENAME.ob */
  obFile = openFile(newFileName, "w"); /* Creates file in write mode */
  free(newFileName);

  writeCheck(fprintf(obFile, "\t%d\t%d\n", IC - MEMORY_BASE, DC))
  IC = MEMORY_BASE;
}

/*
  Writes the current word index and the word itself to the .ob file.
  Accepts an int that has the value of the word that needs to be written, converts it into a string of the binary representation of it, and writes it to the .ob
  file in the format of the special characters.
*/
void writeObject(int bin) {
  char *str = toBinaryString(bin, CPU_BIT_SIZE); /* Turns given number into a string of binary numbers */
  int i;
  
  writeLine(obFile, IC);
  writeCheck(putc('\t', obFile))

  for (i = 0; i < CPU_BIT_SIZE; i += BINARY_TO_SPECIAL_LENGTH) { /* Loops through the binary numbers and write the special characters onto the .ob file */
    char ch = toSpecialChar(str + i);
    writeCheck(putc(ch, obFile))
  }

  writeCheck(putc('\n', obFile))

  free(str);
}

/*
  This function takes a pointer to a pointer to a FILE struct(it will be used with obFile, extFile and entFile), a string that represents a file extension and
  a string that represents the mode to open the file with.
  Creates a string of the fileName with the given extension and opens it with the given mode and points the first paramater value to the result.
*/
void createFileIfNotExists(FILE **file, char *ext, char *mode) {
  if (*file == NULL) {
    char *extFileName = addExtension(fileName, ext);
    *file = openFile(extFileName, mode);
    free(extFileName);
  }
}

/*
  Takes as parameters a string that represents an external label, and the current line that is proccessed.
  Creates the .ext file if it does not exists.
  Write to the .ext file the label that was used with the given line.
*/
void writeExternal(char *ext, int line) {
  createFileIfNotExists(&extFile, EXTERNAL_EXT, "w");

  writeCheck(fprintf(extFile, "%s\t", ext))
  writeLine(extFile, line);
  writeCheck(putc('\n', extFile))
}

/*
  Loops through the symbol table and for each entry symbol writes to the .ent file in which word line it is being used.
  Creates the .ent file if at least 1 entry was found.
*/
void createEntries() {
  symbolNodePtr cur = symbolHead;

  while (cur) {
    if (cur->type == ENTRY) {
      createFileIfNotExists(&entFile, ENTRY_EXT, "w");
      writeCheck(fprintf(entFile, "%s\t", cur->label))
      writeLine(entFile, cur->val);
      writeCheck(putc('\n', entFile))
    }
    cur = cur->next;
  }
}
