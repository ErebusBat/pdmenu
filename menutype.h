/* 
 * These types are used throughout pdmenu, in menu definitions.
 */

typedef struct Menu_Item_Type {
	signed char hotkey;
	char text[MENU_ITEM_TEXTSIZE];
	char command[MENU_ITEM_COMMANDSIZE];
	char flags[NUMFLAGS];
	char type;
#define MENU_EXEC 1
#define MENU_SHOW 2
#define MENU_EXIT 3
#define MENU_NOP  4
#define MENU_NULL 0
} Menu_Item_Type;
          
typedef struct Menu_Type {
	char name[NAMELEN];
	char title[TITLELEN];
	char helptext[HELPTEXTLEN];
	Menu_Item_Type items[MAX_ITEMS_IN_MENU];
	int num;
	int selected;
	int x, y;
	int dx, dy;
	int recalc;
	char mustscroll; /* will we have to handle scrolling for this menu? (y=1,n=0) */
	int offset; /* offset we've scrolled to */
} Menu_Type;
