/* Rc file and data structure handling */

/*
 * Copyright (c) 1995-2001 Joey Hess (joey@kitenet.net)
 * All rights reserved. See COPYING for full copyright information (GPL).
 */

#include "global.h"
#include "rc.h"
#include "pdstring.h"
#include "screen.h"
#include "menu.h"
#include "error.h"
#include <stdio.h>
#include "pdgetline.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <libintl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define _(String) gettext (String)

/*
 * Passed a menu item and a character, set the flag on the menu item that
 * the character corresponds to, if any.
 * Returns the number of flags that were set.
 */
int SetFlags(Menu_Item_Type *i,char c) {
  switch(tolower(c)) {
  case NOCLEAR_FLAG:   
    return i->noclear_flag=1;
  case PAUSE_FLAG: 
    return i->pause_flag=1;
  case DISPLAY_FLAG:
    return i->display_flag=1;
  case EDIT_FLAG:
    return i->edit_flag=1;
  case SETENV_FLAG:
    return i->setenv_flag=1;
  case TRUNCATE_FLAG:
    return i->truncate_flag=1;
  case MAKEMENU_FLAG:
    return i->makemenu_flag=1;
  case COMMAND_FLAG:
    return i->command_flag=1;
  }
  return 0;
}

/* 
 * Read in a pdmenu style rc file. Preserves data from any other files that 
 * might have previously been read. 
 *
 * Returns 0 if it fails, 1 if it succeeds.
 *
 * type should be either RC_FILE (for normal files) or RC_PREPROC (for
 * a command to run and read standard input from).
 * 
 */
