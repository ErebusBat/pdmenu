#include "global.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "slang.h"

int Pdmenu_Action (Menu_Type *);

/* Draw the whole screen, with menus on it. */
void DrawAll () {
	int c;

	DrawTitle(DEFAULTTITLE);
	DrawDesktop();

	for (c=0;c<=NumOnScreen;c++) {
		DrawMenu(OnScreen[c]);
	}

	if (strlen(OnScreen[NumOnScreen]->helptext)>0)
		DrawBase(OnScreen[NumOnScreen]->helptext);
	else 
		DrawBase(DEFAULTBASE);

 	SLsmg_refresh(); 
}

/* Force a redraw of the screen. Clear screen, then redraw everything. */
void Force_Redraw () {
	Screen_Reset();
	Screen_Init();
	DrawAll();
}

/*
 * Call this whenever the screen size changes. It repositions all the windows
 * on the screen to fit the new screen.
 * You will typically wany to call Force_Redraw() after this function.
 */
void Resize_Screen () {
	int c;
	Menu_Type *m;

	Want_Screen_Resize = 0;
	SetScreensize();
	for (c=0;c<=NumMenus;c++) {
		m=menus+c;
		m->recalc=1;
		CalcMenu(m);
	}
}                       

/* Add a window to the group onscreen. */
void AddWindow (int n) {
	if (NumOnScreen==MAX_WINDOWS) {
		Screen_Reset();
		fprintf(stderr,TOO_MANY_WINDOWS,MAX_WINDOWS);
		exit(1);
	}
	OnScreen[++NumOnScreen]=menus+n;
}

/* Remove the topmost window. Returns the number of the menu in array. */
int RemoveWindow () {
	return --NumOnScreen;
}

/* Exit the current window. Returns pointer to new current menu. */
Menu_Type *ExitWindow (Menu_Type* m) {
	if (NumOnScreen>0) { /* Pop down current menu, back to parent */
		m=menus+RemoveWindow();
		DrawAll();
		return m;
	}
	else
		return m; /* Can;t exit menu if there are no others up. */
}                                                                       

/* Handle a menu action by just quitting it */
int NullAction() {
	return QUIT_EXIT;
}
  
/* Display a message in a window.
 * Returns the selected item when the window is exited. 
 * (-1 is returned if they hit q or ESC)
 */
signed int ShowMessage(char *title,char *helptext,char *message[],int arraysize) {
	Menu_Type *m;
	int c=0;

	m=menus+(++NumMenus);
	
	/* load up the menu with the appropriate values */
	strncpy(m->title,title,TITLELEN);
	m->selected=0;
	for (c=0;c<arraysize;c++) {
		strncpy(m->items[c].text,message[c],MENU_ITEM_TEXTSIZE);
		m->items[c].hotkey=-1;
	}
	m->num=arraysize;
	m->recalc=1;

	/* display the menu until they hit q or exit */
	AddWindow(NumMenus);
	DrawAll();
	c=DoMenu(m,NullAction,Handle_Ctrl_C);
	RemoveWindow();
	NumMenus--;
	DrawAll();

	if (c==QUIT_EXIT)
		return m->selected;
	else
		return -1;
}

/* Display usage info then quit */
void usage() {
	printf (USAGEHELP);
	exit(-1);
}

/* 
 * Parse parameters 
 * Returns: 
 * 1 if rc files were processed
 * 0 if no rc files were processed
 */
int ParseParams(int argc, char **argv) {
	int c;
	int ret=0;
	struct option long_options[] = {
		{"help", 0, NULL, 'h'},
		{"color", 0, NULL, 'c'},
		{"quit", 0, NULL, 'q'},
		{0, 0, 0, 0}
	};

	c=0;
	while (c != -1) {
		c=getopt_long(argc,argv,"hcq",long_options,NULL);
		switch (c) {		
		case 'q': /* 'q' does not exit pdmenu */
			Q_Exits=0;
			break;
		case 'c': /* Use colors. */
			Use_Color=1;
			break;
		case 'h':
			usage();
		}
	}

	if (optind < argc) {
		while (optind < argc)
			ReadRc(argv[optind++],RC_FILE);
		ret=1;
	}
	return(ret);
}

/* Figure out the correct rc file, read it, parse params. */
void GetConfig (int argc, char **argv) {
	char home[258+strlen(DEFAULTRC)]; /* Just long enough for 255 chars of $HOME plus filename. */
	char *s;

	if ((ParseParams(argc,argv)==0) || (NumMenus==0)) {
		s=home;
		strncpy(s,getenv("HOME"),255);
 		strcat(s,"/.");
		strcat(s,DEFAULTRC);
		if ((ReadRc(s,RC_FILE)==0) || (NumMenus==0)) {
			strcpy(s,ETCDIR);
			strcat(s,DEFAULTRC);
			if ((ReadRc(s,RC_FILE)==0) || (NumMenus==0)) {
				fprintf(stderr,NO_RC_FILE);
				exit(-1);
			}
		}
	}
}

