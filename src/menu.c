/* 
 * Copyright (c) 1995, 1996, 1997 Joey Hess (joey@kite.ml.org)
 * All rights reserved. See COPYING for full copyright information (GPL).
 */

#include "global.h"
#include <math.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "slang.h"

/* Highlight the hotkey of a given menu item, if there is a hotkey. */
void HighlightHotkey (Menu_Type *m,int item,int highlight_color) {
	if (m->items[item].hotkey!=-1) {
		SLsmg_set_color(highlight_color);
		SLsmg_gotorc(m->y+item+1-m->offset,m->x+2+m->items[item].hotkey);
		SLsmg_write_char(m->items[item].text[m->items[item].hotkey]);
	}
}

/* Select a particular item in a menu. 
 * Pass the object color to change it to as the second parameter. */
void SelMenuItem (Menu_Type *m,int object_color,int object_hi_color) {
		SLsmg_set_color(object_color);
		SLsmg_gotorc(m->y+1+m->selected-m->offset,m->x+1);
		SLsmg_write_char(' ');
		SLsmg_write_nstring(m->items[m->selected].text,m->dx-4);
		SLsmg_write_char(' ');
		HighlightHotkey(m,m->selected,object_hi_color);
		SLsmg_gotorc(SLtt_Screen_Rows-1,SLtt_Screen_Cols-1);
}

/* Fill in the entries in a menu. Set the color first. 
 * Calls SelMenuItem to select the menu item, as well. */
void FillMenu (Menu_Type *m) {
	int c;
	
	for(c=m->offset;c<m->num && c<m->dy-2+m->offset;c++) {
		SLsmg_set_color(MENU);
		SLsmg_gotorc(m->y+c+1-m->offset,m->x+2);

		SLsmg_write_nstring(m->items[c].text,m->dx-4);
		SLsmg_write_char(' ');
		HighlightHotkey(m,c,MENU_HI);
	}
  SelMenuItem(m,SELBAR,SELBAR_HI);
}

/* Calculate the dimentions of a menu so it fits onscreen.
 *
 * Note that since tabs in menus are BAD thing, and this must be called
 * before a menu is displayed, we also expand tabs out to spaces in this 
 * function. We also get rid of any nasty control characters in the menu.
 */
void CalcMenu (Menu_Type *m) {
	int i,j,k,c,dx,dy;
	char s[MENU_ITEM_TEXTSIZE*TABSIZE+2];

	/* NOTE: if we are in B&W mode (SLtt_Use_Ansi_Colors==0), then we don't
	   need to allocate space for shadows. */
	
	if (m->recalc==1) {
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
			for (i=0;i<strlen(m->items[c].text);i++) { /* Expand tab to spaces */
				if (m->items[c].text[i] == '\t') { /* Fill in spaces to the next tab boundry. */
					strncpy(s,m->items[c].text,i);
					j=(rint(i/TABSIZE)+1)*TABSIZE-i-1;
					for (k=0;k<=j;k++)
						s[i+k]=' ';
					for (k=i+1;k<=strlen(m->items[c].text);k++)
						s[k+j]=m->items[c].text[k];
					strcpy(m->items[c].text,s);
				}
				else if ((m->items[c].text[i] < 32) || (m->items[c].text[i] > 126)) { /* get rid of control characters */
					m->items[c].text[i]=' ';
				}
			}
			if (dx<strlen(m->items[c].text)+4)
				dx=strlen(m->items[c].text)+4;
		}
		if (dx>SLtt_Screen_Cols-SLtt_Use_Ansi_Colors)
			dx=SLtt_Screen_Cols-SLtt_Use_Ansi_Colors;

		m->dx=dx;
		m->dy=dy;
		m->x=(SLtt_Screen_Cols-dx)/2;
		m->y=(SLtt_Screen_Rows-dy)/2;
	}
}

/* Draw a menu */
void DrawMenu (Menu_Type *m) {
	SLsmg_set_color(MENU);
	CalcMenu(m);
	DrawDialog(m->title,m->x,m->y,m->dx,m->dy,1); 
	FillMenu(m);

}

/* These are all helper functions for DoMenu() */

