/* 
 * Copyright (c) 1995, 1996, 1997 Joey Hess (joey@kite.ml.org)
 * All rights reserved. See COPYING for full copyright information (GPL).
 */

/*
 * This is a type for a double linked list of windows that are currently 
 * onscreen.
 */
typedef struct Window_List_Type {
  Menu_Type *menu;
  struct Window_List_Type *last;
  struct Window_List_Type *next;
} Window_List_Type;
