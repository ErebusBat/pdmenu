/* Handles various types of menu actions, running commands, etc. */

/*
 * Copyright (c) 1995, 1996, 1997 Joey Hess (joey@kite.ml.org)
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
#include <strings.h>
#include <math.h>
#include <unistd.h>
#include "slang.h"
#include "actions.h"

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

#ifdef PUTENV_FLAG_OK
/*
 * Run a command, and examine stdout for var=val lines, and set the 
 * environment of this program appropriately.
 */
void RunSetenv (char *command) {
	FILE *pcommand;
	static char str[MENU_ITEM_TEXTSIZE+1];
	char *tmp=NULL;

	/* Get the command output. */
	pcommand=popen(command,"r");
	while (fgets(str,MENU_ITEM_TEXTSIZE,pcommand)!=NULL) {}
	pclose(pcommand);

	/* Kill trailing \n */
	tmp=str;			
	while((*tmp != '\0') && (*tmp != '\n'))
		tmp++;
	*tmp='\0';

	if (strncmp(str,"PWD=",3)==0) /* PWD is a special case */
		chdir(str+4);
	else if ((strlen(str) > 1) && (strchr(str, '=') != NULL)) {
		putenv(str);
	}
}
#endif

/* 
 * Run a command, and display its output in a window.
 * If truncate==1, the output will not be wrapped.
 */
void RunShow (char *title, char *command, int truncate) {
	FILE *pcommand;
	char *message[MAX_ITEMS_IN_MENU+1];
	char str[MENU_ITEM_TEXTSIZE+2];
  char leftover[MENU_ITEM_TEXTSIZE+2];
	int i=0,j,k,longestline=0;

	/* Display wait text. */
	DrawBase(WAITTEXT);
	SLsmg_refresh();

  /* Figure out what's the longest line we can display in a menu. */
	if (SLtt_Screen_Cols-4 <= MENU_ITEM_TEXTSIZE)
		longestline=SLtt_Screen_Cols-4;
	else
		longestline=MENU_ITEM_TEXTSIZE;

	str[0]='\0';
	leftover[0]='\0';

	/* Get the command output */
	pcommand=popen(command,"r");
  /* Get a new line, or continue processing an old one. */
	while (strlen(str) > 0 || 
	       fgets(str,longestline - strlen(leftover),pcommand)!=NULL) {
		message[i]=(char *) malloc(MENU_ITEM_TEXTSIZE+1);
		if (i+1==MAX_ITEMS_IN_MENU)
			strncpy(str,TRUNCATED_STRING,MENU_ITEM_TEXTSIZE);

		/* 
		 * If we are not truncating, we have to handle wrapping lines that contain
		 * tabs. This adds a lot of complexity and ugliness here. :-(
		 */
		if (!truncate) {
			/* If there is some leftover string from before, add to new string. */
			if (strlen(leftover) > 0) {
				strcat(leftover,str);
				strcpy(str,leftover);
				leftover[0]='\0'; /* truncate */
			}

			k=0;
			for (j=0;j<strlen(str);j++) {
				if (k == longestline-1) {
					/*
					 * We have reached the end of the line before we reached the end
					 * of the string. Copy the processed part into the menu array,
					 * saving leftover part of string for later processing.
					 */
					strncpy(message[i],str,j-1);
					message[i++][j]='\0';
					/*
					 * Decide: should we keep breaking parts off the string, or read in
					 * more of the current line and add our string to the beginning?
					 * We do the latter only if we haven't read in the complete line yet.
					 */
					if (str[strlen(str)-1] != '\n') { /* read more of current line */
						strcpy(leftover,(char *)str + j-1); /* keep the leftovers */
						str[0]='\0'; /* truncate */
					}
					else { /* keep whittling away at the current string */
						for (k=0;k<j;k++)
							str[k]=str[j+k-1];
						str[k]='\0';
					}
				 
					/* Set j=0 to indicate we shouldn't add str to the list yet. */
					j=0;
					break;
				}
				/* Calculate length of string as it will appear on screen with tabs */
				if (str[j] == '\t') {
					/* Figure out how many characters this tab will take up. */
#ifdef HAVE_RINT
					k+=(rint(j/TABSIZE)+1)*TABSIZE-j-1;
#elif HAVE_FLOOR
					k+=(floor(j/TABSIZE)+1)*TABSIZE-j-1;
#endif
				}
				else {
					k++;
				}
			} 

			/* If j == 0, this is a signal not to add str to the list yet. */
			if (j != 0) {
				strcpy(message[i++],str);
				str[0]='\0';
			}
		}
		else {
			strcpy(message[i++],str);
		}
		
		if (i==MAX_ITEMS_IN_MENU)
			break;

		if (truncate && str[strlen(str)-1] != '\n') { /* Read in and discard rest of line. */
			do {
				j=getc(pcommand);
			}	while (j != '\n' && j != EOF);
		}
	}
	pclose(pcommand);
	if (i==0) {
		message[i]=(char *) malloc(MENU_ITEM_TEXTSIZE);
		strcpy(message[i++],NULL_OUTPUT);
	}

	/* Display it in a window */
	ShowMessage(title,DEFAULT_MESSAGE_HELP,message,i);

	for (j=0;j<i;j++) /* free the array */
		free(message[j]);
}

