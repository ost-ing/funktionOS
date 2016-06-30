#ifndef NUMBER_PICKER_C_H
#define NUMBER_PICKER_C_H

#include "../core/core.h"
#include "render.h"

typedef struct{
	Point Position;
	uint Value;
	uint SelectedIndex;
}NumberPicker;

NumberPicker number_picker_init(Point p);
void number_picker_expsel(NumberPicker* picker, bool increment);
void number_picker_expincr(NumberPicker* picker, bool increment);
void number_picker_draw(NumberPicker* instance);

#endif
