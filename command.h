#ifndef COMMAND_H
#define COMMAND_H

#define MAX_ADDRESS_MODE 4 /* The maximum amount of address mode types */
#define OPCODE_DIST 6 /* The distance from the right(in bits) of the opcode in the binary encoding of a word */
#define ADDRESS_DIST 2 /* The distance form the right(in bits) from the binary encoding of the address mode */

#define DESTINATION_DIST 4 /* The distance from the right(in bits) of the destination operand in a command word */
#define SOURCE_DIST 2 /* The distance from the right(in bits) of the source operand in a command word  */

#define REG_DESTINATION_DIST 5 /* The distance from the right(in bits) of the register index encoding in the argument word */
#define REG_SOURCE_DIST 2      /* The distance from the right(in bits) of the register index encoding in the argument word */

#define MAX_WORDS 5 /* The maximum of words that can be written in result of a single source code line */
#define COMMAND_WORD_INDEX 0 /* The index of the command word in the array of words that correspond to a single source code line */

enum ARG_TYPE /* Types of an argument passed to command line in the source code */
{
  BVAL,
  REG,
  LABEL,
  ARR,
  MAC,
  IVAL
};

enum ADDRESS_MODE /* Types of address mode encoding */
{
  IMMED,
  DIRECT,
  INDEX,
  REGISTER_MODE
};

enum ARE /* Types of encoding of address mode of an argument in its word, the order is important because it determines the value of each of them */
{
  ABS, /* Absolute */
  EXT, /* External */
  REL /* Relocatable */
};

typedef struct command * commandPtr;

typedef struct command { /* A struct that holds data about a command */
  char *name; /* The string representation of a command */
  int opcode, /* The OPCODE of the command, it needs to be encoded in the command word */
      args,   /* Expected amount of arguments to be passed to the command */
      dest[MAX_ADDRESS_MODE], /* In case a destination operand exists, an array that holds the available address modes(enum ADDRESS_MODE) */
      src[MAX_ADDRESS_MODE];  /* In case a source operand exists, an array that holds the available address modes(enum ADDRESS_MODE) */
} command;

int handleFirstCommand(char *line, char *command, char *label); /* Searches for syntax errors in a line of command, updates symbol table about labels, updates instruction count */
int handleSecondCommand(char *line, char *commandName); /* Creates words for a given instruction and writes it to the object file */

#endif
