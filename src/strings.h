/* 
 * All user-visible strings used in this program. 
 * Modify to change languages, and so on.
 */

/*
 * Copyright (c) 1995, 1996, 1997 Joey Hess (joey@kite.ml.org)
 * All rights reserved. See COPYING for full copyright information (GPL).
 */

/* ~/.DEFAULTRC and /etc/DEFAULTRC are checked */
#define DEFAULTRC "pdmenurc"

/* The default line at the base of the screen. */
#define DEFAULTBASE "Welcome to Pdmenu " VER " by Joey Hess <joey@kite.ml.org>"

/* A help line, displayed if a bad key is pressed in a menu. */
#define HELPBASE "Arrow keys move, Esc exits current menu, Enter runs program"

/* The title of the screen */
#define DEFAULTTITLE "Pdmenu"

/* Usage help */
#define USAGEHELP "\
Usage: pdmenu [-h] [-c] [-q] [menufiles ..]\n\
\tmenufiles     the rc files to read instead of\n\
\t              ~/.pdmenurc or " ETCDIR "pdmenurc\n\
\t-h, --help    display this help\n\
\t-c, --color   enable color\n\
\t-q, --quit    'q' key does not exit program\n"

/* Line that is displayed after the usage help if long options are not enabled */
#define NOLONGOPTS_MESSAGE "(Long options are disabled.)\n"

/* Displayed when we are running a program to get it's stdout and display 
   in a window. */
#define WAITTEXT "Please wait..."

/* Help text displayed for a message window. */
#define DEFAULT_MESSAGE_HELP "Press Esc to close window."

/* Help text displayed for en edit window. */
#define INPUTHELP "Press Enter when done, or Esc to cancel" 

/* Displayed after an exec command with a pause flag is done */
#define PRESS_ENTER_STRING "\nPress Enter to return to Pdmenu."

/* Dispalyed at the end of a window displaying output that had to be truncated */
#define TRUNCATED_STRING "[Command output truncated]"

/* Displayed if a command is displayed in a window, and the command oututted 
   nothing to stdout. */
#define NULL_OUTPUT "[Command returned no output]"

/* Displayed when they try to open too many windows onscreen at once.
   %i is replaced with MAX_WINDOWS */
#define TOO_MANY_WINDOWS "Tried to open too many menus at once. (Max %i)\n"

/* Displayed if we can't find a rc, or if no rc files defined a menu */
#define NO_RC_FILE "Unable to find any pdmenurc files, or all pdmenurc files are empty.\n"

/* Displayed when we're reading a rc file that's too long and has too many 
   menus in it. 
   
   1st %s is the filename we are reading. 
   1st %i is the line number in the file
   2nd %i is MAX_NUM_MENUS
*/
#define TOO_MANY_MENUS "%s:%i Too many menus (max %i).\n"

/* Displayed if a keyword that must be inside a menu is found outside of one.
   %s, %i are filename and line number */
#define INVALID_KEYWORD "%s:%i Invalid or misplaced keyword.\n"

/* Displayed if we have a really long menu
   %s, %i are the filename and line number
   A second %i is replaced with MAX_ITEMS_IN_MENU */
#define MENU_TOO_BIG "%s:%i Too many items in menu (max %i).\n"

/*
 * Displayed if a menu is found to be empty after rc files are read.
 * (%s=name of menu)
 */
#define EMPTY_MENU_ERROR "Menu named \"%s\" is empty.\n"
/* Displayed if the menu has nop commands at the very beginning. */
#define MENU_NOPS_ERROR "Menu named \"%s\" has a nop command at very beginning.\n"

/* Displayed if the screen is too small to use */
#define SCREEN_TOOSMALL "Error: The screen is too small.\n"

#define KP_INIT_ERROR "Unable to initialize key mappings.\n"
