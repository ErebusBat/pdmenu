/*
 * Copyright (c) 1995, 1996, 1997 Joey Hess (joey@kite.ml.org)
 * All rights reserved. See COPYING for full copyright information (GPL).
 */

#include <string.h>
#include <strings.h>

/*
 * Remove all '\' characters in the string s that escape out the passed 
 * character or '\'.
 * Return a pointer to the result.
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

	return r;
}

/*
 * This is vaguely like strtok, except '\' can escape out the token,
 * and you may only pass one token (as a character, not as a string).
 * 
 * Taken from Mutt, and heavily modified.
 */
char *pdstrtok (char *s, char token) {
	static char *p;
	char *r, *t, escaped=0;

	if (s) /* initial call. */
		p = s;

	if (!p)
		return NULL;

	r = p;
	t = p;

	/*
	 * Find the first occurence of the token that is not escaped. To do this, 
	 * we have to count the number of slashes in a row, if it is even, the token
	 * is not escaped out.
	 */
	while (*t && (*t != token || escaped)) {
		escaped = ((*t == '\\') && !escaped);
		t++;
	}

	if (*t) { /* found the token */
		*t = 0;
		p = ++t;
	}
	else /* no token found, so all done */
		p = NULL;

	return(r);
}
