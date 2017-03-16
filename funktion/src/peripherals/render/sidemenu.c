#include "sidemenu.h"
#include "render.h"
#include "../peripherals/ILI9341.h"  //temporary

#define SIDEMENU_ITEM_MAX 5
#define SIDEMENU_SCROLL_ITEMS (SIDEMENU_ITEM_MAX - 2)

static RectRender Background;
static IsoTriangleRender UpTriangle;
static IsoTriangleRender DownTriangle;

static SideMenuItem Items[SIDEMENU_ITEM_MAX];
static Point ScrollPositions[SIDEMENU_SCROLL_ITEMS];

static int ItemCount = 0;
static int ScrollIndex = 0;
static volatile bool IsActive = false;

/* IRQ Handlers */
//static void sidemenu_upbutton_pressed(void);
//static void sidemenu_downbutton_pressed(void);
//static void sidemenu_button1_pressed(void);
//static void sidemenu_button2_pressed(void);
//static void sidemenu_button3_pressed(void);

void sidemenu_init(void)
{
  // Allocate background
  Background = render_rect_init(320 - 50, 0, 50, 240, render_properties_init(1, ILI9341_COLOR_GRAY, false, true, ILI9341_COLOR_GRAY));

  // Allocate up/down buttons
  UpTriangle = render_isotriangle_init(320 - 42, 30, 35, 36, Rotation180, render_properties_init(1, ILI9341_COLOR_BLACK, false, true, ILI9341_COLOR_BLACK));
  DownTriangle = render_isotriangle_init(320 - 42, 240 - 30, 35, 36, Rotation0, render_properties_init(1, ILI9341_COLOR_BLACK, false, true, ILI9341_COLOR_BLACK));

  // TODO: Setup up/down buttons GPIO External Interrupt


  // Statically allocate so we can use pointers without crashing. Currently using simple labels for the side menu items
  static LabelRender label1, label2, label3, label4, label5;

  label1 = render_label_init(0, 0, "HOME", Font11x18, ILI9341_COLOR_GRAY, ILI9341_COLOR_BLUE);
  label2 = render_label_init(0, 0, "f(t)", Font11x18, ILI9341_COLOR_GRAY, ILI9341_COLOR_BLUE);
  label3 = render_label_init(0, 0, "Hz",   Font11x18, ILI9341_COLOR_GRAY, ILI9341_COLOR_BLUE);
  label4 = render_label_init(0, 0, "Amp.", Font11x18, ILI9341_COLOR_GRAY, ILI9341_COLOR_BLUE);
  label5 = render_label_init(0, 0, "Auto", Font11x18, ILI9341_COLOR_GRAY, ILI9341_COLOR_BLUE);
  label5 = render_label_init(0, 0, "Cfg",  Font11x18, ILI9341_COLOR_GRAY, ILI9341_COLOR_BLUE);

  // Populate items, pass the pointers of the labels to our SideMenuItem's
  Items[ItemCount++] = sidemenuitem_init(NULL, &label1);
  Items[ItemCount++] = sidemenuitem_init(NULL, &label2);
  Items[ItemCount++] = sidemenuitem_init(NULL, &label3);
  Items[ItemCount++] = sidemenuitem_init(NULL, &label4);
  Items[ItemCount++] = sidemenuitem_init(NULL, &label5);

  // Populate positions for items
  int div =  (240/SIDEMENU_ITEM_MAX);
  ScrollPositions[0] = point_init(320 - 25, div*1 + div/2);
  ScrollPositions[1] = point_init(320 - 25, div*2 + div/2);
  ScrollPositions[2] = point_init(320 - 25, div*3 + div/2);

  // Set the index to some arbitrary value within the bounds
  ScrollIndex = 2;
}

SideMenuItem sidemenuitem_init(void (*buttonPressed)(void), void *drawable)
{
  SideMenuItem item =
  {
    buttonPressed,
    drawable
  };
  return item;
}

void sidemenu_draw_dividers(void)
{
  int y = 240 / 5; // 4 divs

  for (int i = 1; i <= 4; i++)
    render_draw_line(render_line_init(320 - 48, y * i, 320 - 2, y * i, 1, ILI9341_COLOR_BLACK));
}


void sidemenu_show(void)
{
  // Draw the background arrows and dividers
  render_draw_rectangle(Background);
  render_draw_isotriangle(UpTriangle);
  render_draw_isotriangle(DownTriangle);
  sidemenu_draw_dividers();

  // Assume for now, that only 3 items will ever exist at one time on the screen.
  SideMenuItem item1 = Items[ScrollIndex - 1];
  SideMenuItem item2 = Items[ScrollIndex];
  SideMenuItem item3 = Items[ScrollIndex + 1];

  // Cast headers
  RenderHeader* header1 = (RenderHeader*) item1.Render;
  RenderHeader* header2 = (RenderHeader*) item2.Render;
  RenderHeader* header3 = (RenderHeader*) item3.Render;

  // Set position for renders. The scroll position is at the 'centre', therefore to centre-align
  // The Render's we must subtract half the width and height
  header1->Bounds = frame_init(ScrollPositions[0].X - (header1->Bounds.Width /2), ScrollPositions[0].Y - (header1->Bounds.Height /2), header1->Bounds.Width, header1->Bounds.Height);
  header2->Bounds = frame_init(ScrollPositions[1].X - (header2->Bounds.Width /2), ScrollPositions[1].Y - (header2->Bounds.Height /2), header2->Bounds.Width, header2->Bounds.Height);
  header3->Bounds = frame_init(ScrollPositions[2].X - (header3->Bounds.Width /2), ScrollPositions[2].Y - (header3->Bounds.Height /2), header3->Bounds.Width, header3->Bounds.Height);

  // Draw the renders
  render_draw(header1);
  render_draw(header2);
  render_draw(header3);

  // Enable interrupt call backs
  IsActive = true;
}

void sidemenu_hide(void)
{
  IsActive = false;
  int previousBackground = Background.Properties.FillColor;
  Background.Properties.FillColor = 0x000000;
  render_draw_rectangle(Background);  // draw over it all
  Background.Properties.FillColor = previousBackground;
}

void sidemenu_scrolldown(void)
{
  // Only allow Scroll index to get to 1, because ScrollIndex-1 = 0
  if (ScrollIndex <= 1)
    return;

  // Hide/Undraw everything (TODO: Dont undraw the arrows - they dont change!).
  sidemenu_hide();
  ScrollIndex--;
  sidemenu_show();
}

void sidemenu_scrollup(void)
{
  if (ScrollIndex >= SIDEMENU_SCROLL_ITEMS)
    return;

  sidemenu_hide();
  ScrollIndex ++;
  sidemenu_show();
}

/* Button Handlers Piped from External IRQ Handler */

/* TODO:
static void sidemenu_upbutton_pressed(void)
{
  if (!IsActive)
    return;

  sidemenu_scrollup();
}

static void sidemenu_downbutton_pressed(void)
{
  if (!IsActive)
    return;

  sidemenu_scrolldown();
}


static void sidemenu_button1_pressed(void)
{
  if (IsActive)
    return;

  SideMenuItem item = Items[ScrollIndex - 1];
  if (item.button_pressed)
    item.button_pressed();
}

static void sidemenu_button2_pressed(void)
{
  if (IsActive)
    return;

  SideMenuItem item = Items[ScrollIndex];
  if (item.button_pressed)
    item.button_pressed();
}

static void sidemenu_button3_pressed(void)
{
  if (IsActive)
    return;

  SideMenuItem item = Items[ScrollIndex + 1];
  if (item.button_pressed)
    item.button_pressed();
}
*/
