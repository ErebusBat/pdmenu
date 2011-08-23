/*
 * Copyright (c) 1995-1999 Joey Hess (joey@kitenet.net)
 * All rights reserved. See COPYING for full copyright information (GPL).
 */

char ReadRc(char *,int);

#define RC_FILE 1
#define RC_PREPROC 2

/* 
 * This is my guess at the average size of a pdmenu menu. Space for menu 
 * items is malloc()'d in increments of this many items at a time.
 * 25 seems reasonable, since it's the normal screen size. However, tuning 
 * this might be a way to speed up pdmenu.
 */
#define MENU_INC 25
