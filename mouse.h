/*
 * GPM Mouse handling routines for pdmenu
 * by Randolph Chung <rc42@cornell.edu>, modified by Joey.
 */

#ifdef GPM_SUPPORT

extern int CheckMouse(void);
extern void ClearMouse(void);
extern int InitMouse(void);
#include <gpm.h>
#define EndMouse Gpm_Close

#define MOUSE_NOTHING 0
#define MOUSE_BUTTON_LEFT 1
#define MOUSE_BUTTON_RIGHT 2
#define MOUSE_UP 3
#define MOUSE_DOWN 4

#endif
