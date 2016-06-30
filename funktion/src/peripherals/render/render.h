#ifndef RENDER_C_H
#define RENDER_C_H

#include "../core/core.h"
#include "geometry.h"
#include "renderdef.h"

typedef struct
{
	uchar FontWidth;
	uchar FontHeight;
	const ushort *data;
}Font;

//TODO: These cannot be static.
static const Font Font16x26 = { 16, 26, FontStyle16x26 };
static const Font Font11x18 = { 11, 18, FontStyle11x18 };
static const Font Font7x10  = { 7, 10, FontStyle7x10 };


typedef enum
{
	Rotation0 = 0,
	Rotation90,
	Rotation180,
	Rotation360
}RenderRotation;

typedef struct
{
	uint Stroke;
	bool IsInnerStroke;
	bool IsFilled;
	uint StrokeColor;
	uint FillColor;
}RenderProperties;

typedef enum
{
	RenderTypeRect,
	RenderTypeLine,
	RenderTypeIsoTriangle,
	RenderTypeSine,
	RenderTypeLabel,
	RenderTypePoly,
}RenderType;

typedef struct
{
	RenderType Type;
	Frame Bounds;
}RenderHeader;

typedef struct
{
	RenderHeader Header;
	RenderProperties Properties;
}RectRender;

typedef struct
{
	RenderHeader Header;
	RenderProperties Properties;
	ushort Amplitude;
}SineRender;

typedef struct
{
	RenderHeader Header;
	uint Foreground;
	uint Background;
	char* String;
	Font Font;
}LabelRender;

typedef struct
{
	RenderHeader Header;
	RenderProperties Properties;
	uchar Rotation;
}IsoTriangleRender; /* Isosceles Triangle */

typedef struct
{
	RenderHeader Header;
	RenderProperties Properties;
	Point PositionEnd;
}LineRender;

typedef struct
{
	RenderHeader Header;
	uint Stroke;
	uint StrokeColour;
	Point* Vertices;
	uint VertCount;
}PolyRender;


Frame frame_init(uint x, uint y, uint width, uint height);
Point point_init(uint xPosition, uint yPosition);
RenderHeader render_header_init(RenderType type, Frame frame);
RenderProperties render_properties_init(uint stroke, uint strokeColor, bool isInnerStroke, bool isFilled, uint fillColor);

PolyRender render_poly_init(Point* vertices, uint vertCount, uint stroke, uint strokeColour);
RectRender render_rect_init(uint xPosition, uint yPosition, uint xLength, uint yLength, RenderProperties properties);
SineRender render_sine_init(uint xPosition, uint yPosition, uint amplitude, uint stroke, uint strokeColor);
LineRender render_line_init(uint xPosition1, uint yPosition1, uint xPosition2, uint yPosition2, ushort stroke, uint strokeColor);
LabelRender render_label_init(uint xPosition, uint yPosition, char* string, Font font, uint background, uint foreground);
IsoTriangleRender render_isotriangle_init(uint xPosition, uint yPosition, ushort height, ushort width, RenderRotation rotation, RenderProperties properties);

void render_subscribe_draw(void(*callback)(uint16_t x, uint16_t y, uint32_t color));
void render_draw(RenderHeader* header); /* Generic Implementation */
void render_draw_line(LineRender line);
void render_draw_rectangle(RectRender shape);
void render_draw_sinewave(SineRender sine);
void render_draw_label(LabelRender label);
void render_draw_char(uint x, uint y, char c, Font font, uint foreground, uint background);
void render_draw_poly(PolyRender poly);
void render_draw_isotriangle(IsoTriangleRender triangle);
#endif
