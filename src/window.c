/* Handles a set of windows on screen. */

/*
 * Copyright (c) 1995, 1996, 1997 Joey Hess (joey@kite.ml.org)
 * All rights reserved. See COPYING for full copyright information (GPL).
 */

#include "global.h"
#include "screen.h"
#include "menu.h"
#include "mouse.h"
#include "slang.h"
#include "window.h"
#include "actions.h"
#include "rc.h"
#include <stdlib.h>
#include <strings.h>
#include <string.h>

/* Draw the whole screen, with menus on it. */
void DrawAll () {
	int c;

	DrawTitle(DEFAULTTITLE);
	DrawDesktop();

	if (strlen(OnScreen[NumOnScreen]->helptext)>0)
		DrawBase(OnScreen[NumOnScreen]->helptext);
	else
		DrawBase(DEFAULTBASE);

	for (c=0;c<=NumOnScreen-1;c++) {
		DrawMenu(OnScreen[c],0);
	}
	DrawMenu(OnScreen[NumOnScreen],1);

 	SLsmg_refresh(); 
}

/* Force a redraw of the screen. Clear screen, then redraw everything. */
void Force_Redraw () {
	Screen_Reset();
	Screen_Init(); 
	DrawAll();
}

/*
 * Call this whenever the screen size changes. It repositions all the windows
 * on the screen to fit the new screen.
 * You will typically wany to call Force_Redraw() after this function.
 */
void Resize_Screen () {
	int c;
	Menu_Type *m;

	Want_Screen_Resize = 0;
	SetScreensize();
	for (c=0;c<=NumMenus;c++) {
		m=menus[c];
		m->recalc=1;
		CalcMenu(m);
	}
}                       

/* Add a window to the group onscreen, displaying the passed menu. */
void AddWindow (Menu_Type *m) {
	if (NumOnScreen==MAX_WINDOWS) {
#ifdef GPM_SUPPORT
		EndMouse();
#endif
		Screen_Reset();
		fprintf(stderr,TOO_MANY_WINDOWS,MAX_WINDOWS);
		exit(1);
	}
	OnScreen[++NumOnScreen]=m;
}

/* Remove the topmost window. Returns the number of the menu in array. */
int RemoveWindow () {
	return --NumOnScreen;
}

/* Exit the current window. Returns pointer to new current menu. */
Menu_Type *ExitWindow (Menu_Type* m) {
	if (NumOnScreen>0) { /* Pop down current menu, back to parent */
		m=menus[RemoveWindow()];
		DrawAll();
		return m;
	}
	else
		return m; /* Can;t exit menu if there are no others up. */
}                                                                       

/*
 * Free up memory for a menu, and also remove it from the menus[] array. 
 * Pass the name of the menu. 
 */
void RemoveMenuByTitle (char *menuid) {
	int m;
	
	/* Find the menu that we were asked to remove. */
	for(m=0;m<NumMenus;m++) {
		if (strcmp(menus[m]->name,menuid)==0) {
			RemoveMenu(menus[m]);

			/*
			 * Is this safe? Only if we *always* refer to menus by pointers, not
			 * by array index. Hope we do...
			 */
			menus[m]=menus[--NumMenus];

			break;
		}	
	}	  
}

/*
 * Display a message in a window.
 * Returns the index of the selected item when the window is exited. 
 * (-1 is returned if they hit q or ESC)
 */
signed int ShowMessage(char *title,char *helptext,char *message[],int arraysize) {
	Menu_Type *m;
	int c=0, ret=-1;

	/* allocate memory for the window */
	menus[NumMenus]=malloc(sizeof(Menu_Type));            
	m=menus[NumMenus++];

	/* load up the menu with the appropriate values */
	strncpy(m->title,title,TITLELEN);
	m->selected=0;
	for (c=0;c<arraysize;c++) {
		m->items[c]=malloc(sizeof(Menu_Item_Type));
		strncpy(m->items[c]->text,message[c],MENU_ITEM_TEXTSIZE);
		m->items[c]->hotkey=-1;
		m->items[c]->next=NULL;
	}
	m->num=arraysize;
	m->recalc=1;

	/* display the menu until they hit q or exit */
	AddWindow(m);
	DrawAll();
	c=DoMenu(m,NullAction,Handle_Ctrl_C);
	if (c==QUIT_EXIT)
		ret=m->selected;	
	RemoveWindow();
	DrawAll();
	RemoveMenu(m);
	NumMenus--;

	return ret;
}
