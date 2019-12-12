#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "./utils.h"
#include "./guidance.h"
#include "./data.h"
#include "./status.h"
#include "./strings.h"

/*
  Functions that handles source code line that are of type guidance.
*/

/*
  Prototype for functiosn that are used and private only to this file.
  The prototypes for the rest of the functions can be found in guidance.h
*/
int createDefinition(char *line, char *label);
int createExtern(char *line, char *label);
int createString(char *line, char *label);
int createData(char *line, char *label);
int createEntry(char *line, char *label);

/*
  A struct array of all of the type of guidances and for each holds a function that treats the guidance accordingly.
*/
guidance guidances[] = {
  { "data", createData },
  { "string", createString },
  { "entry", createEntry },
  { "extern", createExtern },
  { "define", createDefinition }
};

/*
  Accepts the line of source code, the guidance operator and a label if exists.
  Searches for the guidance in the guidances array, and if found calls the guidance function with the current line of source code and
  the label.
  If a guidance was not found creates an error.
  Returns an int that states if there was an error.
*/
int handleGuidance(char *line, char *word, char *label) {
  guidancePtr guid = getGuidance(word + 1); /* The +1 is to point after the '.' in the operand */

  if (guid == NULL) {
    printe("Unknown guidance operator %s", 0, word + 1); /* Guidace does not exist */
    return UNKNOWN_OPERATOR;
  } else {
    return guid->func(line, label); /* Calls the guidance function */
  }
}

/*
  Accepts a string of the name of the guidance.
  Loops through the guidances array and returns a pointer to that guidance if found.
  Otherwise returns NULL.
*/
guidancePtr getGuidance(char *str) {
  int i;

  for (i = 0; i < sizeof(guidances) / sizeof(guidance); i++) {
    if (strcmp(str, guidances[i].operator) == 0) {
      return &guidances[i];
    }
  }

  return NULL; /* If exection reaches here no guidance was found */
}

/*
  Handles a '.data' guidance.
  Searches for syntax errors, Invalid amount of arguments.
  Updates the data table.
  Creates a label in the symbol table if a label was given.
*/
int createData(char *line, char *label) {
  int args;
  char *arg;

  if (label != NULL) { /* If a label was given adds it to the symbol table */
    addSymbolNode(label, DC, GUIDANCE);
  }

  args = countArgs(line); /* Checks how many arguments were passed in the source code */

  if (args == 0) { /* Checks if .data was called without any arguments */
    printe("Must pass at least 1 argument to .data", 0);
    return NOT_ENOUGH_ARGS;
  }
  if (args < 0) { /* A negative args count means there was a syntax error */
    return INVALID_SYNTAX;
  }

  while (args--) { /* Loops through the arguments */
    arg = getArg(&line);

    if (checkNumeric(arg) == OK_STATUS) { /* If argument was a number adds it immediately */
      addDataNode(label, atoi(arg));
    } else { /* Else the argument is a macro */
      symbolNodePtr mac = symbolNodeByLabel(arg);

      if (mac == NULL) { /* Macro no found in the symbol table */
        printe("%s is not defined", 0, arg);
        return UNKNOWN_OPERATOR;
      } else if (mac->type != MACRO) { /* Symbol is not of type macro */
        printe("%s is not of type macro", 0, arg);
        return INVALID_ARGUMENT;
      } else {
        addDataNode(label, mac->val);
      }
    }
  }

  return OK_STATUS;
}

