/*
 * GPM Mouse handling routines for pdmenu
 * by Randolph Chung <rc42@cornell.edu>, modified by Joey.
 */

#ifdef GPM_SUPPORT

int CheckMouse(void);
void ClearMouse(void);
int InitMouse(void);
void EndMouse();

#define MOUSE_NOTHING 0
#define MOUSE_BUTTON_LEFT 1
#define MOUSE_BUTTON_RIGHT 2
#define MOUSE_UP 3
#define MOUSE_DOWN 4

#include <gpm.h>

#endif
