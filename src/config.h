/*
 * General config 
 */

/* 
 * Copyright (c) 1995, 1996, 1997 Joey Hess (joey@kite.ml.org)
 * All rights reserved. See COPYING for full copyright information (GPL).
 */

#ifdef HAVE_PUTENV
/* Comment out the following line if you don't want pdmenu to support the 
   setenv flag ('s') to exec. This will make pdmenu a bit smaller. */
#define PUTENV_FLAG_OK
#endif

/* If there is a tab in a window, how many spaces will it expand to? */
#define TABSIZE 8  

/*
 * Here are some more files you can edit to further configure pdmenu.
 * You can safely leave these files alone.
 */ 

/* Edit this file to change pdmenu's limits. */
#include "limits.h"

/* Edit this file to change keywords used in the rc file. */
#include "rc_config.h"

/* Edit this file to change the default colors pdmenu will use. */
#include "colors.h"

/* Edit this file to change the strings that are built into pdmenu. */
#include "strings.h"
