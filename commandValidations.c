#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./commandValidations.h"
#include "./command.h"
#include "./commandUtils.h"
#include "./status.h"
#include "./data.h"
#include "./utils.h"

/*
  Holds the function valArg that is used by handleFirstCommand which is used for every comamnd in the first
  scan, this function returns an int that states wether an argument is valid.
  Also holds helper functions that are local only to this file.
*/

/*
  Prototype definition for functions that are available only for this file.
  Their usage is for other functions that are used from others files.
  The rest of the functions prototypes can be found in commandValidations.h
*/
int valAddressMode(int mode, struct command *comm, int args);
int valMac(char *label);
int valReg(char *num);
int valArr(char *label);
int valIndex(char *index);

/*
  Takes a string of an argument, the argument type, a pointer to the command that got that argument, and the
  argument index(1 or 2) as parameters.
  Returns an int that represents wether the arguemnt is valid.
  Checks if the address mode is valid for the given command.
  Uses helper functions for more validations.
*/
int valArg(char *arg, int type, commandPtr comm, int argIndex) {
  if (valAddressMode(argTypeToMode(type), comm, argIndex) != OK_STATUS) { /* Checks wether the address mode is valid */
    return INVALID_ARGUMENT;
  }
  switch (type) {
    case IVAL:
      return OK_STATUS; /* No validations for immediate values */
    case MAC:
      return valMac(arg + 1); /* Macro arguments starts with '#', we pass a char * that points to the start of the value */
    case REG:
      return valReg(arg + 1); /* Register arguments starts with 'r', we pass a char * that points to the start of the register index */
    case ARR:
      return valArr(arg);
    case LABEL:
      return OK_STATUS; /* No validations for labels */
    default:
      return UNKNOWN_OPERATOR;
  }
}

/*
  Takes as paramaters an int that represents the address mode(enum ADDRESS_MODE), a pointer to a command struct
  that holds data about a command, and the index of the argument that was passed to that command.
  Checks wether the address mode of the argument is valid for the corresponding command.
  The index is needed to know wether the argument is a source or destination operand.
  Returns an int that indicates wether the address mode is valid.
*/
int valAddressMode(int mode, commandPtr comm, int argIndex) {
  int i, *modes; /* modes will point to an int array that will hold the valid address modes the argument can have */

  if (argIndex == comm->args) { /* Points modes to the correct int array of address modes, either destination or source */
    modes = comm->dest;
  } else {
    modes = comm->src;
  }

  for (i = 0; i < MAX_ADDRESS_MODE; i++) { /* Loops over the address modes and if a correct one is found returns an ok status */
    if (mode == *(modes + i)) {
      return OK_STATUS;
    }
  }

  printe("Invalid address mode", 0); /* If the address mode was not found in the loop then it's of the wrong type */
  return INVALID_ARGUMENT;
}

/*
  Validates a macro argument.
  Accepts a string of the label of the macro and returns an int that states if it is valid.
*/
int valMac(char *label) {
  symbolNodePtr mac = symbolNodeByLabel(label); /* Fetches the macro from the symbol table */

  if (mac == NULL) { /* If the label does not exists then a NULL was returned */
    printe("Macro %s has not been declared", 0, label);
    return INVALID_ARGUMENT;
  } else {
    if (mac->type != MACRO) { /* Checks if the symbol that was fetched is acctully a macro */
      printe("Argument %s is not a macro", 0, label);
      return INVALID_ARGUMENT;
    }
    return OK_STATUS;
  }
}

/*
  Takes a string of the index of a register and validates wether it is in range.
  Returns an int that represents if it is valid.
*/
int valReg(char *num) {
  int val = atoi(num);

  if (val < 0 || val >= REGISTER_AMOUNT) { /* Checks if index is in range */
    printe("Invalid register index %d, index must be between 0 and %d", 0, val, REGISTER_AMOUNT - 1);
    return INVALID_ARGUMENT;
  }

  return OK_STATUS;
}

/*
  Validates an argument string of type array.
  Checks the label and the index of the array and if an opening brace is present.
  Returns an int that reprensents if it is valid.
*/
int valArr(char *label) {
  int i;
  char ch;

  for (i = 0; i < strlen(label); i++) { /* Loops over all of the characeters of the argument */
    ch = *(label + i);
    if (ch == '[') {
      if (i == 0) { /* If an opening brace was the first character than a label wasn't present */
        printe("No label name found for %s", 0, label);
        return INVALID_SYNTAX;
      } else {
        if (valIndex(label + i + 1) != OK_STATUS) { /* Validates the index of the array, we need to add 1 to the point to point after the opening brace */
          return INVALID_ARGUMENT;
        }

        return OK_STATUS;
      }
    }
  }

  printe("No opening brace found for %s", 0, label); /* If execution reaches this place than no opening brace was found in the argument */
  return INVALID_SYNTAX;
}

/*
  Takes a string of the index that ends with a closing brace. 
  An index can be either a macro or an immediate value(number).
  Returns an int that indicates wether the index is valid.
*/
int valIndex(char *index) {
  int i;
  char ch;

  *(index + strlen(index) - 1) = '\0'; /* Temporary set the closign brace to a null terminator */

  for (i = 0; i < strlen(index); i++) { /* Loops over the index */
    ch = *(index + i);
    if (ch < '0' || ch > '9') { /* A character that is not a digit was found, so the index is a macro */
      if (valMac(index) != OK_STATUS) {
        *(index + strlen(index)) = ']';
        return INVALID_ARGUMENT;
      } else {
        *(index + strlen(index)) = ']';
        return OK_STATUS;
      }
    }
  }

  *(index + strlen(index)) = ']';

  if (i == 0) { /* If the loop had 0 iterations it means the braces were empty */
    printe("No index inserted", 0);
    return NOT_ENOUGH_ARGS;
  }

  return OK_STATUS;
}
