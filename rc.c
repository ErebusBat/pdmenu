#include "global.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* 
 * UGLY HACK:
 * This global variable is needed so ReadRc will know what menu to add 
 * commands to.
 */
Menu_Type *current_rc_menu=NULL;

/* 
 * Read in a pdmenu style rc file. Preserves data from any other files that 
 * might have previously been read. 
 *
 * Returns 0 if file would not open. 
 *
 * type should be either RC_FILE (for normal files) or RC_PREPROC (for
 * a command to run and read standard input from).
 * 
 */
char ReadRc (char *fname,int type) {
	char str[MAX_LINE_LEN+2];
	char *tmp,*str_p;
	int i,line;
	FILE *fp=NULL;

	line=0;
	
	if (type == RC_FILE) 
		fp=fopen(fname,"r");
	else if (type == RC_PREPROC)
		fp=popen(fname,"r");

	if (fp==NULL) {
		return 0;
	}

	while (fgets(str,MAX_LINE_LEN,fp) != NULL ) {

		line++;

		/* Trim leading whitespace */
		str_p=str;
		while (isspace(*str_p)) { ++str_p; };

		/* Ignore comments and blank lines. */
    if ((str_p[0]=='#') || (str_p[0]==';') || (strlen(str_p)==0)) {
			continue;
		}

		/* remove trailing \n */
		tmp=str_p;
		while((*tmp != '\0') && (*tmp != '\n'))
			tmp++;
		*tmp='\0';

		/* Chop off 1st delimeter, to give us our keyword.
		 */
		if ((tmp=strtok(str_p,FIELD_DELIM)) == NULL)
			continue;

    /* process the data depending on the keyword */
 		if (strcasecmp(tmp,PREPROC_KEYWORD)==0) {
			/* We want the rest of the string. \n is just a convienent 
			 * delimeter that cannot ever match. 
			 */
			ReadRc(strtok(NULL,"\n"),RC_PREPROC);
		}
		else if (strcasecmp(tmp,ADD_MENU_KEYWORD)==0) {
			/* We are either adding a new menu, or adding on to an existing one. 
			 * Try to find this menu in the list.
			 */
			if ((tmp=strtok(NULL,FIELD_DELIM)) == NULL)
				continue;
			for(i=0;i<NumMenus;i++) {
				current_rc_menu=menus+i;
				if (strcmp(current_rc_menu->name,tmp)==0)
					break; /* found it! */
			}
			/* If we didn't find it, add a new menu. */
			if (i == NumMenus) {
				if (NumMenus>=MAX_NUM_MENUS) {
					fprintf(stderr,TOO_MANY_MENUS,fname,line,MAX_NUM_MENUS);
					exit(-1);
				}
				current_rc_menu=menus+NumMenus++;
				current_rc_menu->num=0;

				strncpy(current_rc_menu->name,tmp,NAMELEN);
				if ((tmp=strtok(NULL,FIELD_DELIM)) != NULL) {
					strncpy(current_rc_menu->title,tmp,TITLELEN);
					if ((tmp=strtok(NULL,"\n")) !=NULL) {
						strncpy(current_rc_menu->helptext,tmp,HELPTEXTLEN);
					}
				}

				current_rc_menu->selected=0;                                         
				current_rc_menu->x=0;
				current_rc_menu->y=0;
				current_rc_menu->dx=0;
				current_rc_menu->dy=0; 
				current_rc_menu->mustscroll=0;
				current_rc_menu->recalc=1;
			}
		}
		else if (strcasecmp(tmp,COLOR_KEYWORD)==0) { /* color def */
			if ((tmp=strtok(NULL,FIELD_DELIM)) == NULL )
				continue;
			for (i=0;i<NUMSCREENPARTS;i++)
				if (strcasecmp(tmp,ScreenObjNames[i+1])==0) { /* match */
					if ((tmp=strtok(NULL,FIELD_DELIM)) != NULL) {
						strncpy(FG[i],tmp,COLORLEN);
						if ((tmp=strtok(NULL,FIELD_DELIM)) != NULL) {
							strncpy(BG[i],tmp,COLORLEN);
						}
					}
					i=NUMSCREENPARTS;
				}
		}
		else if (strcasecmp(tmp,READ_KEYWORD)==0) { /* read in another file */
			ReadRc(strtok(NULL,FIELD_DELIM),RC_FILE);
		}
		/* Everything from here on out requires a menu be set up first. */
		else if (NumMenus<1) {
			fprintf(stderr,INVALID_KEYWORD,fname,line);
			exit(-1);
		}               
		else if ((strcasecmp(tmp,EXEC_KEYWORD)==0) ||
						 (strcasecmp(tmp,SHOW_MENU_KEYWORD)==0) ||
						 (strcasecmp(tmp,EXIT_KEYWORD)==0) ||
						 (strcasecmp(tmp,NOP_KEYWORD)==0)) {
			if (current_rc_menu->num>MAX_ITEMS_IN_MENU) {
				fprintf(stderr,MENU_TOO_BIG,fname,line,MAX_ITEMS_IN_MENU);
				exit(-1);
			}
			if (strcasecmp(tmp,EXEC_KEYWORD)==0)
				current_rc_menu->items[current_rc_menu->num].type=MENU_EXEC;
			else if (strcasecmp(tmp,SHOW_MENU_KEYWORD)==0)
				current_rc_menu->items[current_rc_menu->num].type=MENU_SHOW;
			else if (strcasecmp(tmp,NOP_KEYWORD)==0)
				current_rc_menu->items[current_rc_menu->num].type=MENU_NOP;
			else
				current_rc_menu->items[current_rc_menu->num].type=MENU_EXIT;

			current_rc_menu->items[current_rc_menu->num].hotkey=-1; 
			if ((tmp=strtok(NULL,FIELD_DELIM)) != NULL) {
				for (i=0;i<strlen(tmp)-1;i++) { /* set hotkey */
					if (tmp[i]==HOTKEY_SEL_CHAR) {
						current_rc_menu->items[current_rc_menu->num].hotkey=i;
						for (;i<strlen(tmp);i++) {
							tmp[i]=tmp[i+1];
						}
						break;
					}
				}
				strncpy(current_rc_menu->items[current_rc_menu->num].text,tmp,MENU_ITEM_TEXTSIZE);
			}
			/* This works around an problem in strtok(): it skips any number of
			 * the delimiting characters. That messes us up if the flags field is 
			 * empty.
			 *
       * Get the rest of the string, break off the flags, if any, then
       * break off the commands, if any, manually.
			 */
			if ((tmp=strtok(NULL,"\n")) != NULL) {
				if (strspn(tmp,FIELD_DELIM)>0) { /* FIELD_DELIM is first letter of string. */
					strncpy(current_rc_menu->items[current_rc_menu->num].command,++tmp,MENU_ITEM_COMMANDSIZE);
				}
				else if ((tmp=strtok(tmp,FIELD_DELIM)) != NULL) { /* got flags */
					strncpy(current_rc_menu->items[current_rc_menu->num].flags,tmp,NUMFLAGS);
					if ((tmp=strtok(NULL,"\n")) != NULL) { /* got command */
						strncpy(current_rc_menu->items[current_rc_menu->num].command,tmp,MENU_ITEM_COMMANDSIZE);		
					}
				}
			}
			current_rc_menu->num++;
		}
	}

	if (type == RC_FILE)
		fclose(fp);
	else if (type == RC_PREPROC)
		pclose(fp);
		
	return 1;
}

/* 
 * Examines all menus, and exits with an error if any menu is all empty or
 * starts with a "nop" command, or ends with one. All of those are things that
 * make the menu display code very unhappy.
 */
void SanityCheckMenus() {
	int c;
	Menu_Type *m=menus+0;

	for (c=0;c < NumMenus;c++) {
		if (m->num == 0) {
			fprintf(stderr,EMPTY_MENU_ERROR,m->name);
			exit(-1);
		}
		else {
			if (m->items[0].type == MENU_NOP) {
				fprintf(stderr,MENU_NOPS_ERROR,m->name);
				exit(-1);
			}
		}
		m++;
		/*
		 * Check for a nop command at the end of the menu, and clean up 
		 * if there is one. 
		 */
		while (m->items[m->num-1].type == MENU_NOP) {
			m->num--;
		}
	}
}