/*
  Handle a '.string. guidance.
  Adds a label if one was given.
  Checks for syntax errors.
  Updates data table.
*/
int createString(char *line, char *label) {
  char ch;
  int i, 
  start = 0; /* Used later in a loop */

  if (label != NULL) { /* Checks if a label was given and if so adds it to symbol table */
    addSymbolNode(label, DC, GUIDANCE);
  }

  skipSpace(&line); /* Skips any spaces to point to the first character after the .string operand */

  if (*line == '\0') { /* If after skipping reached a \0 no arguemnt was passed */
    printe("No string provided to .string", 0);
    return NOT_ENOUGH_ARGS;
  }
  if (*line != '"') { /* If the first character is not a " then syntax is invalid */
    printe("A string must start with '\"'", 0);
    return INVALID_SYNTAX;
  }

  line++;
  i = strlen(line) - 1; /* We need to subtract 1 to point the last character that is not \0 */

  while (i >= 0) { /* Loops throguh all of the characters from the end to the beginning */
    ch = *(line + i);
    if (!isspace(ch)) {
      if (ch != '"') { /* The string must end with a " */
        printe(".string must end with a '\"'", 0);
        return INVALID_SYNTAX;
      } else {
        start = 1; /* Checks to see if there is at least 1 character after the first " */
        break;
      }
    }
    i--;
  }

  if (!start) { /* If start equals zero there is nothing after the opening quotes */
    printe(".string must end with a '\"'", 0);
    return INVALID_SYNTAX;
  }

  *(line + i) = '\0'; /* We want to add a null terminator to ignore trailing spaces */

  for (i = 0; i <= strlen(line); i++) { /* Loops through all the relevant chars and add them to the data table */
    addDataNode(label, (int) *(line + i));
  }

  return OK_STATUS;
}

/*
  Handles an '.extern' guidance operator.
  Checks for syntax errors.
  Sends a warning is a label was given.
  Adds the external to the symbol table.
*/
int createExtern(char *line, char *label) {
  char *ext = lalloc(), *check = lalloc();

  if (label != NULL) { /* Checks if a label was given */
    warning("A label in an extern guidance is meaningless");
  }

  sscanf(line, "%s%s", ext, check); /* Checks if another extra argument was given, extern expects 1 */

  if (!strlen(ext)) { /* .extern was followed by nothing */
    printe("Extern name not received", 2, ext, check);
    return NOT_ENOUGH_ARGS;
  }

  if (strlen(check)) { /* Checks if another argument was passed */
    printe("Too many arguments passed to extern statement", 2, ext, check);
    return TOO_MANY_ARGS;
  }

  addSymbolNode(copyString(ext), 0, EXTERNAL); /* Adds the external to the symbol table */
  frees(2, ext, check);
  return OK_STATUS;
}

/*
  Handle the '.define' guidance operator.
  Searches for syntax errors.
  Adds the macro to the symbol table.
  Sends an error if a label was given.
*/
int createDefinition(char *line, char *label) {
  char *macro = lalloc(), *num = lalloc(), *token, *del = "=", *check = lalloc();
  int val;

  if (label != NULL) { /* Checks if a label was given */
    printe("Cannot add a label to a macro definition", 0);
  }

  token = strtok(line, del); /* Seperate the line with the '=' sign */

  if (token == NULL) { /* Checks if an argument was passed to .define */
    printe("No argument passed to .define statement", 3, macro, num, check);
    return NOT_ENOUGH_ARGS;
  }

  sscanf(token, "%s %s", macro, check);/* Extracts macro name and value from the source code line */

  if (strlen(check)) { /* Checks if a '=' was not present  */
    printe("Macro name needs to be followed by a '='", 3, macro, num, check);
    return INVALID_SYNTAX;
  }

  token = strtok(NULL, del);

  if (token == NULL) {
    printe("Macro name needs to be followed by a '='", 3, macro, num, check);
    return INVALID_SYNTAX;
  }

  sscanf(token, "%s %s", num, check);

  if (strlen(check)) { /* Checks if to many values were passed to .define */
    printe("Macro value cannot be followed by another value", 3, macro, num, check);
    return TOO_MANY_ARGS;
  }
  
  if (checkNumeric(num) != OK_STATUS) { /* Checks if the macro value is numeric */
    printe("Macro value must be a whole number", 0);
  }
  
  val = atoi(num);
  
  addSymbolNode(macro, val, MACRO); /* Adds macro to symbol table */
  frees(3, macro, num, check);
  return OK_STATUS;
}

/*
  Handles an .entry guidance for the first scan.
  Sends a warning if a label was used in the .entry statement
*/
int createEntry(char *line, char *label) {
  if (label != NULL) { /* Checks if a label was given */
    warning("A label in an entry guidance is meaningless");
  }

  return OK_STATUS;
}
