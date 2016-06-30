#include "render.h"

void(*draw_pixel)(uint16_t x, uint16_t y, uint32_t color);

static unsigned long absval(signed long x)
{
    unsigned long int signext = (x >= 0) ? 0 : -1; //This can be done with an ASR instruction
    return (x + signext) ^ signext;
}

void render_subscribe_draw(void(*callback)(uint16_t x, uint16_t y, uint32_t color))
{
	draw_pixel = callback;
}

RenderHeader render_header_init(RenderType type, Frame frame)
{
	return (RenderHeader)
	{
		type,
		frame
	};
}
Frame frame_init(uint x, uint y, uint width, uint height) { return (Frame) { x, y, width, height }; }
Point point_init(uint xPosition, uint yPosition){ return (Point) { xPosition, yPosition }; }

RenderProperties render_properties_init(uint stroke, uint strokeColor, bool isInnerStroke, bool isFilled, uint fillColor)
{
	RenderProperties properties = {0};
	properties.Stroke = stroke;
	properties.StrokeColor = strokeColor;
	properties.IsInnerStroke = isInnerStroke;
	properties.IsFilled = isFilled;
	properties.FillColor = fillColor;
	return properties;
}

RectRender render_rect_init(uint xPosition, uint yPosition, uint xLength, uint yLength, RenderProperties properties)
{
	RectRender shape = {0};
	shape.Header = render_header_init(RenderTypeRect, frame_init(xPosition, yPosition, xLength, yLength));
	shape.Properties = properties;
	return shape;
}

SineRender render_sine_init(uint xPosition, uint yPosition, uint amplitude, uint stroke, uint strokeColor)
{
	RenderProperties properties = {0};
	properties.Stroke = stroke;
	properties.StrokeColor = strokeColor;
	SineRender shape = {0};
	shape.Header = render_header_init(RenderTypeSine, frame_init(xPosition, yPosition, 0, 0)); //TODO: Calc width/height
	shape.Amplitude = amplitude;
	shape.Properties = properties;
	return shape;
}

IsoTriangleRender render_isotriangle_init(uint xPosition, uint yPosition, ushort height, ushort width, RenderRotation rotation, RenderProperties properties)
{
	IsoTriangleRender shape = {0};
	shape.Header = render_header_init(RenderTypeIsoTriangle, frame_init(xPosition, yPosition, width, height));
	shape.Properties = properties;
	shape.Rotation = rotation;
	return shape;
}

LineRender render_line_init(uint xPosition1, uint yPosition1, uint xPosition2, uint yPosition2, ushort stroke, uint strokeColor)
{
	RenderProperties properties = {0};
	properties.Stroke = stroke;
	properties.StrokeColor = strokeColor;
	LineRender shape = {0};
	// calc width and height
	int width = absval(xPosition1 - xPosition2);
	int height = absval(yPosition1 - yPosition2);
	shape.Header = render_header_init(RenderTypeLine, frame_init(xPosition1, yPosition1, width, height));
	shape.PositionEnd = point_init(xPosition2, yPosition2);
	shape.Properties = properties;
	return shape;
}

LabelRender render_label_init(uint xPosition, uint yPosition, char* string, Font font, uint background, uint foreground)
{
	LabelRender label;
	label.Background = background;
	label.Foreground = foreground;
	label.Font = font;
	label.String = string;

	int c = 0;
	while(*string)
	{
		c++;
		string++;
	}

	label.Header = render_header_init(RenderTypeLabel, frame_init(xPosition, yPosition, c * font.FontWidth, font.FontHeight));

	return label;
}

PolyRender render_poly_init(Point* vertices, uint vertCount, uint stroke, uint strokeColour)
{
	// Determine width/height
	uint minX = vertices[0].X;
	uint maxX = vertices[0].X;
	uint minY = vertices[0].Y;
	uint maxY = vertices[0].Y;

	for (int i = 1; i < vertCount; i++)
	{
		if (vertices[i].X < minX)
			minX = vertices[i].X;
		if (vertices[i].X > maxX)
			maxX = vertices[i].X;
		if (vertices[i].Y < minY)
			minY = vertices[i].Y;
		if (vertices[i].Y > maxY)
			maxY = vertices[i].Y;
	}

	PolyRender poly = {0};
	poly.Header.Bounds = frame_init(vertices[0].X, vertices[0].Y, maxX - minX, maxY - minY);
	poly.Header.Type = RenderTypePoly;
	poly.Vertices = vertices;
	poly.VertCount = vertCount;
	poly.Stroke = stroke;
	poly.StrokeColour = strokeColour;

	return poly;
}

