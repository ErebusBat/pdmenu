/* 
 * Copyright (c) 1995, 1996, 1997 Joey Hess (joey@kitenet.net)
 * All rights reserved. See COPYING for full copyright information (GPL).
 */

#include "global.h"
#include "screen.h"
#include "menu.h"
#include "mouse.h"
#include "keyboard.h"
#include "window.h"
#include <math.h>
#include <string.h>
#include <ctype.h>
#include "slang.h"
#include <stdlib.h>
#include "error.h"
#include <libintl.h>
#define _(String) gettext (String)

/* Highlight the hotkey of a given menu item, if there is a hotkey. */
void HighlightHotkey (Menu_Type *m,int item,int highlight_color) {
  if (m->items[item]->hotkey!=-1) {
    SLsmg_set_color(highlight_color);
    SLsmg_gotorc(m->y+item+1-m->offset,m->x+2+m->items[item]->hotkey);
    SLsmg_write_char(m->items[item]->text[m->items[item]->hotkey]);
  }
}

/*
 * Select a particular item in a menu. 
 * Pass the object color to change it to as the second parameter. 
 */
void SelMenuItem (Menu_Type *m,int object_color,int object_hi_color) {
  SLsmg_set_color(object_color);
  SLsmg_gotorc(m->y+1+m->selected-m->offset,m->x+1);
  SLsmg_write_char(' ');
  SLsmg_write_nstring(m->items[m->selected]->text,m->dx-4);
  SLsmg_write_char(' ');
  HighlightHotkey(m,m->selected,object_hi_color);
  /*
   * Position cursor in bottom corner of screen, or to right of selected 
   * menu entry 
   */
  if (Unpark_Cursor == 0)
    SLsmg_gotorc(SLtt_Screen_Rows-1,SLtt_Screen_Cols-1);
  else
    SLsmg_gotorc(m->y+1+m->selected-m->offset,m->x+m->dx-1);
}

/*
 * Fill in the entries in a menu. Set the color first. 
 * Calls SelMenuItem to select the menu item, as well. 
 * Pass active = 1 if this menu is active 
 */
void FillMenu (Menu_Type *m,int active) {
  int c;
  for(c=m->offset;c<m->num && c<m->dy-2+m->offset;c++) {
    if (active || Retro)
      SLsmg_set_color(MENU);
    else
      SLsmg_set_color(UNSEL_MENU);
    SLsmg_gotorc(m->y+c+1-m->offset,m->x+2);

    SLsmg_write_nstring(m->items[c]->text,m->dx-4);
    SLsmg_write_char(' ');
    if (active || Retro)
      HighlightHotkey(m,c,MENU_HI);
  }
  if (active || Retro)
    SelMenuItem(m,SELBAR,SELBAR_HI);
}

/* Calculate the dimentions of a menu so it fits onscreen.
 *
 * Note that since tabs in menus are BAD thing, and this must be called
 * before a menu is displayed, we also expand tabs out to spaces in this 
 * function. We also get rid of any nasty control characters in the menu.
 */
void CalcMenu (Menu_Type *m) {
  int i,j,k,c,len,dx,dy;
  char *s;

  /*
   * If we are in B&W mode (SLtt_Use_Ansi_Colors==0), then we don't
   * need to allocate space for shadows. 
   */
	
  if (m->recalc) {
    m->recalc=0;
    dy=m->num+2;
    if (dy>SLtt_Screen_Rows-2-SLtt_Use_Ansi_Colors) {
      dy=SLtt_Screen_Rows-2-SLtt_Use_Ansi_Colors;
      m->mustscroll=1;
    }
    else {
      m->mustscroll=0;
    }
    m->offset=0;

    dx=strlen(m->title)+2;

    for(c=0;c<m->num;c++) {
      /*
       * Using strchr turns out to be 10 or 20 x faster than 
       * finding tabs by hand. 
       */
      s=m->items[c]->text;
      while ((s=strchr(s,'\t')) != NULL) {
	i=(s - m->items[c]->text);
	j=((i/SLsmg_Tab_Width)+1)*SLsmg_Tab_Width-i-1;
	if (j>0) {
	  len=strlen(m->items[c]->text);
	  /* Expand the string to allow space for the tab. */
	  m->items[c]->text=realloc(m->items[c]->text,len + j + 1);
	  s=m->items[c]->text + i;
	  /*
	   * Shift the contents of the string after the tab right 
	   * by j characters. 
	   */
	  for (k=len - i;k>0;k--) {
	    s[k+j]=s[k];
	  }
	}
				/* Replace the tab with j spaces. */
	for (k=0;k<=j;k++)
	  s[k]=' ';
      }
      if (dx<strlen(m->items[c]->text)+4)
	dx=strlen(m->items[c]->text)+4;
    }
    if (dx>SLtt_Screen_Cols-SLtt_Use_Ansi_Colors)
      dx=SLtt_Screen_Cols-SLtt_Use_Ansi_Colors;

    m->dx=dx;
    m->dy=dy;
    m->x=(SLtt_Screen_Cols-dx)/2;
    m->y=(SLtt_Screen_Rows-dy)/2;
  }
}

