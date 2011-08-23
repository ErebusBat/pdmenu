/* Handles various types of menu actions, running commands, etc. */

/*
 * Copyright (c) 1995-1999 Joey Hess (joey@kitenet.net)
 * All rights reserved. See COPYING for full copyright information (GPL).
 */

#include "global.h"
#include "rc.h"
#include "screen.h"
#include "menu.h"
#include "mouse.h"
#include "window.h"
#include "inputbox.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include "slang.h"
#include "actions.h"
#include <stdio.h>
#include "pdgetline.h"
#ifdef SETENV_FLAG_OK
#ifndef HAVE_SETENV
/* Pull in our own setenv code. */
#include "../contrib/getenv.c"
#include "../contrib/setenv.c"
#endif
#endif
#include <libintl.h>
#define _(String) gettext (String)

/* Handle a control c by either exiting pdmenu or doing nothing. */
void Handle_Ctrl_C() {
  if (Q_Exits) {
    Screen_Reset();
#ifdef GPM_SUPPORT
    EndMouse();
#endif
    exit(0);
  }
}

/* Handle a menu action by just quitting it */
int NullAction() {
  return QUIT_EXIT;
}

#ifdef SETENV_FLAG_OK
/*
 * Run a command, and examine stdout for var=val lines, and set the 
 * environment of this program appropriately.
 */
void RunSetenv (char *command) {
  FILE *pcommand;
  char *str, *val;

  /* Get the command output. */
  pcommand=popen(command,"r");
  str=pdgetline(pcommand,0);
  pclose(pcommand);

  if (!str)
    return;

  if (strlen(str) > 3 && strncmp(str,"PWD=",3)==0) {
    /* PWD is a special case */
    chdir(str+4);
    free(str);
  }
  else if ((strlen(str) > 1) && ((val=strchr(str, '=')) != NULL)) {
    *val='\0';
    val++;
    setenv(str,val,1);
    free(str);
  }
  else {
    free(str);
    /* Could display an error message here. */
  }
}
#endif /* SETENV_FLAG_OK */

/* 
 * Run a command, and display its output in a window.
 * If truncate==1, the output will not be wrapped.
 * This function is a horrible mess and I hope to do away with it one day.
 */
