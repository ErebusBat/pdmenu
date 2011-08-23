/*
 * This files sets up keywords and so on that are used in the pdmenurc files.
 */

/*
 * Copyright (c) 1995-1999 Joey Hess (joey@kitenet.net)
 * All rights reserved. See COPYING for full copyright information (GPL).
 */

/* Delimiter of fields in the rc file */
#define FIELD_DELIM ':'

/* Delimeter of flags within feilds in the rd file */
#define FLAG_DELIM ','

/* Put this before the character in a menu item that is to become the hotkey */
#define HOTKEY_SEL_CHAR '_'

/* The keywords in a rc file are.. */
#define ADD_MENU_KEYWORD "menu"
#define SHOW_MENU_KEYWORD "show"
#define EXEC_KEYWORD "exec"
#define EXIT_KEYWORD "exit"
/* British types may want to change this to colour :-) */
#define COLOR_KEYWORD "color"
#define TITLE_KEYWORD "title"
#define NOP_KEYWORD "nop"
#define READ_KEYWORD "read"
#define PREPROC_KEYWORD "preproc"
#define REMOVE_KEYWORD "remove"
#define GROUP_KEYWORD "group"
#define ENDGROUP_KEYWORD "endgroup"
#define HELP_TEXT_KEYWORD "helptext"

/* The flags that can be used are.. */
#define NOCLEAR_FLAG 'n'
#define PAUSE_FLAG 'p'
#define DISPLAY_FLAG 'd'
#define EDIT_FLAG 'e'
#define SETENV_FLAG 's'
#define TRUNCATE_FLAG 't'
#define MAKEMENU_FLAG 'm'
#define COMMAND_FLAG 'c'

/* The names of the parts of the screen, for color setting, are */
#define DESKTOP_NAME "desktop"
#define TITLE_NAME "title"
#define BASE_NAME "base"
#define MENU_NAME "menu"
#define SELBAR_NAME "selbar"
#define SHADOW_NAME "shadow"
#define MENU_HI_NAME "menuhot"
#define SELBAR_HI_NAME "selbarhot"
#define UNSEL_MENU_NAME "unselmenu"
/* The longest of the strings above is how many characters? */
#define LONGEST_OBJ_NAME 9
