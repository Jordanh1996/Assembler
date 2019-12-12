#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "./data.h"
#include "./utils.h"
#include "./scan.h"
#include "./guidance.h"
#include "./command.h"
#include "./commandUtils.h"
#include "./status.h"
#include "./files.h"
#include "./strings.h"

/*
  Holds functions that scan through the source code.
*/

/*
  Prototypes for functions that are used only within this file.
  The rest of the functions prototypes can be found in scan.h
*/
int validateLabel(char *label);
void updateEntry(char *label);

char line[LINE_MAX]; /* The current line that is being processed */
int lineIndex = 0; /* The source code line index that is being processed */

/*
  This function takes a file pointer to a source code file and a function(That will be the function to treat each line of code(first or second scan)).
  Reads the lines from the source code and calls each line with the function that was passed as a parameter.
  Validates that a line doesn't exceed its max character count.
*/
void scan(FILE *fp, int func(char *)) {
  char *lineCpy;

  while (fgets(line, LINE_MAX, fp) != NULL) { /* Reads a line form the source code and stores it in line */
    lineIndex++;
    if (strlen(line) == (LINE_MAX - 1) && *(line + strlen(line) - 1) != '\n') { /* Validates the character length of the line */
      char ch;
      while ((ch = getc(fp)) != '\n' && ch != EOF);
      printe("\nA line can have at most %d characters", 0, LINE_MAX - 1);
      continue;
    }
    lineCpy = copyString(line); /* Creates a new string with the current line because the scan functions may mutate it */
    func(lineCpy); /* Calls the parameter function with the current line */
  }

  rewind(fp); /* Returns the file pointer to the beginning of the file */
}

/*
  Takes a line of source code and treats it accordingly.
  Checks the type of the line (comment/gudiane/command...)
  Calls a function to thandle the type of the line accordingly.
  Grabs the label if was present.
  Returns an int that states wether an error has occoured in the scan of the current line.
*/
int scanFirst(char *line) {
  char *word = getWord(&line), /* Grabs the first word from the source code line and points line after the first word */
  *label = NULL;
  int status = 0;

  if (strlen(word) == 0 || *(word) == ';') { /* Checks if the line is empty to a comment line */
    return status;
  }

  if (*(word + strlen(word) - 1) == ':') { /* Check if the first word was a label */
    *(word + strlen(word) - 1) = '\0';
    label = word;
    word = getWord(&line); /* Grabs the next word */
    validateLabel(label); /* Validates the syntax of the label */
  }

  if (strlen(word) == 0) { /* Check if after the label the line was blank */
    printe("Label cannot be followed by an empty line", 0);
    return INVALID_SYNTAX;
  }

  if (*(word) == '.') { /* Checks if the word is a guidance or command operator */
    return handleGuidance(line, word, label);
  } else {
    return handleFirstCommand(line, word, label);
  }
}

/*
  Takes a line of source code file and treat it accordingly.
  Encodes each line of type command.
  Update entry file for given entry guidance.
  Returns int that states wether there was an error in the source code line.
*/
int scanSecond(char *line) {
  char *word = getWord(&line); /* Grabs the first word from the source code line */
  int status = 0;

  if (strlen(word) == 0 || *(word) == ';') { /* Checks if the line was either empty or a comment */
    return status;
  }

  if (*(word + strlen(word) - 1) == ':') { /* Check if a label was present */
    word = getWord(&line);
  }

  if (*(word) == '.') { /* Checks if the operator was guidance */
    if (strcmp(word + 1, "entry") == 0) { /* We care only for entry guidances here */
      char *entry = getWord(&line);
      updateEntry(entry); /* Updates the symbol table about an entry */
    }
    return OK_STATUS;
  } else {
    return handleSecondCommand(line, word);
  }
}

/*
  Accepts a label as a parameter.
  Returns an int that specifies if the label syntax is correct of if it is alredy used or if it is a reserved keyword.
*/
int validateLabel(char *label) {
  int status = OK_STATUS;
  guidancePtr guid = getGuidance(label); /* Guidance operand names are reserved */
  commandPtr comm = getCommand(label); /* Command operand names are reserved */
  if (strlen(label) > LABEL_MAX) { /* Checks if a label characters count exceeds the maximum */
    printe("Label characters count must not exceed %d", 0, LABEL_MAX);
    status = INVALID_SYNTAX;
  }
  if (isAlphaNumeric(label) != OK_STATUS) { /* Checks if only alphanumeric characters are in the label */
    printe("Label must include only alphabetic characters and numbers", 0);
    status = INVALID_SYNTAX;
  } else if (isalpha(*label) == 0) { /* First character must be alphabetic, not a number */
    printe("Label must start with an alphabetic character", 0);
    status = INVALID_SYNTAX;
  }
  if (symbolNodeByLabel(label) != NULL) { /* Checks if the label was alredy defiend elsewhere */
    printe("Label %s has alredy been defined", 0, label);
    status = INVALID_ARGUMENT;
  }
  if (guid != NULL || comm != NULL || (*label == 'r' && checkNumericUnsigned(label + 1))) { /* Checks for reserved keyword, registers are also reserved */
    printe("Label %s cannot be used, it is a reserved keyword", 0, label);
    status = INVALID_SYNTAX;
  }
  return status;
}

/*
  Whenever an entry is found we want to search for it in the symbol table and state that this symbol is an entry so later
  we can write it in the .ent file.
*/
void updateEntry(char *label) {
  symbolNodePtr node = symbolNodeByLabel(label);

  if (node == NULL) { /* Warns that the label given to .entry does not exists */
    warning("Label %s does not exist", label);
  } else if (node->type == ENTRY) { /* Checks if .entry for the same label was used multiple times */
    warning("Entry for this label has alredy been declared");
  } else if (node->type != GUIDANCE && node->type != COMMAND) { /* Checks the correct type, for example macro cannot be an entry */
    warning("Entry label must be a command or guidance label %d");
  } else {
    node->type = ENTRY;
  }
}