void render_draw(RenderHeader* header)
{
	// All rendering structs must contain the RenderHeader at the beginning
	RenderType type = header->Type;

	/* Generic implementation */
	switch (type)
	{
		case RenderTypeLabel:
			render_draw_label(*(LabelRender*)header);
			break;
		case RenderTypeLine:
			render_draw_line(*(LineRender*)header);
			break;
		case RenderTypeRect:
			render_draw_rectangle(*(RectRender*)header);
			break;
		case RenderTypeIsoTriangle:
			render_draw_isotriangle(*(IsoTriangleRender*)header);
			break;
		case RenderTypePoly:
			render_draw_poly(*(PolyRender*)header);
		default:
			throw(InvalidOperationException, "Unsupported Render Type for generic render_draw implementation");
			break;
	}
}

void render_draw_sinewave(SineRender sine)
{
	if (draw_pixel == NULL)
		throw(MissingPointerException, "cannot draw sine wave. Subscribe first to draw");

	uint strokeDepth = sine.Properties.Stroke;
	uint xPosition = sine.Header.Bounds.X;
	uint yPosition = sine.Header.Bounds.Y;
	int amplitude = sine.Amplitude;

	for (int s = 0; s < strokeDepth; s++)
	{
		for (int i = 0; i < 128-1; i++)
		{
			int now = sin[i] * amplitude;
			int next = sin[i+1] * amplitude;

			// interpolate the sine-wave's Y values.
			bool isGreater = now > next;
			int dy = (isGreater ? (now - next) : (next - now));
			for (int y = 0; y < dy; y++)
			{
				draw_pixel(xPosition + i, yPosition + now + s +  (isGreater ? -y : y), sine.Properties.StrokeColor);
			}

			draw_pixel(xPosition + i, yPosition + now + s, sine.Properties.StrokeColor);
		}
	}
}

void render_draw_poly(PolyRender poly)
{
	if (draw_pixel == NULL)
		throw(MissingPointerException, "Cannot draw polly. Subscribe to draw");

	Point* vertices = poly.Vertices;

	for (int i = 0; i < poly.VertCount - 1; i++)
	{
		Point p1 = vertices[i];
		Point p2 = vertices[i + 1];
		LineRender line = render_line_init(p1.X, p1.Y, p2.X, p2.Y, poly.Stroke, poly.StrokeColour);
		render_draw_line(line);
	}
}

void render_draw_isotriangle(IsoTriangleRender triangle)
{
	if (draw_pixel == NULL)
		throw(MissingPointerException, "Cannot draw iso-triangle. Subscribe to draw");

	RenderProperties properties = triangle.Properties;

	uint xPosition = triangle.Header.Bounds.X;
	uint yPosition = triangle.Header.Bounds.Y;
	uint strokeColor = properties.StrokeColor;
	uint fillColor = properties.FillColor;

	// Split iso triangle into 2 right angle triangles

	int width = triangle.Header.Bounds.Width;
	int halfWidth = width / 2.0;
	int height = triangle.Header.Bounds.Height;
	int gradient = height / halfWidth;


	if (triangle.Rotation == Rotation0 || triangle.Rotation == Rotation180)
	{
		gradient = (triangle.Rotation == Rotation0 ? gradient : -gradient);

		for (int w = 0; w <= halfWidth; w ++)
		{
			int dy = w * gradient;

			draw_pixel(xPosition + w, yPosition, strokeColor);

			if (properties.IsFilled)
			{
				int ady = absval(dy);
				for (int y = 0; y < ady; y++)
					draw_pixel(xPosition + w, yPosition + (triangle.Rotation == Rotation0 ? y : -y), fillColor);
			}

			draw_pixel(xPosition + w, yPosition + dy, strokeColor);
		}

		for (int w = halfWidth; w <= width; w++)
		{
			int dy =  ((width - w) * gradient);

			draw_pixel(xPosition + w, yPosition, strokeColor);

			if (properties.IsFilled)
			{
				int ady = absval(dy);
				for (int y = 0; y < ady; y++)
					draw_pixel(xPosition + w, yPosition + (triangle.Rotation == Rotation0 ? y : -y), fillColor);
			}

			draw_pixel(xPosition + w, yPosition + dy, strokeColor);
		}
	}
}