void RunShow (char *title, char *command, int truncate) {
  FILE *pcommand;
  char *realstr=NULL,*str=NULL;
  int i=0,j,k;

  /* Display wait text. */
  DrawBase(_("Please wait..."));
  SLsmg_refresh();

  /* allocate memory for the window */
  menus->next=malloc(sizeof(Menu_Type));
  menus->next->last=menus;
  menus->next->next=NULL;
  menus=menus->next;

  /* load up the menu with the appropriate values */
  menus->title=malloc(strlen(title)+1);
  strcpy(menus->title,title);
  menus->helptext=malloc(128);
  strcpy(menus->helptext,_("Press Esc to close window."));
  menus->name=malloc(1);
  strcpy(menus->name,"");
  menus->selected=0;
  menus->num_avail=MENU_INC;
  menus->items=malloc(sizeof(Menu_Item_Type *) * MENU_INC);

  /* Get the command output and create the menu items */
  pcommand=popen(command,"r");
  while ((str && strlen(str) > 0) || (str=realstr=pdgetline(pcommand,0))) {
    /* 
     * If we are not truncating, we have to handle wrapping lines that contain
     * tabs. This adds a lot of complexity and ugliness here. :-(
     */
    if (!truncate) {
      k=0;
      for (j=0;j<strlen(str);j++) {
	if (k == SLtt_Screen_Cols - 4 - 1) {
	  /*
	   * We have reached the end of the line before we reached the end
	   * of the string. Copy the processed part into the menu array,
	   * saving leftover part of string for later processing.
	   */
	  if (i >= menus->num_avail) { 
	    menus->num_avail+=MENU_INC;
	    menus->items=realloc(menus->items, sizeof(Menu_Item_Type *) *
				 menus->num_avail);
	  }
	  menus->items[i]=malloc(sizeof(Menu_Item_Type));
	  menus->items[i]->type=MENU_EXIT;
	  menus->items[i]->text=malloc(j+1);
	  strncpy(menus->items[i]->text,str,j);
	  menus->items[i]->text[j]='\0';
	  menus->items[i]->command=malloc(1);
	  strcpy(menus->items[i]->command,"");
	  menus->items[i]->hotkey=-1;
	  menus->items[i++]->next=NULL;
	  /* keep whittling away at the current string */
	  str=str+j;

	  /* Set j=0 to indicate we shouldn't add str to the list yet. */
	  j=0;
	  break;
	}
	/* Calculate length of string as it will appear on screen with tabs */
	if (str[j] == '\t') {
	  /* Figure out how many characters this tab will take up. */
	  k+=((j/SLsmg_Tab_Width)+1)*SLsmg_Tab_Width-j-1;
	}
	else
	  k++;
      } 

      /* If j == 0, this is a signal not to add str to the list yet. */
      if (j != 0) {
	if (i >= menus->num_avail) { 
	  menus->num_avail+=MENU_INC;
	  menus->items=realloc(menus->items, sizeof(Menu_Item_Type *) *
			       menus->num_avail);
	}
	menus->items[i]=malloc(sizeof(Menu_Item_Type));
	menus->items[i]->type=MENU_EXIT;
	menus->items[i]->text=malloc(j+1);
	strcpy(menus->items[i]->text,str);
	menus->items[i]->command=malloc(1);
	strcpy(menus->items[i]->command,"");
	menus->items[i]->hotkey=-1;
	menus->items[i++]->next=NULL;
	free(realstr);
	str=NULL;
      }
    }
    else {
      if (i >= menus->num_avail) { 
	menus->num_avail+=MENU_INC;
	menus->items=realloc(menus->items, sizeof(Menu_Item_Type *) *
			     menus->num_avail);
      }
      menus->items[i]=malloc(sizeof(Menu_Item_Type));
      menus->items[i]->type=MENU_EXIT;
      menus->items[i]->text=realstr;
      menus->items[i]->command=malloc(1);
      strcpy(menus->items[i]->command,"");
      menus->items[i]->hotkey=-1;
      menus->items[i++]->next=NULL;
      str=NULL;
    }
  }
  pclose(pcommand);

  menus->num=i;
  if (i==0) {
    DrawBase(_("Command returned no output"));
    SLsmg_refresh();
    RemoveMenu(menus);
  }
  else {
    menus->recalc=1;
    
    /* display the menu until they hit q or exit */
    AddWindow(menus);
    DrawAll();
    DoMenu(menus,NullAction,Handle_Ctrl_C);
    ExitWindow();
    RemoveMenu(menus);
  }
}

/* 
 * Change the help text
 */
void ChangeHelpText (Menu_Type *m, Menu_Item_Type *i) {
  FILE *pcommand;
  char *str;

  if (i->command_flag) {
    pcommand=popen(i->command,"r");
    str=pdgetline(pcommand,0);
    pclose(pcommand);
    m->helptext = str;
  } else {
    m->helptext = i->command;
  }

  DrawAll();

}

/* 
 * Run a command from the menus.
 */
