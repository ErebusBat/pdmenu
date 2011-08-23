/*
 * Copyright (c) 1995-2003 Joey Hess (joey@kitenet.net)
 * All rights reserved. See COPYING for full copyright information (GPL).
 */

#include "global.h"
#include "rc.h"
#include "screen.h"
#include "menu.h"
#include "window.h"
#include "mouse.h"
#include "actions.h"
#include "error.h"
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "slang.h"
#include <locale.h>
#include <libintl.h>
#define _(String) gettext (String)

/* Display usage info then quit */
void usage() {
  printf (_("\
Usage: pdmenu [options] [menufiles ..]\n\
\tmenufiles                 the rc files to read instead of\n\
\t                          ~/.pdmenurc or %spdmenurc\n\
\t-h        --help          display this help\n\
\t-s        --superhot      enable super hot keys\n\
\t-c        --color         enable color\n\
\t-u        --unpark        cursor moves to current selection\n\
\t-q        --quit          'q' key does not exit program\n\
\t-mmenuid  --menu=menuid   display menu with this menuid on startup\n\
\t-v        --version       show version information\n\
\t-r        --retro         draw menus using old style\n\
\t-l        --lowbit        do not use high bit line drawing characters\n\
\t-n        --numeric       do not use 8 and 2 for moving up and down\n"),ETCDIR);
#if defined (HAVE_GETOPT_LONG)
#else
  printf ("%s\n", _("(Long options are disabled.)"));
#endif
  exit(-1);
}

/* Display version information then quit */
void version() {
  printf (_("Pdmenu %s GPL Copyright (C) 1995-2002 by Joey Hess <joey@kitenet.net>\n"),VER);
  exit(0);
}

/* 
 * Parse params, Figure out the correct rc file, read it.
 * Returns a pointer to the menu we should display first.
 */