char ReadRc (char *fname,int type) {
  char *str, *str_p, *tmp;
  int i,j,line,newflag;
  Menu_Item_Type *ingroup=NULL; /* points to last item we added to group */
  Menu_Item_Type *currentitem=NULL; /* the item we are adding to the menus */
  FILE *fp=NULL;

  line=0;
	
  if (type == RC_FILE) {
    if (fname[0] == '-' && fname[1] == '\0')
      fp=stdin;
    else
      fp=fopen(fname,"r");
  }
  else if (type == RC_PREPROC)
    fp=popen(fname,"r");

  if (fp==NULL) {
    return 0;
  }

  while ((str=pdgetline(fp,1))) {
    line++;

    /* Trim leading whitespace */
    str_p=str;
    while (isspace(str_p[0]))
      str_p++;

    /* Ignore comments and blank lines. */
    if (str_p[0]=='#' || str_p[0]==';' || str_p[0]=='\0') {
      free(str);
      continue;
    }

    /* Chop off 1st delimeter, to give us our keyword. */
    if ((tmp=unescape(pdstrtok(str_p,FIELD_DELIM),FIELD_DELIM)) == NULL) {
      free(str);
      continue;
    }

    /* process the data depending on the keyword */
    if (strcasecmp(tmp,PREPROC_KEYWORD)==0) {
      /*
       * We want the rest of the string. \n is just a convenient 
       * delemiter that cannot ever match. 
       */
      ReadRc(pdstrtok(NULL,'\n'),RC_PREPROC);
    }
    else if (strcasecmp(tmp,ADD_MENU_KEYWORD)==0) {
      if ((tmp=unescape(pdstrtok(NULL,FIELD_DELIM),FIELD_DELIM)) == NULL) {
	free(str);
	continue;
      }
      /* Try to find the menu by name. */
      if ((current_rc_menu=LookupMenu(tmp)) == NULL) {
	/* Didn't find, so new menu is needed. */
	if (!menus) {
	  /* This is the first menu in the list */
	  menus=malloc(sizeof(Menu_Type));
	  menus->last=NULL;
	}
	else {
	  menus->next=malloc(sizeof(Menu_Type));
	  menus->next->last=menus;
	  menus=menus->next;
	}
	menus->next=NULL;
	current_rc_menu=menus;
	current_rc_menu->num=0;
	current_rc_menu->name=malloc(strlen(tmp)+1);
	strcpy(current_rc_menu->name,tmp);
	current_rc_menu->selected=0;
	current_rc_menu->x=0;
	current_rc_menu->y=0;
	current_rc_menu->dx=0;
	current_rc_menu->dy=0;
	current_rc_menu->mustscroll=0;
	/* Allocate space for some menu items */
	current_rc_menu->items=malloc(sizeof(Menu_Item_Type *) * MENU_INC);
	current_rc_menu->num_avail=MENU_INC;
      }
      if ((tmp=unescape(pdstrtok(NULL,FIELD_DELIM),FIELD_DELIM)) == NULL) {
	current_rc_menu->title=malloc(1);
	strcpy(current_rc_menu->title,"");
      }
      else {
	current_rc_menu->title=malloc(strlen(tmp)+1);
	strcpy(current_rc_menu->title,tmp);
      }
      if ((tmp=pdstrtok(NULL,'\n')) == NULL) {
	current_rc_menu->helptext=malloc(1);
	strcpy(current_rc_menu->helptext,"");
      }
      else {
	current_rc_menu->helptext=malloc(strlen(tmp)+1);
	strcpy(current_rc_menu->helptext,tmp);
      }
      current_rc_menu->recalc=1;
    }
    else if (strcasecmp(tmp,TITLE_KEYWORD)==0) { /* set title */
      free(ScreenTitle);
      ScreenTitle=strdup(pdstrtok(NULL,FIELD_DELIM));
    }
    else if (strcasecmp(tmp,COLOR_KEYWORD)==0) { /* color def */
      if ((tmp=unescape(pdstrtok(NULL,FIELD_DELIM),FIELD_DELIM)) == NULL ) {
	free(str);
	continue;
      }
      for (i=0;i<NUMSCREENPARTS;i++) {
	if (strcasecmp(tmp,ScreenObjNames[i+1]) == 0) { /* match */
	  if ((tmp=unescape(pdstrtok(NULL,FIELD_DELIM),FIELD_DELIM)) != NULL) {
	    strncpy(FG[i],tmp,COLORLEN);
	    if ((tmp=pdstrtok(NULL,FIELD_DELIM)) != NULL) {
	      strncpy(BG[i],tmp,COLORLEN);
	      break;
	    }
	  }
	  i=NUMSCREENPARTS;
	}
      }
      if (i == NUMSCREENPARTS)
      	Error(_("%s:%i Invalid screen part name, \"%s\"."),fname,line,tmp);
    }
    else if (strcasecmp(tmp,READ_KEYWORD)==0) { /* read in another file */
      ReadRc(pdstrtok(NULL,FIELD_DELIM),RC_FILE);
    }
    else if (strcasecmp(tmp,ENDGROUP_KEYWORD)==0) { /* end a group command */
      if (ingroup != NULL) {
	ingroup=NULL;
	current_rc_menu->num++;
      }
      else
	Error(_("%s:%i \"endgroup\" found outside of group block."),fname,line);
    }
    /* Everything from here on out requires a menu be set up first. */
    else if (!menus || current_rc_menu == NULL)
      Error(_("%s:%i Invalid or misplaced keyword, \"%s\"."),fname,line,tmp);
    else if ((strcasecmp(tmp,EXEC_KEYWORD)==0) ||
	     (strcasecmp(tmp,SHOW_MENU_KEYWORD)==0) ||
	     (strcasecmp(tmp,HELP_TEXT_KEYWORD)==0) ||
	     (strcasecmp(tmp,EXIT_KEYWORD)==0) ||
	     (strcasecmp(tmp,NOP_KEYWORD)==0) ||
	     (strcasecmp(tmp,GROUP_KEYWORD)==0) ||
	     (strcasecmp(tmp,REMOVE_KEYWORD)==0)) {

      if (ingroup == NULL) { /* add next item to menu */
	/* Check to see if enough space is malloc()'d for the element. */
	if (current_rc_menu->num >= current_rc_menu->num_avail) { 
	  current_rc_menu->num_avail+=MENU_INC;
	  current_rc_menu->items=realloc(current_rc_menu->items,
					 sizeof(Menu_Item_Type *) *
					 current_rc_menu->num_avail);
	}

	currentitem=current_rc_menu->items[current_rc_menu->num] =
	  malloc(sizeof(Menu_Item_Type));
	current_rc_menu->recalc=1; /* make sure the menu gets resized */
      }
      else /* add to linked list for group */
	currentitem=currentitem->next=malloc(sizeof(Menu_Item_Type));
      currentitem->next=NULL;

      if (strcasecmp(tmp,EXEC_KEYWORD)==0)
	currentitem->type=MENU_EXEC;
      else if (strcasecmp(tmp,SHOW_MENU_KEYWORD)==0)
	currentitem->type=MENU_SHOW;
      else if (strcasecmp(tmp,NOP_KEYWORD)==0)
	currentitem->type=MENU_NOP;
      else if (strcasecmp(tmp,HELP_TEXT_KEYWORD)==0)
	currentitem->type=MENU_HELP_TEXT;
      else if (strcasecmp(tmp,GROUP_KEYWORD)==0) {
	if (ingroup == NULL) {
	  ingroup=currentitem;
	  currentitem->type=MENU_GROUP;
	}
	else
	  Error(_("%s:%i Nested \"group\" commands."),fname,line);
      }
      else if (strcasecmp(tmp,REMOVE_KEYWORD)==0)
	currentitem->type=MENU_REMOVE;
      else
	currentitem->type=MENU_EXIT;

      currentitem->hotkey=-1; 
      if ((tmp=unescape(pdstrtok(NULL,FIELD_DELIM),FIELD_DELIM)) != NULL) {
	for (i=0;i<strlen(tmp)-1;i++) { /* set hotkey */
          if(tmp[i] == '\\') {
	    for (j=i; j<strlen(tmp);j++) {
	      tmp[j]=tmp[j+1];
	    }
	    i++;    /* skip past the escaped character */
	    continue;
	  }
	  if (tmp[i]==HOTKEY_SEL_CHAR) {
	    currentitem->hotkey=i;
	    for (;i<strlen(tmp);i++) {
	      tmp[i]=tmp[i+1];
	    }
	  }
	}
	tmp=unescape(tmp,HOTKEY_SEL_CHAR);
	currentitem->text=malloc(strlen(tmp)+1);
	strcpy(currentitem->text,tmp);
      }
      else {
	currentitem->text=malloc(1);
	strcpy(currentitem->text,"");
      }
      currentitem->noclear_flag=0;
      currentitem->pause_flag=0;
      currentitem->display_flag=0;
      currentitem->truncate_flag=0;
      currentitem->edit_flag=0;
      currentitem->makemenu_flag=0;
      currentitem->setenv_flag=0;
      if ((tmp=unescape(pdstrtok(NULL,FIELD_DELIM),FIELD_DELIM)) != NULL) {
	/*
	 * Process flags. If the flag string is very short, use the old 
	 * flag style for backwards compatability.
	 */
	if (strlen(tmp) < 4)
	  for (i=0;i<strlen(tmp);i++)
	    SetFlags(currentitem,tmp[i]);
	else {
	  newflag=1;
	  for (i=0;i<strlen(tmp);i++)
	    if (newflag) {
	      SetFlags(currentitem,tmp[i]);
	      newflag=0;
	    }
	    else
	      newflag=(tmp[i] == FLAG_DELIM);
	}
      }
      if ((tmp=pdstrtok(NULL,'\n')) != NULL) {
	currentitem->command=malloc(strlen(tmp)+1);
	strcpy(currentitem->command,tmp);
      }
      else {
	currentitem->command=malloc(1);
	strcpy(currentitem->command,"");
      }
      if (ingroup == NULL)
	current_rc_menu->num++;
    }
    else
      Error("%s:%i Invalid or misplaced keyword, \"%s\".",fname,line,tmp);

    free(str);
  }

  if (fp == stdin) {
    /*
     * Make sure that we have the real tty open as stdin.
     * You see, if a rc file is passed to pdmenu on stdin, then
     * after the rc file is read, there is no longer stdin available
     * for programs pdmenu launches to use.
     */
    int fd = open("/dev/tty", O_RDWR);
    dup2(fd, 0);
    close(fd);
  }
  else if (type == RC_FILE)
    fclose(fp);
  else if (type == RC_PREPROC)
    pclose(fp);

  if (ingroup != NULL)
    Error(_("%s: \"group\" command used without \"endgroup\"."),fname);

  return 1;
}
