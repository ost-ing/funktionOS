#include "number_picker.h"
#include "../peripherals/ILI9341.h"	//temporary
#include <stdio.h>
#include <stdlib.h>

#define DIV_STROKE  1
#define NUM_DIV 	9
#define WIDTH   	180					// Should be divisable by NUM_DIV
#define HEIGHT  	30
#define DIV_WIDTH 	WIDTH / NUM_DIV

NumberPicker number_picker_init(Point p)
{
	NumberPicker picker = {{0}};
	picker.Value = 123456789;
	picker.Position = p;
	return picker;
}

// number picker layout.
// _____________________________________
// |   |   |   |   |   |   |   |   |   |
// | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |
// |___|___|___|___|___|___|___|___|___|

void number_picker_draw(NumberPicker* instance)
{
	RenderProperties properties = render_properties_init(1, ILI9341_COLOR_BLUE, false, true, ILI9341_COLOR_GRAY);
	render_draw_rectangle(render_rect_init(instance->Position.X, instance->Position.Y, WIDTH, HEIGHT, properties));

	char characters[DIV_WIDTH];
	sprintf(characters, "%d", instance->Value);

	for (int i = 0; i < NUM_DIV; i++)
	{
		LineRender line = render_line_init(instance->Position.X + (DIV_WIDTH*i),
										   instance->Position.Y,
										   instance->Position.X + (DIV_WIDTH*i),
										   instance->Position.Y + HEIGHT,
										   DIV_STROKE,
										   ILI9341_COLOR_BLACK);
		render_draw_line(line);

		bool selected = (i == instance->SelectedIndex);

		if (characters[i] > '9' || characters[i] < '0' || instance->Value == 0)
			characters[i] = '0';
		char str[2] = { characters[i], '\0' };

		LabelRender label = render_label_init(instance->Position.X + (DIV_WIDTH*i) + Font11x18.FontWidth/2 + 1,
											  instance->Position.Y + Font11x18.FontHeight/2 - 1,
											  (char*)&str,
											  Font11x18,
										 	  selected ? ILI9341_COLOR_ORANGE : ILI9341_COLOR_BLUE,
										 	  selected ? ILI9341_COLOR_WHITE : ILI9341_COLOR_BLACK);
		render_draw_label(label);
	}
}

static void number_picker_drawseg(NumberPicker* instance, int segment)
{
	RenderProperties properties = render_properties_init(1, ILI9341_COLOR_BLUE, false, true, ILI9341_COLOR_GRAY);
	render_draw_rectangle(render_rect_init(instance->Position.X + (segment*DIV_WIDTH) + DIV_STROKE,
										   instance->Position.Y,
										   DIV_WIDTH - DIV_STROKE,
										   HEIGHT,
										   properties));

	bool selected = (segment == instance->SelectedIndex);

	char characters[DIV_WIDTH];
	sprintf(characters, "%d", instance->Value);

	if (characters[segment] > '9' || characters[segment] < '0')
		characters[segment] = '0';

	char str[2] = { characters[segment], '\0' };

	LabelRender label = render_label_init(instance->Position.X + (DIV_WIDTH*segment) + Font11x18.FontWidth/2 + 1,
										  instance->Position.Y + Font11x18.FontHeight/2 - 1,
										  (char*)&str,
										  Font11x18,
										  selected ? ILI9341_COLOR_ORANGE : ILI9341_COLOR_BLUE,
										  selected ? ILI9341_COLOR_WHITE : ILI9341_COLOR_BLACK);
	render_draw_label(label);
}

void number_picker_expsel(NumberPicker* picker, bool increment)
{
	if (picker->SelectedIndex <= 0 && !increment)
		return;
	if (picker->SelectedIndex >= (NUM_DIV-1) && increment)
		return;

	picker->SelectedIndex += (increment ? 1 : -1);

	number_picker_drawseg(picker, picker->SelectedIndex + (increment ? -1 : 1));
	number_picker_drawseg(picker, picker->SelectedIndex);
}

void number_picker_expincr(NumberPicker* picker, bool increment)
{
	char characters[NUM_DIV];
	sprintf(characters, "%d", picker->Value);

//	if (((int)characters[picker->SelectedIndex] <= (int)'0') && !increment)
//		return;
//	if (((int)characters[picker->SelectedIndex] >= (int)'9') && increment)
//		return;

	characters[picker->SelectedIndex] += (increment ? 1 : -1);

	picker->Value = atoi(characters);

	number_picker_drawseg(picker, picker->SelectedIndex);
}
