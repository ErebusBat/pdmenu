/* 
 * These types are used throughout pdmenu, in menu definitions.
 */

/* 
 * Copyright (c) 1995, 1996, 1997 Joey Hess (joey@kitenet.net)
 * All rights reserved. See COPYING for full copyright information (GPL).
 */

/* The structure that holds an indivitual menu item. Can be a linked list. */
typedef struct Menu_Item_Type {
  signed char hotkey;
  char *text;
  char *command;
  int noclear_flag;
  int pause_flag;
  int display_flag;
  int truncate_flag;
  int edit_flag;
  int makemenu_flag;
  int setenv_flag;
  int command_flag;
  char type;
#define MENU_EXEC   1
#define MENU_SHOW   2
#define MENU_EXIT   3
#define MENU_NOP    4
#define MENU_GROUP  5
#define MENU_REMOVE 6
#define MENU_HELP_TEXT 7
  struct Menu_Item_Type *next;
} Menu_Item_Type;

/*
 * The structure that holds a whole menu, and is an element of a linked
 * list of menus.
 */
typedef struct Menu_Type {
  char *name;
  char *title;
  char *helptext;
  Menu_Item_Type **items;
  int num;
  int num_avail; /* the number of items space has been allocated for so far */
  int selected;
  int x, y;
  int dx, dy;
  int recalc;
  char mustscroll; /* will we have to handle scrolling for this menu? */
  int offset; /* offset we've scrolled to */
  struct Menu_Type *next;
  struct Menu_Type *last;
} Menu_Type;
