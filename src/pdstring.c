/* Generic string routines. */

/*
 * Copyright (c) 1995-1999 Joey Hess (joey@kitenet.net)
 * All rights reserved. See COPYING for full copyright information (GPL).
 */

#include <string.h>

/*
 * Remove all '\' characters in the string s that escape out the passed
 * character or '\'.
 * Return a pointer to the result or NULL if the string is empty.
 */
char *unescape (char *s, char token) {
  char escaped=0;
  char *p, *r=s;

  if (!s)
    return r;

  while (*s) {
    escaped = ((*s == '\\') && !escaped);
    s++;
    if (*s && escaped && (*s == token || *s == '\\')) {
      /* found an escape we should remove */
      for (p=s-1; *p ; p++) {
	*p=*(p+1);
      }
      escaped=0;
    }
  }

  if (r[0] != '\0')
    return r;
  else
    return NULL;
}

/*
 * This is vaguely like strtok, except '\' can escape out the token,
 * and you may only pass one token (as a character, not as a string).
 * 
 * Taken from Mutt, and heavily modified.
 */
char *pdstrtok (char *s, char token) {
  static char *p;
  char *r, *t, *u, escaped;

  if (s) /* initial call */
    p = s;

  if (!p)
    return NULL;

  r = p;
  t = p;

  /*
   * Find the first occurence of the token that is not escaped. To do this,
   * we find the token, then seek backwards to count how many slashes in a row
   * are before it. If the number of slashes is even, the token is not 
   * escaped out. This turns out to be about 3 times as fast as running
   * through the string by hand without strchr.
   */
  while ((t=strchr(t,token)) != NULL) {
    u=t;
    escaped=0;
    while (--u>=p && *u =='\\' ) /* search backwards for slashes. */
      escaped = !escaped;
    if (!escaped)
      break;
    else
      t++; /* increment t so we don't hit this character again. */
  }

  if (t && *t) { /* found the token */
    *t = 0;
    p = ++t;
  }
  else /* no token found, so all done */
    p = NULL;

  return(r);
}
