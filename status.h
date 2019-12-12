#ifndef STATUS_H
#define STATUS_H

/* Holds different status codes for different errors or to state that everything is ok */
enum STATUS 
{
  OK_STATUS,
  NOT_ENOUGH_ARGS,
  TOO_MANY_ARGS,
  INVALID_ARGUMENT,
  INVALID_SYNTAX,
  UNKNOWN_OPERATOR,
  BAD_STATUS
};

#endif
