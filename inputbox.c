#include "global.h"
#include <string.h>
#include "slang.h"

void DrawInputBox (char *title) {
	SLsmg_set_color(MENU);
	DrawDialog(title,1,(SLtt_Screen_Rows-3)/2,SLtt_Screen_Cols-2,3,1);
	SLsmg_set_color(SELBAR);
}

/* Display an input box, get input, return it. */
/* Will return NULL if escape is pressed. */
char *DoInputBox (char *title, char *contents) {
	Menu_Type *m;
	int curspos,key;
	char s[SLtt_Screen_Cols-6];
	const char *space=" ";

#ifdef GPM_SUPPORT
	if (gpm_ok) 
		EndMouse(); /* let gpm cut and paste work as normal in edit box */
#endif

	strncpy(s,contents,SLtt_Screen_Cols-6);
	m=menus+MAX_NUM_MENUS-1;
	curspos=strlen(contents);
	strcat(s,space);

	DrawBase(INPUTHELP);
	DrawInputBox(title);
	
	while (1) {
		SLsmg_gotorc((SLtt_Screen_Rows-3)/2+1,3);
		SLsmg_write_nstring(s,SLtt_Screen_Cols-6);
		SLsmg_gotorc((SLtt_Screen_Rows-3)/2+1,3+curspos);
		SLsmg_refresh();
		key=getch();

		/* Has the screen been resized lately? */
		if (Want_Screen_Resize == 1) {
			/* This isn't done too well -- there's some flicker. */
			Resize_Screen();
			Force_Redraw();
			DrawBase(INPUTHELP);
			DrawInputBox(title);
		}

		switch (key) {
		case '\n':
		case '\r':
			DrawAll();
			s[curspos]='\0';
			contents=s;
#ifdef GPM_SUPPORT
			 	/* Start handling our own mouse events again. */
				if (gpm_ok)
					gpm_ok=InitMouse();
#endif
			return contents;
		case 12: /* ctrl-l */
		case 18: /* ctrl-r */
			Force_Redraw();
			DrawInputBox(title);
			break;
		case SL_KEY_BACKSPACE:
			if (curspos>0)
				s[--curspos]='\0';
			break;
		case 27: /* escape */
			DrawAll();
#ifdef GPM_SUPPORT
			/* Start handling our own mouse events again. */
			if (gpm_ok)
				gpm_ok=InitMouse();
#endif
			return NULL;
		default:
			if ((key>=32) && (key<127) && (curspos<SLtt_Screen_Cols-6)) {
				s[curspos++]=key;
				strcat(s,space);
			}
		}
	}
}
