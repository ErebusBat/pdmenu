/*
 * Copyright (c) 1995, 1996, 1997 Joey Hess (joey@kitenet.net)
 * All rights reserved. See COPYING for full copyright information (GPL).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * How many characters to read in at a time. Should be greater than the
 * average line length, otherwise, it doesn't really matter.
 */
#define BUF_INC 256

/*
 * This reads in a full line of arbitrary length from the passed file, 
 * allocates a buffer for the data, and returns a pointer to it.
 *
 * If allow_cont is set, continuation lines are treated as one line.
 *
 * Note that NULL will be returned on eof.
 *
 * Note also that any trailing \n on the line will be removed.
 */
char *pdgetline(FILE *fp, int allow_cont) {
  char *str=NULL;
  int strsize=0;

  if (feof(fp))
    return NULL;

  do {
    str=realloc(str,strsize+BUF_INC+1);
    if (! fgets(str + strsize,BUF_INC,fp)) {
      if (strsize == 0) {
	free(str);
	return NULL; /* reached eof with empty string */
      }
      else {
	str[strsize]='\0'; /* work around a bug (?) in fgets */
	break;
      }
    }
    strsize=strlen(str);
  } while (
	   /* continuation line */
	   (allow_cont && strsize>1 && str[strsize-2] == '\\' && 
	    (strsize=strsize-2)) ||
	   /* long line */
	   (strsize>0 && str[strsize-1] != '\n')
	   );

  /* remove trailing \n */
  if (str[strsize-1] == '\n')
    str[strsize-1]='\0';

  return str;
}