#ifdef SETENV_FLAG_OK
/* Run a command, and examine stdout for var=val lines, and set the environment
 * of this program appropriately.
 */
void RunSetenv (char *command) {
	FILE *pcommand;
	char str[MENU_ITEM_TEXTSIZE+2];
	char rest[MENU_ITEM_TEXTSIZE+2];
	char *tmp=NULL ,*end;
	int i;
	
	/* Get the command output */
	pcommand=popen(command,"r");
	while (fgets(str,MENU_ITEM_TEXTSIZE,pcommand)!=NULL) {
		tmp=str;			
		while((*tmp != '\0') && (*tmp != '\n')) { /* kill \n */
			tmp++;
		}
		*tmp='\0';
		
		if ((end=strchr(str,'='))!=NULL) { /* got the delimeter */

			strcpy(rest,++end);

			/* Make str end at the first delim now. This gives us our var. */
			for( i=0; str[i] != '\0'; i++) {
				if ( str[i]=='=') {
					str[i] = '\0';
					break;
				}
				tmp=str;
			}
		}
	}
  pclose(pcommand);

	if (strcasecmp(tmp,"PWD")==0) {
		chdir(rest); /* PWD is a special case */
	}
	else
		setenv(tmp,rest,1);
}
#endif

/* Run a command, and display its output in a window */
void RunShow (char *title, char *command) {
	FILE *pcommand;
	char *message[MAX_ITEMS_IN_MENU+1];
	char str[MENU_ITEM_TEXTSIZE+2];
	int i=0,j;

	/* Display wait text. */
	DrawBase(WAITTEXT);
	SLsmg_refresh();
	
	/* Get the command output */
	pcommand=popen(command,"r");
	while (fgets(str,MENU_ITEM_TEXTSIZE+1,pcommand)!=NULL) {
		message[i]=(char *) malloc(MENU_ITEM_TEXTSIZE+1);
		if (i+1==MAX_ITEMS_IN_MENU)
			strncpy(str,TRUNCATED_STRING,MENU_ITEM_TEXTSIZE);
		strcpy(message[i++],str);
		if (i==MAX_ITEMS_IN_MENU) 
			break;
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

/* Handle a control c by either exiting pdmenu or doing nothing */
void Handle_Ctrl_C() {
	if (Q_Exits==1) {
		Screen_Reset();
#ifdef GPM_SUPPORT
		EndMouse(); 
#endif
		exit(0);
	}
}

/* Process the string, replacing ~title:text~ flags with input from the user */
/* Returns NULL if the user hits escape */
char *EditTags(char *s) {
	char ret[MENU_ITEM_COMMANDSIZE+MENU_ITEM_COMMANDSIZE];
	char tagtitle[MENU_ITEM_COMMANDSIZE],tagbuf[MENU_ITEM_COMMANDSIZE];
	int tagtitlecount,tagbufcount,retcount=0;
	char *s2,*s3,*s4;
	int i,i2,ok;

	for (i=0;i<strlen(s);i++) {
		if ((s[i] == '~') && (((i>0) && (s[i-1]!='\\')) || (i==0))) { /* Here's a tag, probably. Need to find its end. */
			tagtitlecount=0;
			i2=i;
			ok=0;
			for (i++;i<strlen(s);i++) {
				if ((s[i]==':') && (s[i-1]!='\\')) { /* End of tag title */
					tagtitle[tagtitlecount]='\0';
					tagbufcount=0;
					for (i++;i<strlen(s);i++) {
						if ((s[i]=='~') && (s[i-1]!='\\')) { /* End of tag */
							tagbuf[tagbufcount]='\0';
							ok=1;
							s2=tagbuf;
							s3=tagtitle;
							s4=DoInputBox(s3,s2);
							if (s4 == NULL)
								return NULL; /* user hit escape */
							ret[retcount]='\0';
							strcat(ret,s4);
							retcount=retcount+strlen(s4);
							break;
						}
						else
							tagbuf[tagbufcount++]=s[i];
					}
					break;
				}
				else
					tagtitle[tagtitlecount++]=s[i];
			}
			if (ok==0) 
				i=i2; /* wasn't a tag, after all */
		}
		else 
			ret[retcount++]=s[i];
	}
	ret[retcount]='\0';
	s2=ret;
	return s2;
}

/* Run the command that is selected in the passed menu */
void RunCommand (Menu_Type *m) {
	char *command;

	if (m->items[m->selected].command[0]!='\0') { /* don't try to run a null command */
		command=m->items[m->selected].command;

		if (strchr(m->items[m->selected].flags,EDIT_FLAG)!=NULL) /* edit command on fly */
			command=EditTags(command);
			if (command == NULL)
				return; /* user hit escape */

#ifdef SETENV_FLAG_OK
		if (strchr(m->items[m->selected].flags,SETENV_FLAG)!=NULL) { /* a setenv command */
			RunSetenv(command);
		}
		else
#endif

		if (strchr(m->items[m->selected].flags,DISPLAY_FLAG)==NULL) { /* normal display */
			if (strchr(m->items[m->selected].flags,NO_CLEAR_FLAG)==NULL) { /* clear screen */
				SLsmg_cls();
				SLsmg_normal_video();
				Screen_Reset();
#ifdef GPM_SUPPORT
				EndMouse(); /* return to normal GPM/selection mode */
#endif
			}

			system(command);
	
			if (strchr(m->items[m->selected].flags,NO_CLEAR_FLAG)==NULL) { /* redraw screen */
				Screen_Init();
			
				if (strchr(m->items[m->selected].flags,PAUSE_FLAG)!=NULL) {  /* pause 1st */
					printf(PRESS_ENTER_STRING);
					fflush(stdout); /* make sure above is displayed. */
					SLang_getkey();
					SLang_flush_input(); /* kill any buffered input */
				}

#ifdef GPM_SUPPORT
				gpm_ok=InitMouse(); /* grab mouse pointer again. */
#endif

				/* Has the screen been resized lately? */
				if (Want_Screen_Resize == 1)
					Resize_Screen();
				DrawAll();
			}
		}
		else /* display in window */
			RunShow(m->items[m->selected].text,command);
	}
}

/* Display the submenu that is selected in the passed menu */
void ShowSubMenu (Menu_Type *m) {
	int c;

	/* find matching menu, if any */
	for (c=0;c<NumMenus;c++) {
		if (strcasecmp(m->items[m->selected].command,menus[c].name)==0) {
			/* got match -- show it and handle input for it. */
			m=menus+c;
			AddWindow(c);
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

/* Handle a pdmenu menu, runnnig commands on it, and exiting if neccessary */
int Pdmenu_Action (Menu_Type *m) {

	switch (m->items[m->selected].type) {
		case MENU_EXEC:
			RunCommand(m);
			return 0;
		case MENU_SHOW:
			ShowSubMenu(m);
			return 0;
		case MENU_EXIT:
			return QUIT_EXIT;
		default:
			return 0;
	}
}

int main (int argc, char **argv) {
	Menu_Type *m;
	int ret;

	NumMenus=0;
	NumOnScreen=-1;
	Use_Color=!(getenv("COLORTERM")==NULL); /* If COLORTERM is set, use color by default */
	Q_Exits=1;

	/* SLang screen objects and their default colors */
	strcpy(ScreenObjNames[1],DESKTOP_NAME);	
	strcpy(FG[0],DESKTOP_FG_DEFAULT);	
	strcpy(BG[0],DESKTOP_BG_DEFAULT);
	strcpy(ScreenObjNames[2],TITLE_NAME);	
	strcpy(FG[1],TITLE_FG_DEFAULT);	
	strcpy(BG[1],TITLE_BG_DEFAULT);
	strcpy(ScreenObjNames[3],BASE_NAME);	
	strcpy(FG[2],BASE_FG_DEFAULT);	
	strcpy(BG[2],BASE_BG_DEFAULT);
	strcpy(ScreenObjNames[4],MENU_NAME);	
	strcpy(FG[3],MENU_FG_DEFAULT);	
	strcpy(BG[3],MENU_BG_DEFAULT);
	strcpy(ScreenObjNames[5],SELBAR_NAME);	
	strcpy(FG[4],SELBAR_FG_DEFAULT);	 
	strcpy(BG[4],SELBAR_BG_DEFAULT);
	strcpy(ScreenObjNames[6],SHADOW_NAME);	
	strcpy(FG[5],SHADOW_FG_DEFAULT);	
	strcpy(BG[5],SHADOW_BG_DEFAULT);
	strcpy(ScreenObjNames[7],MENU_HI_NAME);	
	strcpy(FG[6],MENU_HI_FG_DEFAULT);	
	strcpy(BG[6],MENU_HI_BG_DEFAULT);
	strcpy(ScreenObjNames[8],SELBAR_HI_NAME);	
	strcpy(FG[7],SELBAR_HI_FG_DEFAULT);	
	strcpy(BG[7],SELBAR_HI_BG_DEFAULT);

	/* Parse parameters and load pdmenurc file. */
	GetConfig(argc,argv);
	SanityCheckMenus();

	/* color or b&w? Tell slang */
	if (Use_Color==1) { /* color */
 		DESKTOP=1;
		TITLE=2;
		BASE=3;
		MENU=4;
		SELBAR=5;
		SHADOW=6;
		MENU_HI=7;
		SELBAR_HI=8;
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
	}

	Screen_Init();

	Screen_Setcolors();
  SetScreensize();

#ifdef GPM_SUPPORT
	gpm_ok=InitMouse();
#endif

	m=menus+0; /* point m to our first menu */

	AddWindow(0);
	DrawAll();

	do {
		ret=DoMenu(m,Pdmenu_Action,Handle_Ctrl_C);
	} while ((ret!=QUIT_EXIT) && !((ret==Q_KEY_EXIT) && (Q_Exits)));

#ifdef GPM_SUPPORT
	EndMouse(); 
#endif

	Screen_Reset();
	return 0;                  
}
