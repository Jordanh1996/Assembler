#ifndef OUTPUT_H
#define OUTPUT_H

void initOutputVars(void); /* After the first scan we want to initialize some variables in this file */
void freeOutputVars(void); /* After writing the compiled code we want to free some memory that was located to variable that held the words that were needed to be written */
void createOutput(); /* Creates the compiled files */
void addWords(int words[], int wordCount); /* Accepts an array of words and the length of the array and adds the words to a variable that stores all the words to be written */
void addExternal(char *label, int line); /* Each time an external is used in the source code this function is called with the external name and the line of usage */

#endif
