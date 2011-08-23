/* 
 * Copyright (c) 1995-1999 Joey Hess (joey@kitenet.net)
 * All rights reserved. See COPYING for full copyright information (GPL).
 */

#include "global.h"
#include "screen.h"
#include "menu.h"
#include "window.h"
#include "mouse.h"
#include "keyboard.h"
#include "pdmenu.h"
#include "inputbox.h"
#include "pdstring.h"
#include <string.h>
#include <stdlib.h>
#include "slang.h"
#include <libintl.h>
#define _(String) gettext (String)

/* 
 * Draw the input box on screen.
 */
void DrawInputBox (char *title) {
  /* Make the previous topmost menu be redrawn in unselected menu color. */
  if (!Retro)
    DrawMenu(CurrentWindow->menu,0);
  /* Draw the input box. */
  SLsmg_set_color(MENU);
  DrawDialog(title,1,(SLtt_Screen_Rows-3)/2,SLtt_Screen_Cols-2,3,1);
  SLsmg_set_color(SELBAR);
}

/*
 * Display an input box with title and text, get input.
 * ret->ignore is set if the user hits escape.
 */
Conditional_String *DoInputBox (char *title, char *contents) {
  int curspos,key,retcount;
  Conditional_String *ret=malloc(sizeof(Conditional_String));

  retcount=SLtt_Screen_Cols;
  ret->value=malloc(retcount + 1);
  strcpy(ret->value,contents);

#ifdef GPM_SUPPORT
  if (gpm_ok) 
    EndMouse(); /* let gpm cut and paste work as normal in edit box */
#endif

  curspos=strlen(ret->value);

  DrawBase(_("Press Enter when done, or Esc to cancel"));
  DrawInputBox(title);
	
  while (1) {
    SLsmg_gotorc((SLtt_Screen_Rows-3)/2+1,3);
    SLsmg_write_nstring(ret->value,SLtt_Screen_Cols-6);
    SLsmg_gotorc((SLtt_Screen_Rows-3)/2+1,3+curspos);
    SLsmg_refresh();
    key=getch();
    switch (key) {
    case 0:
      if (Want_Screen_Resize) {
	Resize_Screen();
	/*
	 * If the screen got wider, I need to malloc more memory for 
	 * ret->value 
	 */
	if (retcount < SLtt_Screen_Cols) {
	  retcount=SLtt_Screen_Cols;
	  ret->value=realloc(ret->value,retcount + 1);
	}
				
	/* This isn't done too well -- there's some flicker. */
	Force_Redraw();
	DrawBase(_("Press Enter when done, or Esc to cancel"));
	DrawInputBox(title);
      }
      break;
    case '\n':
    case '\r':
      DrawAll();
      ret->value[curspos]='\0';
#ifdef GPM_SUPPORT
      /* Start handling our own mouse events again. */
      if (gpm_ok)
	gpm_ok=InitMouse();
#endif
      ret->ignore=0;
      return ret;
    case 12: /* ctrl-l */
    case 18: /* ctrl-r */
      Force_Redraw();
      DrawInputBox(title);
      break;
    case SL_KEY_BACKSPACE: 
      /* backspace in my xterm generates 127. :-( */
    case 127: /* ctrl-h */
      if (curspos>0)
	ret->value[--curspos]='\0';
      break;
    case 27: /* escape */
      DrawAll();
#ifdef GPM_SUPPORT
      /* Start handling our own mouse events again. */
      if (gpm_ok)
	gpm_ok=InitMouse();
#endif
      ret->ignore=1;
      return ret;
    default:
      if ((key>=32) && (key<127) && (curspos<SLtt_Screen_Cols -6)) {
	ret->value[curspos++]=key;
	ret->value[curspos]='\0';
      }
    }
  }
}

/*
 * Process the string, replacing ~title:text~ flags with input from the user.
 * ret->ignore is set if the user hits escape.
 */
Conditional_String *EditTags(char *s) {
  int tagtitlestart,tagvaluestart,i,old_i,ok,retcount=0;
  char *tagtitle, *input;
  Conditional_String *cs, *ret=malloc(sizeof(Conditional_String));
  ret->value=malloc(strlen(s) + 1);

  for (i=0;i<strlen(s);i++) {
    if ((s[i] == '~') && 
	(((i>0) && (s[i-1]!='\\')) || (i==0))) { /* Start of tag (?) */
      old_i=i;
      ok=0;
      tagtitlestart=i+1;
      for (i++;i<strlen(s);i++) {
	if ((s[i]==':') && (s[i-1]!='\\')) { /* End of tag title */
	  tagtitle=malloc(i - tagtitlestart + 1);
	  strncpy(tagtitle,(char *) s + tagtitlestart,i - tagtitlestart);
	  tagtitle[i - tagtitlestart] ='\0';
	  tagtitle = unescape(tagtitle, ':');
	  tagvaluestart=i+1;
	  for (i++;i<strlen(s);i++) {
	    if ((s[i]=='~') && (s[i-1]!='\\')) { /* End of tag */
	      ok=1;
	      input=malloc(i - tagvaluestart + 1);
	      strncpy(input,(char *) s + tagvaluestart,i - tagvaluestart);
	      input[i - tagvaluestart]='\0';
	      cs=DoInputBox(tagtitle, input);
	      free(input);
	      free(tagtitle);
	      if (cs->ignore) { /* user hit escape */
		free(cs->value);
		free(cs);
		ret->ignore=1;
		return ret;
	      }
	      /*
	       * This is safe because ret->value[retcount] = the
	       * beginning of this tag so was allocated.
	       */
	      ret->value[retcount]='\0';
	      retcount=retcount+strlen(cs->value);
	      /* FIXME: this is allocating too much */
	      ret->value=realloc(ret->value,retcount + strlen(s) + 1);
	      strcat(ret->value,cs->value);
	      free(cs->value);
	      free(cs);
	      break;
	    }
	  }
	  break;
	}
      }
      if (ok==0) 
	i=old_i; /* wasn't a tag, after all */
    }
    else 
      ret->value[retcount++]=s[i];
  }
  ret->value[retcount]='\0';
  ret->ignore=0;
  return ret;
}