void render_draw_line(LineRender shape)
{
	if (draw_pixel == NULL)
		throw(MissingPointerException, "Cannot draw line. Subscribe first to draw");

	int x0 = shape.Header.Bounds.X;
	int x1 = shape.PositionEnd.X;
	int y0 = shape.Header.Bounds.Y;
	int y1 = shape.PositionEnd.Y;

	int16_t dx, dy, sx, sy, err, e2;

	dx = (x0 < x1) ? (x1 - x0) : (x0 - x1);
	dy = (y0 < y1) ? (y1 - y0) : (y0 - y1);
	sx = (x0 < x1) ? 1 : -1;
	sy = (y0 < y1) ? 1 : -1;
	err = ((dx > dy) ? dx : -dy) / 2;

	while (1)
	{
		draw_pixel(x0, y0, shape.Properties.StrokeColor);
		if (x0 == x1 && y0 == y1)
			break;
		e2 = err;
		if (e2 > -dx) {
			err -= dy;
			x0 += sx;
		}
		if (e2 < dy) {
			err += dx;
			y0 += sy;
		}
	}
}

void render_draw_rectangle(RectRender shape)
{
	if (draw_pixel == NULL)
		throw(MissingPointerException, "Cannot draw rectangle. Subscribe first to draw");

	RenderProperties properties = shape.Properties;

	uint xPosition = shape.Header.Bounds.X;
	uint yPosition = shape.Header.Bounds.Y;
	uint xLength = shape.Header.Bounds.Width;
	uint yLength = shape.Header.Bounds.Height;

	/* Draw the stroke */
	uint strokeDepth = properties.Stroke;
	uint strokeColor = properties.StrokeColor;
	bool strokeInner = properties.IsInnerStroke;

	int stroke = 1;

	for (int s = 0; s < strokeDepth; s++)
	{
		stroke = (strokeInner ? s : (s*-1));
		for (int x1 = 0; x1 < xLength; x1++)
			draw_pixel(xPosition + x1, yPosition + stroke, strokeColor);

		stroke = (strokeInner ? (s*-1) : s);
		for (int y1 = 0; y1 < yLength; y1++)
			draw_pixel(xPosition + xLength + stroke, yPosition + y1, strokeColor);

		for (int x2 = 0; x2 < xLength; x2++)
			draw_pixel(xPosition + xLength - x2, yPosition + yLength + stroke, strokeColor);

		stroke = (strokeInner ? s : (s*-1));
		for (int y2 = 0; y2 < yLength; y2++)
			draw_pixel(xPosition + stroke, yPosition + yLength - y2, strokeColor);
	}

	if (!properties.IsFilled)
		return;

	/* Fill it in */

	uint fillColor = properties.FillColor;

	if (strokeInner)
	{
		xLength -= (strokeDepth * 2);
		yLength -= (strokeDepth * 2);
		xPosition += strokeDepth;
		yPosition += strokeDepth;
	}

	for (int x = xPosition; x < (xLength + xPosition); x++)
	{
		for (int y = yPosition; y < (yLength + yPosition); y++)
		{
			draw_pixel(x, y, fillColor);
		}
	}
}

void render_draw_label(LabelRender label)
{
	uint xPosition = label.Header.Bounds.X;
	uint yPosition = label.Header.Bounds.Y;
	char* str = label.String;

	while (*str)
	{
		render_draw_char(xPosition, yPosition, *str++, label.Font, label.Foreground, label.Background);
		xPosition += label.Font.FontWidth;
	}
}

void render_draw_char(uint x, uint y, char c, Font font, uint foreground, uint background)
{
	uint i, b, j;
	uint xPosition = x;
	uint yPosition = y;

	for (i = 0; i < font.FontHeight; i++)
	{
		b = font.data[(c - 32) * font.FontHeight + i];
		for (j = 0; j < font.FontWidth; j++)
		{
			if ((b << j) & 0x8000)
				draw_pixel(xPosition + j, (yPosition + i), foreground);
		//	else if ((background & ILI9341_TRANSPARENT) == 0)
		//		draw_pixel(xPosition + j, (yPosition + i), background);
		}
	}
}
