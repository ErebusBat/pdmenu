/* Handles a set of windows on screen. */

/*
 * Copyright (c) 1995-2002 Joey Hess (joey@kitenet.net)
 * All rights reserved. See COPYING for full copyright information (GPL).
 */

#include "global.h"
#include "screen.h"
#include "menu.h"
#include "slang.h"
#include "window.h"
#include "actions.h"
#include "rc.h"
#include "error.h"
#include <stdlib.h>
#include <string.h>
#include <libintl.h>
#define _(String) gettext (String)

/*
 * This is a pointer to the first window that pdmenu pops up.
 * This is only used by DrawAll, where we need to draw the screen starting
 * with the first menu, and then onwards. Technically, we don't need this, 
 * but I'd prefer not to have to walk back through the whole linked list of 
 * windows to find the first one each time DrawAll is called.
 */
Window_List_Type *FirstWindow;

/* Draw the whole screen, with menus on it. */
void DrawAll () {
  static char *greeting = NULL;
  Window_List_Type *this_window=FirstWindow;

  if (!greeting) {
    greeting = malloc(256);
    sprintf(greeting, _("Welcome to Pdmenu %s by Joey Hess <joey@kitenet.net>"), VER);
  }
   
  DrawTitle(ScreenTitle);
  DrawDesktop();

  if (strlen(CurrentWindow->menu->helptext)>0)
    DrawBase(CurrentWindow->menu->helptext);
  else
    DrawBase(greeting);
  
  /* Follow the linked list, but do not process the last element. */
  while (this_window->next) {
    DrawMenu(this_window->menu,0);
    this_window=this_window->next;
  }
  /* Now draw the last element differently. */
  DrawMenu(this_window->menu,1);

  SLsmg_refresh(); 
}

/* Force a redraw of the screen. Clear screen, then redraw everything. */
void Force_Redraw () {
  Screen_Reset();
  Screen_Init();
  DrawAll();
}

/*
 * Pass it a menu, and it will return 1 if it is visible on the screen, 
 * 0 if not 
 */
int IsVisible (Menu_Type *m) {
  Window_List_Type *this_window=CurrentWindow;
  while (this_window) {
    if (this_window->menu == m)
      return 1;
    this_window=this_window->last;
  }
  return 0;
}

/*
 * Call this whenever the screen size changes. It repositions all the windows
 * on the screen to fit the new screen.
 * You will typically want to call Force_Redraw() after this function.
 */
void Resize_Screen () {
  Window_List_Type *this_window=CurrentWindow;

  Want_Screen_Resize = 0;
  SetScreensize();
  while (this_window) {
    this_window->menu->recalc=1;
    CalcMenu(this_window->menu);
    this_window=this_window->last;
  }
}

/* Add a window to the group onscreen. */
void AddWindow (Menu_Type *m) {
  if (CurrentWindow) {
    CurrentWindow->next=malloc(sizeof(Window_List_Type));
    CurrentWindow->next->last=CurrentWindow;
    CurrentWindow=CurrentWindow->next;
  }
  else {
    /* This is the first window we've added. */
    CurrentWindow=malloc(sizeof(Window_List_Type));
    FirstWindow=CurrentWindow;
    CurrentWindow->last=NULL;
  }
  CurrentWindow->next=NULL;
  CurrentWindow->menu=m;
}

/* Exit and remove the current window. */
void ExitWindow () {
  Window_List_Type *w=CurrentWindow;

  /* Don't remove the very last window. */
  if (CurrentWindow->last) {
    CurrentWindow=CurrentWindow->last;
    free(w);
    CurrentWindow->next=NULL;
    DrawAll();
  }
}
