/* 
 * Copyright (c) 1995-1999 Joey Hess (joey@kitenet.net)
 * All rights reserved. See COPYING for full copyright information (GPL).
 */

/* This type is a string linked to a true/false value. */
typedef struct Conditional_String {
  char *value;
  int ignore; /* 1=ignore the value */
} Conditional_String;            

Conditional_String *DoInputBox (char *,char *);
Conditional_String *EditTags (char *);
