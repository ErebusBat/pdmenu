/*
 *      Mouse support for pdmenu
 *      by randolph chung <rc42@cornell.edu>, modified by Joey.
 */

#ifdef GPM_SUPPORT

#include "global.h"
#include "math.h"
#include <sys/time.h>
#include <sys/types.h>

/* checks mouse status */
/* return vals:
 * MOUSE_NOTHING = nothing interesting, or did something already
 * MOUSE_BUTTON_LEFT = pressed left mouse button
 * MOUSE_BUTTON_RIGHT = pressed right mouse button
 * MOUSE_UP = mouse moved up one character
 * MOUSE_DOWN = mouse moved down one character
 */
int CheckMouse() {
	fd_set rfds;
	struct timeval tv;
	int retval;
	Gpm_Event event;

	while (1) {
		/* wait for input on gpm_fd or STDIN */
		FD_ZERO(&rfds);
		FD_SET(gpm_fd, &rfds);
		FD_SET(0, &rfds);          /* filedes of STDIN is 0 */
		/* wait for 10 seconds, but doesn't really matter .... */
		tv.tv_sec = 10;
		tv.tv_usec = 0;

		retval = select(gpm_fd+1, &rfds, NULL, NULL, &tv);

		if (retval) {    /* data available */
			if ((FD_ISSET(gpm_fd, &rfds)) &&  /* data is for mice */
			    (Gpm_GetEvent(&event))) {     /* can read something */
				if ((event.type & GPM_DOWN) && (event.buttons & GPM_B_LEFT)) 
					return(MOUSE_BUTTON_LEFT);
				if ((event.type & GPM_DOWN) && (event.buttons & GPM_B_RIGHT))
					return(MOUSE_BUTTON_RIGHT);
				if (event.dy > 0)
					return(MOUSE_DOWN);
				if (event.dy < 0)
					return(MOUSE_UP);
			} else { /* data is for keyboard */
				return(MOUSE_NOTHING);
			} /* else */
		} /* if */
	}
}

#ifdef 0
void ClearMouse(void) {
	fd_set rfds;
	struct timeval tv;
	int retval;
	Gpm_Event event;

	do {
		/* check for input on gpm_fd */
		FD_ZERO(&rfds);
		FD_SET(gpm_fd, &rfds);
		/* wait for 500 microseconds -- we are just checking for pending inputs */
		tv.tv_sec = 0;
		tv.tv_usec = 500;
   
		retval = select(gpm_fd+1, &rfds, NULL, NULL, &tv);
		if (retval)
			Gpm_GetEvent(&event);

	} while (retval);
}
#endif

/* Return 1 if mouse detected, 0 otherwise */
int InitMouse() {
	Gpm_Connect conn;
	conn.eventMask = ~0;		/* I want all events */
	conn.defaultMask = 0;		/* no default treatment */
	conn.maxMod = 0;   /* only read unmodified mouse keys */
	conn.minMod = 0;
	gpm_zerobased = 1;		/* coordinates start from zero */

	gpm_fd=Gpm_Open(&conn, 0);
	if (gpm_fd < 0) {
		/* printf("GPM not found. Mouse support disabled.\n"); */
		return 0;
	}
	return 1;
}

#endif