/* 
 * Draw a menu
 * Pass the menu to draw, and pass active = 1 if this menu is active.
 */
void DrawMenu (Menu_Type *m,int active) {
  if (active || Retro)
    SLsmg_set_color(MENU);
  else
    SLsmg_set_color(UNSEL_MENU);
  CalcMenu(m);
  DrawDialog(m->title,m->x,m->y,m->dx,m->dy,1); 
  FillMenu(m,active);
}

/* These are all helper functions for DoMenu() */

void DoMenu_Up (Menu_Type *m) {
  int scrolled=0;

  SelMenuItem(m,MENU,MENU_HI);
  /* Todo: rewrite without the loop. */
  do {
    if (--m->selected < 0)
      m->selected=m->num-1;
			
    if ((m->mustscroll) && ((m->selected==m->num-1) ||
			    (m->selected < m->offset))) {
      if (m->selected==m->num-1)
	m->offset=m->num - m->dy+2;
      else
	m->offset--;
      scrolled=1;
    }
  } while (m->items[m->selected]->type == MENU_NOP);
	
  if (scrolled) {
    FillMenu(m,1);
  }
  else {
    SLsmg_refresh(); /* looks faster on slow terminals */
    SelMenuItem(m,SELBAR,SELBAR_HI);
  }
  SLsmg_refresh();
}

void DoMenu_Down(Menu_Type *m) {
  int scrolled=0;

  SelMenuItem(m,MENU,MENU_HI);
  /* Todo: rewrite without the loop. */
  do {
    if (++m->selected >= m->num)
      m->selected=0; 
    if ((m->mustscroll) && ((m->selected==0) || 
			    (m->selected-m->offset > m->dy-3))) {
      if (m->selected==0)
	m->offset=0;
      else
	m->offset++;
      scrolled=1;
    }
  } while (m->items[m->selected]->type == MENU_NOP);

  if (scrolled) {
    FillMenu(m,1);
  }
  else {
    SLsmg_refresh();
    SelMenuItem(m,SELBAR,SELBAR_HI);
  }
  SLsmg_refresh();
}

void DoMenu_PageUp(Menu_Type *m) {
  SelMenuItem(m,MENU,MENU_HI);
  if (m->mustscroll) {
    m->selected=m->selected - m->dy+2;
    if (m->selected<0) 
      m->selected=0;
    while (m->items[m->selected]->type == MENU_NOP)
      m->selected--;
    m->offset=m->selected;
    FillMenu(m,1);
  }
  else {
    m->selected=0;
    SelMenuItem(m,SELBAR,SELBAR_HI);
  }
  SLsmg_refresh();
}

void DoMenu_PageDown (Menu_Type *m) {
  SelMenuItem(m,MENU,MENU_HI);
  if (m->mustscroll) {
    m->selected=m->selected + m->dy-2;
    if (m->selected>m->num-1)
      m->selected=m->num-1;
    while (m->items[m->selected]->type == MENU_NOP)
      m->selected++;
    m->offset=m->selected;
    if (m->offset > m->num - m->dy+2)
      m->offset=m->num - m->dy+2;
    FillMenu(m,1);
  }
  else {
    m->selected=m->num-1;
    SelMenuItem(m,SELBAR,SELBAR_HI);
  }
  SLsmg_refresh();
}

void DoMenu_Home (Menu_Type *m) {
  SelMenuItem(m,MENU,MENU_HI);
  m->selected=0;
  m->offset=0;
  FillMenu(m,1);
  SLsmg_refresh();
}

