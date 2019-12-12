#ifndef STRINGS_H
#define STRINGS_H

#define BINARY_TO_SPECIAL_LENGTH 2 /* The special machine code syntax takes 2 binary digits and converts them to 1 special character */

char *addExtension(char *fileName, char *ext); /* Takes 2 strings, creats a new strings which is the concatenation of them and returns the new string */
int skipSpace(char **str); /* Takes a pointer to a string, Forwards the pointer to point after every space in the beginning */
char *getWord(char **line); /* Takes a pointer to a string, Delimits the first word from the line that is delimited by a space or null terminator, points the string after the word and returns a pointer to that first word */
char *toBinaryString(int num, int length); /* Turns a number to string of its binary representation, the amount of bits will be the second argument */
char toSpecialChar(char *str); /* Takes a string of binary digits and returns the speical character representation of the first digits */
int countArgs(char *line); /* Takes a line of source code and count how many arguments are there, argument are delimited by commas ',' */
char *copyString(char *str); /* Takes a string, creates a new one and copies all of the character of the original string to it, returns the new string */
int checkNumericUnsigned(char *str); /* Checks if a string characters are all digits */
int checkNumeric(char *str); /* Checks if a string characters are all digits, except for the first one that can also be a sign '+'/'-' */
char *getIndexFromArr(char *arr); /* Takes a string that represents an array argument, replaces the braces with null terminators and returns a pointer to the start of the index */
int isAlphaNumeric(char *str); /* Takes a string and checks if all characters are alphanumeric */
char *getArg(char **line); /* Takes a pointer to a line of source code, returns the first argument stumpled upon and points the line after that argument */

#endif
