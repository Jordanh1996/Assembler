#ifndef COMMANDUTILS_H
#define COMMANDUTILS_H

struct command; /* States the a struct command exists, it is declared in command.h */

struct command *getCommand(char *commandName); /* Takes a string of a name of a command are returns a pointer to a struct that holds data about the given command */
int getArgType(char *arg); /* Takes an argument as a string and returns an int that represents its type(enum ARG_TYPE) */
void incIC(int type); /* Takes a type of an argument as a parameter and increments the instruction count */
int argTypeToMode(int type); /* Takes an int that represents a type of an argument(enum ARG_TYPE) and returns an int that represents an address mode(enum ADDRESS_MODE) */

#endif
