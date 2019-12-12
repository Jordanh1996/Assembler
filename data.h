#ifndef DATA_H
#define DATA_H

#define REGISTER_AMOUNT 8 /* Defines the amount of available registers */
#define MEMORY_BASE 100 /* Defines to what base memory address the machine code needs to be loaded to */
#define DATA_BASE 0 /* Defines in which memory address the data starts */

enum SYMBOL_TYPE /* Different types of instructions */
{
  GUIDANCE,
  COMMAND,
  MACRO,
  EXTERNAL,
  ENTRY
};

enum ERROR /* Types of errors */
{
  OK, /* No error */
  FIRST, /* Error on first scan */
  SECOND /* Error on second scan */
};

typedef struct symbolNode *symbolNodePtr;
typedef struct symbolNode /* The symbol table is defined in the code as a linked list, this is the struct of each node */
{
  char *label;
  int val;
  enum SYMBOL_TYPE type;
  symbolNodePtr next;
} symbolNode;

typedef struct dataNode *dataNodePtr;
typedef struct dataNode /* The data table is defined in the code as a linked list, this is the struct of each node */
{
  char *label;
  int index,
  val;
  dataNodePtr next;
} dataNode;

extern dataNodePtr dataHead; /* This will point to the first node of the data table */
extern symbolNodePtr symbolHead; /* This will point to the first node of the symbol table */
extern int DC; /* The data count */
extern int IC; /* The instruction count */
extern int error; /* A global variable that states if there was an error(enum ERROR) */
extern int scanCount; /* A global variable that states the current amount of scans(enum ERROR is also used here) */
extern char line[]; /* A global variable that holds the current line that is scanned */
extern int lineIndex; /* A global variable that holds the current scanned line index in the source code */

#endif
