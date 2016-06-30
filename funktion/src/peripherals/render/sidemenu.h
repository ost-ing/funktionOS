#ifndef SIDEMENU_C_H
#define SIDEMENU_C_H

#include "../core/core.h"

typedef struct
{
	void (*button_pressed)(void);
	void* Render;
}SideMenuItem;

void sidemenu_init(void);
void sidemenu_show(void);
void sidemenu_hide(void);
void sidemenu_draw_dividers(void);
void sidemenu_scrolldown(void);
void sidemenu_scrollup(void);

SideMenuItem sidemenuitem_init(void (*buttonPressed)(void), void *drawable);

#endif
