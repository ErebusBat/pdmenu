/*
 * Error routines
 */

/* 
 * Copyright (c) 1997-1999 by Joey Hess (joey@kitenet.net) and William Thompson
 * (wakko@kitenet.net)
 * All rights reserved. See COPYING for full copyright information (GPL).
 */

#include "global.h"
#include "mouse.h"
#include "error.h"
#include "screen.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <libintl.h>
#define _(String) gettext (String)

/* Display an error. The parameters work like printf() */
void Error(const char *fmt, ...) {
  va_list arglist;

#ifdef GPM_SUPPORT
  EndMouse();
#endif
  Screen_Reset();

  va_start(arglist, fmt);
  fprintf(stderr, _("pdmenu error: "));
  vfprintf(stderr, fmt, arglist);
  fprintf(stderr, "\n");
  va_end(arglist);

  exit(1);
}
