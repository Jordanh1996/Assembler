#ifndef COMMANDVALIDATIONS_H
#define COMMANDVALIDATIONS_H

struct command; /* States the a struct command exists, it is declared in command.h */

/* 
  Takes as parameters a string of the argument, int that states the argument type, 
  a pointer to the struct of a command that holds data about the command which the 
  argument was passed to, and the index of the current argument and returns wether
  the argument is valid or not.
*/
int valArg(char *arg, int type, struct command * comm, int argIndex); 

#endif
