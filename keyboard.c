#include "global.h"
#include "slang.h"

#define TIMEOUT 2 /* 2/10 of a second */

int getch (void) {
	int ch;

	while (0 == SLang_input_pending (1000))
		continue;

	ch = SLang_getkey ();

	if (ch == 033) {	/* escape */
		if (0 == SLang_input_pending (TIMEOUT))
			return 033;
	}

	SLang_ungetkey (ch);
	return SLkp_getkey ();
}
