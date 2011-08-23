/*
 * Copyright (c) 1995-2001 Joey Hess (joey@kitenet.net)
 * All rights reserved. See COPYING for full copyright information (GPL).
 */

/* The number of parts that make up the screen and have different colors. */   
#define NUMSCREENPARTS 9

/* Minimum size of the screen where pdmenu still works */
#define MIN_ROWS 6
#define MIN_COLS 6

/* must be long enough for "brightmagenta", our longest color name */
#define COLORLEN 13
char FG[NUMSCREENPARTS][COLORLEN];
char BG[NUMSCREENPARTS][COLORLEN];

char *ScreenTitle;

void DrawTitle(char *);
void DrawDesktop(void);
void DrawBase(char *);
void DrawDialog(char *,int,int,int,int,int);
void Screen_Reset(void);
void Screen_Init(void);
void Screen_Setcolors(void);
void SetScreensize(void);