void RunCommand (Menu_Item_Type *i) {
  char *command;
  int must_redraw=0;
  Conditional_String *cs=NULL;
  Window_List_Type *this_window=CurrentWindow;

  if (i->command[0] != '\0') { /* don't try to run a null command */

    if (i->edit_flag) { /* edit command on fly */
      cs=EditTags(i->command);
      if (cs->ignore) { /* user hit escape */
	free(cs->value);
	free(cs);
	return;
      }
      else { /* user hit enter */
	command=malloc(strlen(cs->value)+1);
	strcpy(command,cs->value);
	free(cs->value);
	free(cs);
      }
    }
    else { /* don't edit command on fly */
      command=malloc(strlen(i->command)+1);
      strcpy(command,i->command);
    }

#ifdef SETENV_FLAG_OK
    if (i->setenv_flag) { /* a setenv command */
      RunSetenv(command);
    }
    else
#endif
      if (i->makemenu_flag) { /* process command output as rc file */
	ReadRc(command,RC_PREPROC);
	SanityCheckMenus();
	/*
	 * make sure that all modified menus currently on screen
	 * get recalced.
	 */
	while (this_window) {
	  if (this_window->menu->recalc) {
	    CalcMenu(this_window->menu);
	    must_redraw=1;
	  }
	  this_window=this_window->last;
	}
	if (must_redraw)
	  DrawAll();
      }
      else if (i->truncate_flag) { /* display in a window and truncate */
	RunShow(i->text,command,1);
      }
      else if (i->display_flag) {	/* display in a window and wrap. */
	RunShow(i->text,command,0);
      }
      else { /* normal display */
	if (! i->noclear_flag) {
				/* clear screen */
	  SLsmg_cls();
	  SLsmg_normal_video();
	  Screen_Reset();
#ifdef GPM_SUPPORT
	  EndMouse(); /* return to normal GPM/selection mode */
#endif
	}

	/* 
	 * This is what the whole pdmenu program comes down to.
	 * The rest is fluff. ;-) 
	 */

	/*
	 * start: Steve Blott (smblott@gmail.com)
	 *
	 * add capability to exec() (rather than system()) a command, thereby
	 * replacing the current process;  if the first word of command is
	 * "exec", then exec() it, otherwise system() it
	 */

	char *cp = command;

        while (isspace(cp[0]))
	   cp++;

	if ( strncmp(cp, "exec", 4) == 0 && isspace(cp[4]) )
	{
	   char *cv[4]; /* command vector */
	   cv[0] = "sh";
	   cv[1] = "-c";
	   cv[2] = cp;
	   cv[3] = 0;
	   execvp(cv[0],cv);
	   /* should not reach here; if the execvp fails, then pdmenu will
	    * continue to run, silently ignoring the failure; if the execvp
	    * succeeds but the subsequent exec fails, then pdmenu will silently
	    * disappear (its process no longer exists), and no feedback will be
	    * received */
	}
	else
	   system(command);

	if (! i->noclear_flag) { /* redraw screen */
	  Screen_Init();
			
	  if (i->pause_flag) { /* pause 1st */
	    printf("\n%s",_("Press Enter to return to Pdmenu."));
	    fflush(stdout); /* make sure above is displayed. */
	    /* Now wait for the keypress. */
	    while (1) {
		    int k;
		    k = SLang_getkey();
		    if (k == '\n' || k == '\r') {
			    break;
		    }
	    }
	    SLang_flush_input(); /* kill any buffered input */
	    printf("\n");
	  }

#ifdef GPM_SUPPORT
	  gpm_ok=InitMouse(); /* grab mouse pointer again. */
#endif

	  /* 
	   * we need to account for the screen size changing behind our backs
	   * while the program was running.
	   */
	  SetScreensize();
	  Resize_Screen();
	  DrawAll();
	}
      }
    free(command);
  }
}

/* Display the submenu pointed to by the passed show: menu item. */
void ShowSubMenu (Menu_Item_Type *i) {
  int c;
  Menu_Type *m;

  /* find matching menu, if any */
  if ((m=LookupMenu(i->command)) != NULL) {
    /* got match -- show it and handle input for it. */
    AddWindow(m);
    DrawAll();
    do {
      c=DoMenu(m,Pdmenu_Action,Handle_Ctrl_C);
      if ((c==QUIT_EXIT) || (c==Q_KEY_EXIT))
        ExitWindow();
    } while (c==0);
  }
}

/* Remove a menu, given the name of the menu. */
void RemoveMenuByName (char *menuname) {
  Menu_Type *m;

  if ((m=LookupMenu(menuname)) != NULL) {
    RemoveMenu(m);
  }
}

/*
 * Run a menu item, return QUIT_EXIT if it is an exit item, or is a group
 * containing such an item.
 */
int RunItem (Menu_Type *m, Menu_Item_Type *i) {
  switch (i->type) {
  case MENU_EXEC:
    RunCommand(i);
    break;
  case MENU_SHOW:
    ShowSubMenu(i);
    break;
  case MENU_EXIT:
    return QUIT_EXIT;
  case MENU_REMOVE:
    RemoveMenuByName(i->command);
    break;
  case MENU_HELP_TEXT:
    ChangeHelpText(m, i);
    break;
  }

  if (i->next == NULL)
    return 0;
  else
    return RunItem(m, i->next); /* follow the linked list */		
}

/* This is called when an item is picked from a menu. */
int Pdmenu_Action (Menu_Type *m) {
  return RunItem(m, m->items[m->selected]);
}