Menu_Type *GetConfig (int argc, char **argv) {
#ifdef HAVE_ASPRINTF
  char **s=malloc(sizeof(char *));
#else
  /* Just long enough for 255 chars of $HOME plus filename. */
  char *s=(char *) malloc(258+strlen("pdmenurc"));
#endif
  char *startmenu=NULL;
  Menu_Type *m;
  int menu_opt_flag=0;
  extern int optind;
  extern char *optarg;
  
#ifdef HAVE_GETOPT_LONG
  struct option long_options[] = {
    {"help",0,NULL,'h'},
    {"color",0,NULL,'c'},
    {"quit",0,NULL,'q'},
    {"unpark",0,NULL,'u'},
    {"version",0,NULL,'v'},
    {"menu",1,NULL,'m'},
    {"retro",0,NULL,'r'},
    {"superhot",0,NULL,'s'},
    {"lowbit",0,NULL,'l'},
    {"numeric",0,NULL,'n'},
    {0, 0, 0, 0}
  };
#endif

#if defined (HAVE_GETOPT_LONG) || defined (HAVE_GETOPT)
  int c=0;

  while (c != -1) {
#ifdef HAVE_GETOPT_LONG
    c=getopt_long(argc,argv,"hscquvm:rln",long_options,NULL);
#elif HAVE_GETOPT
    c=getopt(argc,argv,"hscquvm:rln");
#endif
    switch (c) {
    case 'q': /* 'q' does not exit pdmenu. */
      Q_Exits=0;
      break;
    case 'c': /* Use colors. */
      Use_Color=1;
      break;
    case 'h':
      usage(); /* exits program */
    case 'u': /* Unpark cursor. */
      Unpark_Cursor=1;
      break;
    case 'm': /* Display a particular menu on start. */
      menu_opt_flag=1;
      startmenu=malloc(strlen(optarg)+1);
      strcpy(startmenu,optarg);
      break;
    case 'v':
      version(); /* exits program */
    case 'r':
      Retro=1;
      break;
    case 's':
      Superhot=1;
      break;
    case 'l':
      Lowbit=1;
      break;
    case 'n':
	/*
	 * Override the default values for up and down, so 2 and 8 
	 * can be used for hotkeys instead. 
	 */
	Numeric=1;
	break;
    }
  }
#else
  optind=1;
#endif /* have one of the getopts */

  if (optind < argc) { /* we are asked to process a rc file */
    while (optind < argc)
      ReadRc(argv[optind++],RC_FILE);
  }
  else { /* fallback rc files */
#ifdef HAVE_ASPRINTF
    asprintf(s,"%s/.%s",getenv("HOME"),"pdmenurc");
    if ((ReadRc(*s,RC_FILE)==0) || (!menus)) {
      asprintf(s,"%s/%s",ETCDIR,"pdmenurc");
      ReadRc(*s,RC_FILE);
#else
    strncpy(s,getenv("HOME"),255);
    strcat(s,"/.");
    strcat(s,"pdmenurc");
    if ((ReadRc(s,RC_FILE)==0) || (!menus)) {
      strcpy(s,ETCDIR);
      strcat(s,"pdmenurc");
      ReadRc(s,RC_FILE);
#endif
    }
  }

  SanityCheckMenus();
  if (!menus) {
    free(s);
    Error(_("Unable to find any pdmenurc files, or all pdmenurc files are empty."));
  }

  if (menu_opt_flag) { /* menu to display was specified on the command line. */
    m=LookupMenu(startmenu);
    if (m) {
      free(s);
      free(startmenu);
      return m;
    }
    /* Couldn't find the menu if we get to here. */
    Error(_("Menu \"%s\" is not defined."),startmenu);
    exit(1); /* just here to shut up gcc -wall */
  }
  else { /* display first menu we read that still exists */
    free(s);
    /*
     * Sigh I wish we displayed the last menu to appear, then I wouln't 
     * need this loop. Backwards-compatability sucks..
     */
    m=menus;
    while (m->last)
      m=m->last;
    return m;
  }
}

int main (int argc, char **argv) {
  Menu_Type *m;
  int ret;

  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE,LOCALEDIR);
  textdomain(PACKAGE);
  
  menus=NULL;
  CurrentWindow=NULL;

  ScreenTitle=strdup(_("Pdmenu"));
  
  /* If COLORTERM is set, use color by default */
  Use_Color=!(getenv("COLORTERM")==NULL);
  Q_Exits=1;
  Unpark_Cursor=0;

  /* SLang screen objects and their default colors */
  strcpy(ScreenObjNames[1],DESKTOP_NAME);	
  strcpy(FG[0],DESKTOP_FG_DEFAULT);	
  strcpy(BG[0],DESKTOP_BG_DEFAULT);
  strcpy(ScreenObjNames[6],TITLE_NAME);	
  strcpy(FG[5],TITLE_FG_DEFAULT);	
  strcpy(BG[5],TITLE_BG_DEFAULT);
  strcpy(ScreenObjNames[3],BASE_NAME);	
  strcpy(FG[2],BASE_FG_DEFAULT);	
  strcpy(BG[2],BASE_BG_DEFAULT);
  strcpy(ScreenObjNames[4],MENU_NAME);	
  strcpy(FG[3],MENU_FG_DEFAULT);	
  strcpy(BG[3],MENU_BG_DEFAULT);
  strcpy(ScreenObjNames[5],SELBAR_NAME);	
  strcpy(FG[4],SELBAR_FG_DEFAULT);	 
  strcpy(BG[4],SELBAR_BG_DEFAULT);
  strcpy(ScreenObjNames[2],SHADOW_NAME);	
  strcpy(FG[1],SHADOW_FG_DEFAULT);
  strcpy(BG[1],SHADOW_BG_DEFAULT);
  strcpy(ScreenObjNames[7],MENU_HI_NAME);
  strcpy(FG[6],MENU_HI_FG_DEFAULT);	
  strcpy(BG[6],MENU_HI_BG_DEFAULT);
  strcpy(ScreenObjNames[8],SELBAR_HI_NAME);	
  strcpy(FG[7],SELBAR_HI_FG_DEFAULT);	
  strcpy(BG[7],SELBAR_HI_BG_DEFAULT);
  strcpy(ScreenObjNames[9],UNSEL_MENU_NAME);
  strcpy(FG[8],UNSEL_MENU_FG_DEFAULT);
  strcpy(BG[8],UNSEL_MENU_BG_DEFAULT);
 
#if (defined(SLANG_VERSION) && (SLANG_VERSION >= 20000))
  /* Enable utf-8 in slang. */
  SLutf8_enable(-1);
#endif

  /* Parse parameters and load pdmenurc file. */
  m=GetConfig(argc,argv);

  /* color or b&w? Tell slang */
  if (Use_Color) { /* color */
    DESKTOP=1;
    TITLE=6;
    BASE=3;
    MENU=4;
    SELBAR=5;
    SHADOW=2;
    MENU_HI=7;
    SELBAR_HI=8;
    UNSEL_MENU=9;
  }
  else { /* b&w: those numbered greater than 1 are inverse. */
    DESKTOP=0;
    TITLE=2;
    BASE=2;
    MENU=0;
    SELBAR=2;
    SHADOW=0;
    MENU_HI=2;
    SELBAR_HI=0;
    UNSEL_MENU=0;
  }

  Screen_Setcolors();
  SetScreensize();

#ifdef GPM_SUPPORT
  gpm_ok=InitMouse();
#endif

  AddWindow(m);
  Screen_Init();
  DrawAll();

  do {
    ret=DoMenu(m,Pdmenu_Action,Handle_Ctrl_C);
    if (tcgetpgrp(0) == -1) break; /* Detect tty gone away. */
  } while ((ret!=QUIT_EXIT) && !((ret==Q_KEY_EXIT) && (Q_Exits)));

#ifdef GPM_SUPPORT
  EndMouse(); 
#endif
  Screen_Reset();
  return 0;                  
}
