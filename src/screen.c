/*
 * Screen functions for use with slang. 
 * Draw dialogs, init and reset terminal, etc 
 */

/*
 * Copyright (c) 1995-2003 Joey Hess (joey@kitenet.net)
 * All rights reserved. See COPYING for full copyright information (GPL).
 */

#include "global.h"
#include "screen.h"
#include "menu.h"
#include "error.h"
#include <signal.h>
#include "slang.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libintl.h>
#define _(String) gettext (String)

/* equals 1 if the screen is set up in slang mode */
int screen_is_setup=0;

/* Draw the desktop; the background of the screen */
void DrawDesktop () {
  int x;

  SLsmg_set_color(DESKTOP);
  for (x=1;x<SLtt_Screen_Rows-1;x++) {
    SLsmg_gotorc(x,0);
    SLsmg_erase_eol();	
  }
}

/* Draw the title at the top of the screen (centered) */
void DrawTitle (char *title) {
  SLsmg_gotorc(0,0);
  SLsmg_set_color(TITLE);
  SLsmg_erase_eol();
  SLsmg_gotorc(0,(SLtt_Screen_Cols-strlen(title))/2);
  SLsmg_write_string(title);
}

/* Draw the message at the base of the screen */
void DrawBase (char *base) {
  SLsmg_gotorc(SLtt_Screen_Rows-1,0);
  SLsmg_set_color(BASE);
  SLsmg_write_nstring(base,SLtt_Screen_Cols);
  SLsmg_gotorc(SLtt_Screen_Rows-1,SLtt_Screen_Cols-1);
}

/* Convert the character at the given screen position to a shadow */
void _shadow_char (int x, int y) {
	SLsmg_set_color_in_region(SHADOW, y, x, 1, 1);
}

/*
 * Draw a shadow "under" a given rectangle.
 * Shadows are omitted in B&W mode.
 */
void DrawShadow (int x,int y,int dx,int dy) {
  int c;
  
  if (SLtt_Use_Ansi_Colors) {
    for (c=0;c<dy-1;c++) {
      _shadow_char(x+dx, c+1+y);
      _shadow_char(x+dx+1, c+1+y);
    }
    for (c=0;c<dx;c++) {
      _shadow_char(x+2+c, y+dy);
    }
  }
}

/*
 * Draw a dialog box on the screen. Doesn't save whatever's under it; sorry.
 * Pass 1 as shadow to enable a shadow
 * Set the color beforehand 
 */
void DrawDialog (char *title,int x,int y,int dx,int dy,int shadow) {
  int c;
  char *empty="";

  SLsmg_draw_box(y,x,dy,dx);
  for(c=y+1;c<y+dy-1;c++) {
    SLsmg_gotorc(c,x+1);
    SLsmg_write_nstring(empty,dx-2);
  }	
  SLsmg_gotorc(y,x+(dx-strlen(title))/2);
  SLsmg_write_string(title);

  if (shadow) 
    DrawShadow(x,y,dx,dy);
}

/* 
 * SIGWINCH=resize window.
 * This just sets a global variable because the SL* functions that
 * are used to redraw the screen arn't reentrant.
 */
RETSIGTYPE Sigwinch_Handler (int sig) {
  Want_Screen_Resize = 1;
  /* 
   * Ugly hack: push 0 into keyboard buffer, becuase pdmenu is often
   * waiting on keyboard input when the window is resized. Note that
   * this means that all the functions that do keyboard input need to check
   * for this and do a screen resize. There has to be a better way to do
   * this!
   */
  SLang_ungetkey(0);
  signal(SIGWINCH,Sigwinch_Handler);
}

/* init terminal
 * turn on color if the global Use_Color is = 1
 */
void Screen_Init () {
  static int SLkp_init_ran;
  static int got_terminfo;
  
  if (screen_is_setup == 0) {
    SLsig_block_signals(); /* Block signals while initializing. */

    if (got_terminfo != 1) {
 	   SLtt_get_terminfo();
	   got_terminfo = 1;
    }

    /* note this has to run immediatly after SLtt_get_terminfo to work */
    if (Lowbit)
      SLtt_Has_Alt_Charset = 0;

    if (SLkp_init_ran != 1) {
      if (SLkp_init() == -1) {
	SLsig_unblock_signals();
	Error(_("Unable to initialize key mappings."));
      }
      SLkp_init_ran=1;
    }

    SLang_init_tty(0, 0, 1);
    SLsmg_init_smg();
    SLtt_Use_Ansi_Colors=Use_Color;

    SLsignal(SIGWINCH,Sigwinch_Handler);
    
    SLsig_unblock_signals();

    screen_is_setup=1;
  }
}

/*
 * set object colors
 * This requres that the FG[] and BG[] arrays be filled with the strings
 * that denote the colors of the different objects.
 */
void Screen_Setcolors () {
  int c;
	
  for (c=0;c<NUMSCREENPARTS;c++) {
    SLtt_set_color(c+1,"",FG[c],BG[c]);
  }
}

/* reset terminal */
void Screen_Reset () {	
  if (screen_is_setup) {
    SLsmg_gotorc(SLtt_Screen_Rows - 1, 0);
    SLsmg_refresh();
    SLang_reset_tty ();
    SLsmg_reset_smg ();
    screen_is_setup=0;
  }
}	

/* figure out the current screen size */
void SetScreensize () {
  SLtt_get_screen_size();

  if ((SLtt_Screen_Rows < MIN_ROWS) || (SLtt_Screen_Cols < MIN_COLS))
    Error(_("Error: The screen is too small."));
}
