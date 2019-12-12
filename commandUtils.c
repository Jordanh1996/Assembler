#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "./commandUtils.h"
#include "./utils.h"
#include "./command.h"
#include "./data.h"
#include "./status.h"
#include "./strings.h"

/*
  This files holds utilities regarding commands and their arguments.
  Holds an array of all of the different commands and data about them.
*/

/*
  An array of all of the avilable commands, their opcode, expected argument count, and address types of their arguemnts.
*/
static command commands[] = {
  { "mov", 0, 2, {DIRECT, INDEX, REGISTER_MODE}, {IMMED, DIRECT, INDEX, REGISTER_MODE} },
  { "cmp", 1, 2, {IMMED, DIRECT, INDEX, REGISTER_MODE}, {IMMED, DIRECT, INDEX, REGISTER_MODE} },
  { "add", 2, 2, {DIRECT, INDEX, REGISTER_MODE}, {IMMED, DIRECT, INDEX, REGISTER_MODE} },
  { "sub", 3, 2, {DIRECT, INDEX, REGISTER_MODE}, {IMMED, DIRECT, INDEX, REGISTER_MODE} },
  { "not", 4, 1, {DIRECT, INDEX, REGISTER_MODE} },
  { "clr", 5, 1, {DIRECT, INDEX, REGISTER_MODE} },
  { "lea", 6, 2, {DIRECT, INDEX, REGISTER_MODE}, {DIRECT, ARR} },
  { "inc", 7, 1, {DIRECT, INDEX, REGISTER_MODE} },
  { "dec", 8, 1, {DIRECT, INDEX, REGISTER_MODE} },
  { "jmp", 9, 1, {DIRECT, REGISTER_MODE} },
  { "bne", 10, 1, {DIRECT, REGISTER_MODE} },
  { "red", 11, 1, {DIRECT, INDEX, REGISTER_MODE} },
  { "prn", 12, 1, {IMMED, DIRECT, INDEX, REGISTER_MODE} },
  { "jsr", 13, 1, {DIRECT, REGISTER_MODE} },
  { "rts", 14, 0 },
  { "stop", 15, 0 }
};

/*
  Takes a string of a name of a command and returns a pointer to a struct of that command with data about it
  from the commands array.
  If the command is not present in the array returns NULL.
*/
commandPtr getCommand(char *commandName) {
  int i = 0;

  for (i = 0; i < sizeof(commands) / sizeof(command); i++) { /* Loops over the commands structs array */
    if (strcmp(commandName, commands[i].name) == 0) {
      return &commands[i];
    }
  }

  return NULL;
}

/*
  Takes a string of an argument as a parameter.
  Returns an int that represents its type(enum ARG_TYPE)
*/
int getArgType(char *arg) {
  if (*arg == '#') { /* Operands that start with '#' are either immediate numbers or macros */
    int numeric = checkNumeric((arg + 1));

    if (numeric == OK_STATUS) {
      return IVAL;
    } else {
      return MAC;
    }
  }

  if (*arg == 'r' && checkNumericUnsigned(arg + 1) == OK_STATUS) { /* Checks wether argument is a register */
    return REG;
  }

  if (*(arg + strlen(arg) - 1) == ']') { /* If argument ends with ']' it must be an array */
    return ARR;
  } else {
    return LABEL; /* If none of the above all that is left is a label */
  }
}

/*
  Takes an int that represents the type of an argument(enum ARG_TYPE) and returns an int that
  represents the address mode of the arguemnt(enum ADDRESS_MODE).
*/
int argTypeToMode(int type) {
  switch(type) {
    case IVAL:
      return IMMED;
    case MAC:
      return IMMED;
    case LABEL:
      return DIRECT;
    case ARR:
      return INDEX;
    case REG:
      return REGISTER_MODE;
    default:
      return IMMED;
  }
}

/*
  Takes an int as a parameter that represents a type of an argument(enum ARG_TYPE) and increments the
  instruction count accordingly, all are incremented by 1 except in the case of an array.
*/
void incIC(int type) {
  switch(type) {
    case IVAL:
      IC++;
      break;
    case MAC:
      IC++;
      break;
    case REG:
      IC++;
      break;
    case ARR:
      IC += 2;
      break;
    case LABEL:
      IC++;
      break;
    default:
      return;
  }
}