void DoMenu_End (Menu_Type *m) {
  SelMenuItem(m,MENU,MENU_HI);
  m->selected=m->num-1;
  m->offset=m->selected;
  if (m->offset > m->num -m->dy +2)
    m->offset=m->num - m->dy +2;
  FillMenu(m,1);
  SLsmg_refresh();
}

/* 
 * Pass this a menu and a key and it will try to move to an item on the
 # menu that has the key for a hotkey.
 * Returns 1 if it moves to the hotkey ok, 0 otherwise
 */
int SelHotKey(Menu_Type *m, int key) {
  int c=m->selected+1;

  if (c >= m->num)
    c=0;

  do {
    if (
	(m->items[c]->hotkey != -1) &&
	(
	 (key==m->items[c]->text[m->items[c]->hotkey]) ||
	 (toupper(key)==m->items[c]->text[m->items[c]->hotkey])
	 )
	) {
      SelMenuItem(m,MENU,MENU_HI);
      m->selected=c;
      if ((m->mustscroll) && ((c > m->offset+m->dy-3) ||
			      (c < m->offset))) { 
	/* have to set the correct offset to it */
	m->offset=c;
	if (m->offset > m->num - m->dy +2)
	  m->offset=m->num - m->dy +2;
	FillMenu(m,1);
      }
      else {
	SelMenuItem(m,SELBAR,SELBAR_HI);
      }
      SLsmg_refresh();
      return 1;
    }
    c++;
    if (c >= m->num)
      c=0;
  } while (c!=m->selected);

  return 0;
}

int HotKeyMatches(Menu_Type *m, int key, int i) {
  return m->items[i]->hotkey != -1 &&
	    ( (key==m->items[i]->text[m->items[i]->hotkey]) || toupper(key)==m->items[i]->text[m->items[i]->hotkey] );
}

int UniqHotKeyMatches(Menu_Type *m, int key) {
   int i = 0;
   int n = m->num;
   int c = 0;

   if ( HotKeyMatches(m,key,m->selected) )
      for (i=0; i<n; i+=1)
	if ( HotKeyMatches(m,key,i) )
	  c += 1;
   return c == 1;
}

/* 
 * Handle input in a menu.
 * Pass it the menu to handle, and a pointer to the function to be called 
 * when Enter is pressed, and a pointer to a function to be called when 
 * ctrl-c is pressed
 * Returns Q_KEY_EXIT or QUIT_EXIT.
 *
 * The menu action function must take a pointer to a Menu_Type, and return 
 * one of the same values this returns, or 0 to continue with the menu.
 */
int DoMenu (Menu_Type *m,int (*MenuAction)(), void (*Ctrl_C_Action)()) {
  int key, c;
#ifdef GPM_SUPPORT
  int done;
#endif

  while (1) {
#ifdef GPM_SUPPORT
    /* Check for mouse events, break out when a key is hit. */
    if (gpm_ok) {
      done=0;
      do {
	switch(CheckMouse()) {
	case MOUSE_NOTHING: /*no mouse events, so we check the keyboard.. */
	  done=1;
	  break;
	case MOUSE_BUTTON_LEFT: /* run a command on the menu */
	  c=MenuAction(m);
	  if (c!=0)
	    return c;
	  break;
	case MOUSE_BUTTON_RIGHT: /* quit menu as if 'q' were pressed */
	  return Q_KEY_EXIT;
	case MOUSE_UP:
	  /* Don't scroll off top of menu with mouse */
	  if (m->selected > 0)
	    DoMenu_Up(m);
	  break;
	case MOUSE_DOWN:
	  /* Don't scroll off bottom of menu with mouse */
	  if (m->selected < m->num-1)
	    DoMenu_Down(m);
	  break;
	} /* switch */
      } while (!done);
    }
#endif

    /* Handle keypresses. */
    key=getch();

    switch (key) {
    case 0:
      if (Want_Screen_Resize) {
	Resize_Screen();
	Force_Redraw();
      }
      break;
    case '-':
    case '8':
    case SL_KEY_UP:
			if (key != '8' || ! Numeric)
	      DoMenu_Up(m);
			else
				SelHotKey(m,key);
			break;
    case '+':
    case '=': /* no need to shift for + */
    case '2':
    case SL_KEY_DOWN:
			if (key != '2' || ! Numeric)
	      DoMenu_Down(m);
			else
				SelHotKey(m,key);
      break;
    case ' ':
    case 22: /* ctrl-v */
    case SL_KEY_NPAGE:
      DoMenu_PageDown(m);
      break;
    case 21: /* ctrl-u */
    case SL_KEY_PPAGE:
      DoMenu_PageUp(m);
      break;
    case SL_KEY_HOME:
      DoMenu_Home(m);
      break;
    case SL_KEY_END:
      DoMenu_End(m);
      break;
    case 3: /* ctrl-c */
      Ctrl_C_Action(m);
      break;
    case 12: /* ctrl-l */
    case 18: /* ctrl-r */
      Force_Redraw();
      break;
    case '\n':
    case '\r':
    case SL_KEY_RIGHT:
      c=MenuAction(m);
      if (c!=0)
	return c;
      break;
    case 65535:
      break;
    default:
      c=SelHotKey(m,key);
      if ( Superhot && UniqHotKeyMatches(m,key) )
	return MenuAction(m);
      if (c!=0)
	break;
      /* No hotkeys match, so use q or ESC to close the menu. */
      if ((key=='q') || (key=='Q') || (key==27) || (key ==SL_KEY_LEFT))
	return Q_KEY_EXIT;
      /* They hit a bad key. Display help. */
      DrawBase(_("Arrow keys move, Esc exits current menu, Enter runs program"));
      SLsmg_refresh();
    }
  }
}

