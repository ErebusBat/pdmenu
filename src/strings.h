/* 
 * All user-visible strings used in this program. 
 * Modify to change languages, and so on.
 */

/*
 * Copyright (c) 1995-1999 Joey Hess (joey@kitenet.net)
 * All rights reserved. See COPYING for full copyright information (GPL).
 */

/* ~/.DEFAULTRC and /etc/DEFAULTRC are checked */
#define DEFAULTRC "pdmenurc"

/* The default line at the base of the screen. */
#define DEFAULTBASE "Welcome to Pdmenu " VER " by Joey Hess <joey@kitenet.net>"

/* A help line, displayed if a bad key is pressed in a menu. */
#define HELPBASE "Arrow keys move, Esc exits current menu, Enter runs program"

/* The title of the screen */
#define DEFAULTTITLE "Pdmenu"

/* Basic version information for pdmenu -v */
#define VERSION "Pdmenu " VER " GPL Copyright (C) 1995-2001 by Joey Hess <joey@kitenet.net>\n"

/* Usage help */
#define USAGEHELP "\
Usage: pdmenu [options] [menufiles ..]\n\
\tmenufiles                 the rc files to read instead of\n\
\t                          ~/.pdmenurc or " ETCDIR "pdmenurc\n\
\t-h        --help          display this help\n\
\t-c        --color         enable color\n\
\t-u        --unpark        cursor moves to current selection\n\
\t-q        --quit          'q' key does not exit program\n\
\t-mmenuid  --menu=menuid   display menu with this menuid on startup\n\
\t-v        --version       show version information\n\
\t-r        --retro         draw menus using old style\n\
\t-l        --lowbit        do not use high bit line drawing characters\n\
\t-n        --numeric       do not use 8 and 2 for moving up and down\n"

/* 
 * Parameters that can be passed to the program, both long and short 
 * versions.
 */
#define PARAM_LONG_HELP "help"
#define PARAM_SHORT_HELP 'h'
#define PARAM_LONG_COLOR "color"
#define PARAM_SHORT_COLOR 'c'
#define PARAM_LONG_QUIT "quit"
#define PARAM_SHORT_QUIT 'q'
#define PARAM_LONG_UNPARK "unpark"
#define PARAM_SHORT_UNPARK 'u'
#define PARAM_LONG_VERSION "version"
#define PARAM_SHORT_VERSION 'v'
#define PARAM_LONG_MENU "menu"
#define PARAM_SHORT_MENU 'm'
#define PARAM_LONG_RETRO "retro"
#define PARAM_SHORT_RETRO 'r'
#define PARAM_LONG_LOWBIT "lowbit"
#define PARAM_SHORT_LOWBIT 'l'
#define PARAM_LONG_NUMERIC "numeric"
#define PARAM_SHORT_NUMERIC 'n'

/*
 * List here all the short options as one string 
 * this is formated for getopt, there should be colons after all options that
 * take parameters 
 */
#define PARAM_SHORT_ALL "hcquvm:rln"

/*
 * Line that is displayed after the usage help if long options are 
 * not enabled 
 */
#define NOLONGOPTS_MESSAGE "(Long options are disabled.)\n"

/*
 * Displayed when we are running a program to get its stdout and display 
 * in a window.
 */
#define WAITTEXT "Please wait..."

/* Help text displayed for a message window. */
#define DEFAULT_MESSAGE_HELP "Press Esc to close window."
/* The length of the above */
#define DEFAULT_MESSAGE_HELP_LEN 26

/* Help text displayed for en edit window. */
#define INPUTHELP "Press Enter when done, or Esc to cancel" 

/* Displayed after an exec command with a pause flag is done */
#define PRESS_ENTER_STRING "\nPress Enter to return to Pdmenu."

/* Header that comes before all error mssages */
#define ERROR_HEADER "pdmenu error: "

/*
 * Displayed if a command is displayed in a window, and the command output
 * nothing to stdout.
 */
#define NULL_OUTPUT "Command returned no output"

/* Displayed if we can't find a rc, or if no rc files defined a menu */
#define NO_RC_FILE "Unable to find any pdmenurc files, or all pdmenurc files are empty."

/*
 * Displayed if a keyword that must be inside a menu is found outside of one.
 * %s, %i are filename and line number, the third %s is the keyword in 
 * question.
 */
#define INVALID_KEYWORD "%s:%i Invalid or misplaced keyword, \"%s\"."

/* Displayed if an invalid screen part name is used. */
#define INVALID_SCREEN_PART "%s:%i Invalid screen part name, \"%s\"."

/* Displayed if the screen is too small to use */
#define SCREEN_TOOSMALL "Error: The screen is too small."

/* Displayed if slang has problems with key mappings. */
#define KP_INIT_ERROR "Unable to initialize key mappings."

/*
 * Displayed if -menu= is specified, but the menu they ask for is not in the 
 * rc file. The %s is replaced with the name of the menu specified.
 */
#define NO_SUCH_MENU "Menu \"%s\" is not defined."

/* Displayed if endgroup appears when we are not in a group. */
#define ENDGROUP_WITHOUT_GROUP "%s:%i \"endgroup\" found outside of group block."

/* Displayed if we try to use a group command within a group. */
#define NESTED_GROUP "%s:%i Nested \"group\" commands."

/* 
 * Displayed if the end of a file is reached with no endgroup command 
 * to close a group command
 */
#define GROUP_WITHOUT_ENDGROUP "%s: \"group\" command used without \"endgroup\"."

/* 
 * Displayed if they try to remove a menu that is presently on the screen
 * %s is replaced with the menuid that was to be removed.
 */
#define REMOVE_ONSCREEN_MENU "Attempt to remove menu \"%s\" failed: menu is on screen."
