/* 
 * Copyright (c) 1995-1999 Joey Hess (joey@kitenet.net)
 * All rights reserved. See COPYING for full copyright information (GPL).
 */

#include "global.h"
#include "slang.h"
#include <unistd.h>

#define TIMEOUT 2 /* 2/10 of a second */

/* Get a key from keyboard, handling escape properly. */
int getch (void) {
  int ch;

  if (tcgetpgrp(0) == -1) return 033; /* Detect if the terminal went away. */
  
  while (SLang_input_pending(1000) == 0)
    continue;

  ch = SLang_getkey();

  if (ch == 033) {	/* escape */
    if (SLang_input_pending(TIMEOUT) == 0) {
      return 033;
    }
    else {
      /* If two or more escapes are queued up, assume the first is a
       * "real" escape, and not part of some escape sequance. */
      int ch2 = SLang_getkey();
      SLang_ungetkey(ch2);
      if (ch2 == 033) {
	return 033;
      }
    }
  }

  SLang_ungetkey(ch);
  return SLkp_getkey();
}
