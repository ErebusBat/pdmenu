/*
 * Copyright (c) 1995, 1996, 1997 Joey Hess (joey@kite.ml.org)
 * All rights reserved. See COPYING for full copyright information (GPL).
 */
   
#ifndef NUMSCREENPARTS
#define NUMSCREENPARTS 8
#endif

/* must be long enough for "brightmagenta", our longest color name */
#define COLORLEN 13
char FG[NUMSCREENPARTS][COLORLEN];
char BG[NUMSCREENPARTS][COLORLEN];

extern void DrawTitle(char *);
extern void DrawDesktop(void);
extern void DrawBase(char *);
extern void DrawDialog(char *,int,int,int,int,int);
extern void Screen_Reset(void);
extern void Screen_Init(void);
extern void Screen_Setcolors(void);
extern void SetScreensize(void);