void DoMenu_Up (Menu_Type *m) {
	int scrolled=0;

	SelMenuItem(m,MENU,MENU_HI);
	/* Todo: rewrite without the loop. */
	do {
		if (--m->selected < 0)
			m->selected=m->num-1;
			
		if ((m->mustscroll==1) && ((m->selected==m->num-1) ||
			                         (m->selected < m->offset))) {
			if (m->selected==m->num-1)
				m->offset=m->num - m->dy+2;
			else
				m->offset--;
			scrolled=1;
		}
	} while (m->items[m->selected].type == MENU_NOP);
	
	if (scrolled) {
		FillMenu(m);
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
		if ((m->mustscroll==1) && ((m->selected==0) || 
				 (m->selected-m->offset > m->dy-3))) {
			if (m->selected==0)
				m->offset=0;
			else
				m->offset++;
			scrolled=1;
		}
	} while (m->items[m->selected].type == MENU_NOP);

	if (scrolled) {
		FillMenu(m);
	}
	else {
		SLsmg_refresh();
		SelMenuItem(m,SELBAR,SELBAR_HI);
	}
	SLsmg_refresh();
}

void DoMenu_PageUp(Menu_Type *m) {
	SelMenuItem(m,MENU,MENU_HI);
	if (m->mustscroll==1) {
		m->selected=m->selected - m->dy+2;
		if (m->selected<0) 
			m->selected=0;
		while (m->items[m->selected].type == MENU_NOP)
			m->selected--;
		m->offset=m->selected;
		FillMenu(m);				
	}
	else {
		m->selected=0;
		SelMenuItem(m,SELBAR,SELBAR_HI);
	}
	SLsmg_refresh();
}

void DoMenu_PageDown (Menu_Type *m) {
	SelMenuItem(m,MENU,MENU_HI);
	if (m->mustscroll==1) {
		m->selected=m->selected + m->dy-2;
		if (m->selected>m->num-1)
			m->selected=m->num-1;
		while (m->items[m->selected].type == MENU_NOP)
			m->selected++;
		m->offset=m->selected;
		if (m->offset > m->num - m->dy+2)
			m->offset=m->num - m->dy+2;
		FillMenu(m);					
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
	FillMenu(m);
	SLsmg_refresh();
}

void DoMenu_End (Menu_Type *m) {
	SelMenuItem(m,MENU,MENU_HI);
	m->selected=m->num-1;
	m->offset=m->selected;
	if (m->offset > m->num -m->dy +2)
		m->offset=m->num - m->dy +2;
	FillMenu(m);
	SLsmg_refresh();
}

/* 
 * Pass this a menu and a key and it will try to move to an item on the
 # menu that has the key for a hotkey.
 * Returns 1 if it moves to the hotkey ok, 0 otherwise
 */
int SelHotKey(Menu_Type *m, int key) {
	int c=m->selected+1;

	do {
		if ((key==m->items[c].text[m->items[c].hotkey]) ||
		    (toupper(key)==m->items[c].text[m->items[c].hotkey])) {
			SelMenuItem(m,MENU,MENU_HI);
			m->selected=c;
			if ((m->mustscroll==1) && ((c > m->offset+m->dy-3) ||
			    (c < m->offset))) { /* have to set the correct offset to it */
				m->offset=c;
				if (m->offset > m->num - m->dy +2)
					m->offset=m->num - m->dy +2;
				FillMenu(m);
			}
			else {
				SelMenuItem(m,SELBAR,SELBAR_HI);
			}
			SLsmg_refresh();
			return 1;
		}
		c++;
		if (c > m->num)
			c=0;
	} while (c!=m->selected+1);

	return 0;
}

/* Handle input in a menu.
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
		if (gpm_ok == 1) {
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
					case MOUSE_BUTTON_RIGHT: /* quit out of the menu as if 'q' were pressed */
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

		/* Has the screen been resized lately? */
		if (Want_Screen_Resize == 1) {
			Resize_Screen();
			Force_Redraw();
		}

		switch (key) {
			case '-':
			case '8':
			case SL_KEY_UP:
				DoMenu_Up(m);
				break;
			case '+':
			case '=': /* no need to shift for + */
			case '2':
			case SL_KEY_DOWN:
				DoMenu_Down(m);
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
				c=MenuAction(m);
				if (c!=0)
					return c;
			case 65535:
				break;
			default:
				c=SelHotKey(m,key);
				if (c!=0)
					break;
				/* No hotkeys match, so use q or ESC to close the menu. */
				if ((key=='q') || (key=='Q') || (key==27))
					return Q_KEY_EXIT;
				/* They hit a bad key. Display help. */
				DrawBase(HELPBASE);
				SLsmg_refresh();
		}
	}
}
