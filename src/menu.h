/* 
 * Copyright (c) 1995, 1996, 1997 Joey Hess (joey@kite.ml.org)
 * All rights reserved. See COPYING for full copyright information (GPL).
 */

char ScreenObjNames[NUMSCREENPARTS+1][LONGEST_OBJ_NAME]; /* must be long enough for "selbarhi" */
int DESKTOP;
int TITLE;
int BASE;
int MENU;
int SELBAR;
int SHADOW;
int MENU_HI;
int SELBAR_HI;

extern void DrawMenu(Menu_Type *);
extern void SelMenuItem(Menu_Type *,int,int);
extern void FillMenu(Menu_Type *);
extern int DoMenu(Menu_Type *,int (),void ());
extern signed int ShowMessage(char *,char *,char *[],int);
extern void CalcMenu(Menu_Type *);

#define Q_KEY_EXIT 2
#define QUIT_EXIT 3