/*
 * Pass this a menu name, and it will return a pointer to the menu structure,
 * or NULL if it cannot find the menu.
 */
Menu_Type *LookupMenu (const char *menuname) {
  Menu_Type *m=menus;
 
  while (m) {
    if (strcasecmp(menuname,m->name) == 0)
      return m;
    m=m->last;
  }

  return NULL;
}

/*
 * Remove a menu from the menu list.
 */
void RemoveMenu(Menu_Type *m) {
  int i;
  Menu_Item_Type *next_item, *this_item;

  /* Check to see if this menu is currently visible onscreen */
  if (IsVisible(m))
    Error(_("Attempt to remove menu \"%s\" failed: menu is on screen."),m->name);

  /* Check to see if this is the menu new rc lines add to. */
  /*  if (current_rc_menu == m)
    current_rc_menu=NULL; */

  /* Remove all the items in the menu. */
  for(i=0;i < m->num;i++) {
    next_item=m->items[i]->next;
    while (next_item != NULL) { /* remove whole linked list */
      this_item=next_item;
      next_item=this_item->next;
      free(this_item->command);
      free(this_item->text);
      free(this_item);
    }
    free(m->items[i]->command);
    free(m->items[i]->text);
    free(m->items[i]);
  }

  /* Free other parts of the menu structure. */
  free(m->name);
  free(m->title);
  free(m->helptext);

  /* Remove the menu from the linked list. */
  if (m->last)
    m->last->next=m->next;
 if (m->next)
    m->next->last=m->last;
  else {
    /* 
     * No next menu means this is the last menu. So we need to change menus to
     * point to the menu before this one. Of course, if that menu is 
     * nonexistant, there are no menus at all left.
     */
    if (m->last)
      menus=m->last;
    else
      menus=NULL;
  }

  /* Finally, free the menu */
  free(m);
}

/* 
 * Examines all menus, and checks to see if any menu is all empty or
 * starts with a "nop" command, or ends with one. These are things that
 * make the menu display code very unhappy, so it fixes them.
 */
void SanityCheckMenus() {
  int i;
  Menu_Type *m=menus, *nextm;

  while (m) {
    /* Remove NOPs at end of menu. */
    while (m->num > 1 && m->items[m->num-1]->type == MENU_NOP) {
      free(m->items[m->num-1]);
      m->num--;
    }

    /* Remove NOPs at beginning of menu. */
    while (m->num > 0 && m->items[0]->type == MENU_NOP) {
      free(m->items[0]);
      for (i=1;i < m->num; i++)
	m->items[i-1]=m->items[i];
      m->num--;
    }

    /* Remove empty menus. */
    if (m->num == 0) {
      nextm=m->last;
      RemoveMenu(m);
      m=nextm;
    }
    else
      m=m->last;
  }
}
