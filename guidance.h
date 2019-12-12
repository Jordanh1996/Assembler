#ifndef GUIDANCE_H
#define GUIDANCE_H

typedef struct guidance *guidancePtr;

/*
  A structure that will hold data about a single guidance operator
*/
typedef struct guidance {
  char *operator; /* The name of the guidance operator */
  int (*func)(char *, char *); /* A function to handle it */
} guidance;

int handleGuidance(char *line, char *word, char *label); /* Accepts a source code line, the guidance operator, and a label if was given and uses the correct funciton to handle the operator. This function is for the first scan */
guidancePtr getGuidance(char *str); /* Accepts a name of a guidance operator and returns a pointer to a struct that holds data about that guidance operator */

#endif
