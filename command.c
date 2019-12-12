#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "./command.h"
#include "./commandValidations.h"
#include "./commandUtils.h"
#include "./utils.h"
#include "./data.h"
#include "./status.h"
#include "./output.h"
#include "./strings.h"

/*
  This file holds logic regarding assembly code lines that are instructions and the way to treat them.
*/

/*
  Prototypes for functions that are available only for this file.
  The prototypes of the rest of the functions can be found in command.h and are
  placed there so they can be used by other files.
*/
void argToBinary(char *arg, enum ARG_TYPE type, int *bin, int isSrc, int curW);
void updateCommandAddress(int *bin, enum ARG_TYPE type, int isSrc);
void handleArgument(char *arg, enum ARG_TYPE type, int words[], int *curW, int isSrc);

/*
  Each line of source code that is of type command/instruction that is in the first scan is treated with this function.
  This function accepts the line of the source code, the name of the command and a label if was present.
  Adds a label to the symbol table if a label is present.
  Fetches the command and check if it exists.
  Increments the instruction count for the command and for each type of argument accordingly.
  Checks wether the amount of argumnts passed in the source code matches the expected arguments count for the command/instruction.
  Validates wether each argument matches the valid argument options for the command/instrution.
  Returns an int that represents success.
*/
int handleFirstCommand(char *line, char *commandName, char *label) {
  commandPtr comm;
  int status = OK_STATUS; /* In the end returns the status, if the status was changed somewhere in the funtion it means something wrong has happend */

  if (label != NULL) { /* Adds a label to the symbol table if a label is present */
    addSymbolNode(label, IC, COMMAND);
  }
  IC++; /* There is a word in the machine code for each instruction, This increment corresponds to the instruction iteslf */

  comm = getCommand(commandName); /* Fetches a pointer to a struct that holds metadata about the current command */

  if (comm != NULL) { /* Checks if the command was found */
    int args = countArgs(line); /* Finds the amount of arguments passed in the source code */
    
    if (args < 0) { /* A value less than 0 for args means there was a syntax error */
      return args;
    }
    if (args != comm->args) { /* Validates wether the recevied argument count matches the expected argument count for the given command */
      printe("Invalid amount of arguments for command %s, expected %d, but recevied %d", 0, commandName, comm->args, args);
      return TOO_MANY_ARGS;
    } else {
      char *arg;
      enum ARG_TYPE type;
      int argStatus, regC = 0;

      while (args > 0) { /* Loops through each argument */
        arg = getArg(&line); /* Fetches current argument from the source code */
        type = getArgType(arg);
        argStatus = valArg(arg, type, comm, args); /* Validates wether the command can accept this type of argument for the argument index(source/destination) */
        if (argStatus != OK_STATUS) {
          status = INVALID_ARGUMENT;
        }

        if (type == REG) /* We want to see if we had 2 register arguments because they share the same word in machine code and it means we need to decrement IC */
          regC++;

        incIC(type);
        args--;
      }

      if (regC > 1) { /* 2 registers were used for the current command */
        IC--;
      }

      return status;
    }
  } else {
    printe("Command '%s' does not exist", 0, commandName);
    return UNKNOWN_OPERATOR;
  }
}

/*
  Each command has a word in the result machine code.
  The word specifies the type of the operands in the command(register, array, etc...)
  Updates the word to hold data about the type of arguments passed to it.
  This function accepts a pointer to an int that is the binary value of the word, the type of the argument that was passed,
  and an int that indicates wether the argument is a source or destination operand.
*/
void updateCommandAddress(int *bin, enum ARG_TYPE type, int isSrc) {
  int addVal = argTypeToMode(type); /* Returns the value of the address mode of the argument */

  *bin += addVal << (isSrc ? SOURCE_DIST : DESTINATION_DIST); /* Updates the correct bits in the value that bin points to (The word of the command) */
}

