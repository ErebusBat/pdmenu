/* 
 * Copyright (c) 1995, 1996, 1997 Joey Hess (joey@kitenet.net)
 * All rights reserved. See COPYING for full copyright information (GPL).
 */

/* This must be long enough for "selbarhi". */
char ScreenObjNames[NUMSCREENPARTS+1][LONGEST_OBJ_NAME+1]; 
int DESKTOP;
int TITLE;
int BASE;
int MENU;
int SELBAR;
int SHADOW;
int MENU_HI;
int SELBAR_HI;
int UNSEL_MENU;

void DrawMenu(Menu_Type *,int);
void SelMenuItem(Menu_Type *,int,int);
void FillMenu(Menu_Type *,int);
int DoMenu(Menu_Type *,int (),void ());
void CalcMenu(Menu_Type *);
Menu_Type *LookupMenu (const char *);
void SanityCheckMenus(void);
void RemoveMenu(Menu_Type *);

#define Q_KEY_EXIT 2
#define QUIT_EXIT 3
