/*
 * All pdmenu code will want to include this file.
 */

/* 
 * Copyright (c) 1995, 1996, 1997 Joey Hess (joey@kite.ml.org)
 * All rights reserved. See COPYING for full copyright information (GPL).
 */

#include "config.h"
#include "pdmenu.h"
#include "menutype.h"
#include "screen.h"
#include "menu.h"
#include "rc.h"
#include "inputbox.h"
#include "keyboard.h"
#include "mouse.h"

/* Global variable */
Menu_Type *OnScreen[MAX_WINDOWS];	/* Pointers to windows on the screen */
signed int NumOnScreen;           /* Number of windows currently onscreen */
Menu_Type menus[MAX_NUM_MENUS];   /* Holds all info about each menu */
int NumMenus;                     /* Number of menus that have been loaded from menufiles */
int Use_Color;                    /* 0=no color, 1=color */
int Q_Exits;                      /* 0=no, 1=yes */
#ifdef GPM_SUPPORT
int gpm_ok;                       /* 0=no mouse detected, 1=mouse detected */
#endif
int Want_Screen_Resize;						/* 1=the window has been resized */
