/* Screen functions for use with slang. 
 * Draw dialogs, init and reset terminal, etc 
 */

/*
 * Copyright (c) 1995, 1996, 1997 Joey Hess (joey@kite.ml.org)
 * All rights reserved. See COPYING for full copyright information (GPL).
 */

#include "global.h"
#include <signal.h>
#include <stdio.h>
#include "slang.h"

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

/* Draw a shadow "under" a given rectangle
 * Shadows are omintted in B&W mode 
 */
void DrawShadow (int x,int y,int dx,int dy) {
	int c;
	char *empty="";

	if (SLtt_Use_Ansi_Colors==1) {
		SLsmg_set_color(SHADOW);
		for (c=0;c<dy-1;c++) {
			SLsmg_gotorc(c+1+y,x+dx);
			SLsmg_write_char(' ');
			SLsmg_write_char(' ');
		}
		SLsmg_gotorc(y+dy,x+2);
		SLsmg_write_nstring(empty,dx);
	}
}

/* Draw a dialog box on the screen. Doesn't save whatever's under it; sorry.
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

	if (shadow==1) DrawShadow(x,y,dx,dy);
}

/* 
 * SIGWINCH=resize window.
 * This just sets a global variable because the SL* functions that
 * are used to redraw the screen arn't reentrant.
 */
RETSIGTYPE Sigwinch_Handler (int sig) {
	Want_Screen_Resize = 1;
	signal(SIGWINCH,Sigwinch_Handler);
}

/* init terminal
 * turn on color if the global Use_Color is = 1
 */
void Screen_Init () {
	static int SLkp_init_ran;

	SLsig_block_signals(); /* Block signals while initializing. */

	SLtt_get_terminfo();
	if (SLkp_init_ran != 1) {
		if (SLkp_init() == -1) {
			SLsig_unblock_signals();
			fprintf(stderr,KP_INIT_ERROR);
			exit(-1);
		}
		SLkp_init_ran=1;
	}

	SLang_init_tty(0, 0, 1);
	SLsmg_init_smg();
	SLtt_Use_Ansi_Colors=Use_Color;

	SLsignal(SIGWINCH,Sigwinch_Handler);

	SLsig_unblock_signals();
}

/* set object colors
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
	SLsmg_gotorc(SLtt_Screen_Rows - 1, 0);
	SLsmg_refresh();
	SLang_reset_tty ();
	SLsmg_reset_smg ();
}	

/* figure out the current screen size */
void SetScreensize () {
	SLtt_get_screen_size();

	if ((SLtt_Screen_Rows < MIN_ROWS) || (SLtt_Screen_Cols < MIN_COLS)) {
#ifdef GPM_SUPPORT
		EndMouse();
#endif
		Screen_Reset();
  	fprintf(stderr,SCREEN_TOOSMALL);
		exit(2);
	}
}
