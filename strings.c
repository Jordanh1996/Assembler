#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "./strings.h"
#include "./status.h"
#include "./utils.h"

/*
  This file holds many functions that help deal with strings.
*/

/*
  Accepts as arguments 2 strings, and creates a new string that is the concatenation of them, returns the new string.
  This is used to create file name with an extension.
  The first paramater should be the name of the file, and the second is the extension.
*/
char *addExtension(char *fileName, char *ext) {
  char *newStr;

  newStr = (char *) malloc((sizeof(char) * (strlen(fileName) + strlen(ext) + 1))); /* We need to add extra 1 for the null terminator */
  if (newStr == NULL) {
    printf("Cannot allocate memory\n");
    exit(0);
  }

  strcpy(newStr, fileName);
  strcat(newStr, ext);
  return newStr;
}

/*
  Accepts a pointer to a string.
  Points the string forward after every space.
  Returns the amount of steps forwarded.
*/
int skipSpace(char **str) {
  int i = 0;

  while (isspace(*(*str + i)))
    i++;

  *str = *str + i;
  return i;
}

/*
  Accepts a pointer to a source code line.
  Forwards that line to the after the first word that was being stumbled, a space deliimts between words.
  Seperates the line with a null terminator and returns the first word.
*/
char *getWord(char **line) {
  char ch, *word;
  int i = 0;

  skipSpace(line); /* We point the line forward to the first character that is not a space */
  word = *line;
  while ((ch = *(*line + i)) && !isspace(ch)) { /* For each character that is not a space we point line forward */
    (*line)++;
  }

  **line = '\0'; /* Delimits the word and the source code line */
  (*line)++; /* Point line after the null terminator */

  return word; /* Returns the first word */
}

/*
  Accepts a number that represents a binary number and the length of digits for the binary number to be created.
  Creates a string of binary digits that represents the first argument, the amount of digits correspond to the second argument.
  Returns the string.
*/
char * toBinaryString(int num, int length) {
  char *binaryString;
  int i, mask = 1;

  binaryString = (char *) malloc(sizeof(char) * (length + 1));

  if (binaryString == NULL) {
    printf("Cannot allocate memory\n");
    exit(0);
  }
  length--;
  for (i = 0; length >= 0; i++, length--) { /* Loop through every bit of num */
    if (mask<<i & num) { /* We do this with bit masking */
      *(binaryString + length) = '1';
    } else {
      *(binaryString + length) = '0';
    }
  }

  *(binaryString + i) = '\0'; /* Add a null terminator to end the string */

  return binaryString;
}

/*
  Accepts as paramter a binary string.
  Compares the first BINARY_TO_SPECIAL_LENGTH digits of the string and returns a special character to be encoded onto the machine code.
*/
char toSpecialChar(char *str) {
  if (strncmp(str, "00", BINARY_TO_SPECIAL_LENGTH) == 0) {
    return '*';
  } else if (strncmp(str, "01", BINARY_TO_SPECIAL_LENGTH) == 0) {
    return '#';
  } else if (strncmp(str, "10", BINARY_TO_SPECIAL_LENGTH) == 0) {
    return '%';
  } else {
    return '!';
  }
}

/*
  Accepts a line of source code and count the amount of arguments in there.
  Each argument is delimited by a comma ','
  Checks for syntax errors.
  Returns the amount of arguments, if there are syntax errors then returns a negative number.
*/
int countArgs(char *line) {
  int num = 0, start = 0, end = 0, i;
  char ch;

  for (i = 0; i < strlen(line); i++) {
    ch = *(line + i);
    if (isspace(ch)) {
      if (start == 1) {
        end = 1;
      }
    } else if (ch == ',') {
      if (start == 0) { /* This means there were 2 commas in a row */
        printe("A comma cannot be preceeded by blank space", 0);
        return -1;
      }
      start = end = 0;
    } else {
      if (end == 1) { /* It means 2 arguments were seperated only by spaces */
        printe("Arguments must be seperated by commas", 0);
        return -2;
      }
      if (start == 0) {
        num++;
      }
      start = 1;
    }
  }

  if (start == 0 && num != 0) { /* This means the line ended with a comma ',' */
    printe("Line cannot end with a comma", 0);
    return -3;
  }

  return num;
}

/*
  Accepts a string, allocates memory and copies the parameter string to the allocated memory.
  Returns the char * to that new string.
*/
char *copyString(char *str) {
  char *new;
  int i;
  new = (char *)malloc(sizeof(char) * (strlen(str) + 1));
  if (new == NULL) {
    printf("Cannot allocate memory\n");
    exit(0);
  }

  for (i = 0; i <= strlen(str); i++) {
    *(new + i) = *(str + i);
  }

  return new;
}

/*
  Checks if a string is an unsigned number(doesn't start with + or - and doesn't include chars other than numbers).
  Returns an int that states wether the string is an unsinged number.
*/
int checkNumericUnsigned(char *str) {
  int i = 0;
  char ch;

  if (strlen(str) == 0) { /* The string is empty */
    return NOT_ENOUGH_ARGS;
  }

  while ((ch = *(str + i))) { /* Loops thorugh the string */
    if (ch < '0' || ch > '9') { /* Check if current char is a digit */
      return INVALID_ARGUMENT;
    }
    i++;
  }

  return OK_STATUS;
}

/*
  Accepts a string as a parameter, checks if that string is a number(can be signed, started with + or -)
  Returns a number that states wether the string is a number.
*/
int checkNumeric(char *str) {
  char *strLocal = str;

  if (*(str) == '+' || *(str) == '-') { /* Checks if the first character is a sign, if so points it forward */
    strLocal++;
  }

  if (checkNumericUnsigned(strLocal) != OK_STATUS) { /* Checks if the rest of the characters are unsigned, digits only */
    return INVALID_ARGUMENT;
  }

  return OK_STATUS;
}

/*
  Takes as a parameter an argument that is of type array. eg: STR[1]
  Eliminates the braces from that string with null terminators and returns a pointer
  to the start of the index.
*/
char *getIndexFromArr(char *arr) {
  int i = 0;
  char *index;
  
  while (*(arr + i)) {
    if (*(arr + i) == '[') {
      *(arr + i) = '\0';
      index = arr + i + 1; /* We add 1 to point the index after the null terminator */
    }
    if (*(arr + i) == ']') {
      *(arr + i) = '\0';
    }
    i++;
  }

  return index;
}

/*
  Takes as parameter a string and checks if all of the characters in it are alphanumeric.
  Returns an int that states wether the given string is alphanumeric.
*/
int isAlphaNumeric(char *str) {
  int i;

  for (i = 0; i < strlen(str); i++) { /* Loops through the characters */
    if (!isalnum(*(str + i))) {
      return INVALID_ARGUMENT;
    }
  }

  return OK_STATUS;
}

/*
  Works very much alike 'getWord' excpect in here arguments can be delimited with a comma ',' without spaces in between it.
  This return the first argument in the paramter which is a pointer to a source code line.
  Points the source code line after the first argument stumpled. If argument was not last replaces the comma with a null terminator to
  delimit if from the line.
*/
char *getArg(char **line) {
  char ch, *arg;

  skipSpace(line);
  arg = *line;
  while ((ch = **line) && !isspace(ch) && ch != ',') {
    (*line)++;
  }

  (**line) = '\0';
  (*line)++;

  return arg;
}
