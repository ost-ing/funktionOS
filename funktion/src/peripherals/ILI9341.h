#ifndef ILI9341_C_H
#define ILI9341_C_H

#include "../core/core.h"

//Colors
#define ILI9341_COLOR_WHITE     0xFFFF
#define ILI9341_COLOR_BLACK     0x0000
#define ILI9341_COLOR_RED       0xF800
#define ILI9341_COLOR_GREEN     0x07E0
#define ILI9341_COLOR_GREEN2    0xB723
#define ILI9341_COLOR_BLUE      0x001F
#define ILI9341_COLOR_BLUE2     0x051D
#define ILI9341_COLOR_YELLOW    0xFFE0
#define ILI9341_COLOR_ORANGE    0xFBE4
#define ILI9341_COLOR_CYAN      0x07FF
#define ILI9341_COLOR_MAGENTA   0xA254
#define ILI9341_COLOR_GRAY      0x7BEF //1111 0111 1101 1110
#define ILI9341_COLOR_BROWN     0xBBCA

typedef enum
{
  ILI9341_Orientation_Portrait1 = 0x58,    /* Default */
  ILI9341_Orientation_Portrait2 = 0x88,
  ILI9341_Orientation_Landscape1 = 0x28,
  ILI9341_Orientation_Landscape2 = 0xE8,
}ILI9341Orientation;

typedef struct
{
  ILI9341Orientation Orientation;
  ushort Width;
  ushort Height;
}ILI9341Settings;


/* ILI9341 API */
void ili9341_init(void);
void ili9341_fill(uint32_t color);
void ili9341_set_cursor(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void ili9341_send_command(uint8_t command);
void ili9341_send_data(uint8_t data);
void ili9341_send_command_buffer(uint8_t* command, uint length);
void ili9341_send_data_buffer(int* data, uint length);
void ili9341_draw_pixel(uint16_t x, uint16_t y, uint32_t color);
void ili9341_rotate(ILI9341Orientation orientation);
#endif
