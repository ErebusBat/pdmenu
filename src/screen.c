/*
 * Screen functions for use with slang. 
 * Draw dialogs, init and reset terminal, etc 
 */

/*
 * Copyright (c) 1995-1999 Joey Hess (joey@kitenet.net)
 * All rights reserved. See COPYING for full copyright information (GPL).
 */

#include "global.h"
#include "screen.h"
#include "menu.h"
#include "error.h"
#include <signal.h>
#include "slang.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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

/* Draw a shadow "under" a given rectangle.
 * Shadows are omitted in B&W mode.
 */
void DrawShadow (int x,int y,int dx,int dy) {
  int c;
  unsigned short ch;

  if (SLtt_Use_Ansi_Colors) {
    for (c=0;c<dy-1;c++) {
      SLsmg_gotorc(c+1+y,x+dx);
      /*
       * Note: 0x02 corresponds to the current color.  0x80FF gets the
       * character plus alternate character set attribute. -- JED 
       */
      ch = SLsmg_char_at();
      ch = (ch & 0x80FF) | (0x02 << 8);
      /*
       * Notice that we have to subtract 1 from the color specification, for 
       * some reason.
       */
      SLsmg_write_raw (&ch,SHADOW - 1);
      SLsmg_gotorc(c+1+y,x+dx+1);
      ch = SLsmg_char_at();
      ch = (ch & 0x80FF) | (0x02 << 8);
      SLsmg_write_raw (&ch,SHADOW - 1);
    }
    for (c=0;c<dx;c++) {
      SLsmg_gotorc(y+dy,x+2+c);
      ch = SLsmg_char_at();
      ch = (ch & 0x80FF) | (0x02 << 8);
      SLsmg_write_raw (&ch,SHADOW - 1);
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
  int fd;

  if (screen_is_setup == 0) {
    /*
     * Make sure that we have /dev/tty open as stdin. 
     * You see, if a rc file is passed to pdmenu on stdin, then
     * after the rc file is read, there is no longer stdin available
     * for programs pdmenu launches to use.
     */
    fd = open("/dev/tty", O_RDWR);
    dup2(fd, 0);
    close(fd);

    SLsig_block_signals(); /* Block signals while initializing. */

    SLtt_get_terminfo();

    /* note this has to run immediatly after SLtt_get_terminfo to work */
    if (Lowbit)
      SLtt_Has_Alt_Charset = 0;

    if (SLkp_init_ran != 1) {
      if (SLkp_init() == -1) {
	SLsig_unblock_signals();
	Error(KP_INIT_ERROR);
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
    Error(SCREEN_TOOSMALL);
}
