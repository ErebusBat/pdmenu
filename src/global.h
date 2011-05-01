/*
 * All pdmenu code will want to include this file.
 */

/* 
 * Copyright (c) 1995-1999 Joey Hess (joey@kitenet.net)
 * All rights reserved. See COPYING for full copyright information (GPL).
 */

#include "config.h"
#include "menutype.h"
#include "windowtype.h"

/* Global variables */

/* Pointer to linked list of all windows on the screen */
Window_List_Type *CurrentWindow;
/* Pointer to linked list of all available menus. */
Menu_Type *menus;
/* Pointer to the menu that will be modified by new rc commands. */
Menu_Type *current_rc_menu;

/* Boolean variables, 0=no, 1=yes */
int Use_Color;
int Q_Exits;
int Unpark_Cursor;
#ifdef GPM_SUPPORT
/* Was the mouse detected? */
int gpm_ok;
#endif
/* Set when the screen has resized */
int Want_Screen_Resize;
/* Use old-style look if set. */
int Retro;
/* Don't use high bit line drawing chars. */
int Lowbit;
/* Don't use 2 or 8 for up and down if set */
int Numeric;
/* Use superhot hot keys */
int Superhot;