/*
  Updates the value of the word that is the binary representation of the argument to be encoded.
  Accepts the string of the argument, its type, a pointer to the word, an indication wether 
  it is a source operand(relevant for registers) and an int that represents the current index
  of the words to be written in result to the current 1 line of instruction.
*/
void argToBinary(char *arg, enum ARG_TYPE type, int *bin, int isSrc, int curW) {
  switch(type) { /* Switches of the type and handles it accordingly */
    case IVAL:
      if (*arg == '#') { /* An immediate value as an argument starts with #, we point arg forward to later use atoi */
        arg++;
      }
      *bin += (atoi(arg) << ADDRESS_DIST) + ABS; /* Update the correct bits in the word and set the encoding type(absolute) */
      break;
    case MAC:
      {
        symbolNodePtr node;

        if (*arg == '#') { /* When a macro is used as an argument it starts with #, we point 'arg' forward */
          arg++;
        }

        node = symbolNodeByLabel(arg); /* Fetch the macro from the symbol table */

        if (node == NULL) { /* If the macro was not declared we print an error and break */
          printe("Label %s hasn't been declared", 0, arg);
          break;
        }

        *bin += (node->val << ADDRESS_DIST) + ABS; /* Updates the bits in the word and set encoding type to absolute */
        break;
      }
    case REG:
      *bin += (atoi(arg + 1) << (isSrc ? REG_SOURCE_DIST : REG_DESTINATION_DIST)) + ABS; /* For a register the encoding of its index position in the word vary wether it is a source/destination operand */
      break;
    case LABEL:
    {
      symbolNodePtr node = symbolNodeByLabel(arg); /* Fetch the label from the symbol table */

      if (node == NULL) { /* Validate wether the symbol exists */
        printe("Label %s hasn't been declared", 0, arg);
        break;
      }

      if (node->type == EXTERNAL) {
        *bin += EXT; /* In the case of an external label the word bits all should be zero instead of the encoding type */
        addExternal(node->label, IC + curW); /* Update the external file about the usage of an external in the correct instruction line */
      } else {
        *bin += (node->val << ADDRESS_DIST) + REL; /* In any other case we use val for the bits of the word and set encoding type to relocatable */
      }
    }
    break;
  default:
    break;
  }
}

/*
  Each arguemnt in the second scan is called with this function.
  This function will call argToBinary with the correct arguments to encode the words
  that correspond to the current command.
  Accepts a string of the argument, the type of the arguemnt, the array of the words
  that needs to be written, a pointer to an int that represents the amount of words that
  needs to be written in result of the entire command, and an int that represent wether
  the argument is a source operand.
  In the case of an argument that is an array 2 extra words need to be written.
*/
void handleArgument(char *arg, enum ARG_TYPE type, int words[], int *curW, int isSrc) {
  if (type == ARR) {
    char *index = getIndexFromArr(arg); /* Gets a string of the index of the array */
    enum ARG_TYPE indexType = checkNumeric(index) ? MAC : IVAL; /* Checks wether the index is a number or macro */

    argToBinary(arg, LABEL, &words[*curW], isSrc, *curW); /* Encodes the label of the array, the result is in words[] */
    (*curW)++;
    argToBinary(index, indexType, &words[*curW], isSrc, *curW); /* Encodes the index */
  } else {
    argToBinary(arg, type, &words[*curW], isSrc, *curW);
  }
  (*curW)++; /* Updates that another word has been added */
}

/*
  Each line of source code that is of type command/instruction that is in the first second is treated with this function.
  This function accepts the line of the source code, and the name of the command.
  Initializes an array that holds a numeric value of the words thats need to be written to the object file.
  Initializes an int that represents the amount of words to be written in regard to the current command.
  Writes the words to the object file.
  Returns an int that represents success.
*/
int handleSecondCommand(char *line, char *commandName) {
  commandPtr comm;
  int words[MAX_WORDS] = {0}, /* Sets all ints in words to 0 */
  curW = 0;

  comm = getCommand(commandName); /* Fetches command struct that holds data */

  words[COMMAND_WORD_INDEX] += (comm->opcode) << OPCODE_DIST; /* Updates the word bits that represent the command opcode */
  curW++;

  if (comm != NULL) {
    int args = comm->args, i, isSrc = 0, 
    isFirstReg = 0; /* Indicates wether the first argument was of type register */

    for (i = 0; i < args; i++) { /* Loop over the arguments */
      char *arg;
      enum ARG_TYPE type;

      arg = getArg(&line);
      type = getArgType(arg);

      updateCommandAddress(&words[COMMAND_WORD_INDEX], type, isSrc || args == 1); /* Updates the word corresponds to the command about the argument type, in the case where there is 1 argument it is always a source operand */

      if (isFirstReg && type == REG) { /* In the case where 2 arguments are register they share a word, this handles this edge case */
        curW--;
      }

      handleArgument(arg, type, words, &curW, isSrc); /* Encodes the current argument */

      if (type == REG) {
        isFirstReg++;
      }
      isSrc++;
    }

    addWords(words, curW); /* Add all the new words to a variable that stores them until the scan is finished */

    return OK_STATUS;
  } else {
    return UNKNOWN_OPERATOR;
  }
}