/* 
 * Run a command from the menus.
 */
void RunCommand (Menu_Item_Type *i) {
	char *command=(char *) malloc(MENU_ITEM_COMMANDSIZE+MENU_ITEM_COMMANDSIZE);
	int c, must_redraw=0;

	if (i->command[0] != '\0') { /* don't try to run a null command */
		strcpy(command,i->command);

		if (i->edit_flag)
			/* edit command on fly */
			if (EditTags(command) == 0) { /* user hit escape */
				free(command);
				return;
			}

#ifdef PUTENV_FLAG_OK
		if (i->setenv_flag) { /* a putenv command */
			RunSetenv(command);
		}
		else
#endif
		if (i->makemenu_flag) {
			/* process command output as rc file */
			ReadRc(command,RC_PREPROC);
			SanityCheckMenus();
			/* make sure that all modified menus currently on screen get recalced. */
			for (c=0;c<=NumOnScreen;c++) {
				if (OnScreen[c]->recalc) {
					CalcMenu(OnScreen[c]);
					must_redraw=1;
				}
			}
			if (must_redraw)
				DrawAll();
		}
		else if (i->truncate_flag) {
			/* display in a window and truncate */
			RunShow(i->text,command,1);
		}
		else if (i->display_flag) {
			/* display in a window and wrap. */
			RunShow(i->text,command,0);
		}
		else {
			/* normal display */
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
			system(command);
	
			if (! i->noclear_flag) {
				/* redraw screen */
				Screen_Init();
			
				if (i->pause_flag) { 
					/* pause 1st */
					printf(PRESS_ENTER_STRING);
					fflush(stdout); /* make sure above is displayed. */
					SLang_getkey();
					SLang_flush_input(); /* kill any buffered input */
					printf("\n");
				}

#ifdef GPM_SUPPORT
				gpm_ok=InitMouse(); /* grab mouse pointer again. */
#endif

				/* Has the screen been resized lately? */
				if (Want_Screen_Resize)
					Resize_Screen();
				DrawAll();
			}
		}
	}
	free(command);
}

/* Display the submenu pointed to by the passed show: menu item. */
void ShowSubMenu (Menu_Item_Type *i) {
	Menu_Type *m;
	int c;

	/* find matching menu, if any */
	for (c=0;c<NumMenus;c++) {
		if (strcasecmp(i->command,menus[c]->name)==0) {
			/* got match -- show it and handle input for it. */
			m=menus[c];
			AddWindow(m);
			DrawAll();
			do {
				c=DoMenu(m,Pdmenu_Action,Handle_Ctrl_C);
				if ((c==QUIT_EXIT) || (c==Q_KEY_EXIT))
					m=ExitWindow(m);
			} while (c==0);
			break;
		}
	}
}

/*
 * Run a menu item, return QUIT_EXIT if it is an exit item, or is a group
 * containing such an item
 */
int RunItem (Menu_Item_Type *i) {
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
			RemoveMenuByTitle(i->command);
			break;
	}

	if (i->next == NULL)
		return 0;
	else
		return RunItem(i->next); /* follow the linked list */		
}

/* This is called when an item is picked from a menu. */
int Pdmenu_Action (Menu_Type *m) {
	return RunItem(m->items[m->selected]);
}
